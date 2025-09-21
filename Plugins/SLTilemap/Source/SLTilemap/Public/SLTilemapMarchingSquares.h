// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SLTilemapCore.h"
//#include "SLTilemapMarchingSquares.generated.h"


namespace SLTileMap
{
	inline TArray<int32> GetMarchingSquaresCases(const FTileMap& TileMap, const uint8 TileHigh, const uint8 TileLow)
	{
		TArray<int32> Cases;
		//Cases.Reserve((Size.X - 1) * (Size.Y - 1));
		for (int32 y = 0; y < TileMap.Size.Y - 1; y++)
		{
			for (int32 x = 0; x < TileMap.Size.X - 1; x++)
			{
				int32 CaseIndex = 0;

				// Grab the 2x2 block of samples
				uint8 c0 = GetTile(TileMap, FTileMapCoords(x, y));
				uint8 c3 = GetTile(TileMap, FTileMapCoords(x + 1, y));
				uint8 c1 = GetTile(TileMap, FTileMapCoords(x, y + 1));
				uint8 c2 = GetTile(TileMap, FTileMapCoords(x + 1, y + 1));

				// Build 4-bit mask (inside == 1, outside == 0)
				if (c0 == TileHigh) CaseIndex |= 1; // bit 0 = top-left
				if (c1 == TileHigh) CaseIndex |= 2; // bit 1 = top-right
				if (c2 == TileHigh) CaseIndex |= 4; // bit 2 = bottom-right
				if (c3 == TileHigh) CaseIndex |= 8; // bit 3 = bottom-left

				Cases.Add(CaseIndex);
			}
		}
		return Cases;
	}
}
