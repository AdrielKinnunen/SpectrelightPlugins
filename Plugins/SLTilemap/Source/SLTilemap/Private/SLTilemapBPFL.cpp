#include "SLTilemapBPFL.h"

#include "SLTilemapColor.h"
#include "SLTilemapPaint.h"
#include "SLTilemapSpline.h"

void USLTilemapBPFL::RunTests()
{
	/*
    constexpr int32 Width = 10;
    constexpr int32 Height = 5;
    
    constexpr int32 TLC = 0;
    constexpr int32 TRC = Width - 1;
    constexpr int32 BLC = Width * (Height - 1);
    constexpr int32 BRC = Width * Height - 1;
    constexpr int32 TRCT = Height - 1;
    constexpr int32 BLCT = Height * (Width - 1);
    
    check(inline_XYToIndex(Width - 1, Height - 1, Width) == BRC);
    check(inline_IndexToX(BRC, Width) == Width - 1);
    check(inline_IndexToY(BRC, Width) == Height - 1);
    
    check(inline_IndexToIndexReflected(TLC, Width) == TRC);
    check(inline_IndexToIndexReflected(BLC, Width) == BRC);
    
    check(inline_IndexToIndexTransposed(TLC, Width, Height) == TLC);
    check(inline_IndexToIndexTransposed(TRC, Width, Height) == BLCT);
    check(inline_IndexToIndexTransposed(BLC, Width, Height) == TRCT);
    check(inline_IndexToIndexTransposed(BRC, Width, Height) == BRC);

    check(inline_IndexToIndexRotated(TLC, Width, Height) == TRCT);
    check(inline_IndexToIndexRotated(TRC, Width, Height) == BRC);
    check(inline_IndexToIndexRotated(BRC, Width, Height) == BLCT);
    check(inline_IndexToIndexRotated(BLC, Width, Height) == TLC);

    FTileMap OriginalMap = FTileMap(Width, Height, 0);
    OriginalMap.SetTile(1,0,0);
    OriginalMap.SetTile(2,Width - 1,0);
    OriginalMap.SetTile(3,Width - 1,Height - 1);
    OriginalMap.SetTile(4,0,Height - 1);
    FTileMap TestMap = OriginalMap;
    check(TestMap == OriginalMap);
    
    //TestMap = FTileMap::Reflect(TestMap);
    //TestMap = FTileMap::Reflect(TestMap);
    check(TestMap == OriginalMap);
    
    //TestMap = FTileMap::Transpose(TestMap);
    //TestMap = FTileMap::Transpose(TestMap);
    check(TestMap == OriginalMap);

    //TestMap = FTileMap::Rotate(TestMap);
    //TestMap = FTileMap::Rotate(TestMap);
    //TestMap = FTileMap::Rotate(TestMap);
    //TestMap = FTileMap::Rotate(TestMap);
    check(TestMap == OriginalMap);

    //TestMap = FTileMap::Transpose(TestMap);
    //TestMap = FTileMap::Reflect(TestMap);\
    //TestMap = FTileMap::Rotate(TestMap);
    check(TestMap == OriginalMap);
	*/
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
/*
	//Rotation Tests
	FTileMapCoords N = FTileMapCoords(1, 0);
	FTileMapCoords NE = FTileMapCoords(1, 1);
	FTileMapCoords E = FTileMapCoords(0, 1);
	FTileMapCoords SE = FTileMapCoords(-1, 1);
	FTileMapCoords S = FTileMapCoords(-1, 0);
	FTileMapCoords SW = FTileMapCoords(-1, -1);
	FTileMapCoords W = FTileMapCoords(0, -1);
	FTileMapCoords NW = FTileMapCoords(1, -1);

	FTileMapCoords X = N;

	X = SLTileMap::RotateDirection45(X, 1);
	check(X == NE);
	X = SLTileMap::RotateDirection45(X, 1);
	check(X == E);
	X = SLTileMap::RotateDirection45(X, 1);
	check(X == SE);
	X = SLTileMap::RotateDirection45(X, 1);
	check(X == S);
	X = SLTileMap::RotateDirection45(X, 1);
	check(X == SW);
	X = SLTileMap::RotateDirection45(X, 1);
	check(X == W);
	X = SLTileMap::RotateDirection45(X, 1);
	check(X == NW);
	X = SLTileMap::RotateDirection45(X, 1);
	check(X == N);
	X = SLTileMap::RotateDirection45(X, -12);
	check(X == S);
	X = SLTileMap::RotateDirection45(X, -1);
	check(X == SE);
	X = SLTileMap::RotateDirection45(X, 14);
	check(X == NE);
	X = SLTileMap::RotateDirection45(X, 2);
	check(X == SE);
	*/

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

void USLTilemapBPFL::ApplyPaintOpsStack(FTileMap& TileMap, FTilePaintOpsStack& PaintOpsStack)
{
	SLTileMap::ApplyPaintOpsStack(TileMap, PaintOpsStack);
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
	SLTileMap::SetBorder(TileMap, Tile);
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
