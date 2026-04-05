// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "SLVisionComponent.h"
#include "SLVisionTypes.h"
#include "CompGeom/Delaunay2.h"
#include "Curve/GeneralPolygon2.h"
#include "SLVisionSubsystem.generated.h"


UCLASS()
class SLVISION_API USLVisionSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()
public:
    //properties
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vision")
    TArray<USLVisionComponent*> VisionSources;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vision")
    TArray<FVisionPolygon> VisionPolygons;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vision")
    TArray<FCanvasUVTri> VisionTriangles;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vision")
    FVector LocalPawnViewLocation;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vision")
    float UUPerPixel = 8;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vision")
    float RenderTargetSize = 2048;

    //functions
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;
    void AddVisionSource(USLVisionComponent* SourceToAdd);
    void RemoveVisionSource(USLVisionComponent* SourceToRemove);
    UFUNCTION(Blueprintcallable, Category = "Vision")
    void CalculateVisionPolygons();
    UFUNCTION(Blueprintcallable, Category = "Vision")
    void CalculateVisionTriangles();
	UFUNCTION(Blueprintcallable, Category = "Vision")
	void DrawDebug();
private:
	//properties
	TArray<UE::Geometry::TGeneralPolygon2<double>> Visible;
	TArray<UE::Geometry::TGeneralPolygon2<double>> Explored;
	TArray<UE::Geometry::FIndex3i> Indices;
	TArray<UE::Math::TVector2<double>> Vertices;
	UE::Geometry::FDelaunay2 Triangulator;
	int32 TestCounter = 0;
	//functions
    FVisionPolygon CalculateVisionPolygonFromSource(USLVisionComponent* SourceComponent) const;
    TArray<FCanvasUVTri> CalculateVisionTrianglesFromPolygon(FVisionPolygon& SourcePolygon) const;
};
