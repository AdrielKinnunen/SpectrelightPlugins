// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SLTilemapLib.h"
#include "UObject/Object.h"
#include "SLWave.generated.h"


USTRUCT()
struct FCellAllowedPatterns
{
    GENERATED_BODY()
    //FCellAllowedPatterns()
    //{
    //}

    //FCellAllowedPatterns(const TArray<int32>& NewAllowedPatternIndices)
    //{
     //   AllowedPatterns = NewAllowedPatternIndices;
    //}
    TArray<int32> AllowedPatterns;
};

USTRUCT()
struct FCellsSOA
{
	GENERATED_BODY()
	
	TArray<FCellAllowedPatterns> AllowedPatterns;
	TArray<FTileMapCoords> Coords;
	TArray<float> Entropy;
	TBitArray<> IsObserved;
	TArray<int32> Dirty;
	int32 Num;

	void Reset(int32 NewNum)
	{
		Num = NewNum;
		AllowedPatterns.SetNumZeroed(Num);
		Coords.SetNumZeroed(Num);
		Entropy.SetNumZeroed(Num);
		IsObserved.Init(false, Num);
	}
};

USTRUCT(BlueprintType)
struct FCellNeighbors
{
	GENERATED_BODY()
	int32 Neighbors[8];
};

UENUM(BlueprintType)
enum class ECellSelectionHeuristic : uint8
{
	Entropy,
	Scanline
};

UCLASS(BlueprintType)
class SLTILEMAP_API USLWave : public UObject
{
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, Category = "SLTilemap")
    int32 RandomSeed = 133742069;
    UPROPERTY(BlueprintReadWrite, Category = "SLTilemap")
    FRandomStream RandomStream;
	UPROPERTY(BlueprintReadWrite, Category = "SLTilemap")
	ECellSelectionHeuristic SelectionHeuristic = ECellSelectionHeuristic::Scanline;

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
    int32 LastCellObserved;
    UPROPERTY(BlueprintReadOnly, Category = "SLTilemap")
    TArray<int32> CellsUpdatedLastStep;
    
private:
    //Wave
	FTileMapCoords WaveSize = FTileMapCoords();
    bool Failed = false;
    int32 FailedAtIndex = -1;
	FCellsSOA Cells;

    void InitPatternCells();
    void UpdateCell(int32 CellIndex);
	int32 SelectCellToObserve();
	void UpdateCellEntropy(int32 CellIndex);
	float GetEntropy(TArray<int32> PatternIndices);
    void OnFailed();
	void Propagate();
    void ObserveCell(int32 CellIndex);
	void DirtyUnobservedNeighbors(int32 CellIndex);
	bool CellNeedsUpdate(int32 CellIndex);
};


