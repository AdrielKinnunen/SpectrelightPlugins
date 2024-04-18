// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SLTilemapLib.h"
#include "UObject/Object.h"
#include "SLWave.generated.h"


USTRUCT(BlueprintType)
struct FCell
{
    GENERATED_BODY()
    FCell()
    {
    }

    FCell(const TArray<int32>& NewAllowedPatternIndices, const TArray<int32>& NewNeighborIndices)
    {
        AllowedPatternIndices = NewAllowedPatternIndices;
        NeighborIndices = NewNeighborIndices;
    }

    TArray<int32> AllowedPatternIndices;
    TArray<int32> NeighborIndices;
};


UCLASS()
class SLTILEMAP_API USLWave : public UObject
{
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, Category = "SLTilemap")
    int32 RandomSeed = 133742069;
    UPROPERTY(BlueprintReadWrite, Category = "SLTilemap")
    FRandomStream RandomStream;

    UPROPERTY(BlueprintReadWrite, Category = "SLTilemap")
    FTileMap InputTileMap;
    UPROPERTY(BlueprintReadWrite, Category = "SLTilemap")
    FTileMap OutputTileMap;

    UFUNCTION(BlueprintCallable, Category = "SLTilemap")
    bool Initialize();
    UFUNCTION(BlueprintCallable, Category = "SLTilemap")
    bool Step();
    UFUNCTION(Blueprintcallable, Category = "SLTilemap")
    bool Run();

    UFUNCTION(Blueprintcallable, Category = "SLTilemap")
    TArray<FTileMap> GetPatternsAsTileMaps();

    UFUNCTION(Blueprintcallable, Category = "SLTilemap")
    TArray<float> GetEntropy();
    UPROPERTY(BlueprintReadOnly, Category = "SLTilemap")
    int32 CellObservedLastStep;
    UPROPERTY(BlueprintReadOnly, Category = "SLTilemap")
    TArray<int32> CellsUpdatedLastStep;
    
    
    
private:
    //Wave
    TArray<FTilePattern> Patterns;
    TArray<int32> Counts;
    TArray<float> Weights;
    TArray<float> PlogP;
    bool Failed = false;
    int32 FailedAtIndex = -1;

    //Cells
    TArray<FCell> CellArray;
    TArray<int32> CellXArray;
    TArray<int32> CellYArray;
    TArray<float> CellEntropyArray;
    TArray<float> CellSumWeightsArray;
    TArray<bool> CellIsObservedArray;

    void GeneratePatterns();
    void InitPatternCells();
    bool UpdateCell(const int32 CellIndex);
    void OnFailed();
    void ObserveCell(const int32 CellIndex);
    bool CanPatternFitAtThisLocation(const FTilePattern& Pattern, int32 x, int32 y) const;
    FTilePattern OrCellPatternsTogether(const int32 CellIndex);
};
