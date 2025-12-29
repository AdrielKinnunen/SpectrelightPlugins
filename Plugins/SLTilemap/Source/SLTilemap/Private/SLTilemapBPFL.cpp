#include "SLTileMapBPFL.h"

#include "EdGraphSchema_K2_Actions.h"
#include "SLTileMapColor.h"
#include "SLTileMapOps.h"
#include "SLTileMapPaint.h"
#include "SLTileMapPointFilters.h"
#include "SLTileMapPointGenerators.h"

void USLTileMapBPFL::RunTests()
{
	FTilePattern OriginalPattern;
	for (int32 i = 0; i < 9; i++)
	{
		OriginalPattern.Data[i] = i;
	}
	FTilePattern TestPattern = OriginalPattern;
	check(TestPattern == OriginalPattern);
	TestPattern.ReflectX();
	check(TestPattern != OriginalPattern);
	TestPattern.ReflectX();
	check(TestPattern == OriginalPattern);
	TestPattern.Transpose();
	check(TestPattern != OriginalPattern);
	TestPattern.Transpose();
	check(TestPattern == OriginalPattern);
	TestPattern.Rotate();
	check(TestPattern != OriginalPattern);
	TestPattern.Rotate();
	check(TestPattern != OriginalPattern);
	TestPattern.Rotate();
	check(TestPattern != OriginalPattern);
	TestPattern.Rotate();
	check(TestPattern == OriginalPattern);


	UE_LOG(LogTemp, Warning, TEXT("Tests ran OK"));
}

void USLTileMapBPFL::DebugPrintGraph(const FTileGraph& Graph)
{
	SLTileMap::DebugPrintGraph(Graph);
}

UTexture2D* USLTileMapBPFL::TileMapToTexture(FTileMap& TileMap)
{
	return SLTileMap::TileMap8ToTexture(TileMap);
}

UTexture2D* USLTileMapBPFL::DistanceFieldToTexture(FTileMap& TileMap)
{
	return SLTileMap::DistanceFieldToTexture(TileMap);
}



FTileMap USLTileMapBPFL::CreateTileMap(const FCoords Size, const int32 Color)
{
	return FTileMap(Size, Color);
}

int32 USLTileMapBPFL::GetTileAtCoords(FTileMap& TileMap, const FCoords Coords, const int32 Color)
{
	return SLTileMap::GetTileChecked(TileMap, Coords);
}

void USLTileMapBPFL::SetTileAtCoords(FTileMap& TileMap, const FCoords Coords, const int32 Color)
{
	SLTileMap::SetTileChecked(TileMap, Color, Coords);
}

FCoords USLTileMapBPFL::WorldLocationToCoords(const FTileMap& TileMap, const FVector& Location)
{
	return SLTileMap::WorldLocationToCoords(TileMap, Location);
}

void USLTileMapBPFL::Fill(FTileMap& TileMap, const int32 Tile)
{
	SLTileMap::Fill(TileMap, Tile);
}

void USLTileMapBPFL::Flood(FTileMap& TileMap, const FCoords Coords, const int32 Tile)
{
	if (SLTileMap::IsValidCoordinate(TileMap, Coords))
	{
		const int32 Index = SLTileMap::CoordsToIndex(Coords, TileMap.Size);
		SLTileMap::Flood(TileMap, Tile, Index);
	}
}

void USLTileMapBPFL::SetBorder(FTileMap& TileMap, const int32 Thickness, const int32 Tile)
{
	SLTileMap::SetBorder(TileMap, Thickness, Tile);
}



TArray<FCoords> USLTileMapBPFL::ShapeMask(FTileMap& TileMap, FRandomStream& RandomStream, const FShapeOptions ShapeOptions, const FPaintOptions PaintOptions)
{
	return SLTileMap::ShapeMask(TileMap, RandomStream, ShapeOptions, PaintOptions);
}

bool USLTileMapBPFL::WaveFunctionCollapse(FTileMap& TileMap, FTileMap& Example, FRandomStream& RandomStream, FWFCOptions Options)
{
	Options.RandomSeed = RandomStream.GetUnsignedInt();
	USLWave* Wave = NewObject<USLWave>();
	Wave->InitializeWithOptions(TileMap, Example, Options);
	Wave->Run();
	TileMap = Wave->OutputTileMap;
	return Wave->GetRunState() == ERunState::Succeeded;
}

void USLTileMapBPFL::KeepOnlyBiggestIsland(FTileMap& TileMap, const int32 ColorToReplace, const int32 ColorToReplaceWith)
{
	SLTileMap::KeepOnlyBiggestIsland(TileMap, ColorToReplace, ColorToReplaceWith);
}

