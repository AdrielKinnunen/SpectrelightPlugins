// Fill out your copyright notice in the Description page of Project Settings.


#include "SLVisionSubsystem.h"
#include "DrawDebugHelpers.h"


void USLVisionSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
}

void USLVisionSubsystem::Deinitialize()
{
}

void USLVisionSubsystem::AddVisionSource(USLVisionComponent* SourceToAdd)
{
	VisionSources.Add(SourceToAdd);
}

void USLVisionSubsystem::RemoveVisionSource(USLVisionComponent* SourceToRemove)
{
	VisionSources.Remove(SourceToRemove);
}

void USLVisionSubsystem::CalculateVisionPolygons()
{
	VisionPolygons.Empty();
	for (auto SourceComponent : VisionSources)
	{
		VisionPolygons.Add(CalculateVisionPolygonFromSource(SourceComponent));
	}
}

void USLVisionSubsystem::CalculateVisionTriangles()
{
	VisionTriangles.Empty();
	for (auto& Polygon : VisionPolygons)
	{
		VisionTriangles.Append(CalculateVisionTrianglesFromPolygon(Polygon));
	}
}


FVisionPolygon USLVisionSubsystem::CalculateVisionPolygonFromSource(USLVisionComponent* SourceComponent) const
{
	FVector Origin = SourceComponent->GetComponentLocation();
	float ViewYaw = SourceComponent->GetComponentRotation().Yaw;
	TArray<FVector> RelativeTargetLocations = SourceComponent->RelativeTargetPoints;
	TArray<FVector2D> PolygonVertices;

	FCollisionQueryParams TraceParams = FCollisionQueryParams();
	FHitResult Hit;

	for (auto& RelativeLocation : RelativeTargetLocations)
	{
		FVector End = Origin + RelativeLocation.RotateAngleAxis(ViewYaw, FVector(0, 0, 1));
		GetWorld()->LineTraceSingleByChannel(Hit, Origin, End, ECC_Visibility, TraceParams);
		PolygonVertices.Add(Hit.bBlockingHit ? FVector2D(Hit.Location) : FVector2D(End));
		//DrawDebugPoint(GetWorld(),End,5.f,FColor(255,255,255,255));
	}
	return FVisionPolygon(FVector2D(Origin), PolygonVertices);
}

TArray<FCanvasUVTri> USLVisionSubsystem::CalculateVisionTrianglesFromPolygon(FVisionPolygon& SourcePolygon) const
{
	TArray<FCanvasUVTri> OutTriangles;
	const int NumTriangles = SourcePolygon.Vertices.Num();
	const FVector2D PawnView2D = FVector2D(LocalPawnViewLocation);
	const FVector2D RTCenterPixels = FVector2D(RenderTargetSize * 0.5, RenderTargetSize * 0.5);
	const FVector2D Origin = SourcePolygon.Origin;
	FCanvasUVTri TempTriangle;

	const FColor Color = FColor(255, 255, 255, 255);
	TempTriangle.V0_Color = Color;
	TempTriangle.V1_Color = Color;
	TempTriangle.V2_Color = Color;

	for (int i = 0; i < NumTriangles; i++)
	{
		int j = (i + 1) % NumTriangles;
		FVector2D Vertex1 = SourcePolygon.Vertices[i];
		FVector2D Vertex2 = SourcePolygon.Vertices[j];
		TempTriangle.V0_Pos = (Origin - PawnView2D) / UUPerPixel + RTCenterPixels;
		TempTriangle.V1_Pos = (Vertex1 - PawnView2D) / UUPerPixel + RTCenterPixels;
		TempTriangle.V2_Pos = (Vertex2 - PawnView2D) / UUPerPixel + RTCenterPixels;

		OutTriangles.Add(TempTriangle);
	}
	return OutTriangles;
}
