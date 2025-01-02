// Fill out your copyright notice in the Description page of Project Settings.

#include "SLWave.h"

bool USLWave::Initialize()
{
    const double StartTime = FPlatformTime::Seconds();
    Failed = false;
    RandomStream.Initialize(RandomSeed);
	
    if (!OutputTileMap.bIsValid())
    {
        return false;
    }

    InitPatternCells();
    
    for (int32 i = 0; i < CellArray.Num(); i++)
    {
        UpdateCell(i);
        if (Failed)
        {
            OnFailed();
            break;
        }
    }
    
    const double EndTime = FPlatformTime::Seconds();
    const double TotalTimems = 1000 * (EndTime - StartTime);
    UE_LOG(LogTemp, Warning, TEXT("Initialization took %f ms"), TotalTimems);
    UE_LOG(LogTemp, Warning, TEXT("There are %d cells"), CellArray.Num());
    UE_LOG(LogTemp, Warning, TEXT("There are %d patterns"), PatternSet.Patterns.Num());

    for (int32 i = 0; i < PatternSet.Patterns.Num(); i++)
    {
        //UE_LOG(LogTemp, Warning, TEXT("Pattern %d has count %d, probability %f, and PlogP %f"), i, Counts[i], Probabilities[i], PlogP[i]);
    }
    return true;
}

bool USLWave::Step()
{
    if (Failed)
    {
        return false;
    }
    
    //Find unobserved PatternCell with lowest entropy
    int32 CellToObserve = -1;
    float LowestEntropy = BIG_NUMBER;
    for (int32 i = 0; i < CellArray.Num(); i++)
    {
        if (!CellIsObservedArray[i] && CellEntropyArray[i] < LowestEntropy)
        {
            LowestEntropy = CellEntropyArray[i];
            CellToObserve = i;
        }
    }
    CellObservedLastStep = CellToObserve;
    CellsUpdatedLastStep.Empty();

    //Check for no unobserved cells found
    if (CellToObserve == -1)
    {
        UE_LOG(LogTemp, Warning, TEXT("No unobserved cell was found"));
        return false;
    }

    //Observe Pattern cell with lowest entropy if found and propagate
    ObserveCell(CellToObserve);

    //Enqueue unobserved neighbors
    TArray<int32> CellsToUpdate;
	int32 UpdateIndex = 0;

	for (int32 i = 0; i < 8; i++)
	{
		const int32 NeighborIndex = CellArray[CellToObserve].Neighbors[i];
		if (NeighborIndex >= 0)
		{
			if (!CellIsObservedArray[NeighborIndex])
			{
				CellsToUpdate.Add(NeighborIndex);
			}
		}
	}
	

    //Update Cells in queue, enqueuing their unobserved neighbors if cell changes
    while (UpdateIndex < CellsToUpdate.Num())
    {
        //Get next cell in queue
        int32 ThisCellIndex = CellsToUpdate[UpdateIndex];
        
        //Update cell, enqueuing neighbors if cell changed
        const bool CellChanged = UpdateCell(ThisCellIndex);
        if (Failed)
        {
            OnFailed();
            break;
        }
        if (CellChanged)
        {
            CellsUpdatedLastStep.Add(ThisCellIndex);
        	
            //Update Map Data with new cell state
            const FTilePattern CombinedPatterns = OrCellPatternsTogether(ThisCellIndex);
            OutputTileMap.WritePattern(CombinedPatterns, CellCoordsArray[ThisCellIndex]);

            //Enqueue unobserved neighbors
        	for (int32 i = 0; i < 8; i++)
        	{
        		const int32 NeighborIndex = CellArray[ThisCellIndex].Neighbors[i];
        		if (NeighborIndex >= 0)
        		{
        			if (!CellIsObservedArray[NeighborIndex])
        			{
        				CellsToUpdate.Add(NeighborIndex);
        			}
        		}
        	}
        }
    	UpdateIndex++;
    }
	UE_LOG(LogTemp, Warning, TEXT("UpdateIndex got to %i"), UpdateIndex);
    return true;
}

bool USLWave::Run()
{
    if (!Initialize())
    {
        return false;
    }
    const double StartTime = FPlatformTime::Seconds();
    while (Step())
    {
    }
    const double EndTime = FPlatformTime::Seconds();
    const double TotalTimems = 1000 * (EndTime - StartTime);
    UE_LOG(LogTemp, Warning, TEXT("Run took %f ms"), TotalTimems);
    return true;
}

TArray<float> USLWave::GetEntropy()
{
    return CellEntropyArray;
}

