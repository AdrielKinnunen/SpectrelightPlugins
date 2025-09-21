// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Containers/RingBuffer.h"
#include "SLTilemapCore.h"
#include "SLTilemapQuery.generated.h"


USTRUCT(BlueprintType)
struct FTileIndexSet
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tilemap")
	TArray<int32> Indices;
};



USTRUCT(BlueprintType)
struct FTileNeighborIndices4
{
	GENERATED_BODY()
	int32 Neighbors[4] = {-1,-1,-1,-1};
};



USTRUCT(BlueprintType)
struct FTileNeighborIndices8
{
	GENERATED_BODY()
	int32 Neighbors[8] = {-1,-1,-1,-1,-1,-1,-1,-1};
};


namespace SLTileMap
{
	inline FTileNeighborIndices4 GetNeighborsAdjacent(const FTileMap& TileMap, const int32 Index)
	{
		const FTileMapCoords Coords = IndexToCoords(Index, TileMap.Size);
		const FTileMapCoords Offsets[4] = {
			FTileMapCoords(Coords.X, Coords.Y - 1),
			FTileMapCoords(Coords.X - 1, Coords.Y),
			FTileMapCoords(Coords.X + 1, Coords.Y),
			FTileMapCoords(Coords.X, Coords.Y + 1),
		};
		FTileNeighborIndices4 Result;
		for (int32 i = 0; i < 4; i++)
		{
			int32 NeighborIndex = -1;
			const FTileMapCoords Offset = Offsets[i];
			if (IsValidCoordinate(TileMap, Offset))
			{
				NeighborIndex = CoordsToIndex(Offset, TileMap.Size);
			}
			Result.Neighbors[i] = NeighborIndex;
		}
		return Result;
	}


	
	inline FTileNeighborIndices4 GetNeighborsDiagonal(const FTileMap& TileMap, const int32 Index)
	{
		const FTileMapCoords Coords = IndexToCoords(Index, TileMap.Size);
		const FTileMapCoords Offsets[4] = {
			FTileMapCoords(Coords.X - 1, Coords.Y - 1),
			FTileMapCoords(Coords.X + 1, Coords.Y - 1),
			FTileMapCoords(Coords.X - 1, Coords.Y + 1),
			FTileMapCoords(Coords.X + 1, Coords.Y + 1),
		};
		FTileNeighborIndices4 Result;
		for (int32 i = 0; i < 4; i++)
		{
			int32 NeighborIndex = -1;
			const FTileMapCoords Offset = Offsets[i];
			if (IsValidCoordinate(TileMap, Offset))
			{
				NeighborIndex = CoordsToIndex(Offset, TileMap.Size);
			}
			Result.Neighbors[i] = NeighborIndex;
		}
		return Result;
	}



	inline FTileNeighborIndices8 GetNeighborsAll(const FTileMap& TileMap, const int32 Index)
	{
		const FTileMapCoords Coords = IndexToCoords(Index, TileMap.Size);
		const FTileMapCoords Offsets[8] = {
			FTileMapCoords(Coords.X - 1, Coords.Y - 1),
			FTileMapCoords(Coords.X, Coords.Y - 1),
			FTileMapCoords(Coords.X + 1, Coords.Y - 1),
			FTileMapCoords(Coords.X - 1, Coords.Y),
			FTileMapCoords(Coords.X + 1, Coords.Y),
			FTileMapCoords(Coords.X - 1, Coords.Y + 1),
			FTileMapCoords(Coords.X, Coords.Y + 1),
			FTileMapCoords(Coords.X + 1, Coords.Y + 1)
		};
		FTileNeighborIndices8 Result;
		for (int32 i = 0; i < 8; i++)
		{
			int32 NeighborIndex = -1;
			const FTileMapCoords Offset = Offsets[i];
			if (IsValidCoordinate(TileMap, Offset))
			{
				NeighborIndex = CoordsToIndex(Offset, TileMap.Size);
			}
			Result.Neighbors[i] = NeighborIndex;
		}
		return Result;
	}



