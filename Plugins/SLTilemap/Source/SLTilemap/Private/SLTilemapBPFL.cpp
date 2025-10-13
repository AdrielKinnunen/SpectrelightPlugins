#include "SLTilemapBPFL.h"

#include "SLTilemapColor.h"
#include "SLTilemapGen.h"
#include "SLTilemapPaint.h"
#include "SLTilemapSpline.h"

void USLTilemapBPFL::RunTests()
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

FTileMap USLTilemapBPFL::CreateTileMap(const FTileMapCoords Size, const uint8 InitialValue)
{
	return FTileMap(Size, InitialValue);
}

bool USLTilemapBPFL::IsTilemapValid(const FTileMap& TileMap)
{
	return SLTileMap::IsTileMapValid(TileMap);
}

uint8 USLTilemapBPFL::GetTileAtCoords(const FTileMap& TileMap, const FTileMapCoords Coords)
{
	return SLTileMap::GetTile(TileMap, Coords);
}

void USLTilemapBPFL::SetTileAtCoords(FTileMap& TileMap, const uint8 Tile, const FTileMapCoords Coords)
{
	SLTileMap::SetTile(TileMap, Tile, Coords);
}

int32 USLTilemapBPFL::CoordsToIndex(const FTileMapCoords Coords, const FTileMapCoords Size)
{
	return SLTileMap::CoordsToIndex(Coords, Size);
}

FTileMapCoords USLTilemapBPFL::IndexToCoords(const int32 Index, const FTileMapCoords Size)
{
	return SLTileMap::IndexToCoords(Index, Size);
}

FVector USLTilemapBPFL::CoordsToWorldLocation(const FTileMap& TileMap, const FTileMapCoords Coords)
{
	return SLTileMap::CoordsToWorldLocation(TileMap, Coords);
}

FVector USLTilemapBPFL::IndexToWorldLocation(const FTileMap& TileMap, const int32 Index)
{
	const FTileMapCoords Coords = IndexToCoords(Index, TileMap.Size);
	return SLTileMap::CoordsToWorldLocation(TileMap, Coords);
}

FTileMapCoords USLTilemapBPFL::WorldLocationToCoords(const FTileMap& TileMap, const FVector& Location)
{
	return SLTileMap::WorldLocationToCoords(TileMap, Location);
}

uint8 USLTilemapBPFL::BitwiseXOR(const uint8 A, const uint8 B)
{
	return A ^ B;
}

bool USLTilemapBPFL::GetBit(const uint8 Byte, const int32 Index)
{
	return (Byte >> Index) & 1;
}

void USLTilemapBPFL::ApplyGenOpsStack(FTileMap& TileMap, FTileGenOpsStack& PaintOpsStack, const int32 Seed)
{
	SLTileMap::ApplyGenOpsStack(TileMap, PaintOpsStack, Seed);
}

void USLTilemapBPFL::Fill(FTileMap& TileMap, const uint8 Tile)
{
	SLTileMap::Fill(TileMap, Tile);
}

void USLTilemapBPFL::Flood(FTileMap& TileMap, const uint8 Tile, const FTileMapCoords Coords)
{
	const int32 Index = SLTileMap::CoordsToIndex(Coords, TileMap.Size);
	SLTileMap::Flood(TileMap, Tile, Index);
}

void USLTilemapBPFL::SetBorder(FTileMap& TileMap, const uint8 Tile)
{
	SLTileMap::SetBorder(TileMap, Tile, 1);
}

TArray<FTileIndexSet> USLTilemapBPFL::GetIslands(const FTileMap& TileMap, const uint8 Foreground)
{
	return SLTileMap::GetIslands(TileMap, Foreground);
}

TArray<FTileIndexSet> USLTilemapBPFL::GetBorderSets(const FTileMap& TileMap, const uint8 Foreground)
{
	return SLTileMap::GetBorderSets(TileMap, Foreground);
}

void USLTilemapBPFL::SmoothPoints(TArray<FVector>& Points, const float Alpha)
{
	SLTileMap::SmoothPoints(Points, Alpha);
}

FTilePatternSet USLTilemapBPFL::GeneratePatternSet(FTileMap& TileMap, const ESymmetryLevel Symmetry)
{
	return SLTileMap::GeneratePatternSet(TileMap, Symmetry);
}


UTexture2D* USLTilemapBPFL::TileMapToTexture(FTileMap& TileMap)
{
	return SLTileMap::TileMapToTexture(TileMap);
}

FColor USLTilemapBPFL::TileToColor(const uint8 Tile)
{
	return SLTileMap::TileToColor(Tile);
}