FTileMap USLTileMapBPFL::GetDistanceField(const FTileMap& TileMap, const int32 Foreground)
{
	return SLTileMap::GetDistanceField(TileMap, Foreground);
}

TArray<FVector> USLTileMapBPFL::GeneratePointsFromGraph(const FTileGraph& Graph, const int32 MinDegree, const int32 MaxDegree)
{
	return SLTileMap::GeneratePointsFromGraph(Graph, MinDegree, MaxDegree);
}

void USLTileMapBPFL::FilterPointsByDistanceField(TArray<FVector>& Points, const FTileMap& DistanceField, const float DesiredDistance, const float MinDistance, const float MaxDistance)
{
	SLTileMap::FilterPointsByDistanceField(Points, DistanceField, DesiredDistance, MinDistance, MaxDistance);
}

void USLTileMapBPFL::FilterPointsByDistance(TArray<FVector>& Points, const FVector& TargetPoint, const float DesiredDistance, const float MinDistance, const float MaxDistance)
{
	SLTileMap::FilterPointsByDistance(Points, TargetPoint, DesiredDistance, MinDistance, MaxDistance);
}

void USLTileMapBPFL::ShuffleAndLimitPointCount(TArray<FVector>& Points, const FRandomStream& RandomStream, const bool bShuffle, const int32 MinCount, const int32 MaxCount)
{
	SLTileMap::ShuffleAndLimitPointCount(Points, RandomStream, bShuffle, MinCount, MaxCount);
}

FTileGraph USLTileMapBPFL::TileMapToEdgeGraph(FTileMap& TileMap, const int32 ForegroundColor, const FTileGraphEdgeOptions EdgeOptions =  {true, false, true})
{
	return SLTileMap::TileMapToGraph(TileMap, ForegroundColor, EdgeOptions);
}

FTileGraph USLTileMapBPFL::TileMapToSkeletonGraph(FTileMap& TileMap, const int32 ColorToSkeletonize, const FPaintOptions PaintOptions)
{
	return SLTileMap::TileMapToSkeletonGraph(TileMap, ColorToSkeletonize, PaintOptions);
}

void USLTileMapBPFL::SmoothGraph(FTileGraph& Graph, const FTileMap& TileMap, const int32 Iterations)
{
	SLTileMap::SmoothGraph(Graph, TileMap, Iterations);
}

TArray<FTileMapIndexSet> USLTileMapBPFL::GetChains(const FTileGraph& Graph)
{
	return SLTileMap::GetChains(Graph);
}

TArray<FVector> USLTileMapBPFL::GetChainPositions(const FTileGraph& Graph, const FTileMapIndexSet& Chain)
{
	return SLTileMap::GetChainPositions(Graph, Chain);
}


/*
FColor USLTilemapBPFL::TileToColor(const int32 Color)
{
	return SLTileMap::TileToColor(Color);
}

TArray<FTileMapIndexSet> USLTileMapBPFL::GetIslands(const FTileMap& TileMap, const int32 Foreground)
{
	return SLTileMap::GetIslands(TileMap, Foreground);
}

bool USLTilemapBPFL::IsTilemapValid(const FTileMap& TileMap)
{
	return SLTileMap::IsTileMapValid(TileMap);
}

uint8 USLTilemapBPFL::GetTileAtCoords(const FTileMap& TileMap, const FCoords Coords)
{
	return SLTileMap::GetTile(TileMap, Coords);
}

int32 USLTilemapBPFL::CoordsToIndex(const FCoords Coords, const FCoords Size)
{
	return SLTileMap::CoordsToIndex(Coords, Size);
}

FCoords USLTilemapBPFL::IndexToCoords(const int32 Index, const FCoords Size)
{
	return SLTileMap::IndexToCoords(Index, Size);
}

FVector USLTilemapBPFL::CoordsToWorldLocation(const FTileMap& TileMap, const FCoords Coords)
{
	return SLTileMap::CoordsToWorldLocation(TileMap, Coords);
}

FVector USLTilemapBPFL::IndexToWorldLocation(const FTileMap& TileMap, const int32 Index)
{
	const FCoords Coords = IndexToCoords(Index, TileMap.Size);
	return SLTileMap::CoordsToWorldLocation(TileMap, Coords);
}

uint8 USLTilemapBPFL::BitwiseXOR(const uint8 A, const uint8 B)
{
	return A ^ B;
}

bool USLTilemapBPFL::GetBit(const uint8 Byte, const int32 Index)
{
	return (Byte >> Index) & 1;
}

FTilePatternSet USLTilemapBPFL::GeneratePatternSet(FTileMap& TileMap, const ESymmetryLevel Symmetry)
{
	return SLTileMap::GeneratePatternSet(TileMap, Symmetry);
}
*/
