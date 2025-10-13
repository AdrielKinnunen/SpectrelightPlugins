// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SLTilemapCore.h"
#include "SLTilemapQuery.h"
//#include "SLTilemapPaint.generated.h"


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


	inline void SetBorder(FTileMap& TileMap, const uint8 Tile, const int32 Thickness)
	{
		check(Thickness > 0)

		const int32 LastX = TileMap.Size.X-1;
		const int32 LastY = TileMap.Size.Y-1;

		for (int32 b = 0; b<Thickness; b++)
		{
			for (int32 i = 0; i<TileMap.Size.X; i++)
			{
				SetTileChecked(TileMap, Tile, FTileMapCoords(i, b));
				SetTileChecked(TileMap, Tile, FTileMapCoords(i, LastY - b));
			}
			for (int32 i = 0; i<TileMap.Size.Y; i++)
			{
				SetTileChecked(TileMap, Tile, FTileMapCoords(b, i));
				SetTileChecked(TileMap, Tile, FTileMapCoords(LastX - b, i));
			}
		}
	}


	inline void DrawLine(FTileMap& TileMap, const uint8 Tile, const FTileMapCoords Start, const FTileMapCoords End)
	{
		int32 x0 = Start.X;
		int32 y0 = Start.Y;
		int32 x1 = End.X;
		int32 y1 = End.Y;

		int32 dx = abs(x1-x0);
		int32 dy = abs(y1-y0);
		int32 sx = (x0<x1) ? 1 : -1; // step in x
		int32 sy = (y0<y1) ? 1 : -1; // step in y
		int32 err = dx-dy;

		while (true)
		{
			SLTileMap::SetTile(TileMap, Tile, {x0, y0});

			if (x0==x1&&y0==y1) break;

			int32 e2 = 2*err;
			if (e2>-dy)
			{
				err -= dy;
				x0 += sx;
			}
			if (e2<dx)
			{
				err += dx;
				y0 += sy;
			}
		}
	}
}
