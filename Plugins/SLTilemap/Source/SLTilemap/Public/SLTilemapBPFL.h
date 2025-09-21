// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SLTilemapCore.h"
#include "SLTilemapQuery.h"
#include "SLTilemapPatterns.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SLTilemapBPFL.generated.h"



UCLASS()
class SLTILEMAP_API USLTilemapBPFL : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
public:
	//Tests
    UFUNCTION(BlueprintCallable, Category = "SLTileMap")
    static void RunTests();
	UFUNCTION(BlueprintCallable, Category = "SLTileMap")
	static int32 GetIslandCount(const FTileMap& TileMap, const uint8 TileToMatch);
	UFUNCTION(BlueprintCallable, Category = "SLTileMap")
	static TArray<FTileIndexSet> GetBorderSets(const FTileMap& TileMap, const uint8 TileToMatch);
	UFUNCTION(BlueprintCallable, Category = "SLTileMap")
	static TArray<int32> GetMarchingSquaresCases(const FTileMap& TileMap, const uint8 TileHigh, const uint8 TileLow);
	
    //Coordinates
    UFUNCTION(BlueprintPure, Category = "SLTileMap")
    static int32 CoordsToIndex(const FTileMapCoords Coords, const FTileMapCoords Size);
    UFUNCTION(BlueprintPure, Category = "SLTileMap")
    static FTileMapCoords IndexToCoords(const int32 Index, const FTileMapCoords Size);
	
    //TileMap
    UFUNCTION(BlueprintPure, Category = "SLTileMap")
    static FTileMap CreateTileMap(const FTileMapCoords Size, const uint8 InitialValue);
    UFUNCTION(BlueprintPure, Category = "SLTileMap")
    static bool IsTilemapValid(const FTileMap& TileMap);
	UFUNCTION(BlueprintPure, Category = "SLTileMap")
	static uint8 GetTileAtCoords(const FTileMap& TileMap, const FTileMapCoords Coords);
	UFUNCTION(BlueprintPure, Category = "SLTileMap")
	static uint8 BitwiseXOR(const uint8 A, const uint8 B);
	UFUNCTION(BlueprintPure, Category = "SLTileMap")
	static bool GetBit(const uint8 Byte, const int32 Index);
	UFUNCTION(BlueprintCallable, Category = "SLTileMap")
	static void SetTileAtCoords(UPARAM(ref) FTileMap& TileMap, const uint8 Tile, const FTileMapCoords Coords);
	UFUNCTION(BlueprintPure, Category = "SLTileMap")
	static FVector CoordsToWorldLocation(const FTileMap& TileMap, const FTileMapCoords Coords);
	UFUNCTION(BlueprintPure, Category = "SLTileMap")
	static FTileMapCoords WorldLocationToCoords(const FTileMap& TileMap, const FVector& Location);
	UFUNCTION(BlueprintCallable, Category = "SLTileMap")
	static void Fill(UPARAM(ref) FTileMap& TileMap, const uint8 Tile);
	UFUNCTION(BlueprintCallable, Category = "SLTileMap")
	static void Flood(UPARAM(ref) FTileMap& TileMap, const uint8 Tile, const FTileMapCoords Coords);
	UFUNCTION(BlueprintCallable, Category = "SLTileMap")
	static void SetBorder(UPARAM(ref) FTileMap& TileMap, const uint8 Tile);
	UFUNCTION(BlueprintCallable, Category = "SLTileMap")
    static FTilePatternSet GeneratePatternSet(UPARAM(ref) FTileMap& TileMap, const ESymmetryLevel Symmetry);
	
	//Visualization
    UFUNCTION(BlueprintCallable, Category = "SLTileMap")
    static UTexture2D* TileMapToTexture(UPARAM(ref) FTileMap& TileMap);
    UFUNCTION(BlueprintCallable, Category = "SLTileMap")
    static FColor TileToColor(const uint8 Tile);
};
