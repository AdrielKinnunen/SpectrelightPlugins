// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SLTilemapCore.h"
#include "SLTilemapPatterns.generated.h"


UENUM(BlueprintType)
enum class ESymmetryLevel : uint8
{
	None,
	ReflectY,
	ReflectXY,
	All
};

USTRUCT(BlueprintType)
struct FTilePattern
{
	GENERATED_BODY()
	uint8 Data[9] = {};

	friend uint32 GetTypeHash(const FTilePattern& Other)
	{
		return GetArrayHash(Other.Data, 9);
	}

	void ReflectY()
	{
		uint8 Result[9];
		Result[6] = Data[0];
		Result[7] = Data[1];
		Result[8] = Data[2];
		Result[3] = Data[3];
		Result[4] = Data[4];
		Result[5] = Data[5];
		Result[0] = Data[6];
		Result[1] = Data[7];
		Result[2] = Data[8];
		Data[0] = Result[0];
		Data[1] = Result[1];
		Data[2] = Result[2];
		Data[3] = Result[3];
		Data[4] = Result[4];
		Data[5] = Result[5];
		Data[6] = Result[6];
		Data[7] = Result[7];
		Data[8] = Result[8];
	}

	void ReflectX()
	{
		uint8 Result[9];
		Result[2] = Data[0];
		Result[1] = Data[1];
		Result[0] = Data[2];
		Result[5] = Data[3];
		Result[4] = Data[4];
		Result[3] = Data[5];
		Result[8] = Data[6];
		Result[7] = Data[7];
		Result[6] = Data[8];
		Data[0] = Result[0];
		Data[1] = Result[1];
		Data[2] = Result[2];
		Data[3] = Result[3];
		Data[4] = Result[4];
		Data[5] = Result[5];
		Data[6] = Result[6];
		Data[7] = Result[7];
		Data[8] = Result[8];
	}

	void Transpose()
	{
		uint8 Result[9];
		Result[0] = Data[0];
		Result[3] = Data[1];
		Result[6] = Data[2];
		Result[1] = Data[3];
		Result[4] = Data[4];
		Result[7] = Data[5];
		Result[2] = Data[6];
		Result[5] = Data[7];
		Result[8] = Data[8];
		Data[0] = Result[0];
		Data[1] = Result[1];
		Data[2] = Result[2];
		Data[3] = Result[3];
		Data[4] = Result[4];
		Data[5] = Result[5];
		Data[6] = Result[6];
		Data[7] = Result[7];
		Data[8] = Result[8];
	}

	void Rotate()
	{
		uint8 Result[9];
		Result[6] = Data[0];
		Result[3] = Data[1];
		Result[0] = Data[2];
		Result[7] = Data[3];
		Result[4] = Data[4];
		Result[1] = Data[5];
		Result[8] = Data[6];
		Result[5] = Data[7];
		Result[2] = Data[8];
		Data[0] = Result[0];
		Data[1] = Result[1];
		Data[2] = Result[2];
		Data[3] = Result[3];
		Data[4] = Result[4];
		Data[5] = Result[5];
		Data[6] = Result[6];
		Data[7] = Result[7];
		Data[8] = Result[8];
	}

	void AndWith(FTilePattern Other)
	{
		for (int i = 0; i < 9; i++)
		{
			Data[i] = Data[i] & Other.Data[i];
		}
	}

	void OrWith(FTilePattern Other)
	{
		for (int i = 0; i < 9; i++)
		{
			Data[i] = Data[i] | Other.Data[i];
		}
	}

	bool Allows(FTilePattern Other) const
	{
		FTilePattern Temp;
		for (int i = 0; i < 9; i++)
		{
			Temp.Data[i] = (Data[i] & Other.Data[i]) ^ Other.Data[i];
		}
		int Sum = 0;
		for (int i = 0; i < 9; i++)
		{
			Sum += Temp.Data[i];
		}
		return Sum == 0;
	}
};

FORCEINLINE bool operator ==(const FTilePattern& A, const FTilePattern& B)
{
	for (int i = 0; i < 9; i++)
	{
		if (A.Data[i] != B.Data[i])
		{
			return false;
		}
	}
	return true;
}


USTRUCT(BlueprintType)
struct FTilePatternSet
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tilemap")
	TArray<FTilePattern> Patterns;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tilemap")
	TArray<float> Weights;
};


namespace SLTileMap
{
	inline FTilePattern ReadPattern(const FTileMap& TileMap, const FCoords StartCoords)
	{
		FTilePattern Result = FTilePattern();
		for (int32 Y = 0; Y < 3; Y++)
		{
			for (int32 X = 0; X < 3; X++)
			{
				const FCoords Coords = FCoords(X + StartCoords.X, Y + StartCoords.Y);
				const uint8 Tile = GetTile(TileMap, Coords);
				const int32 Index = CoordsToIndex(FCoords(X, Y), FCoords(3, 3));
				Result.Data[Index] = Tile;
			}
		}
		return Result;
	}


	inline void WritePattern(FTileMap& TileMap, const FTilePattern Pattern, const FCoords Coords)
	{
		for (int32 Y = 0; Y < 3; Y++)
		{
			for (int32 X = 0; X < 3; X++)
			{
				const int32 PatternIndex = CoordsToIndex(FCoords(X, Y), FCoords(3, 3));
				const uint8 Tile = Pattern.Data[PatternIndex];
				SetTile(TileMap, FCoords(X + Coords.X, Y + Coords.Y), Tile);
			}
		}
	}


	inline FTilePatternSet GeneratePatternSet(FTileMap& TileMap, ESymmetryLevel Symmetry)
	{
		FTilePatternSet PatternSet;
		TArray<FTilePattern> Variants;
		TArray<int32> Counts;
		Variants.Reserve(8);
		Counts.Reserve(8);

		for (int32 y = 0; y < TileMap.Size.Y - 3 + 1; y++)
		{
			for (int32 x = 0; x < TileMap.Size.X - 3 + 1; x++)
			{
				Variants.Reset();
				FTilePattern Pattern = ReadPattern(TileMap, FCoords(x, y));
				Variants.Add(Pattern);
				if (Symmetry >= ESymmetryLevel::ReflectY)
				{
					Pattern.ReflectY();
					Variants.Add(Pattern);
				}
				if (Symmetry >= ESymmetryLevel::ReflectXY)
				{
					Pattern.ReflectX();
					Variants.Add(Pattern);
					Pattern.ReflectY();
					Variants.Add(Pattern);
				}
				if (Symmetry >= ESymmetryLevel::All)
				{
					Pattern.Transpose();
					Variants.Add(Pattern);
					Pattern.ReflectY();
					Variants.Add(Pattern);
					Pattern.ReflectX();
					Variants.Add(Pattern);
					Pattern.ReflectY();
					Variants.Add(Pattern);
				}
				for (const auto& Variant : Variants)
				{
					if (const int Index = PatternSet.Patterns.Find(Variant); Index > -1)
					{
						Counts[Index]++;
					}
					else
					{
						PatternSet.Patterns.Add(Variant);
						Counts.Add(1);
					}
				}
			}
		}

		float SumCounts = 0;
		for (const auto& Count : Counts)
		{
			SumCounts += Count;
		}

		PatternSet.Weights.SetNum(Counts.Num());
		for (int32 i = 0; i < Counts.Num(); i++)
		{
			const float Probability = Counts[i] / SumCounts;
			PatternSet.Weights[i] = Probability;
			//UE_LOG(LogTemp, Warning, TEXT("Pattern %d has count %d, probability %f"), i, Counts[i], Probability);
		}
		return PatternSet;
	}
}
