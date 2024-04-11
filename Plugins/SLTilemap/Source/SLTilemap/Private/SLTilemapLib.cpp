#include "SLTilemapLib.h"

int32 USLTilemapLib::XYToIndex(const int32 X, const int32 Y, const int32 SizeX)
{
    return inline_XYToIndex(X, Y, SizeX);
}

FTileMap USLTilemapLib::CreateTileMap(const int32 NewSizeX, const int32 NewSizeY, const uint8 InitialValue)
{
    return FTileMap(NewSizeX, NewSizeY, InitialValue);
}

bool USLTilemapLib::IsTilemapValid(const FTileMap& TileMap)
{
    return TileMap.Data.Num() == TileMap.SizeX * TileMap.SizeY;
}

uint8 USLTilemapLib::GetTileAtXY(const FTileMap& TileMap, const int32 X, const int32 Y)
{
    return TileMap.GetTile(X, Y);
}

FTileMap USLTilemapLib::GetTilemapSection(const FTileMap& Tilemap, const int32 X, const int32 Y, const int32 SizeX, const int32 SizeY)
{
    FTileMap OutSection = FTileMap(SizeX, SizeY);

    for (int32 j = 0; j < SizeY; j++)
    {
        for (int32 i = 0; i < SizeX; i++)
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
    //const int32 TileIndex = TileMapXYToIndex(TileMap, X, Y);
    //TileMap.Data[TileIndex] = Tile;
    TileMap.SetTile(Tile, X, Y);
}

void USLTilemapLib::SetBorder(FTileMap& TileMap, const uint8 Tile)
{
    const int32 LastX = TileMap.SizeX - 1;
    const int32 LastY = TileMap.SizeY - 1;
    for (int32 i = 0; i < TileMap.SizeX; i++)
    {
        TileMap.SetTile(Tile, i, 0);
        TileMap.SetTile(Tile, i, LastY);
    }
    for (int32 i = 0; i < TileMap.SizeY; i++)
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

		//const int32 ThisCellX = ThisCellIndex % TileMap.SizeX;
		//const int32 ThisCellY = ThisCellIndex / TileMap.SizeX;
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
        if (Y < TileMap.SizeY - 1)
        {
            FloodFill(TileMap, X, Y + 1, Tile, TileToReplace);
        }
        if (X < TileMap.SizeX - 1)
        {
            FloodFill(TileMap, X + 1, Y, Tile, TileToReplace);
        }
    }
}

FTileMap USLTilemapLib::MirrorTilemap(const FTileMap& TileMap)
{
    FTileMap OutTileMap = TileMap;
    for (int32 Y = 0; Y < TileMap.SizeY; Y++)
    {
        for (int32 X = 0; X < TileMap.SizeX; X++)
        {
            const uint8 Tile = TileMap.GetTile(TileMap.SizeX - 1 - X, Y);
            OutTileMap.SetTile(Tile, X, Y);
        }
    }
    return OutTileMap;
}

FTileMap USLTilemapLib::RotateTilemap(const FTileMap& TileMap)
{
    FTileMap OutTilemap = TileMap;
    OutTilemap.SizeY = TileMap.SizeX;
    OutTilemap.SizeX = TileMap.SizeY;

    for (int32 Y = 0; Y < OutTilemap.SizeY; Y++)
    {
        for (int32 X = 0; X < OutTilemap.SizeX; X++)
        {
            const uint8 Tile = TileMap.GetTile(Y, TileMap.SizeY - 1 - X);
            OutTilemap.SetTile(Tile, X, Y);
            //SetTileAtXY(OutTilemap, GetTileAtXY(Tilemap, y, Tilemap.SizeY - 1 - x), x, y);
        }
    }
    return OutTilemap;
}

UTexture2D* USLTilemapLib::TileMapToTexture(FTileMap& TileMap)
{
    TArray<FColor> Colors;
    const int32 SizeX = TileMap.SizeX;
    const int32 SizeY = TileMap.SizeY;
    const int32 PixelCount = SizeX * SizeY;
    for (const auto& Tile : TileMap.Data)
    {
        Colors.Add(TileToColor(Tile));
    }

    UTexture2D* Texture = UTexture2D::CreateTransient(SizeX, SizeY, PF_B8G8R8A8, "");
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
