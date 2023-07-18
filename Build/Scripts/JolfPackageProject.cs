using System.Collections.Generic;
using AutomationTool;
using Tools.DotNETCommon;
using UnrealBuildTool;

namespace Jolf.Automation
{
	public class JolfPackageProject : BuildCommand
	{
		public override void ExecuteBuild()
		{
			BuildVersion Version;
			if (!BuildVersion.TryRead(BuildVersion.GetDefaultFileName(), out Version))
			{
				throw new AutomationException("Unable to read build version");
			}

			string ProjectName = ParseParamValue("Project", "");
			FileReference InputProjectFile = GetCommandletProjectFile(ProjectName);
			DirectoryReference InputProjectDirectory = InputProjectFile.Directory;

			// Project files are copied into temp directory, processed, and then moved to RedistProjectDirectory.
			DirectoryReference TempProjectDirectory = ParseRequiredDirectoryReferenceParam("TempOutput");
			DirectoryReference RedistProjectDirectory = ParseRequiredDirectoryReferenceParam("RedistOutput");

			FileFilter Filter = new FileFilter();
			Filter.Include(InputProjectFile.GetFileName());
			Filter.Include("/Binaries/...UE4Editor*.dll");
			Filter.Include("/Binaries/...UE4Editor*.modules");
			Filter.Include("/Config/...*.ini");
			Filter.Include("/Content/...*.uasset");
			Filter.Include("/Content/...*.umap");
			Filter.Include("/Platforms/...*.ini");
			Filter.Exclude("/Saved/..."); // Explictly exclude saved otherwise uproject in saved folder is copied.

			DirectoryReference GamePluginsDirectory = DirectoryReference.Combine(InputProjectDirectory, "Plugins");
			foreach (DirectoryReference GamePluginDirectory in DirectoryReference.EnumerateDirectories(GamePluginsDirectory))
			{
				string PluginPrefix = "/Plugins/" + GamePluginDirectory.GetDirectoryName();
				Filter.Include(PluginPrefix + "/...*.uplugin");
				Filter.Include(PluginPrefix + "/Binaries/...UE4Editor*.dll");
				Filter.Include(PluginPrefix + "/Binaries/...UE4Editor*.modules");
				Filter.Include(PluginPrefix + "/Config/...*.ini");
				Filter.Include(PluginPrefix + "/Content/...*.uasset");
				Filter.Include(PluginPrefix + "/Content/...*.umap");
			}

			// Files which cannot be redistributed to modders e.g. sounds from a library.
			Filter.Exclude(".../NotForModders/...");

			// Apply the standard exclusion rules.
			foreach (string RestrictedFolderName in RestrictedFolder.GetNames())
			{
				Filter.Exclude(string.Format(".../{0}/...", RestrictedFolderName));
			}

			List<FileReference> InputFiles = Filter.ApplyToDirectory(InputProjectDirectory, true);

			if (ParseParam("DryRun"))
			{
				LogInformation("Input: {0}", InputProjectDirectory);
				LogInformation("Temp: {0}", TempProjectDirectory);
				LogInformation("Redist: {0}", RedistProjectDirectory);

				foreach (FileReference File in InputFiles)
				{
					LogInformation(File.ToString());
				}

				return;
			}

			CopyProjectFiles(InputProjectDirectory, TempProjectDirectory, InputFiles);

			FileReference TempProjectFile = FileReference.Combine(TempProjectDirectory, InputProjectFile.GetFileName());

			// Resave duplicated assets to forget references to missing assets i.e. NotForModders.
			// -IgnoreChangelist disables engine version check.
			ResavePackagesCommandlet(TempProjectFile, Parameters: "-IgnoreChangelist");

			// Mod packager uses release asset registry to determine which files are already included in the base game.
			// Originally we cooked this during BuildCookRun for the base game, but that includes references to assets
			// which are not available in the mod project, and the mod cooker warns that they are missing.
			RunCommandlet(TempProjectFile, "UE4Editor-Cmd.exe", "Cook", "-TargetPlatform=WindowsNoEditor -CreateReleaseVersion=Vanilla");

			// Build a compressed DDC Pak (.ddp) for mod editor distribution. (Required by EGS)
			// Presence of certain files e.g. /NotForModders/ will change the DDC keys, and invalidate the data built
			// here unless there are removed beforehand. This is why it must be copied out before DDC is cooked.
			string Commandlet = "DerivedDataCache";
			string Parameters = "-Fill -DDC=CreatePak -TargetPlatform=Windows -ProjectOnly";
			RunCommandlet(TempProjectFile, "UE4Editor-Cmd.exe", Commandlet, Parameters);

			// Running the above commandlets generate lots of intermediate/saved files which can be safely excluded from
			// the redistributed mod project, so we re-apply the filter to move the files we actually want to keep.
			Filter.AddRule("/DerivedDataCache/...*.ddc");
			Filter.AddRule("/DerivedDataCache/...*.ddp");
			Filter.AddRule("/Releases/...");
			List<FileReference> RedistProjectFiles = Filter.ApplyToDirectory(TempProjectDirectory, true);
			MoveProjectFiles(TempProjectDirectory, RedistProjectDirectory, RedistProjectFiles);

			// Resave .uproject file with Rocket engine association.
			FileReference RedistProjectFile = FileReference.Combine(RedistProjectDirectory, InputProjectFile.GetFileName());
			ProjectDescriptor RedistProjectDescriptor = ProjectDescriptor.FromFile(RedistProjectFile);
			RedistProjectDescriptor.EngineAssociation = string.Format("{0}.{1}", Version.MajorVersion, Version.MinorVersion);
			RedistProjectDescriptor.Save(RedistProjectFile);

			// Optionally log the files left behind after the move.
			if (ParseParam("LogTempFiles"))
			{
				LogInformation("Generated file(s) ignored:");
				foreach (FileReference TrashFile in DirectoryReference.EnumerateFiles(TempProjectDirectory, "*", System.IO.SearchOption.AllDirectories))
				{
					LogInformation(TrashFile.MakeRelativeTo(TempProjectDirectory));
				}
				LogInformation("****************************");
			}
		}
		
		private void CopyProjectFiles(DirectoryReference SourceDirectory, DirectoryReference TargetDirectory, List<FileReference> SourceFiles)
		{
			foreach (FileReference SourceFile in SourceFiles)
			{
				string RelativeToInput = SourceFile.MakeRelativeTo(SourceDirectory);
				FileReference TargetFile = FileReference.Combine(TargetDirectory, RelativeToInput);
				DirectoryReference.CreateDirectory(TargetFile.Directory);
				
				FileReference.Copy(SourceFile, TargetFile, /*bOverwrite*/ true);
				FileReference.MakeWriteable(TargetFile);
			}
		}

		private void MoveProjectFiles(DirectoryReference SourceDirectory, DirectoryReference TargetDirectory, List<FileReference> SourceFiles)
		{
			foreach (FileReference SourceFile in SourceFiles)
			{
				string RelativeToInput = SourceFile.MakeRelativeTo(SourceDirectory);
				FileReference TargetFile = FileReference.Combine(TargetDirectory, RelativeToInput);
				DirectoryReference.CreateDirectory(TargetFile.Directory);

				FileReference.Move(SourceFile, TargetFile);
			}
		}
	}
}
