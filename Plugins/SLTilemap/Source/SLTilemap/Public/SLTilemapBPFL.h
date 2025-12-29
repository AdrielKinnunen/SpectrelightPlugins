// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SLTileMapCore.h"
#include "SLTileMapGraph.h"
#include "SLTileMapQuery.h"
#include "SLTileMapShape.h"
#include "SLTileMapWFC.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SLTileMapBPFL.generated.h"



UCLASS()
class SLTILEMAP_API USLTileMapBPFL : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	//Debug
	UFUNCTION(BlueprintCallable, Category = "SLTileMap | Debug")
	static void RunTests();

	UFUNCTION(BlueprintCallable, Category = "SLTileMap | Debug")
	static void DebugPrintGraph(const FTileGraph& Graph);

	UFUNCTION(BlueprintCallable, Category = "SLTileMap | Debug")
	static UTexture2D* TileMapToTexture(UPARAM(ref) FTileMap& TileMap);

	UFUNCTION(BlueprintCallable, Category = "SLTileMap | Debug")
	static UTexture2D* DistanceFieldToTexture(UPARAM(ref) FTileMap& DistanceField);


	
	//Basic
	UFUNCTION(BlueprintPure, Category = "SLTileMap | Paint", meta=(ReturnDisplayName="TileMap"))
	static FTileMap CreateTileMap(const FCoords Size, UPARAM(meta = (Bitmask, BitmaskEnum = "/Script/SLTilemap.ETileMapColor")) const int32 Color);

	UFUNCTION(BlueprintCallable, Category = "SLTileMap | Paint")
	static int32 GetTileAtCoords(UPARAM(ref) FTileMap& TileMap, const FCoords Coords,  UPARAM(meta = (Bitmask, BitmaskEnum = "/Script/SLTilemap.ETileMapColor")) const int32 Color);
	
	UFUNCTION(BlueprintCallable, Category = "SLTileMap | Paint")
	static void SetTileAtCoords(UPARAM(ref) FTileMap& TileMap, const FCoords Coords,  UPARAM(meta = (Bitmask, BitmaskEnum = "/Script/SLTilemap.ETileMapColor")) const int32 Color);
	
	UFUNCTION(BlueprintPure, Category = "SLTileMap | Paint")
	static FCoords WorldLocationToCoords(const FTileMap& TileMap, const FVector& Location);

	UFUNCTION(BlueprintCallable, Category = "SLTileMap | Paint")
	static void Fill(UPARAM(ref) FTileMap& TileMap, UPARAM(meta = (Bitmask, BitmaskEnum = "/Script/SLTilemap.ETileMapColor")) const int32 Tile);

	UFUNCTION(BlueprintCallable, Category = "SLTileMap | Paint")
	static void Flood(UPARAM(ref) FTileMap& TileMap, const FCoords Coords, UPARAM(meta = (Bitmask, BitmaskEnum = "/Script/SLTilemap.ETileMapColor")) const int32 Tile);

	UFUNCTION(BlueprintCallable, Category = "SLTileMap | Paint")
	static void SetBorder(UPARAM(ref) FTileMap& TileMap, const int32 Thickness, UPARAM(meta = (Bitmask, BitmaskEnum = "/Script/SLTilemap.ETileMapColor")) const int32 Tile);



	//Operations
	UFUNCTION(BlueprintCallable, Category = "SLTileMap | Operations", meta=(ReturnDisplayName="Random Points"))
	static TArray<FCoords> ShapeMask(UPARAM(ref) FTileMap& TileMap, UPARAM(ref) FRandomStream& RandomStream, const FShapeOptions ShapeOptions, const FPaintOptions PaintOptions);

	UFUNCTION(BlueprintCallable, Category = "SLTileMap | Operations", meta=(ReturnDisplayName="Success"))
	static bool WaveFunctionCollapse(UPARAM(ref) FTileMap& TileMap, UPARAM(ref) FTileMap& Example, UPARAM(ref) FRandomStream& RandomStream, FWFCOptions Options);

	UFUNCTION(BlueprintCallable, Category = "SLTileMap | Operations")
	static void KeepOnlyBiggestIsland(UPARAM(ref) FTileMap& TileMap, UPARAM(meta = (Bitmask, BitmaskEnum = "/Script/SLTilemap.ETileMapColor")) const int32 ColorToReplace, UPARAM(meta = (Bitmask, BitmaskEnum = "/Script/SLTilemap.ETileMapColor")) const int32 ColorToReplaceWith);
	
	UFUNCTION(BlueprintCallable, Category = "SLTileMap | Operations", meta=(ReturnDisplayName="Distance Field"))
	static FTileMap GetDistanceField(const FTileMap& TileMap, UPARAM(meta = (Bitmask, BitmaskEnum = "/Script/SLTilemap.ETileMapColor")) const int32 Foreground);


	
	//POI Point Generators
	UFUNCTION(BlueprintCallable, Category = "SLTileMap | POI | Generators", meta=(ReturnDisplayName="Points"))
	static TArray<FVector> GeneratePointsFromGraph(const FTileGraph& Graph, const int32 MinDegree, const int32 MaxDegree);

	//POI Point Filters
	UFUNCTION(BlueprintCallable, Category = "SLTileMap | POI | Filters", meta=(ReturnDisplayName="Points"))
	static void FilterPointsByDistanceField(UPARAM(ref) TArray<FVector>& Points, const FTileMap& DistanceField, const float DesiredDistance, const float MinDistance, const float MaxDistance);

	UFUNCTION(BlueprintCallable, Category = "SLTileMap | POI | Filters", meta=(ReturnDisplayName="Points"))
	void FilterPointsByDistance(UPARAM(ref) TArray<FVector>& Points, const FVector& TargetPoint, const float DesiredDistance, const float MinDistance, const float MaxDistance);

	UFUNCTION(BlueprintCallable, Category = "SLTileMap | POI | Filters", meta=(ReturnDisplayName="Points"))
	static void ShuffleAndLimitPointCount(UPARAM(ref) TArray<FVector>& Points, const FRandomStream& RandomStream, const bool bShuffle, const int32 MinCount, const int32 MaxCount);


	//Graph
	UFUNCTION(BlueprintCallable, Category = "SLTileMap | Operations", meta=(ReturnDisplayName="EdgeGraph"))
	static FTileGraph TileMapToEdgeGraph(UPARAM(ref) FTileMap& TileMap, UPARAM(meta = (Bitmask, BitmaskEnum = "/Script/SLTilemap.ETileMapColor")) const int32 ForegroundColor, const FTileGraphEdgeOptions EdgeOptions);
	
	UFUNCTION(BlueprintCallable, Category = "SLTileMap | Operations", meta=(ReturnDisplayName="Skeleton Graph"))
	static FTileGraph TileMapToSkeletonGraph(UPARAM(ref) FTileMap& TileMap, UPARAM(meta = (Bitmask, BitmaskEnum = "/Script/SLTilemap.ETileMapColor")) const int32 ColorToSkeletonize, const FPaintOptions PaintOptions);
	
	UFUNCTION(BlueprintCallable, Category = "SLTileMap | Graph")
	static void SmoothGraph(UPARAM(ref) FTileGraph& Graph, const FTileMap& TileMap, const int32 Iterations);

	UFUNCTION(BlueprintCallable, Category = "SLTileMap | Graph", meta=(ReturnDisplayName="Chains"))
	static TArray<FTileMapIndexSet> GetChains(const FTileGraph& Graph);
	
	UFUNCTION(BlueprintCallable, Category = "SLTileMap | Graph", meta=(ReturnDisplayName="Positions"))
	static TArray<FVector> GetChainPositions(const FTileGraph& Graph, const FTileMapIndexSet& Chain);
	
	
};



