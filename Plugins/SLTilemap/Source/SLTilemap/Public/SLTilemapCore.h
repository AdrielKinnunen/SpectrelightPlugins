// Copyright Spectrelight Studios, LLC

#pragma once

#include "CoreMinimal.h"
#include "SLTilemapCore.generated.h"


USTRUCT(BlueprintType)
struct FCoords
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tilemap")
	int32 X = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tilemap")
	int32 Y = 0;
};

FORCEINLINE bool operator ==(const FCoords& A, const FCoords& B)
{
	return A.X == B.X && A.Y == B.Y;
}

inline uint32 GetTypeHash(const FCoords& In)
{
	return HashCombine(GetTypeHash(In.X), GetTypeHash(In.Y));
}

FORCEINLINE FCoords operator +(const FCoords& A, const FCoords& B)
{
	return {A.X + B.X, A.Y + B.Y};
}


USTRUCT(BlueprintType)
struct FTileMap
{
	GENERATED_BODY()
	FTileMap()
	{
		Size = FCoords(3, 3);
		Data.Init(0, Size.X * Size.Y);
		Origin = FVector::ZeroVector;
		TileSizeUU = 100;
	}

	FTileMap(const FCoords NewSize)
	{
		Size = NewSize;
		Data.Init(0, Size.X * Size.Y);
		Origin = FVector::ZeroVector;
		TileSizeUU = 100;
	}

	FTileMap(const FCoords NewSize, const uint8 InitialValue)
	{
		Size = NewSize;
		Data.Init(InitialValue, Size.X * Size.Y);
		Origin = FVector::ZeroVector;
		TileSizeUU = 100;
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tilemap")
	FCoords Size;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Tilemap")
	TArray<uint8> Data;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tilemap")
	FVector Origin;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tilemap")
	float TileSizeUU;
};

FORCEINLINE bool operator ==(const FTileMap& A, const FTileMap& B)
{
	return A.Size.X == B.Size.X && A.Data == B.Data;
}


namespace SLTileMap
{
	inline int32 CoordsToIndex(const FCoords Coords, const FCoords Size)
	{
		return Coords.Y * Size.X + Coords.X;
	}

	inline FCoords IndexToCoords(const int32 Index, const FCoords Size)
	{
		return FCoords(Index % Size.X, Index / Size.X);
	}

	inline bool IsValidCoordinate(const FTileMap& TileMap, const FCoords Coords)
	{
		const bool bXIsValid = (Coords.X >= 0 && Coords.X < TileMap.Size.X);
		const bool bYIsValid = (Coords.Y >= 0 && Coords.Y < TileMap.Size.Y);
		return bXIsValid && bYIsValid;
	}

	inline uint8 GetTile(const FTileMap& TileMap, const FCoords Coords)
	{
		return TileMap.Data[CoordsToIndex(Coords, TileMap.Size)];
	}

	inline uint8 GetTileChecked(const FTileMap& TileMap, const FCoords Coords)
	{
		if (IsValidCoordinate(TileMap, Coords))
		{
			return GetTile(TileMap, Coords);
		}
		return 0;
	}

	inline void SetTile(FTileMap& TileMap, const FCoords Coords, const uint8 Color)
	{
		TileMap.Data[CoordsToIndex(Coords, TileMap.Size)] = Color;
	}

	inline void SetTileChecked(FTileMap& TileMap, const uint8 Tile, const FCoords Coords)
	{
		if (IsValidCoordinate(TileMap, Coords))
		{
			SetTile(TileMap, Coords, Tile);
		}
	}

	inline FVector CoordsToWorldLocation(const FTileMap& TileMap, const FCoords Coords)
	{
		const float TileSizeUU = TileMap.TileSizeUU;
		return FVector(Coords.X, Coords.Y, 0) * TileSizeUU + TileMap.Origin + FVector(0.5 * TileSizeUU, 0.5 * TileSizeUU, 0);
	}

	inline FCoords WorldLocationToCoords(const FTileMap& TileMap, const FVector& Location)
	{
		FCoords Result;
		Result.X = (Location - TileMap.Origin).X / TileMap.TileSizeUU;
		Result.Y = (Location - TileMap.Origin).Y / TileMap.TileSizeUU;
		return Result;
	}

	inline FTileMap GetTilemapSection(const FTileMap& TileMap, const FCoords Coords, const FCoords SectionSize)
	{
		FTileMap OutSection = FTileMap(SectionSize);
		for (int32 j = 0; j < SectionSize.Y; j++)
		{
			for (int32 i = 0; i < SectionSize.X; i++)
			{
				const uint8 Tile = GetTile(TileMap, FCoords(Coords.X + i, Coords.Y + j));
				SetTile(OutSection, FCoords(i, j), Tile);
			}
		}
		return OutSection;
	}

	inline bool IsTileMapValid(const FTileMap& TileMap)
	{
		return TileMap.Data.Num() == TileMap.Size.X * TileMap.Size.Y;
	}
}
