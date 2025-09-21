// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SLTilemapCore.generated.h"



USTRUCT(BlueprintType)
struct FTileMapCoords
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tilemap")
	int32 X = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tilemap")
	int32 Y = 0;
};

FORCEINLINE bool operator ==(const FTileMapCoords& A, const FTileMapCoords& B)
{
	return A.X == B.X && A.Y == B.Y;
}










USTRUCT(BlueprintType)
struct FTileMap
{
	GENERATED_BODY()
	FTileMap()
	{
		Size = FTileMapCoords(3, 3);
		Data.Init(0, Size.X * Size.Y);
		Origin = FVector::ZeroVector;
		TileSizeUU = 100;
	}

	FTileMap(const FTileMapCoords NewSize)
	{
		Size = NewSize;
		Data.Init(0, Size.X * Size.Y);
		Origin = FVector::ZeroVector;
		TileSizeUU = 100;
	}

	FTileMap(const FTileMapCoords NewSize, const uint8 InitialValue)
	{
		Size = NewSize;
		Data.Init(InitialValue, Size.X * Size.Y);
		Origin = FVector::ZeroVector;
		TileSizeUU = 100;
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tilemap")
	FTileMapCoords Size;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tilemap")
	TArray<uint8> Data;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tilemap")
	FVector Origin;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tilemap")
	float TileSizeUU;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tilemap")
	FName Name;
};

FORCEINLINE bool operator ==(const FTileMap& A, const FTileMap& B)
{
	return A.Size.X == B.Size.X && A.Data == B.Data;
}


namespace SLTileMap
{

	inline int32 CoordsToIndex(const FTileMapCoords Coords, const FTileMapCoords Size)
	{
		return Coords.Y * Size.X + Coords.X;
	}

	inline FTileMapCoords IndexToCoords(const int32 Index, const FTileMapCoords Size)
	{
		return FTileMapCoords(Index % Size.X, Index / Size.X);
	}
	
	inline bool IsValidCoordinate(const FTileMap& TileMap, const FTileMapCoords Coords)
	{
		const bool bXIsValid = (Coords.X >= 0 && Coords.X < TileMap.Size.X);
		const bool bYIsValid = (Coords.Y >= 0 && Coords.Y < TileMap.Size.Y);
		return bXIsValid && bYIsValid;
	}

	inline uint8 GetTile(const FTileMap& TileMap, const FTileMapCoords Coords)
	{
		return TileMap.Data[CoordsToIndex(Coords, TileMap.Size)];
	}

	inline void SetTile(FTileMap& TileMap, const uint8 Tile, const FTileMapCoords Coords)
	{
		TileMap.Data[CoordsToIndex(Coords, TileMap.Size)] = Tile;
	}

	inline FVector CoordsToWorldLocation(const FTileMap& TileMap, const FTileMapCoords Coords)
	{
		return FVector(Coords.X, Coords.Y, 0) * TileMap.TileSizeUU + TileMap.Origin;
	}

	inline FTileMapCoords WorldLocationToCoords(const FTileMap& TileMap, const FVector& Location)
	{
		FTileMapCoords Result;
		Result.X = (Location - TileMap.Origin).X / TileMap.TileSizeUU;
		Result.Y = (Location - TileMap.Origin).Y / TileMap.TileSizeUU;
		return Result;
	}

	inline FTileMap GetTilemapSection(const FTileMap& TileMap, const FTileMapCoords Coords, const FTileMapCoords SectionSize)
	{
		FTileMap OutSection = FTileMap(SectionSize);
		for (int32 j = 0; j < SectionSize.Y; j++)
		{
			for (int32 i = 0; i < SectionSize.X; i++)
			{
				const uint8 Tile = GetTile(TileMap, FTileMapCoords(Coords.X + i, Coords.Y + j));
				SetTile(OutSection, Tile, FTileMapCoords(i, j));
			}
		}
		return OutSection;
	}

	inline bool IsTileMapValid(const FTileMap& TileMap)
	{
		return TileMap.Data.Num() == TileMap.Size.X * TileMap.Size.Y;
	}
}
