// Fill out your copyright notice in the Description page of Project Settings.

#include "SLTilemapWFC.h"

/*
 Pseudocode for overlapping WFC

Initialize
	For each cell
		If cell needs update
			Mark cell dirty
	Propagate
			
Step
	Select unsolved cell by heuristic
		if no unsolved cell, succeeded
	Collapse cell, mark as solved
	if cell changed
		mark its unsolved neighbors dirty
	Propagate

Run
	Step until failed or succeeded

Propagate
	while any dirty
		pop DirtyCell
		DirtyCell.Update
			if update ends up with 0 patterns
				failed
			if update ends up with 1 pattern
				mark DirtyCell solved
			if DirtyCell changed
				mark unsolved neighbors dirty
	
 */

void USLWave::InitializeWithOptions(FTileMap InTileMap, FTilePatternSet InPatternSet, FWFCOptions InOptions)
{
	InputTileMap = InTileMap;
	PatternSet = InPatternSet;
	Options = InOptions;
	Initialize();
}

void USLWave::Initialize()
{
	//Timekeeping
    const double StartTime = FPlatformTime::Seconds();

	//Initial setup and validity checks
    RunState = ERunState::UnInitialized;
	LastCellObserved = -1;
	AttemptsRemaining = Options.Attempts;
    RandomStream.Initialize(Options.RandomSeed);
	Cells.Dirty.Reset();
	OutputTileMap = InputTileMap;
    
    if (!SLTileMap::IsTileMapValid(OutputTileMap))
    {
	    return;
    }

	RunState = ERunState::Initialized;

    InitPatternCells();

	const double EndTime = FPlatformTime::Seconds();
    const double TotalTimems = 1000 * (EndTime - StartTime);
    if (Options.PrintDebug)
    {
    	UE_LOG(LogTemp, Warning, TEXT("Initialization took %f ms"), TotalTimems);
    	UE_LOG(LogTemp, Warning, TEXT("There are %d cells"), Cells.Num);
    	UE_LOG(LogTemp, Warning, TEXT("There are %d patterns"), PatternSet.Patterns.Num());
    }
}

void USLWave::Step()
{
    if (RunState == ERunState::Failed || RunState == ERunState::Succeeded)
    {
    	return;
    }

	RunState = ERunState::Running;
	Cells.Dirty.Reset();
	
    LastCellObserved = SelectCellToObserve();
	if (LastCellObserved == -1)
    {
        UE_LOG(LogTemp, Warning, TEXT("Success! No unobserved cell was found"));
		RunState = ERunState::Succeeded;
    	return;
    }
	
	ObserveCell(LastCellObserved);
	Propagate();
}

void USLWave::Run()
{
    const double StartTime = FPlatformTime::Seconds();
	RunState = ERunState::Running;
    while (RunState == ERunState::Running)
    {
    	Step();
    }
    const double EndTime = FPlatformTime::Seconds();
    const double TotalTimems = 1000 * (EndTime - StartTime);
    UE_LOG(LogTemp, Warning, TEXT("Run took %f ms"), TotalTimems);
}

TArray<float> USLWave::GetEntropy()
{
    return Cells.Entropy;
}

FCellDebugData USLWave::GetCellDebugData(const FTileMapCoords Coords)
{
	FCellDebugData DebugData;
	int32 CellIndex = Cells.Coords.Find(Coords);
	if (CellIndex > -1)
	{
		DebugData.CellIndex = CellIndex;
		DebugData.NumAllowedPatterns = Cells.AllowedPatterns[CellIndex].AllowedPatterns.Num();
		DebugData.Entropy = Cells.Entropy[CellIndex];
		DebugData.bIsObserved = Cells.IsObserved[CellIndex];
	}
	return DebugData;
}

