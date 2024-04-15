// Fill out your copyright notice in the Description page of Project Settings.

#include "SLWave.h"

bool USLWave::Initialize()
{
    const double StartTime = FPlatformTime::Seconds();

    RandomStream.Initialize(RandomSeed);

    if (!(USLTilemapLib::IsTilemapValid(OutputTileMap) && USLTilemapLib::IsTilemapValid(InputTileMap)))
    {
        return false;
    }

    GeneratePatterns();
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
    UE_LOG(LogTemp, Warning, TEXT("There are %d patterns"), Patterns.Num());

    for (int32 i = 0; i < Patterns.Num(); i++)
    {
        //UE_LOG(LogTemp, Warning, TEXT("Pattern %d has count %d, probability %f, and PlogP %f"), i, Counts[i], Probabilities[i], PlogP[i]);
    }
    return true;
}

bool USLWave::Step()
{
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

    //Check for no unobserved cells found
    if (CellToObserve == -1)
    {
        UE_LOG(LogTemp, Warning, TEXT("No unobserved cell was found"));
        return false;
    }

    //Check for bad cell found during search
    if (CellArray[CellToObserve].AllowedPatternIndices.Num() < 1)
    {
        return false;
    }

    //Observe Pattern cell with lowest entropy if found and propagate
    //UE_LOG(LogTemp, Warning, TEXT("Observing cell at %d,%d and it has entropy %f"), CellXArray[CellToObserve], CellYArray[CellToObserve], CellEntropyArray[CellToObserve]);
    ObserveCell(CellToObserve);

    //Enqueue unobserved neighbors
    TQueue<int32> CellsToUpdate;
    TSet<int32> CellsAlreadyUpdated;
    CellsAlreadyUpdated.Add(CellToObserve);
    for (int32 i = 0; i < CellArray[CellToObserve].NeighborIndices.Num(); i++)
    {
        const int32 NeighborIndex = CellArray[CellToObserve].NeighborIndices[i];
        if (!CellIsObservedArray[NeighborIndex])
        {
            CellsToUpdate.Enqueue(NeighborIndex);
        }
    }

    //Update Cells in queue, enquing their unobserved neighbors if cell changes
    
    while (!CellsToUpdate.IsEmpty())
    {
        //Get next cell in queue
        int32 ThisCellIndex;
        CellsToUpdate.Dequeue(ThisCellIndex);
        CellsAlreadyUpdated.Add(ThisCellIndex);

        //Update cell, enquing neighbors if cell changed
        const bool CellChanged = UpdateCell(ThisCellIndex);
        if (Failed)
        {
            OnFailed();
            break;
        }
        if (CellChanged)
        {
            //Update Map Data with new cell state
            const FTilePattern CombinedPatterns = OrCellPatternsTogether(ThisCellIndex);
            WritePatternToMapData(CombinedPatterns, CellXArray[ThisCellIndex], CellYArray[ThisCellIndex]);
            //Enqueue unobserved neighbors
            for (int32 i = 0; i < CellArray[ThisCellIndex].NeighborIndices.Num(); i++)
            {
                const int32 NeighborIndex = CellArray[ThisCellIndex].NeighborIndices[i];
                if (!CellIsObservedArray[NeighborIndex] && !CellsAlreadyUpdated.Contains(NeighborIndex))
                {
                    CellsToUpdate.Enqueue(NeighborIndex);
                }
            }
        }
    }
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

void USLWave::GeneratePatterns()
{
    Patterns.Empty();
    Counts.Empty();
    TArray<FTilePattern> Variants;
    Variants.Reserve(8);

    for (int32 y = 0; y < InputTileMap.Height - 3 + 1; y++)
    {
        for (int32 x = 0; x < InputTileMap.Width - 3 + 1; x++)
        {
            Variants.Reset();
            FTilePattern Pattern = InputTileMap.ReadPattern(x, y);
            Variants.AddUnique(Pattern);
            Pattern = FTilePattern::Transpose(Pattern);
            Variants.AddUnique(Pattern);
            Pattern = FTilePattern::Reflect(Pattern);
            Variants.AddUnique(Pattern);
            Pattern = FTilePattern::Transpose(Pattern);
            Variants.AddUnique(Pattern);
            Pattern = FTilePattern::Reflect(Pattern);
            Variants.AddUnique(Pattern);
            Pattern = FTilePattern::Transpose(Pattern);
            Variants.AddUnique(Pattern);
            Pattern = FTilePattern::Reflect(Pattern);
            Variants.AddUnique(Pattern);
            Pattern = FTilePattern::Transpose(Pattern);
            Variants.AddUnique(Pattern);
            for (const auto& Variant : Variants)
            {
                RegisterPattern(Variant);
            }
        }
    }

    float SumCounts = 0;
    for (const auto& Count : Counts)
    {
        SumCounts += Count;
    }

    Weights.SetNum(Counts.Num());
    PlogP.SetNum(Counts.Num());
    for (int32 i = 0; i < Counts.Num(); i++)
    {
        const float Probability = Counts[i] / SumCounts;
        Weights[i] = Probability;
        PlogP[i] = Probability * log2(Probability);
        UE_LOG(LogTemp, Warning, TEXT("Pattern %d has count %d, probability %f, and PlogP %f"), i, Counts[i], Probability, PlogP[i]);
    }
}

void USLWave::InitPatternCells()
{
    //Calculate constants
    const int32 WaveWidth = OutputTileMap.Width - 2;
    const int32 WaveHeight = OutputTileMap.Height - 2;
    const int32 ArrayNum = WaveWidth * WaveHeight;

    //Clear Arrays
    CellArray.Empty();
    CellXArray.Empty();
    CellYArray.Empty();
    CellEntropyArray.Empty();
    CellSumWeightsArray.Empty();
    CellIsObservedArray.Empty();
    
    //Size Arrays
    CellArray.SetNum(ArrayNum);
    CellXArray.SetNum(ArrayNum);
    CellYArray.SetNum(ArrayNum);
    CellEntropyArray.SetNum(ArrayNum);
    CellSumWeightsArray.SetNum(ArrayNum);
    CellIsObservedArray.SetNum(ArrayNum);

    //Cache AllowedPatternIndices
    TArray<int32> AllowedPatternIndices;
    for (int32 i = 0; i < Patterns.Num(); i++)
    {
        AllowedPatternIndices.Add(i);
    }

    //Create PatternCells and initialize them
    for (int32 Y = 0; Y < WaveHeight; Y++)
    {
        for (int32 X = 0; X < WaveWidth; X++)
        {
            TArray<int32> Neighbors;
            for (int32 OffsetY = -2; OffsetY <= 2; OffsetY++)
            {
                for (int32 OffsetX = -2; OffsetX <= 2; OffsetX++)
                {
                    const int32 NeighborX = X + OffsetX;
                    const int32 NeighborY = Y + OffsetY;
                    const int32 NeighborIndex = inline_XYToIndex(NeighborX, NeighborY, WaveWidth);

                    if (NeighborX > -1 && NeighborX < WaveWidth && NeighborY > -1 && NeighborY < WaveHeight && !(OffsetX == 0 && OffsetY == 0))
                    {
                        Neighbors.Add(NeighborIndex);
                    }
                }
            }
            const int32 CellIndex = inline_XYToIndex(X, Y, WaveWidth);
            CellXArray[CellIndex] = X;
            CellYArray[CellIndex] = Y;
            CellArray[CellIndex] = FCell(AllowedPatternIndices, Neighbors);
        }
    }
}

void USLWave::RegisterPattern(const FTilePattern Pattern)
{
    const int Index = Patterns.Find(Pattern);
    if (Index > -1)
    {
        Counts[Index]++;
    }
    else
    {
        Patterns.Add(Pattern);
        Counts.Add(1);
    }
}

bool USLWave::UpdateCell(const int32 CellIndex)
{
    // Updates Cell state based on underlying OutputTileMap state

    //Cache values
    const int32 X = CellXArray[CellIndex];
    const int32 Y = CellYArray[CellIndex];
    const int32 PreNumPatterns = CellArray[CellIndex].AllowedPatternIndices.Num();

    //PreNumPatterns should never be 0
    check(PreNumPatterns > 0);

    //Update allowed patterns
    TArray<int32> NewAllowedPatterns;
    for (const auto Index : CellArray[CellIndex].AllowedPatternIndices)
    {
        if (CanPatternFitAtThisLocation(Patterns[Index], X, Y))
        {
            NewAllowedPatterns.Add(Index);
        }
    }
    CellArray[CellIndex].AllowedPatternIndices = NewAllowedPatterns;
    const int32 PostNumPatterns = CellArray[CellIndex].AllowedPatternIndices.Num();

    //Check if cell changed state
    const bool CellChangedState = (PreNumPatterns != PostNumPatterns);

    //Check for failure
    if (PostNumPatterns == 0)
    {
        Failed = true;
        FailedAtIndex = CellIndex;
        return CellChangedState;
    }

    /*
    //Check for implicit observation
    if (PostNumPatterns == 1)
    {
        ObserveCell(CellIndex);
        return CellChangedState;
    }
    */

    //Update Entropy
    float SumP = 0;
    float SumPlogP = 0;
    for (const auto& i : CellArray[CellIndex].AllowedPatternIndices)
    {
        SumP += Weights[i];
        SumPlogP += PlogP[i];
    }
    CellSumWeightsArray[CellIndex] = SumP;
    CellEntropyArray[CellIndex] = log2(SumP) - SumPlogP / SumP + RandomStream.FRand() * KINDA_SMALL_NUMBER;
    return CellChangedState;
}

void USLWave::OnFailed()
{
    UE_LOG(LogTemp, Warning, TEXT("Failed at cell %d, %d"), CellXArray[FailedAtIndex], CellYArray[FailedAtIndex]);
}

void USLWave::WritePatternToMapData(const FTilePattern& Pattern, int32 x, int32 y)
{
    OutputTileMap.WritePattern(Pattern, x, y);
}

bool USLWave::CanPatternFitAtThisLocation(const FTilePattern& Pattern, const int32 x, const int32 y) const
{
    const FTilePattern LocationAsPattern = OutputTileMap.ReadPattern(x, y);
    const FTilePattern TestPattern = FTilePattern::And(Pattern, LocationAsPattern);
    return TestPattern == Pattern;
}

void USLWave::ObserveCell(const int32 CellIndex)
{
    float SumP = 0;
    for (const auto& i : CellArray[CellIndex].AllowedPatternIndices)
    {
        SumP += Weights[i];
    }

    float RandomFloat = SumP * RandomStream.FRand();
    int32 RandomIndex = -1;
    for (const auto& i : CellArray[CellIndex].AllowedPatternIndices)
    {
        RandomFloat -= Weights[i];
        if (RandomFloat < 0)
        {
            RandomIndex = i;
            break;
        }
    }
    UE_LOG(LogTemp, Warning, TEXT("Random index was %d"), RandomIndex);
    check(RandomIndex > -1);

    const FTilePattern ObservedPattern = Patterns[RandomIndex];
    WritePatternToMapData(ObservedPattern, CellXArray[CellIndex], CellYArray[CellIndex]);
    CellIsObservedArray[CellIndex] = true;
    CellEntropyArray[CellIndex] = 0.0;
}

//TODO Make Sure this is correct
FTilePattern USLWave::OrCellPatternsTogether(const int32 CellIndex)
{
    FTilePattern Out = FTilePattern();
    for (int32 Index = 0; Index < CellArray[CellIndex].AllowedPatternIndices.Num(); Index++)
    {
        const int32 PatternIndex = CellArray[CellIndex].AllowedPatternIndices[Index];
        Out = FTilePattern::Or(Out, Patterns[PatternIndex]);
    }
    return Out;
}
