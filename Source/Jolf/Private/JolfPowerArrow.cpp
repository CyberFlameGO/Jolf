// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "JolfPowerArrow.h"
#include "Components/StaticMeshComponent.h"

AJolfPowerArrow::AJolfPowerArrow(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	// Todo should create segment components in loop?

	// Each segment has a different stencil value so the post-process material can draw lines between them.

	LineSegmentComponent0 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LineSegment0"));
	LineSegmentComponent0->SetupAttachment(RootComponent);
	LineSegmentComponent0->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LineSegmentComponent0->CastShadow = false;
	LineSegmentComponent0->bRenderInMainPass = false;
	LineSegmentComponent0->bRenderInDepthPass = false;
	LineSegmentComponent0->bReceivesDecals = false;
	LineSegmentComponent0->bRenderCustomDepth = true;
	LineSegmentComponent0->CustomDepthStencilValue = 1;
	LineSegmentComponent0->bOnlyOwnerSee = true;

	LineSegmentComponent1 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LineSegment1"));
	LineSegmentComponent1->SetupAttachment(RootComponent);
	LineSegmentComponent1->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LineSegmentComponent1->CastShadow = false;
	LineSegmentComponent1->bRenderInMainPass = false;
	LineSegmentComponent1->bRenderInDepthPass = false;
	LineSegmentComponent1->bReceivesDecals = false;
	LineSegmentComponent1->bRenderCustomDepth = true;
	LineSegmentComponent1->CustomDepthStencilValue = 2;
	LineSegmentComponent1->bOnlyOwnerSee = true;

	LineSegmentComponent2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LineSegment2"));
	LineSegmentComponent2->SetupAttachment(RootComponent);
	LineSegmentComponent2->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LineSegmentComponent2->CastShadow = false;
	LineSegmentComponent2->bRenderInMainPass = false;
	LineSegmentComponent2->bRenderInDepthPass = false;
	LineSegmentComponent2->bReceivesDecals = false;
	LineSegmentComponent2->bRenderCustomDepth = true;
	LineSegmentComponent2->CustomDepthStencilValue = 3;
	LineSegmentComponent2->bOnlyOwnerSee = true;

	LineSegmentComponent3 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LineSegment3"));
	LineSegmentComponent3->SetupAttachment(RootComponent);
	LineSegmentComponent3->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LineSegmentComponent3->CastShadow = false;
	LineSegmentComponent3->bRenderInMainPass = false;
	LineSegmentComponent3->bRenderInDepthPass = false;
	LineSegmentComponent3->bReceivesDecals = false;
	LineSegmentComponent3->bRenderCustomDepth = true;
	LineSegmentComponent3->CustomDepthStencilValue = 4;
	LineSegmentComponent3->bOnlyOwnerSee = true;

	TriangleComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Arrow"));
	TriangleComponent->SetupAttachment(RootComponent);
	TriangleComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	TriangleComponent->CastShadow = false;
	TriangleComponent->bRenderInMainPass = false;
	TriangleComponent->bRenderInDepthPass = false;
	TriangleComponent->bReceivesDecals = false;
	TriangleComponent->bRenderCustomDepth = true;
	TriangleComponent->CustomDepthStencilValue = 5;
	TriangleComponent->bOnlyOwnerSee = true;
}

void AJolfPowerArrow::SetPower(float InPower)
{
	// Todo could improve perf here?

	const float ArrowScale = FMath::Lerp(ArrowMinScale, 1.f, InPower);
	const float LineSegmentsTotalLength = InPower * ArrowLength; // Distance from offset to base of triangle.
	const float LineSegmentMaxLength = ArrowLength * 0.25f;

	UStaticMeshComponent* LineSegmentComponents[4] = { LineSegmentComponent0, LineSegmentComponent1, LineSegmentComponent2, LineSegmentComponent3 };

	float LineSegmentLocation = OffsetLength;
	float LineSegmentLength = LineSegmentsTotalLength;

	float SegmentLength0 = FMath::Min(LineSegmentLength, LineSegmentMaxLength);
	LineSegmentLocation += SegmentLength0;
	LineSegmentLength -= SegmentLength0;
	LineSegmentComponents[0]->SetRelativeScale3D(FVector((SegmentLength0 + OffsetLength) * 0.01f, ArrowScale, 1.f));
	int32 TriangleCustomDepthValue = LineSegmentComponents[0]->CustomDepthStencilValue;

	for (int32 Index = 1; Index < 4; ++Index)
	{
		LineSegmentComponents[Index]->SetRelativeLocation(FVector(LineSegmentLocation, 0.f, 0.f));

		float SegmentLengthN = FMath::Min(LineSegmentLength, LineSegmentMaxLength);
		LineSegmentLocation += SegmentLengthN;
		LineSegmentLength -= SegmentLengthN;

		LineSegmentComponents[Index]->SetRelativeScale3D(FVector(SegmentLengthN * 0.01f, ArrowScale, 1.f));

		if (SegmentLengthN > 0.f)
		{
			TriangleCustomDepthValue = LineSegmentComponents[Index]->CustomDepthStencilValue;
		}
	}
	
	TriangleComponent->SetRelativeLocation(FVector(InPower * ArrowLength + OffsetLength, 0.f, 0.f));
	TriangleComponent->SetRelativeScale3D(FVector(ArrowScale, ArrowScale, 1.f));
	TriangleComponent->SetCustomDepthStencilValue(TriangleCustomDepthValue);
}

void AJolfPowerArrow::SetColorArrowMask()
{
	LineSegmentComponent0->SetCustomDepthStencilValue(LineSegmentComponent0->CustomDepthStencilValue |= 128);
	LineSegmentComponent1->SetCustomDepthStencilValue(LineSegmentComponent1->CustomDepthStencilValue |= 128);
	LineSegmentComponent2->SetCustomDepthStencilValue(LineSegmentComponent2->CustomDepthStencilValue |= 128);
	LineSegmentComponent3->SetCustomDepthStencilValue(LineSegmentComponent3->CustomDepthStencilValue |= 128);
	TriangleComponent->SetCustomDepthStencilValue(TriangleComponent->CustomDepthStencilValue |= 128);
}

void AJolfPowerArrow::SetShadowArrowMask()
{
	LineSegmentComponent0->SetCustomDepthStencilValue(64);
	LineSegmentComponent1->SetCustomDepthStencilValue(64);
	LineSegmentComponent2->SetCustomDepthStencilValue(64);
	LineSegmentComponent3->SetCustomDepthStencilValue(64);
	TriangleComponent->SetCustomDepthStencilValue(64);
}