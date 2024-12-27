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

    FCell(const TArray<int32>& NewAllowedPatternIndices)
    {
        AllowedPatternIndices = NewAllowedPatternIndices;
    }

    TArray<int32> AllowedPatternIndices;
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
    FTilePatternSet PatternSet;
    UPROPERTY(BlueprintReadWrite, Category = "SLTilemap")
    FTileMap OutputTileMap;

    UFUNCTION(BlueprintCallable, Category = "SLTilemap")
    bool Initialize();
    UFUNCTION(BlueprintCallable, Category = "SLTilemap")
    bool Step();
    UFUNCTION(Blueprintcallable, Category = "SLTilemap")
    bool Run();

    UFUNCTION(Blueprintcallable, Category = "SLTilemap")
    TArray<float> GetEntropy();
    UPROPERTY(BlueprintReadOnly, Category = "SLTilemap")
    int32 CellObservedLastStep;
    UPROPERTY(BlueprintReadOnly, Category = "SLTilemap")
    TArray<int32> CellsUpdatedLastStep;
    
    
    
private:
    //Wave
	FTileMapCoords WaveSize = FTileMapCoords();
    bool Failed = false;
    int32 FailedAtIndex = -1;

    //Cells
    TArray<FCell> CellArray;
	TArray<FTileMapCoords> CellCoordsArray;
    TArray<float> CellEntropyArray;
    TArray<float> CellSumWeightsArray;
    TArray<bool> CellIsObservedArray;

    void InitPatternCells();
    bool UpdateCell(int32 CellIndex);
    void OnFailed();
    void ObserveCell(int32 CellIndex);
	void EnqueueUnobservedNeighbors(int32 CellIndex, TQueue<int32>& Queue) const;
    bool CanPatternFitAtThisLocation(const FTilePattern& Pattern, const FTileMapCoords Coords) const;
    FTilePattern OrCellPatternsTogether(const int32 CellIndex);
};