void USLWave::InitPatternCells()
{
    //Cache AllowedPatternIndices and Initial Entropy
    TArray<int32> AllowedPatternIndices;
    for (int32 i = 0; i < PatternSet.Patterns.Num(); i++)
    {
        AllowedPatternIndices.Add(i);
    }
	const float InitialEntropy = CalculateEntropy(AllowedPatternIndices);
	
    //Create PatternCells and Initialize Them
	WaveSize = FTileMapCoords(OutputTileMap.Size.X - 2, OutputTileMap.Size.Y - 2);
	Cells.Reset(WaveSize.X * WaveSize.Y);
    for (int32 Y = 0; Y < WaveSize.Y; Y++)
    {
        for (int32 X = 0; X < WaveSize.X; X++)
        {
        	//const int32 CellIndex = XYToIndex(X, Y, WaveWidth);
        	const int32 CellIndex = SLTileMap::CoordsToIndex(FTileMapCoords(X, Y), WaveSize);
        	Cells.Coords[CellIndex] = FTileMapCoords(X, Y);
            Cells.AllowedPatterns[CellIndex].AllowedPatterns = AllowedPatternIndices;
        	Cells.OriginalAllowedPatterns[CellIndex].AllowedPatterns = AllowedPatternIndices;
        	Cells.SmallRandomValue[CellIndex] = RandomStream.FRand() * KINDA_SMALL_NUMBER;
        	Cells.Entropy[CellIndex] = InitialEntropy + Cells.SmallRandomValue[CellIndex];
        }
    }

	//Mark dirty all cells that require update and propegate
	for (int32 i = 0; i < Cells.Num; i++)
	{
		if (CellNeedsUpdate(i))
		{
			Cells.Dirty.Add(i);
		}
	}
	Propagate();	
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
	const FTilePattern LocationAsPattern = SLTileMap::ReadPattern(OutputTileMap, Coords);
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
		SLTileMap::WritePattern(OutputTileMap, CombinedPatterns, Coords);
	}
	
	//Check for failure
    if (PostNumPatterns < 1)
    {
		RunState = ERunState::Failed;
    	AttemptsRemaining--;
		if (AttemptsRemaining > 0)
		{
			RunState = ERunState::Running;
			FTileMapCoords ChunkCenter = FTileMapCoords(Coords.X+1, Coords.Y+1);
			RevertChunk(ChunkCenter, 10);
		}
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

	switch (Options.SelectionHeuristic)
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
	Cells.Entropy[CellIndex] = CalculateEntropy(Cells.AllowedPatterns[CellIndex].AllowedPatterns) + Cells.SmallRandomValue[CellIndex];
}

float USLWave::CalculateEntropy(TArray<int32> PatternIndices)
{
	float SumPLogP = 0;
	for (const auto i : PatternIndices)
	{
		const float P = PatternSet.Weights[i];
		SumPLogP += P * log2(P);
	}
	return -SumPLogP;
}

void USLWave::OnFailed()
{
	//UE_LOG(LogTemp, Warning, TEXT("Failed at cell %d location %d, %d"), FailedAtIndex, Cells.Coords[FailedAtIndex].X, Cells.Coords[FailedAtIndex].Y);
	/*
	switch (FailureResponse)
	{
	case EFailureResponse::RevertChunk:
		{
	    		
		}
		break;
	case EFailureResponse::Restart:
		{
			AttemptsRemaining--;
			if (AttemptsRemaining > 0)
			{
				
			}
			OutputTileMap = InputTileMap;
		}
		break;
	}
	*/
}


void USLWave::Propagate()
{
	//Update Cells in queue, enqueuing their unobserved neighbors if cell changes
	while (!Cells.Dirty.IsEmpty())
	{
		int32 CellIndex = Cells.Dirty.PopValue();
		UpdateCell(CellIndex);
		if (RunState == ERunState::Failed)
		{
			OnFailed();
			break;
		}
	}
}



