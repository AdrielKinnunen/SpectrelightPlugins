// Fill out your copyright notice in the Description page of Project Settings.

#include "SLVisionSubsystem.h"
#include "GeometryAlgorithms/Public/Curve//PolygonIntersectionUtils.h"

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

	Visible.Empty();
	for (auto& VisionPolygon : VisionPolygons)
	{
		UE::Geometry::TGeneralPolygon2<double> Temp;
		auto P = UE::Geometry::TPolygon2(VisionPolygon.Vertices);
		auto GP = UE::Geometry::TGeneralPolygon2(P);
		Visible.Add(GP);
	}
	
	/*
	VisionPolygons.Empty();
	Visible.Empty();
	FVisionPolygon VP;
	VP.Origin = FVector2D(1,1);
	FVector2D Offset = FVector2D(TestCounter * 100,TestCounter * 100);
	
	VP.Vertices.Add(FVector2D(0,0) + Offset);
	VP.Vertices.Add(FVector2D(1000,0) + Offset);
	VP.Vertices.Add(FVector2D(1000,1000) + Offset);
	VP.Vertices.Add(FVector2D(0,1000) + Offset);
	TestCounter++;
	
	auto P = UE::Geometry::TPolygon2(VP.Vertices);
	auto GP = UE::Geometry::TGeneralPolygon2(P);
	Visible.Add(GP);
	*/

	TArray<UE::Geometry::TGeneralPolygon2<double>> Combined = Visible;
	Combined.Append(Explored);
	TArray<UE::Geometry::TGeneralPolygon2<double>> Union;
	UE::Geometry::PolygonsUnion(Combined, Union, false);
	Explored = Union;

	for (auto& GeneralPolygon : Explored)
	{
		GeneralPolygon.Simplify(1, 1);
	}

	
	UE_LOG(LogTemp, Warning, TEXT("-------------------------------Log Start------------------------------------"));
	UE_LOG(LogTemp, Warning, TEXT("Visible has %i polygons"), Visible.Num());
	UE_LOG(LogTemp, Warning, TEXT("Explored has %i polygons"), Explored.Num());
	
	for (auto& VisionPolygon : VisionPolygons)
	{
		UE_LOG(LogTemp, Warning, TEXT("VisionPolygon has %i verts"), VisionPolygon.Vertices.Num());
	}
	
	int32 VisibleVerts = 0;
	for (auto& GeneralPolygon : Visible)
	{
		VisibleVerts += GeneralPolygon.GetOuter().GetVertices().Num();
	}
	UE_LOG(LogTemp, Warning, TEXT("Visible has %i verts total."), VisibleVerts);

	int32 ExploredVerts = 0;
	for (auto& GeneralPolygon : Explored)
	{
		ExploredVerts += GeneralPolygon.GetOuter().GetVertices().Num();
	}
	UE_LOG(LogTemp, Warning, TEXT("Explored has %i verts total."), ExploredVerts);
	UE_LOG(LogTemp, Warning, TEXT("-------------------------------Log End------------------------------------"));


	for (auto& GeneralPolygon : Explored)
	{
		Triangulator.Triangulate(GeneralPolygon, &Indices, &Vertices);
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

void USLVisionSubsystem::DrawDebug()
{
	for (auto& Polygon : Explored)
	{
		/*
		auto Outer = Polygon.GetOuter();
		for (auto Segment : Outer.Segments())
		{
			const UE::Math::TVector2<double> Start2 = Segment.StartPoint();
			const UE::Math::TVector2<double> End2 = Segment.EndPoint();
			const FVector Start = FVector(Start2.X, Start2.Y, 100);
			const FVector End = FVector(End2.X, End2.Y, 100);
			//const FColor Color = FColor::MakeRandomColor();
			const FColor Color = FColor::Blue;
			DrawDebugDirectionalArrow(GetWorld(), Start, End, 500.0f, Color, false, 0.0f, 0, 5.0f);
		}
		*/
	}
	for (auto asdf : Indices)
	{
		const UE::Math::TVector2<double> A2 = Vertices[asdf.A];
		const UE::Math::TVector2<double> B2 = Vertices[asdf.B];
		const UE::Math::TVector2<double> C2 = Vertices[asdf.C];
		
		const FVector A = FVector(A2.X, A2.Y, 100);
		const FVector B = FVector(B2.X, B2.Y, 100);
		const FVector C = FVector(C2.X, C2.Y, 100);
		const FColor Color = FColor::Blue;
		DrawDebugDirectionalArrow(GetWorld(), A, B, 500.0f, Color, false, 0.0f, 0, 5.0f);
		DrawDebugDirectionalArrow(GetWorld(), B, C, 500.0f, Color, false, 0.0f, 0, 5.0f);
		DrawDebugDirectionalArrow(GetWorld(), C, A, 500.0f, Color, false, 0.0f, 0, 5.0f);
	}
}

FVisionPolygon USLVisionSubsystem::CalculateVisionPolygonFromSource(USLVisionComponent* SourceComponent) const
{
    FVector Origin = SourceComponent->GetComponentLocation();
    float ViewYaw = SourceComponent->GetComponentRotation().Yaw;
    TArray<FVector> RelativeTargetLocations = SourceComponent->RelativeTargetPoints;
    TArray<FVector2D> PolygonVertices;

    FCollisionQueryParams TraceParams = FCollisionQueryParams();
	TraceParams.AddIgnoredComponent(SourceComponent->IgnoredComponent);
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

    constexpr FColor Color = FColor(255, 255, 255, 255);
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
