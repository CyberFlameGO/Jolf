// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

namespace JolfWidgetsUtils
{
	/** Unreal does not have built-in timespan text format with milliseconds as of 4.26.2. */
	FText SecondsAsText(float InSeconds)
	{
		FTimespan Timespan = FTimespan::FromSeconds(InSeconds);

		int32 Minutes = Timespan.GetMinutes();
		int32 Seconds = Timespan.GetSeconds();
		int32 Milliseconds = Timespan.GetFractionMilli();

		FNumberFormattingOptions NumberFormattingOptions;
		NumberFormattingOptions.MinimumIntegralDigits = 2;
		NumberFormattingOptions.MaximumIntegralDigits = 2;

		FNumberFormattingOptions FracFormattingOptions;
		FracFormattingOptions.MinimumIntegralDigits = 3;
		FracFormattingOptions.MaximumIntegralDigits = 3;
	
		if (Minutes > 0)
		{
			FText TimespanFormatPattern = NSLOCTEXT("JolfTimespan", "Format_MinutesSecondsMilliseconds", "{Minutes}:{Seconds}.{Milliseconds}");
			FFormatNamedArguments TimeArguments;
			TimeArguments.Add(TEXT("Minutes"), FText::AsNumber(Minutes, &NumberFormattingOptions));
			TimeArguments.Add(TEXT("Seconds"), FText::AsNumber(Seconds, &NumberFormattingOptions));
			TimeArguments.Add(TEXT("Milliseconds"), FText::AsNumber(Milliseconds, &FracFormattingOptions));
			return FText::Format(TimespanFormatPattern, TimeArguments);
		}
		else
		{
			FText TimespanFormatPattern = NSLOCTEXT("JolfTimespan", "Format_SecondsMilliseconds", "{Seconds}.{Milliseconds}");
			FFormatNamedArguments TimeArguments;
			TimeArguments.Add(TEXT("Seconds"), FText::AsNumber(Seconds, &NumberFormattingOptions));
			TimeArguments.Add(TEXT("Milliseconds"), FText::AsNumber(Milliseconds, &FracFormattingOptions));
			return FText::Format(TimespanFormatPattern, TimeArguments);
		}
	}
}