// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SLTilemapCore.h"
#include "SLTilemapQuery.h"
#include "StructUtils/InstancedStruct.h"
#include "SLTilemapPaint.generated.h"


USTRUCT(BlueprintType)
struct FTilePaintOpBlackboard
{
	GENERATED_BODY()
	TMap<FName, int32> NameToIndex;
};


USTRUCT(BlueprintType)
struct FTilePaintOp
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 Color;

	virtual ~FTilePaintOp()
	{
	}

	virtual void Execute(FTileMap& TileMap, FTilePaintOpBlackboard& Blackboard)
	{
	};
};


USTRUCT(BlueprintType)
struct FTilePaintOpLine : public FTilePaintOp
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Length;


	virtual void Execute(FTileMap& TileMap, FTilePaintOpBlackboard& Blackboard) override
	{

		int32 StartIndex = FMath::RandRange(0, TileMap.Data.Num() - 1);
		int32 EndIndex = FMath::RandRange(0, TileMap.Data.Num() - 1);
		FTileMapCoords Start = SLTileMap::IndexToCoords(StartIndex, TileMap.Size);
		FTileMapCoords End = SLTileMap::IndexToCoords(EndIndex, TileMap.Size);
		
		int32 x0 = Start.X;
		int32 y0 = Start.Y;
		int32 x1 = End.X;
		int32 y1 = End.Y;
		
		int32 dx = abs(x1 - x0);
		int32 dy = abs(y1 - y0);
		int32 sx = (x0 < x1) ? 1 : -1; // step in x
		int32 sy = (y0 < y1) ? 1 : -1; // step in y
		int32 err = dx - dy;

		while (true)
		{
			SLTileMap::SetTile(TileMap, Color, {x0, y0});

			if (x0 == x1 && y0 == y1)
				break;

			int32 e2 = 2 * err;
			if (e2 > -dy)
			{
				err -= dy;
				x0 += sx;
			}
			if (e2 < dx)
			{
				err += dx;
				y0 += sy;
			}
		}
	};
};

USTRUCT(BlueprintType)
struct FTilePaintOpCircle : public FTilePaintOp
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Radius;

	virtual void Execute(FTileMap& TileMap, FTilePaintOpBlackboard& Blackboard) override
	{
		UE_LOG(LogTemp, Warning, TEXT("Circle radius is %d"), Radius);
	};
};


USTRUCT(BlueprintType)
struct FTilePaintOpsStack
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, meta=(ExcludeBaseStruct))
	TArray<TInstancedStruct<FTilePaintOp>> PaintOps;
	UPROPERTY(EditAnywhere)
	FTilePaintOpBlackboard Blackboard;
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
		const int32 LastX = TileMap.Size.X-1;
		const int32 LastY = TileMap.Size.Y-1;
		for (int32 i = 0; i<TileMap.Size.X; i++)
		{
			SetTile(TileMap, Tile, FTileMapCoords(i, 0));
			SetTile(TileMap, Tile, FTileMapCoords(i, LastY));
		}
		for (int32 i = 0; i<TileMap.Size.Y; i++)
		{
			SetTile(TileMap, Tile, FTileMapCoords(0, i));
			SetTile(TileMap, Tile, FTileMapCoords(LastX, i));
		}
	}

	inline void ApplyPaintOpsStack(FTileMap& TileMap, FTilePaintOpsStack& PaintOpsStack)
	{
		for (auto Op : PaintOpsStack.PaintOps)
		{
			if (FTilePaintOp* P = Op.GetMutablePtr<>())
			{
				P->Execute(TileMap, PaintOpsStack.Blackboard);
			}
		}
	}
}
