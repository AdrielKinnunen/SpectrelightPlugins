// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
//#include "SLTilemapBPFL.h"
#include "SLTilemapCore.h"
#include "SLTilemapPatterns.h"
#include "Containers/RingBuffer.h"
#include "UObject/Object.h"
#include "SLTilemapWFC.generated.h"


USTRUCT()
struct FCellAllowedPatterns
{
    GENERATED_BODY()
    TArray<int32> AllowedPatterns;
};

USTRUCT()
struct FCellsSOA
{
	GENERATED_BODY()

	TArray<FCellAllowedPatterns> OriginalAllowedPatterns;
	TArray<FCellAllowedPatterns> AllowedPatterns;
	TArray<FCoords> Coords;
	TArray<float> Entropy;
	TArray<float> SmallRandomValue;
	TBitArray<> IsObserved;
	TRingBuffer<int32> Dirty;
	int32 Num;

	void Reset(int32 NewNum)
	{
		Num = NewNum;
		AllowedPatterns.SetNumZeroed(Num);
		OriginalAllowedPatterns.SetNumZeroed(Num);
		Coords.SetNumZeroed(Num);
		Entropy.SetNumZeroed(Num);
		SmallRandomValue.SetNumZeroed(Num);
		IsObserved.Init(false, Num);
		Dirty.Empty();
	}
};

USTRUCT(BlueprintType)
struct FCellDebugData
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tilemap")
	int32 CellIndex = -1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tilemap")
	int32 NumAllowedPatterns = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tilemap")
	float Entropy = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tilemap")
	bool bIsObserved = false;
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

UENUM(BlueprintType)
enum class EFailureResponse : uint8
{
	Restart,
	RevertChunk
};

UENUM(BlueprintType)
enum class ERunState : uint8
{
	UnInitialized,
	Initialized,
	Running,
	Failed,
	Succeeded
};

USTRUCT(BlueprintType)
struct FWFCOptions
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLTilemap")
	int32 RandomSeed = 133742069;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLTilemap")
	ESymmetryLevel SymmetryLevel = ESymmetryLevel::All;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLTilemap")
	ECellSelectionHeuristic SelectionHeuristic = ECellSelectionHeuristic::Entropy;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLTilemap")
	EFailureResponse FailureResponse = EFailureResponse::RevertChunk;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLTilemap")
	int32 Attempts = 10;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLTilemap")
	bool PrintDebug = true;	
};

UCLASS(BlueprintType)
class SLTILEMAP_API USLWave : public UObject
{
    GENERATED_BODY()
public:
	//State
	UPROPERTY(BlueprintReadWrite, Category = "SLTilemap")
	FTileMap OutputTileMap;
	

	//API
	UFUNCTION(BlueprintCallable, Category = "SLTilemap")
	void InitializeWithOptions(FTileMap InTileMap, FTileMap Example, FWFCOptions InOptions);
	UFUNCTION(BlueprintCallable, Category = "SLTilemap")
    void Step();
    UFUNCTION(Blueprintcallable, Category = "SLTilemap")
    void Run();
	UFUNCTION(Blueprintcallable, Category = "SLTilemap")
	ERunState GetRunState();

	//Debug
    UFUNCTION(Blueprintcallable, Category = "SLTilemap")
    TArray<float> GetEntropy();
	UFUNCTION(Blueprintcallable, Category = "SLTilemap")
	FCellDebugData GetCellDebugData(FCoords Coords);
    UPROPERTY(BlueprintReadOnly, Category = "SLTilemap")
    int32 LastCellObserved;
    UPROPERTY(BlueprintReadOnly, Category = "SLTilemap")
    TArray<int32> CellsUpdatedLastStep;
    
private:
	//State
	FTileMap InputTileMap;
	FTilePatternSet PatternSet;
	FWFCOptions Options;
	FCoords WaveSize;
	int32 AttemptsRemaining;
	ERunState RunState = ERunState::UnInitialized;
    //bool Failed = false;
	//bool Succeeded = false;
    //int32 FailedAtIndex = -1;
	FCellsSOA Cells;
	FRandomStream RandomStream;

	//Functions
	void Initialize();
    void InitPatternCells();
    void UpdateCell(int32 CellIndex);
	int32 SelectCellToObserve();
	void UpdateCellEntropy(int32 CellIndex);
	float CalculateEntropy(TArray<int32> PatternIndices);
    //void OnFailed();
	void Propagate();
    void ObserveCell(int32 CellIndex);
	void DirtyUnobservedNeighbors(int32 CellIndex);
	bool CellNeedsUpdate(int32 CellIndex);
	void RevertChunk(FCoords Coords, int32 Radius);
};