/*
 
UFUNCTION(BlueprintCallable, Category = "SLTileMap", meta=(ReturnDisplayName="Islands"))
static TArray<FTileMapIndexSet> GetIslands(const FTileMap& TileMap, UPARAM(meta = (Bitmask, BitmaskEnum = "/Script/SLTilemap.ETileMapColor")) const int32 Foreground);

UFUNCTION(BlueprintPure, Category = "SLTileMap")
static bool IsTilemapValid(const FTileMap& TileMap);

UFUNCTION(BlueprintPure, Category = "SLTileMap")
static uint8 GetTileAtCoords(const FTileMap& TileMap, const FCoords Coords);

UFUNCTION(BlueprintPure, Category = "SLTileMap")
static int32 CoordsToIndex(const FCoords Coords, const FCoords Size);

UFUNCTION(BlueprintPure, Category = "SLTileMap")
static FCoords IndexToCoords(const int32 Index, const FCoords Size);

UFUNCTION(BlueprintPure, Category = "SLTileMap")
static FVector CoordsToWorldLocation(const FTileMap& TileMap, const FCoords Coords);

UFUNCTION(BlueprintPure, Category = "SLTileMap")
static FVector IndexToWorldLocation(const FTileMap& TileMap, const int32 Index);

UFUNCTION(BlueprintPure, Category = "SLTileMap")
static uint8 BitwiseXOR(const uint8 A, const uint8 B);

UFUNCTION(BlueprintPure, Category = "SLTileMap")
static bool GetBit(const uint8 Byte, const int32 Index);

UFUNCTION(BlueprintCallable, Category = "SLTileMap")
static FTilePatternSet GeneratePatternSet(UPARAM(ref) FTileMap& TileMap, const ESymmetryLevel Symmetry);

UFUNCTION(BlueprintCallable, Category = "SLTileMap")
static FColor TileToColor(UPARAM(meta = (Bitmask, BitmaskEnum = "/Script/SLTilemap.ETileMapColor")) const int32 Color);

*/