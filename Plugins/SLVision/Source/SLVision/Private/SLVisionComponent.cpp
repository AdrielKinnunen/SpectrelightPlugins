// Fill out your copyright notice in the Description page of Project Settings.

#include "SLVisionComponent.h"
#include "DrawDebugHelpers.h"
#include "SLVisionSubsystem.h"

void USLVisionComponent::CalculateRelativeTargetPoints()
{
	RelativeTargetPoints.Empty();

	switch (VisionShape)
	{
	case EVisionShape::Directional:
		{
			//Rear Arc
			const float RearArcDeg = 90 - FMath::RadiansToDegrees(FMath::Atan(VisionSlope));
			const float RearArcLength = VisionCloseRadius * PI * RearArcDeg / 180;
			const int RearArcRaysCount = RearArcLength / DistanceBetweenPoints;
			const float RearArcDegPerRay = RearArcDeg / RearArcRaysCount;
			RelativeTargetPoints.Add(FVector(-VisionCloseRadius, 0, 0));
			for (int i = 1; i < RearArcRaysCount; i++)
			{
				const float RayAngle = RearArcDegPerRay * i;
				RelativeTargetPoints.Add(
					VisionCloseRadius * FVector(-1, 0, 0).RotateAngleAxis(-RayAngle, FVector(0, 0, 1)));
			}

			//Line
			const FVector RearLinePoint = FVector(-VisionCloseRadius, 0, 0).RotateAngleAxis(
				-RearArcDeg, FVector(0, 0, 1));
			const float B = RearLinePoint.Y;
			const float D = VisionRadius * VisionRadius * (1 + VisionSlope * VisionSlope) - B * B;
			const float X = (-B * VisionSlope + FMath::Sqrt(D)) / (1 + VisionSlope * VisionSlope);
			const float Y = VisionSlope * X + B;
			const FVector FrontLinePoint = FVector(X, Y, 0);
			const FVector LineSegment = FrontLinePoint - RearLinePoint;
			const int LineRaysCount = LineSegment.Size() / DistanceBetweenPoints;
			const FVector LineSegmentDelta = LineSegment / LineRaysCount;
			//UE_LOG(LogTemp, Warning, TEXT("LineRaysCount is %d"), LineRaysCount);
			for (int i = 0; i < LineRaysCount; i++)
			{
				RelativeTargetPoints.Add(RearLinePoint + LineSegmentDelta * i);
			}
			RelativeTargetPoints.Add(FrontLinePoint);

			//Front Arc
			const FVector FrontArcPoint = FVector(VisionRadius, 0, 0);
			const float FrontArcDeg = FMath::RadiansToDegrees(FMath::Atan(FrontLinePoint.Y / FrontLinePoint.X));
			//UE_LOG(LogTemp, Warning, TEXT("FrontArcDeg is %f"), FrontArcDeg);
			const float FrontArcLength = VisionRadius * PI * FrontArcDeg / 180;
			const int FrontArcRaysCount = FrontArcLength / DistanceBetweenPoints;
			const float FrontArcDegPerRay = FrontArcDeg / (FrontArcRaysCount + 1);
			for (int i = FrontArcRaysCount; i > 0; i--)
			{
				const float RayAngle = FrontArcDegPerRay * i;
				RelativeTargetPoints.Add(VisionRadius * FVector(1, 0, 0).RotateAngleAxis(RayAngle, FVector(0, 0, 1)));
			}
			RelativeTargetPoints.Add(FrontArcPoint);


			//Mirror
			for (int i = RelativeTargetPoints.Num() - 2; i > 0; i--)
			{
				const FVector Mirrored = RelativeTargetPoints[i] * FVector(1, -1, 1);
				RelativeTargetPoints.Add(Mirrored);
			}

			//UE_LOG(LogTemp, Warning, TEXT("Text, %d"), RelativeTargetPoints.Num());
		}
		break;

	case EVisionShape::Circle:
		{
			const int RaysPerHalf = PI * VisionRadius / DistanceBetweenPoints;
			const float DegreesPerRay = 180.0 / RaysPerHalf;

			// Add point directly backwards
			RelativeTargetPoints.Add(FVector(-VisionRadius, 0, 0));
			// Add other points.
			for (int i = 1 - RaysPerHalf; i < RaysPerHalf; i++)
			{
				const float RayAngle = DegreesPerRay * i;
				RelativeTargetPoints.Add(VisionRadius * FVector(1, 0, 0).RotateAngleAxis(RayAngle, FVector(0, 0, 1)));
			}
		}
		break;
	default: break;
	}
}

// Sets default values for this component's properties
USLVisionComponent::USLVisionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void USLVisionComponent::BeginPlay()
{
	Super::BeginPlay();
	GetWorld()->GetSubsystem<USLVisionSubsystem>()->AddVisionSource(this);
	CalculateRelativeTargetPoints();
	// ...
}

void USLVisionComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorld()->GetSubsystem<USLVisionSubsystem>()->RemoveVisionSource(this);
	Super::EndPlay(EndPlayReason);
}


// Called every frame
void USLVisionComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                       FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}
