#include "SLTilemapLib.h"

void USLTilemapLib::RunTests()
{
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
    
    TestMap = FTileMap::Reflect(TestMap);
    TestMap = FTileMap::Reflect(TestMap);
    check(TestMap == OriginalMap);
    
    TestMap = FTileMap::Transpose(TestMap);
    TestMap = FTileMap::Transpose(TestMap);
    check(TestMap == OriginalMap);

    TestMap = FTileMap::Rotate(TestMap);
    TestMap = FTileMap::Rotate(TestMap);
    TestMap = FTileMap::Rotate(TestMap);
    TestMap = FTileMap::Rotate(TestMap);
    check(TestMap == OriginalMap);

    TestMap = FTileMap::Transpose(TestMap);
    TestMap = FTileMap::Reflect(TestMap);\
    TestMap = FTileMap::Rotate(TestMap);
    check(TestMap == OriginalMap);

    UE_LOG(LogTemp, Warning, TEXT("Tests ran OK"));
}

int32 USLTilemapLib::XYToIndex(const int32 X, const int32 Y, const int32 Width)
{
    return inline_XYToIndex(X, Y, Width);
}

FTileMap USLTilemapLib::CreateTileMap(const int32 NewWidth, const int32 NewHeight, const uint8 InitialValue)
{
    return FTileMap(NewWidth, NewHeight, InitialValue);
}

bool USLTilemapLib::IsTilemapValid(const FTileMap& TileMap)
{
    return TileMap.Data.Num() == TileMap.Width * TileMap.Height;
}

uint8 USLTilemapLib::GetTileAtXY(const FTileMap& TileMap, const int32 X, const int32 Y)
{
    return TileMap.GetTile(X, Y);
}

FTileMap USLTilemapLib::GetTilemapSection(const FTileMap& Tilemap, const int32 X, const int32 Y, const int32 Width, const int32 Height)
{
    FTileMap OutSection = FTileMap(Width, Height);

    for (int32 j = 0; j < Height; j++)
    {
        for (int32 i = 0; i < Width; i++)
        {
            //const int32 temp = GetTileAtXY(Tilemap, x + i, y + j);
            //SetTileAtXY(OutSection, temp, i, j);
            const uint8 Tile = Tilemap.GetTile(X + i, Y + j);
            OutSection.SetTile(Tile, i, j);
        }
    }
    return OutSection;
}

void USLTilemapLib::SetTileAtXY(FTileMap& TileMap, const uint8 Tile, const int32 X, const int32 Y)
{
    TileMap.SetTile(Tile, X, Y);
}

void USLTilemapLib::SetBorder(FTileMap& TileMap, const uint8 Tile)
{
    const int32 LastX = TileMap.Width - 1;
    const int32 LastY = TileMap.Height - 1;
    for (int32 i = 0; i < TileMap.Width; i++)
    {
        TileMap.SetTile(Tile, i, 0);
        TileMap.SetTile(Tile, i, LastY);
    }
    for (int32 i = 0; i < TileMap.Height; i++)
    {
        TileMap.SetTile(Tile, 0, i);
        TileMap.SetTile(Tile, LastX, i);
    }
}

/*
void USLTilemapLib::FloodFill(FTileMap& TileMap, const int32 X, const int32 Y, const uint8 Tile)
{
	
	const int32 StartIndex = TileMap.GetIndex(X, Y);
	const uint8 TileToFill = TileMap.Data[StartIndex];
	
	TArray<int32> IndicesOfTilesToFill;

	//IndicesOfTilesToFill.Add(StartIndex);
	TQueue<int32> FloodQueue;
	FloodQueue.Enqueue(StartIndex);
	while (!FloodQueue.IsEmpty())
	{
		int32 ThisCellIndex;
		FloodQueue.Dequeue(ThisCellIndex);
		IndicesOfTilesToFill.Add(ThisCellIndex);

		//const int32 ThisCellX = ThisCellIndex % TileMap.Width;
		//const int32 ThisCellY = ThisCellIndex / TileMap.Width;
		const int32 ThisCellX = TileMap.GetX(ThisCellIndex);
		const int32 ThisCellY = TileMap.GetY(ThisCellIndex);
		
		int32 NeighborX = ThisCellX - 1; 
		if (NeighborX >= 0)
		{
			if (TileMap.GetTile(NeighborX, ThisCellY) == TileToFill)
			{
				FloodQueue.Enqueue(TileMap.GetIndex(NeighborX, ThisCellY));
			}
		}
		if (TileMap.Data[ThisCellIndex] == TileToFill && IndicesOfTilesToFill.Contains(ThisCellIndex))
		{
			FloodQueue.Enqueue(ThisCellIndex);
		}
	}

	for (const auto& i : IndicesOfTilesToFill)
	{
		TileMap.Data[i] = TileToFill;
	}
}
*/