void USLWave::InitPatternCells()
{
    //Calculate constants
	WaveSize = FTileMapCoords(OutputTileMap.Size.X - 2, OutputTileMap.Size.Y - 2);
    const int32 ArrayNum = WaveSize.X * WaveSize.Y;

    //Clear Arrays
    CellArray.Empty();
	CellCoordsArray.Empty();
    CellEntropyArray.Empty();
    CellSumWeightsArray.Empty();
    CellIsObservedArray.Empty();
    
    //Size Arrays
    CellArray.SetNum(ArrayNum);
	CellCoordsArray.SetNum(ArrayNum);
    CellEntropyArray.SetNum(ArrayNum);
    CellSumWeightsArray.SetNum(ArrayNum);
    //CellIsObservedArray.SetNum(ArrayNum);
	CellIsObservedArray.SetNum(ArrayNum, false);

    //Cache AllowedPatternIndices
    TArray<int32> AllowedPatternIndices;
    for (int32 i = 0; i < PatternSet.Patterns.Num(); i++)
    {
        AllowedPatternIndices.Add(i);
    }

    //Create PatternCells and initialize them    
    for (int32 Y = 0; Y < WaveSize.Y; Y++)
    {
        for (int32 X = 0; X < WaveSize.X; X++)
        {
        	//const int32 CellIndex = inline_XYToIndex(X, Y, WaveWidth);
        	const int32 CellIndex = inline_CoordsToIndex(FTileMapCoords(X, Y), WaveSize);
        	CellCoordsArray[CellIndex] = FTileMapCoords(X, Y);
            CellArray[CellIndex] = FCell(AllowedPatternIndices);

        	constexpr int32 XOffsets[8] = {-1,0,1,-1,1,-1,0,1};
        	constexpr int32 YOffsets[8] = {-1,-1,-1,0,0,1,1,1};
        	for (int32 i = 0; i < 8; i++)
        	{
        		const FTileMapCoords NeighborCoords = FTileMapCoords(X + XOffsets[i], Y + YOffsets[i]);
        		if (NeighborCoords.X > -1 && NeighborCoords.X < WaveSize.X && NeighborCoords.Y > -1 && NeighborCoords.Y < WaveSize.Y)
        		{
        			const int32 NeighborIndex = inline_CoordsToIndex(NeighborCoords, WaveSize);
        			CellArray[CellIndex].Neighbors[i] = NeighborIndex;
        		}
        	}
        }
    }
}

bool USLWave::UpdateCell(const int32 CellIndex)
{
    // Updates Cell state based on underlying OutputTileMap state

    //Cache values
	const FTileMapCoords CellCoords = CellCoordsArray[CellIndex];
    const int32 PreNumPatterns = CellArray[CellIndex].AllowedPatternIndices.Num();

    //PreNumPatterns should never be 0
    //check(PreNumPatterns > 0);
    if (PreNumPatterns < 1)
    {
        int a = 3;
    }
    //Update allowed patterns
    TArray<int32> NewAllowedPatterns;
    for (const auto Index : CellArray[CellIndex].AllowedPatternIndices)
    {
        if (CanPatternFitAtThisLocation(PatternSet.Patterns[Index], CellCoords))
        {
            NewAllowedPatterns.Add(Index);
        }
    }
    CellArray[CellIndex].AllowedPatternIndices = NewAllowedPatterns;
    const int32 PostNumPatterns = CellArray[CellIndex].AllowedPatternIndices.Num();

    //Check if cell changed state
    const bool CellChangedState = (PreNumPatterns != PostNumPatterns);

    //Check for failure
    if (PostNumPatterns < 1)
    {
        Failed = true;
        FailedAtIndex = CellIndex;
        return CellChangedState;
    }

    
    //Check for implicit observation
    if (PostNumPatterns == 1)
    {
        ObserveCell(CellIndex);
        return CellChangedState;
    }

    //Update Entropy
    float SumP = 0;
    float SumPlogP = 0;
    for (const auto& i : CellArray[CellIndex].AllowedPatternIndices)
    {
        SumP += PatternSet.Weights[i];
        SumPlogP += PatternSet.PlogP[i];
    }
    CellSumWeightsArray[CellIndex] = SumP;
    CellEntropyArray[CellIndex] = log2(SumP) - SumPlogP / SumP + RandomStream.FRand() * KINDA_SMALL_NUMBER;
    return CellChangedState;
}

void USLWave::OnFailed()
{
    UE_LOG(LogTemp, Warning, TEXT("Failed at cell %d location %d, %d"), FailedAtIndex, CellCoordsArray[FailedAtIndex].X, CellCoordsArray[FailedAtIndex].Y);
}

bool USLWave::CanPatternFitAtThisLocation(const FTilePattern& Pattern, const FTileMapCoords Coords) const
{
    FTilePattern LocationAsPattern = OutputTileMap.ReadPattern(Coords);
	LocationAsPattern.AndWith(Pattern);
    return LocationAsPattern == Pattern;
}

void USLWave::ObserveCell(const int32 CellIndex)
{
    int32 PatternIndex = CellArray[CellIndex].AllowedPatternIndices[0];
    if (CellArray[CellIndex].AllowedPatternIndices.Num() > 1)
    {
        PatternIndex = -1;
        float SumP = 0;
        for (const auto& i : CellArray[CellIndex].AllowedPatternIndices)
        {
            SumP += PatternSet.Weights[i];
        }
        float RandomFloat = SumP * RandomStream.FRand();
        for (const auto& i : CellArray[CellIndex].AllowedPatternIndices)
        {
            RandomFloat -= PatternSet.Weights[i];
            if (RandomFloat < 0)
            {
                PatternIndex = i;
                break;
            }
        }
    }
    
    //UE_LOG(LogTemp, Warning, TEXT("Random index was %d"), PatternIndex);
    check(PatternIndex > -1);

    const FTilePattern ObservedPattern = PatternSet.Patterns[PatternIndex];
    OutputTileMap.WritePattern(ObservedPattern, CellCoordsArray[CellIndex]);
    CellIsObservedArray[CellIndex] = true;
    CellEntropyArray[CellIndex] = 0.0;
}

//TODO Make Sure this is correct
FTilePattern USLWave::OrCellPatternsTogether(const int32 CellIndex)
{
    FTilePattern Out = FTilePattern();
	for (const auto PatternIndex : CellArray[CellIndex].AllowedPatternIndices)
	{
		Out.OrWith(PatternSet.Patterns[PatternIndex]);
	}
    return Out;
}