void USLWave::ObserveCell(const int32 CellIndex)
{
	//const FTileMapCoords CellCoords = Cells.Coords[CellIndex];
	//const float CellX = static_cast<float>(CellCoords.X) / (WaveSize.X - 3);
	//const float CellY = static_cast<float>(CellCoords.Y) / (WaveSize.Y - 3);
	//const float CellDistance = FMath::Sqrt((0.5 - CellX) * (0.5 - CellX) + (0.5 - CellY) * (0.5 - CellY));
    int32 PatternIndex = Cells.AllowedPatterns[CellIndex].AllowedPatterns[0];
    if (Cells.AllowedPatterns[CellIndex].AllowedPatterns.Num() > 1)
    {
        PatternIndex = -1;
        float SumP = 0;
        for (const auto& i : Cells.AllowedPatterns[CellIndex].AllowedPatterns)
        {
        	//const float RawWeight = PatternSet.Weights[i];
        	//const float Weight = FMath::Pow(RawWeight, Options.PowerFactor);
        	//const float Spread = Options.DistanceFactor;
        	//const float Distance = PatternSet.AvgDistance[i];
        	//const float DistanceError = FMath::Abs(Distance - CellDistance);
        	//const float WeightScalar = 0.99*FMath::Exp(-100*Spread*Spread*DistanceError*DistanceError) + 0.01;
        	
            SumP += PatternSet.Weights[i];
        }
        float RandomFloat = SumP * RandomStream.FRand();
        for (const auto& i : Cells.AllowedPatterns[CellIndex].AllowedPatterns)
        {
        	//const float RawWeight = PatternSet.Weights[i];
        	//const float Weight = FMath::Pow(RawWeight, Options.PowerFactor);
        	//const float Spread = Options.DistanceFactor;
        	//const float Distance = PatternSet.AvgDistance[i];
        	//const float DistanceError = FMath::Abs(Distance - CellDistance);
        	//const float WeightScalar = 0.99*FMath::Exp(-100*Spread*Spread*DistanceError*DistanceError) + 0.01;
            //RandomFloat -= Weight * WeightScalar;
        	RandomFloat -= PatternSet.Weights[i];
            if (RandomFloat < 0)
            {
                PatternIndex = i;
            	//UE_LOG(LogTemp, Warning, TEXT("Random index was %d, weight was %f"), PatternIndex, PatternSet.Weights[i]);
                break;
            }
        }
    }
    
    //UE_LOG(LogTemp, Warning, TEXT("Random index was %d"), PatternIndex);
    check(PatternIndex > -1);

    const FTilePattern ObservedPattern = PatternSet.Patterns[PatternIndex];
	SLTileMap::WritePattern(OutputTileMap, ObservedPattern, Cells.Coords[CellIndex]);
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
			const int32 NeighborIndex = SLTileMap::CoordsToIndex(NeighborCoords, WaveSize);
			if (!Cells.IsObserved[NeighborIndex])
			{
				Cells.Dirty.Add(NeighborIndex);
			}
		}
	}
}

bool USLWave::CellNeedsUpdate(int32 CellIndex)
{
	const FTilePattern LocationAsPattern = SLTileMap::ReadPattern(OutputTileMap, Cells.Coords[CellIndex]);
	for (const auto Index : Cells.AllowedPatterns[CellIndex].AllowedPatterns)
	{
		if (!LocationAsPattern.Allows(PatternSet.Patterns[Index]))
		{
			return true;
		}
	}
	return false;
}

void USLWave::RevertChunk(FTileMapCoords Coords, int32 Radius)
{
	for (int32 YOffset = -Radius; YOffset < Radius; YOffset++)
	{
		for (int32 XOffset = -Radius; XOffset < Radius; XOffset++)
		{
			const int32 X = Coords.X + XOffset;
			const int32 Y = Coords.Y + YOffset;
			if (X >= 0 && Y >= 0 && X < OutputTileMap.Size.X && Y < OutputTileMap.Size.Y)
			{
				const FTileMapCoords Location = FTileMapCoords(X, Y);
				const uint8 Tile = SLTileMap::GetTile(InputTileMap, Location);
				SLTileMap::SetTile(OutputTileMap, Tile, Location);
			}
		}
	}
	
	LastCellObserved = -1;
	Cells.Dirty.Reset();
	InitPatternCells();
	
	for (int32 i = 0; i < Cells.Num; i++)
	{
		if (CellNeedsUpdate(i))
		{
			Cells.Dirty.Add(i);
		}
	}
}
