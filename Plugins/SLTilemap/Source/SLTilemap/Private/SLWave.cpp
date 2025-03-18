// Fill out your copyright notice in the Description page of Project Settings.

#include "SLWave.h"

/*
 Pseudocode for overlapping WFC

Initialize
	For each Cell
		If cell needs update
			Mark Cell dirty
	while any dirty
		pop DirtyCell
		DirtyCell.Update
			if update ends up with 1 pattern
				mark DirtyCell solved
			if DirtyCell changed
				mark unsolved neighbors dirty
			
Step
	Select Cell by heuristic
	Collapse Cell, mark as solved
	if Cell changed
		mark its unsolved neighbors dirty
	while any dirty
		pop DirtyCell
		DirtyCell.Update
			if update ends up with 1 pattern
				mark DirtyCell solved
			if DirtyCell changed
				mark unsolved neighbors dirty
	
 */

bool USLWave::Initialize()
{
    const double StartTime = FPlatformTime::Seconds();
	
    Failed = false;
	LastCellObserved = -1;
    RandomStream.Initialize(RandomSeed);
	Cells.Dirty.Reset();
    if (!OutputTileMap.bIsValid())
    {
        return false;
    }

    InitPatternCells();
	
    for (int32 i = 0; i < Cells.Num; i++)
    {
        if (CellNeedsUpdate(i))
        {
            Cells.Dirty.Add(i);
        }
    }
	
	Propagate();
	PrintState();
    
    const double EndTime = FPlatformTime::Seconds();
    const double TotalTimems = 1000 * (EndTime - StartTime);
    UE_LOG(LogTemp, Warning, TEXT("Initialization took %f ms"), TotalTimems);
    UE_LOG(LogTemp, Warning, TEXT("There are %d cells"), Cells.Num);
    UE_LOG(LogTemp, Warning, TEXT("There are %d patterns"), PatternSet.Patterns.Num());
	
    return true;
}

bool USLWave::Step()
{
    if (Failed)
    {
	    return false;
    }

	Cells.Dirty.Reset();
	
    LastCellObserved = SelectCellToObserve();
	
	if (LastCellObserved == -1)
    {
        UE_LOG(LogTemp, Warning, TEXT("No unobserved cell was found"));
        return false;
    }
	
	ObserveCell(LastCellObserved);
	Propagate();
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
    return Cells.Entropy;
}

void USLWave::InitPatternCells()
{
    //Calculate constants
	WaveSize = FTileMapCoords(OutputTileMap.Size.X - 2, OutputTileMap.Size.Y - 2);

    //Clear Arrays
	Cells.Reset(WaveSize.X * WaveSize.Y);
	
    //Cache AllowedPatternIndices and initial Entropy
    TArray<int32> AllowedPatternIndices;
    for (int32 i = 0; i < PatternSet.Patterns.Num(); i++)
    {
        AllowedPatternIndices.Add(i);
    }
	const float InitialEntropy = GetEntropy(AllowedPatternIndices);
	
    //Create PatternCells and initialize them    
    for (int32 Y = 0; Y < WaveSize.Y; Y++)
    {
        for (int32 X = 0; X < WaveSize.X; X++)
        {
        	//const int32 CellIndex = inline_XYToIndex(X, Y, WaveWidth);
        	const int32 CellIndex = inline_CoordsToIndex(FTileMapCoords(X, Y), WaveSize);
        	Cells.Coords[CellIndex] = FTileMapCoords(X, Y);
            Cells.AllowedPatterns[CellIndex].AllowedPatterns = AllowedPatternIndices;
        	Cells.Entropy[CellIndex] = InitialEntropy;
        }
    }
}

void USLWave::UpdateCell(const int32 CellIndex)
{
    // Updates Cell state based on underlying OutputTileMap state, then "stamps" Cell state into the OutputTileMap

    //Cache values
	const FTileMapCoords Coords = Cells.Coords[CellIndex];
    const int32 PreNumPatterns = Cells.AllowedPatterns[CellIndex].AllowedPatterns.Num();

    //PreNumPatterns should never be 0
    check(PreNumPatterns > 0);

	//Update allowed patterns
    TArray<int32> NewAllowedPatterns;
	NewAllowedPatterns.Reserve(PreNumPatterns);
	const FTilePattern LocationAsPattern = OutputTileMap.ReadPattern(Coords);
    for (const auto Index : Cells.AllowedPatterns[CellIndex].AllowedPatterns)
    {
	    if (LocationAsPattern.Allows(PatternSet.Patterns[Index]))
	    {
	    	NewAllowedPatterns.Add(Index);
	    }
    }
	NewAllowedPatterns.Shrink();
    Cells.AllowedPatterns[CellIndex].AllowedPatterns = NewAllowedPatterns;
    const int32 PostNumPatterns = Cells.AllowedPatterns[CellIndex].AllowedPatterns.Num();
	
    //Check if cell changed state, update stuff if so
	if (PreNumPatterns != PostNumPatterns)
	{
		UpdateCellEntropy(CellIndex);
		DirtyUnobservedNeighbors(CellIndex);
		FTilePattern CombinedPatterns = FTilePattern();
		for (const auto PatternIndex : Cells.AllowedPatterns[CellIndex].AllowedPatterns)
		{
			CombinedPatterns.OrWith(PatternSet.Patterns[PatternIndex]);
		}
		OutputTileMap.WritePattern(CombinedPatterns, Coords);
	}
	
	//Check for failure
    if (PostNumPatterns < 1)
    {
        Failed = true;
        FailedAtIndex = CellIndex;
    }

	//Check for implicit observation
    if (PostNumPatterns == 1)
    {
    	Cells.IsObserved[CellIndex] = true;
    }
}