void USLTilemapLib::FloodFill(FTileMap& TileMap, const int32 X, const int32 Y, const uint8 Tile, const uint8 TileToReplace)
{
    if (TileMap.GetTile(X, Y) == TileToReplace && TileToReplace != Tile)
    {
        TileMap.SetTile(Tile, X, Y);
        if (Y > 0)
        {
            FloodFill(TileMap, X, Y - 1, Tile, TileToReplace);
        }
        if (X > 0)
        {
            FloodFill(TileMap, X - 1, Y, Tile, TileToReplace);
        }
        if (Y < TileMap.Height - 1)
        {
            FloodFill(TileMap, X, Y + 1, Tile, TileToReplace);
        }
        if (X < TileMap.Width - 1)
        {
            FloodFill(TileMap, X + 1, Y, Tile, TileToReplace);
        }
    }
}

FTileMap USLTilemapLib::ReflectTilemap(const FTileMap& TileMap)
{
    return FTileMap::Reflect(TileMap);
}

FTileMap USLTilemapLib::TransposeTilemap(const FTileMap& TileMap)
{
    return FTileMap::Transpose(TileMap);
}

FTileMap USLTilemapLib::RotateTilemap(const FTileMap& TileMap)
{
    return FTileMap::Rotate(TileMap);
}

UTexture2D* USLTilemapLib::TileMapToTexture(FTileMap& TileMap)
{
    TArray<FColor> Colors;
    const int32 Width = TileMap.Width;
    const int32 Height = TileMap.Height;
    const int32 PixelCount = Width * Height;
    for (const auto& Tile : TileMap.Data)
    {
        Colors.Add(TileToColor(Tile));
    }

    UTexture2D* Texture = UTexture2D::CreateTransient(Width, Height, PF_B8G8R8A8, "");
    void* Data = Texture->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE);

    FMemory::Memcpy(Data, Colors.GetData(), PixelCount * 4);
    Texture->GetPlatformData()->Mips[0].BulkData.Unlock();
    Texture->Filter = TF_Nearest;
    Texture->UpdateResource();

    return Texture;
}

FColor USLTilemapLib::TileToColor(const uint8 Tile)
{
    const ETileState Flags = static_cast<ETileState>(Tile);
    const uint8 Void = EnumHasAnyFlags(Flags, ETileState::Void | ETileState::RoofedVoid);
    const uint8 Ground = EnumHasAnyFlags(Flags, ETileState::Ground | ETileState::RoofedGround);
    const uint8 Wall = EnumHasAnyFlags(Flags, ETileState::Wall | ETileState::RoofedWall);
    const uint8 Window = EnumHasAnyFlags(Flags, ETileState::Window | ETileState::RoofedWindow);
    const uint8 Roofed = EnumHasAnyFlags(Flags, ETileState::RoofedVoid | ETileState::RoofedGround | ETileState::RoofedWall | ETileState::RoofedWindow);
    const uint8 IsPow2 = (Tile & (Tile - 1)) == 0;

    const uint8 R = (6 * IsPow2 + 2) * (Wall * 16 + Window * 16);
    const uint8 G = (6 * IsPow2 + 2) * (Window * 16 + Ground * 16);
    const uint8 B = (6 * IsPow2 + 2) * (Void * 16 + Ground * 8 + Window * 8);
    const uint8 A = 255 * Roofed;

    return FColor(R, G, B, A);
}