	inline FTileIndexSet GetConnectedIndices4(const FTileMap& TileMap, const int32 Index)
	{    	
		const uint8 Tile = TileMap.Data[Index];
		TSet<int32> Connected;
		TRingBuffer<int32> Buffer;
		Buffer.Add(Index);
		while (!Buffer.IsEmpty())
		{
			int32 ThisIndex = Buffer.PopValue();
			Connected.Add(ThisIndex);
			const FTileNeighborIndices4 NeighborIndices = GetNeighborsAdjacent(TileMap, ThisIndex);
			for (int32 i = 0; i < 4; i++)
			{
				const int32 NeighborIndex = NeighborIndices.Neighbors[i];
				if (NeighborIndex < 0)
				{
					continue;
				}
				const uint8 NeighborTile = TileMap.Data[NeighborIndex];
				if (NeighborTile == Tile && !Connected.Contains(NeighborIndex))
				{
					Buffer.Add(NeighborIndex);
				}
			}
		}
		FTileIndexSet Result;
		Result.Indices = Connected.Array();
		return Result;
	}


	
	inline TArray<FTileIndexSet> GetIslands(const FTileMap& TileMap, const uint8 TileToMatch)
	{
		TArray<FTileIndexSet> Islands;
		
		for (int32 i = 0; i < TileMap.Data.Num(); i++)
		{
			const uint8 Tile = TileMap.Data[i];
			if (Tile != TileToMatch)
			{
				continue;
			}
			bool bNewIsland = true;
			for (auto Island : Islands)
			{
				if (Island.Indices.Contains(i))
				{
					bNewIsland = false;
					break;
				}
			}
			if (!bNewIsland)
			{
				continue;
			}
    		
			//We passed all checks and are creating a new island
			const FTileIndexSet Island = GetConnectedIndices4(TileMap, i);
			Islands.Add(Island);
		}
		return Islands;
	}
	


	inline bool IsBorder(const FTileMap& TileMap, const int32 Index)
	{
		const uint8 ThisTile = TileMap.Data[Index];
		int32 NonMatchingAdjacentCount = 0;
		const FTileNeighborIndices4 Adjacent = GetNeighborsAdjacent(TileMap, Index);
		for (int32 j = 0; j < 4; j++)
		{
			const int32 NeighborIndex = Adjacent.Neighbors[j];
			if (NeighborIndex < 0)
			{
				continue;
			}
			const uint8 NeighborTile = TileMap.Data[NeighborIndex];
			if (NeighborTile != ThisTile)
			{
				NonMatchingAdjacentCount++;
			}
		}
		int32 NonMatchingDiagonalCount = 0;
		const FTileNeighborIndices4 Diagonal = GetNeighborsDiagonal(TileMap, Index);
		for (int32 j = 0; j < 4; j++)
		{
			const int32 NeighborIndex = Diagonal.Neighbors[j];
			if (NeighborIndex < 0)
			{
				continue;
			}
			const uint8 NeighborTile = TileMap.Data[NeighborIndex];
			if (NeighborTile != ThisTile)
			{
				NonMatchingDiagonalCount++;
			}
		}
		const bool bBorderAdjacent = NonMatchingAdjacentCount > 0;
		const bool bBorderDiagonal = NonMatchingDiagonalCount > 1;
		const bool bBorder = bBorderAdjacent || bBorderDiagonal;
		return bBorder;
	}



	inline FTileIndexSet GetAllBorderIndices(const FTileMap& TileMap, const uint8 TileToMatch)
	{
		FTileIndexSet Result;
		for (int32 i = 0; i < TileMap.Data.Num(); i++)
		{
			if (TileMap.Data[i] != TileToMatch)
			{
				continue;
			}
			const bool bIsBorder = IsBorder(TileMap, i);
			if (bIsBorder)
			{
				Result.Indices.Add(i);
			}
		}
		return Result;
	}

	
	inline TArray<FTileIndexSet> GetBorderSets(const FTileMap& TileMap, const uint8 TileToMatch)
	{
		TArray<FTileIndexSet> BorderSets;
		BorderSets.Add(GetAllBorderIndices(TileMap, TileToMatch));
		return BorderSets;
	}	
}
