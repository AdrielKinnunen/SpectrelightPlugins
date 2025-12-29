// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SLTilemapCore.h"
#include "SLTilemapPaint.h"
#include "SLTilemapQuery.h"
#include "SLTilemapShape.generated.h"


UENUM(BlueprintType)
enum class ESLTileMapShapeType : uint8
{
	Rectangle,
	Ellipse
};

USTRUCT(BlueprintType)
struct FShapeOptions
{
	GENERATED_BODY()

	FShapeOptions()
	{
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ESLTileMapShapeType ShapeType = ESLTileMapShapeType::Rectangle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 XMin = 10;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 XMax = 20;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 YMin = 10;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 YMax = 20;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 NumRandomPoints = 0;
};


namespace SLTileMap
{
	inline TArray<FCoords> ShapeMask(FTileMap& TileMap, const FRandomStream& RandomStream, const FShapeOptions& ShapeOptions, const FPaintOptions PaintOptions)
	{
		const int32 BoundsMinX = FMath::Max(ShapeOptions.XMin, 0);
		const int32 BoundsMinY = FMath::Max(ShapeOptions.YMin, 0);
		const int32 BoundsMaxX = FMath::Min(ShapeOptions.XMax, TileMap.Size.X - 1);
		const int32 BoundsMaxY = FMath::Min(ShapeOptions.YMax, TileMap.Size.Y - 1);
		const float RadiusX = (ShapeOptions.XMax - ShapeOptions.XMin) / 2.f;
		const float RadiusY = (ShapeOptions.YMax - ShapeOptions.YMin) / 2.f;
		const float CenterX = (ShapeOptions.XMin + ShapeOptions.XMax) / 2.f;
		const float CenterY = (ShapeOptions.YMin + ShapeOptions.YMax) / 2.f;

		TArray<FCoords> RandomPoints;
		for (int32 j = BoundsMinY; j <= BoundsMaxY; j++)
		{
			for (int32 i = BoundsMinX; i <= BoundsMaxX; i++)
			{
				switch (ShapeOptions.ShapeType)
				{
					case ESLTileMapShapeType::Rectangle:
					{
						const FCoords Coords = FCoords(i, j);
						if (PaintOptions.bDrawColor)
						{
							SetTile(TileMap, Coords, PaintOptions.Color);
						}
						RandomPoints.Add(Coords);
						break;
					}
					case ESLTileMapShapeType::Ellipse:
					{
						const int32 dx = i - CenterX;
						const int32 dy = j - CenterY;
						const int32 lhs = dx * dx * RadiusY * RadiusY + dy * dy * RadiusX * RadiusX + 1;
						const int32 rhs = RadiusX * RadiusX * RadiusY * RadiusY;
						if (lhs <= rhs)
						{
							const FCoords Coords = FCoords(i, j);
							if (PaintOptions.bDrawColor)
							{
								SetTile(TileMap, Coords, PaintOptions.Color);
							}
							RandomPoints.Add(Coords);
						}
						break;
					}
				}
			}
		}

		//Shuffle RandomPoints
		const int32 LastIndex = RandomPoints.Num() - 1;
		for (int32 i = 0; i <= LastIndex; ++i)
		{
			const int32 Index = RandomStream.RandRange(i, LastIndex);
			if (i != Index)
			{
				RandomPoints.Swap(i, Index);
			}
		}
		TArray<FCoords> Result;
		const int32 NumPointsToAdd = FMath::Min(ShapeOptions.NumRandomPoints, RandomPoints.Num());
		for (int32 i = 0; i < NumPointsToAdd; i++)
		{
			Result.AddUnique(RandomPoints[i]);
		}
		return Result;
	}
}
