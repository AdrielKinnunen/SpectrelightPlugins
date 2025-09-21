// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SLTilemapCore.h"
#include "SLTilemapQuery.h"
#include "SLTilemapPaint.generated.h"


USTRUCT(BlueprintType)
struct FTilePaintOperation
{
	GENERATED_BODY()
	//TODO this
};


namespace SLTileMap
{
	inline void Fill(FTileMap& TileMap, const uint8 Tile)
	{
		for (auto& Element : TileMap.Data)
		{
			Element = Tile;
		}
	}

	inline void Flood(FTileMap& TileMap, const uint8 Tile, const int32 Index)
	{
		const FTileIndexSet ConnectedIndices = GetConnectedIndices4(TileMap, Index);
		for (const auto i : ConnectedIndices.Indices)
		{
			TileMap.Data[i] = Tile;
		}
	}


	inline void SetBorder(FTileMap& TileMap, const uint8 Tile)
	{
		const int32 LastX = TileMap.Size.X - 1;
		const int32 LastY = TileMap.Size.Y - 1;
		for (int32 i = 0; i < TileMap.Size.X; i++)
		{
			SetTile(TileMap, Tile, FTileMapCoords(i, 0));
			SetTile(TileMap, Tile, FTileMapCoords(i, LastY));
		}
		for (int32 i = 0; i < TileMap.Size.Y; i++)
		{
			SetTile(TileMap, Tile, FTileMapCoords(0, i));
			SetTile(TileMap, Tile, FTileMapCoords(LastX, i));
		}
	}
}
