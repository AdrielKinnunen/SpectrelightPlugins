#include "SLTilemapLib.h"

void USLTilemapLib::RunTests()
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
	TestPattern.Reflect();
	check(TestPattern != OriginalPattern);
	TestPattern.Reflect();
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

int32 USLTilemapLib::CoordsToIndex(const FTileMapCoords Coords, const FTileMapCoords Size)
{
	return inline_CoordsToIndex(Coords, Size);
}

FTileMapCoords USLTilemapLib::IndexToCoords(const int32 Index, const FTileMapCoords Size)
{
	return inline_IndexToCoords(Index, Size);
}

FTileMap USLTilemapLib::CreateTileMap(const FTileMapCoords Size, const uint8 InitialValue)
{
	return FTileMap(Size, InitialValue);
}

bool USLTilemapLib::IsTilemapValid(const FTileMap& TileMap)
{
	return TileMap.bIsValid();
}

uint8 USLTilemapLib::GetTileAtCoords(const FTileMap& TileMap, const FTileMapCoords Coords)
{
	return TileMap.GetTile(Coords);
}

void USLTilemapLib::SetTileAtCoords(FTileMap& TileMap, const uint8 Tile, const FTileMapCoords Coords)
{
	TileMap.SetTile(Tile, Coords);
}

FVector USLTilemapLib::CoordsToWorldLocation(const FTileMap& TileMap, const FTileMapCoords Coords)
{
	return TileMap.CoordsToWorldLocation(Coords);
}

FTileMapCoords USLTilemapLib::WorldLocationToCoords(const FTileMap& TileMap, const FVector& Location)
{
	return TileMap.WorldLocationToCoords(Location);
}

void USLTilemapLib::Fill(FTileMap& TileMap, const uint8 Tile)
{
	TileMap.Fill(Tile);
}

void USLTilemapLib::Flood(FTileMap& TileMap, const uint8 Tile, const FTileMapCoords Coords)
{
	TileMap.Flood(Tile, Coords);
}

void USLTilemapLib::SetBorder(FTileMap& TileMap, const uint8 Tile)
{
	TileMap.SetBorder(Tile);
}

FTilePatternSet USLTilemapLib::GeneratePatternSet(FTileMap& TileMap)
{
	return TileMap.GeneratePatternSet();
}

UTexture2D* USLTilemapLib::TileMapToTexture(FTileMap& TileMap)
{
    return TileMap.TileMapToTexture();
}

FColor USLTilemapLib::TileToColor(const uint8 Tile)
{
    return inline_TileToColor(Tile);
}