int32 USLWave::SelectCellToObserve()
{
	int32 CellToObserve = -1;

	switch (SelectionHeuristic)
    {
		case ECellSelectionHeuristic::Entropy:
			{
				float LowestEntropy = BIG_NUMBER;
				for (int32 i = 0; i < Cells.Num; i++)
				{
					if (!Cells.IsObserved[i] && Cells.Entropy[i] < LowestEntropy)
					{
						LowestEntropy = Cells.Entropy[i];
						CellToObserve = i;
					}
				}
			}
        break;
		case ECellSelectionHeuristic::Scanline:
			{
				for (int32 i = LastCellObserved + 1; i < Cells.Num; i++)
				{
					if (!Cells.IsObserved[i])
					{
						return i;
					}
				}
			}
        break;
    }
	return CellToObserve;
}

void USLWave::UpdateCellEntropy(int32 CellIndex)
{
	Cells.Entropy[CellIndex] = GetEntropy(Cells.AllowedPatterns[CellIndex].AllowedPatterns);
}

float USLWave::GetEntropy(TArray<int32> PatternIndices)
{
	//Update Entropy
	float SumOfWeights = 0;
	for (const auto i : PatternIndices)
	{
		SumOfWeights += PatternSet.Weights[i];
	}
	float InvSumOfWeights = 1 / SumOfWeights;
	float Entropy = 0;
	for (const auto i : PatternIndices)
	{
		float P = PatternSet.Weights[i] * InvSumOfWeights;
		Entropy -= P * log2(P);
	}
	return Entropy;// + RandomStream.FRand() * KINDA_SMALL_NUMBER;
}

void USLWave::OnFailed()
{
    UE_LOG(LogTemp, Warning, TEXT("Failed at cell %d location %d, %d"), FailedAtIndex, Cells.Coords[FailedAtIndex].X, Cells.Coords[FailedAtIndex].Y);
}


void USLWave::Propagate()
{
	//Update Cells in queue, enqueuing their unobserved neighbors if cell changes
	while (!Cells.Dirty.IsEmpty())
	{
		int32 CellIndex = Cells.Dirty.PopValue();
		UpdateCell(CellIndex);
		if (Failed)
		{
			OnFailed();
			break;
		}
	}
}



void USLWave::ObserveCell(const int32 CellIndex)
{
    int32 PatternIndex = Cells.AllowedPatterns[CellIndex].AllowedPatterns[0];
    if (Cells.AllowedPatterns[CellIndex].AllowedPatterns.Num() > 1)
    {
        PatternIndex = -1;
        float SumP = 0;
        for (const auto& i : Cells.AllowedPatterns[CellIndex].AllowedPatterns)
        {
            SumP += PatternSet.Weights[i];
        }
        float RandomFloat = SumP * RandomStream.FRand();
        for (const auto& i : Cells.AllowedPatterns[CellIndex].AllowedPatterns)
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
    OutputTileMap.WritePattern(ObservedPattern, Cells.Coords[CellIndex]);
    Cells.IsObserved[CellIndex] = true;
    Cells.Entropy[CellIndex] = 0.0;
	DirtyUnobservedNeighbors(CellIndex);
}

void USLWave::DirtyUnobservedNeighbors(int32 CellIndex)
{
	FTileMapCoords Coords = Cells.Coords[CellIndex];
	constexpr int32 NeighborXOffsets[8] = {-1,0,1,-1,1,-1,0,1};
	constexpr int32 NeighborYOffsets[8] = {-1,-1,-1,0,0,1,1,1};
	//constexpr int32 NeighborXOffsets[8] = {-1,0,1,-1,1,-1,0,1,-2,-1,0,1,2,-2,2,-2,2,-2,2,-2,-1,0,1,2};
	//constexpr int32 NeighborYOffsets[8] = {-1,-1,-1,0,0,1,1,1,-2,-2,-2,-2,-2,-1,-1,0,0,1,1,2,2,2,2,2};
	for (int32 i = 0; i < 8; i++)
	{
		const FTileMapCoords NeighborCoords = FTileMapCoords(Coords.X + NeighborXOffsets[i], Coords.Y + NeighborYOffsets[i]);
		if (NeighborCoords.X > -1 && NeighborCoords.X < WaveSize.X && NeighborCoords.Y > -1 && NeighborCoords.Y < WaveSize.Y)
		{
			const int32 NeighborIndex = inline_CoordsToIndex(NeighborCoords, WaveSize);
			if (!Cells.IsObserved[NeighborIndex])
			{
				Cells.Dirty.Add(NeighborIndex);
			}
		}
	}
}

bool USLWave::CellNeedsUpdate(int32 CellIndex)
{
	const FTilePattern LocationAsPattern = OutputTileMap.ReadPattern(Cells.Coords[CellIndex]);
	for (const auto Index : Cells.AllowedPatterns[CellIndex].AllowedPatterns)
	{
		if (!LocationAsPattern.Allows(PatternSet.Patterns[Index]))
		{
			return true;
		}
	}
	return false;
}


void USLWave::PrintState()
{
	for (int32 i = 0; i < Cells.Num; i++)
	{
		UE_LOG(LogTemp, Warning, TEXT("Cell %d has entropy %f"), i, Cells.Entropy[i]);	
	}
}
