// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Containers/RingBuffer.h"
#include "SLTilemapCore.h"
#include "SLTilemapQuery.generated.h"


USTRUCT(BlueprintType)
struct FTileMapIndexSet
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tilemap")
	TArray<int32> Indices;
};

USTRUCT(BlueprintType)
struct FTilemapCoordsSet
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tilemap")
	TArray<FCoords> Coords;
};

USTRUCT(BlueprintType)
struct FTileNeighborIndices4
{
	GENERATED_BODY()
	int32 Indices[4] = {-1, -1, -1, -1};
};


USTRUCT(BlueprintType)
struct FTileNeighborIndices8
{
	GENERATED_BODY()
	int32 Indices[8] = {-1, -1, -1, -1, -1, -1, -1, -1};
};




namespace SLTileMap
{
	inline FTileNeighborIndices4 GetNeighborsAdjacent4(const FTileMap& TileMap, const int32 Index)
	{
		const FCoords Coords = IndexToCoords(Index, TileMap.Size);
		const int32 x = Coords.X;
		const int32 y = Coords.Y;
		const FCoords Offsets[4] = {{x, y - 1}, {x + 1, y}, {x, y + 1}, {x - 1, y}};
		FTileNeighborIndices4 Result;
		for (int32 i = 0; i < 4; i++)
		{
			int32 NeighborIndex = -1;
			const FCoords Offset = Offsets[i];
			if (IsValidCoordinate(TileMap, Offset))
			{
				NeighborIndex = CoordsToIndex(Offset, TileMap.Size);
			}
			Result.Indices[i] = NeighborIndex;
		}
		return Result;
	}

	inline FTileNeighborIndices8 GetNeighborsAdjacent8(const FTileMap& TileMap, const int32 Index)
	{
		const FCoords Coords = IndexToCoords(Index, TileMap.Size);
		const int32 x = Coords.X;
		const int32 y = Coords.Y;
		//const FTileMapCoords Offsets[8] = {{x, y-1}, {x+1, y}, {x, y+1}, {x-1, y}, {x, y-2}, {x+2, y}, {x, y+2}, {x-2, y}};
		const FCoords Offsets[8] = {{x + 1, y}, {x + 1, y + 1}, {x, y + 1}, {x - 1, y + 1}, {x - 1, y}, {x - 1, y - 1}, {x, y - 1}, {x + 1, y - 1}};
		FTileNeighborIndices8 Result;
		for (int32 i = 0; i < 8; i++)
		{
			int32 NeighborIndex = -1;
			const FCoords Offset = Offsets[i];
			if (IsValidCoordinate(TileMap, Offset))
			{
				NeighborIndex = CoordsToIndex(Offset, TileMap.Size);
			}
			Result.Indices[i] = NeighborIndex;
		}
		return Result;
	}

	inline FTileNeighborIndices4 GetNeighborsDiagonal4(const FTileMap& TileMap, const int32 Index)
	{
		const FCoords Coords = IndexToCoords(Index, TileMap.Size);
		const int32 x = Coords.X;
		const int32 y = Coords.Y;
		const FCoords Offsets[4] = {{x - 1, y - 1}, {x + 1, y - 1}, {x + 1, y + 1}, {x - 1, y + 1}};
		FTileNeighborIndices4 Result;
		for (int32 i = 0; i < 4; i++)
		{
			int32 NeighborIndex = -1;
			const FCoords Offset = Offsets[i];
			if (IsValidCoordinate(TileMap, Offset))
			{
				NeighborIndex = CoordsToIndex(Offset, TileMap.Size);
			}
			Result.Indices[i] = NeighborIndex;
		}
		return Result;
	}


	inline FTileMapIndexSet GetConnectedIndices4(const FTileMap& TileMap, const int32 Index)
	{
		const uint8 Tile = TileMap.Data[Index];
		TSet<int32> Connected;
		TRingBuffer<int32> Buffer;
		Buffer.Add(Index);
		while (!Buffer.IsEmpty())
		{
			int32 ThisIndex = Buffer.PopValue();
			Connected.Add(ThisIndex);
			const FTileNeighborIndices4 NeighborIndices = GetNeighborsAdjacent4(TileMap, ThisIndex);
			for (int32 i = 0; i < 4; i++)
			{
				const int32 NeighborIndex = NeighborIndices.Indices[i];
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
		FTileMapIndexSet Result;
		Result.Indices = Connected.Array();
		return Result;
	}


	inline TArray<FTileMapIndexSet> GetIslands(const FTileMap& TileMap, const uint8 Foreground)
	{
		TArray<FTileMapIndexSet> Islands;

		for (int32 i = 0; i < TileMap.Data.Num(); i++)
		{
			const uint8 Tile = TileMap.Data[i];
			if (Tile != Foreground)
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
			const FTileMapIndexSet Island = GetConnectedIndices4(TileMap, i);
			Islands.Add(Island);
		}
		return Islands;
	}


	//TODO: Fix up this AI Generated Slop
	inline FTileMapIndexSet ShrinkIsland(const FTileMapIndexSet& Island, const FTileMap& TileMap)
	{
		//Zhang-Suen Thinning Algorithm
		TSet<int32> IslandSet;
		IslandSet.Reserve(Island.Indices.Num());
		for (const int32 Index : Island.Indices)
		{
			IslandSet.Add(Index);
		}

		bool bPixelsRemoved = true;
		while (bPixelsRemoved)
		{
			bPixelsRemoved = false;

			// Step 1: Mark pixels for removal
			TSet<int32> ToRemoveStep1;
			for (const int32 Index : IslandSet)
			{
				const FCoords Coords = IndexToCoords(Index, TileMap.Size);
				const int32 x = Coords.X;
				const int32 y = Coords.Y;

				// Get 8-connected neighbors in specific order: P2,P3,P4,P5,P6,P7,P8,P9
				// P9 P2 P3
				// P8 P1 P4
				// P7 P6 P5
				int32 P[9];
				P[0] = Index;                                              // P1 (current pixel)
				P[1] = CoordsToIndex(FCoords(x, y - 1), TileMap.Size);     // P2 (North)
				P[2] = CoordsToIndex(FCoords(x + 1, y - 1), TileMap.Size); // P3 (NE)
				P[3] = CoordsToIndex(FCoords(x + 1, y), TileMap.Size);     // P4 (East)
				P[4] = CoordsToIndex(FCoords(x + 1, y + 1), TileMap.Size); // P5 (SE)
				P[5] = CoordsToIndex(FCoords(x, y + 1), TileMap.Size);     // P6 (South)
				P[6] = CoordsToIndex(FCoords(x - 1, y + 1), TileMap.Size); // P7 (SW)
				P[7] = CoordsToIndex(FCoords(x - 1, y), TileMap.Size);     // P8 (West)
				P[8] = CoordsToIndex(FCoords(x - 1, y - 1), TileMap.Size); // P9 (NW)

				// Check if neighbors are in island
				bool Neighbors[9];
				Neighbors[0] = true; // P1 is always in island
				for (int32 i = 1; i < 9; i++)
				{
					Neighbors[i] = IsValidCoordinate(TileMap, IndexToCoords(P[i], TileMap.Size)) && IslandSet.Contains(P[i]);
				}

				// Condition (a): 2 <= B(P1) <= 6 (number of non-zero neighbors)
				int32 B = 0;
				for (int32 i = 1; i < 9; i++)
				{
					if (Neighbors[i]) B++;
				}
				if (B < 2 || B > 6) continue;

				// Condition (b): A(P1) = 1 (number of 0-1 transitions in ordered sequence)
				int32 A = 0;
				for (int32 i = 1; i < 8; i++)
				{
					if (!Neighbors[i] && Neighbors[i + 1]) A++;
				}
				if (!Neighbors[8] && Neighbors[1]) A++; // Wrap around
				if (A != 1) continue;

				// Condition (c): P2 * P4 * P6 = 0
				if (Neighbors[1] && Neighbors[3] && Neighbors[5]) continue;

				// Condition (d): P4 * P6 * P8 = 0
				if (Neighbors[3] && Neighbors[5] && Neighbors[7]) continue;

				ToRemoveStep1.Add(Index);
			}

			// Remove marked pixels
			for (const int32 Index : ToRemoveStep1)
			{
				IslandSet.Remove(Index);
				bPixelsRemoved = true;
			}

			// Step 2: Mark pixels for removal with modified conditions
			TSet<int32> ToRemoveStep2;
			for (const int32 Index : IslandSet)
			{
				const FCoords Coords = IndexToCoords(Index, TileMap.Size);
				const int32 x = Coords.X;
				const int32 y = Coords.Y;

				int32 P[9];
				P[0] = Index;
				P[1] = CoordsToIndex(FCoords(x, y - 1), TileMap.Size);
				P[2] = CoordsToIndex(FCoords(x + 1, y - 1), TileMap.Size);
				P[3] = CoordsToIndex(FCoords(x + 1, y), TileMap.Size);
				P[4] = CoordsToIndex(FCoords(x + 1, y + 1), TileMap.Size);
				P[5] = CoordsToIndex(FCoords(x, y + 1), TileMap.Size);
				P[6] = CoordsToIndex(FCoords(x - 1, y + 1), TileMap.Size);
				P[7] = CoordsToIndex(FCoords(x - 1, y), TileMap.Size);
				P[8] = CoordsToIndex(FCoords(x - 1, y - 1), TileMap.Size);

				bool Neighbors[9];
				Neighbors[0] = true;
				for (int32 i = 1; i < 9; i++)
				{
					Neighbors[i] = IsValidCoordinate(TileMap, IndexToCoords(P[i], TileMap.Size)) && IslandSet.Contains(P[i]);
				}

				// Conditions (a) and (b) remain the same
				int32 B = 0;
				for (int32 i = 1; i < 9; i++)
				{
					if (Neighbors[i]) B++;
				}
				if (B < 2 || B > 6) continue;

				int32 A = 0;
				for (int32 i = 1; i < 8; i++)
				{
					if (!Neighbors[i] && Neighbors[i + 1]) A++;
				}
				if (!Neighbors[8] && Neighbors[1]) A++;
				if (A != 1) continue;

				// Condition (c'): P2 * P4 * P8 = 0
				if (Neighbors[1] && Neighbors[3] && Neighbors[7]) continue;

				// Condition (d'): P2 * P6 * P8 = 0
				if (Neighbors[1] && Neighbors[5] && Neighbors[7]) continue;

				ToRemoveStep2.Add(Index);
			}

			// Remove marked pixels
			for (const int32 Index : ToRemoveStep2)
			{
				IslandSet.Remove(Index);
				bPixelsRemoved = true;
			}
		}

		FTileMapIndexSet Result;
		Result.Indices = IslandSet.Array();
		return Result;
	}


	//TODO: Confirm it's correct
	inline FTileMapIndexSet ShrinkIsland2(const FTileMapIndexSet& Island, const FTileMap& TileMap)
	{
		//		https://www.researchgate.net/publication/308822048_A_new_thinning_algorithm_for_binary_images

		TSet<int32> IslandSet;
		IslandSet.Reserve(Island.Indices.Num());
		for (const int32 Index : Island.Indices)
		{
			IslandSet.Add(Index);
		}

		bool bPixelsRemoved = true;
		while (bPixelsRemoved)
		{
			bPixelsRemoved = false;

			// Step 1: Mark pixels for removal
			TSet<int32> ToRemoveStep1;
			for (const int32 Index : IslandSet)
			{
				const auto Neighbors = GetNeighborsAdjacent8(TileMap, Index);
				int32 NumNeighbors = 0;  //Count of black neighbors
				int32 NumComponents = 0; //Count of connected components in 8-neighborhood
				for (int32 i = 0; i < 8; i++)
				{
					const bool ThisPoint = IslandSet.Contains(Neighbors.Indices[i]);
					const bool NextPoint = IslandSet.Contains(Neighbors.Indices[(i + 1) % 8]);
					NumNeighbors += ThisPoint;
					if (!ThisPoint && NextPoint)
					{
						NumComponents++;
					}
				}
				const auto [X, Y] = IndexToCoords(Index, TileMap.Size);
				const bool A = ((X + Y) % 2 == 0);
				const bool B = NumComponents == 1;
				const bool C = (2 <= NumNeighbors) && (NumNeighbors <= 7);
				const bool P2 = IslandSet.Contains(Neighbors.Indices[0]);
				const bool P4 = IslandSet.Contains(Neighbors.Indices[2]);
				const bool P6 = IslandSet.Contains(Neighbors.Indices[4]);
				const bool P8 = IslandSet.Contains(Neighbors.Indices[6]);
				const bool D = !(P2 && P4 && P6);
				const bool E = !(P4 && P6 && P8);
				if (A && B && C && D && E)
				{
					ToRemoveStep1.Add(Index);
				}
			}
			for (const int32 Index : ToRemoveStep1)
			{
				IslandSet.Remove(Index);
				bPixelsRemoved = true;
			}


			TSet<int32> ToRemoveStep2;
			for (const int32 Index : IslandSet)
			{
				const auto Neighbors = GetNeighborsAdjacent8(TileMap, Index);
				int32 NumNeighbors = 0;  //Count of black neighbors
				int32 NumComponents = 0; //Count of connected components in 8-neighborhood
				for (int32 i = 0; i < 8; i++)
				{
					const bool ThisPoint = IslandSet.Contains(Neighbors.Indices[i]);
					const bool NextPoint = IslandSet.Contains(Neighbors.Indices[(i + 1) % 8]);
					NumNeighbors += ThisPoint;
					if (!ThisPoint && NextPoint)
					{
						NumComponents++;
					}
				}
				const auto [X, Y] = IndexToCoords(Index, TileMap.Size);
				const bool A = ((X + Y) % 2 != 0);
				const bool B = NumComponents == 1;
				const bool C = (2 <= NumNeighbors) && (NumNeighbors <= 7);
				const bool P2 = IslandSet.Contains(Neighbors.Indices[0]);
				const bool P4 = IslandSet.Contains(Neighbors.Indices[2]);
				const bool P6 = IslandSet.Contains(Neighbors.Indices[4]);
				const bool P8 = IslandSet.Contains(Neighbors.Indices[6]);
				const bool D = !(P2 && P4 && P8);
				const bool E = !(P2 && P6 && P8);
				if (A && B && C && D && E)
				{
					ToRemoveStep2.Add(Index);
				}
			}
			for (const int32 Index : ToRemoveStep2)
			{
				IslandSet.Remove(Index);
				bPixelsRemoved = true;
			}
		}

		FTileMapIndexSet Result;
		Result.Indices = IslandSet.Array();
		return Result;
	}


	//TODO: implement Moumita Sarkar's algorithm
	//https://rntujournals.aisect.org/assets/upload_files/articles/dd377fbe69ef3fc0ecdddc46803d53b2.pdf

	inline FTileMapIndexSet ShrinkIsland3(const FTileMapIndexSet& Island, const FTileMap& TileMap)
	{
		TSet<FCoords> IslandSet;
		IslandSet.Reserve(Island.Indices.Num());
		for (const int32 Index : Island.Indices)
		{
			IslandSet.Add(IndexToCoords(Index, TileMap.Size));
		}


		//Phase 1
		bool bPixelsRemoved = true;
		bool bIsEvenStep = false;
		while (bPixelsRemoved)
		{
			bPixelsRemoved = false;
			bIsEvenStep = !bIsEvenStep;
			TArray<FCoords> ToRemove;
			for (const FCoords Point : IslandSet)
			{
				const FCoords P1C = Point + FCoords{0, 0};
				const FCoords P2C = Point + FCoords{1, 0};
				const FCoords P3C = Point + FCoords{1, 1};
				const FCoords P4C = Point + FCoords{0, 1};
				const FCoords P5C = Point + FCoords{-1, 1};
				const FCoords P6C = Point + FCoords{-1, 0};
				const FCoords P7C = Point + FCoords{-1, -1};
				const FCoords P8C = Point + FCoords{0, -1};
				const FCoords P9C = Point + FCoords{1, -1};
				const FCoords P43C = Point + FCoords{1, 2};
				const FCoords P44C = Point + FCoords{0, 2};
				const FCoords P54C = Point + FCoords{-1, 2};
				const FCoords P55C = Point + FCoords{-2, 2};
				const FCoords P56C = Point + FCoords{-2, 1};
				const FCoords P66C = Point + FCoords{-2, 0};
				const FCoords P67C = Point + FCoords{-2, -1};
				const int32 P1 = IslandSet.Contains(P1C);
				const int32 P2 = IslandSet.Contains(P2C);
				const int32 P3 = IslandSet.Contains(P3C);
				const int32 P4 = IslandSet.Contains(P4C);
				const int32 P5 = IslandSet.Contains(P5C);
				const int32 P6 = IslandSet.Contains(P6C);
				const int32 P7 = IslandSet.Contains(P7C);
				const int32 P8 = IslandSet.Contains(P8C);
				const int32 P9 = IslandSet.Contains(P9C);
				const int32 P43 = IslandSet.Contains(P43C);
				const int32 P44 = IslandSet.Contains(P44C);
				const int32 P54 = IslandSet.Contains(P54C);
				const int32 P55 = IslandSet.Contains(P55C);
				const int32 P56 = IslandSet.Contains(P56C);
				const int32 P66 = IslandSet.Contains(P66C);
				const int32 P67 = IslandSet.Contains(P67C);

				const int32 A = (!P2 && P3) + (!P3 && P4) + (!P4 && P5) + (!P5 && P6) + (!P6 && P7) + (!P7 && P8) + (!P8 && P9) + (!P9 && P2);
				const int32 B = P2 + P3 + P4 + P5 + P6 + P7 + P8 + P9;
				const int32 M1 = bIsEvenStep ? P2 * P4 * P6 : P2 * P4 * P8;
				const int32 M2 = bIsEvenStep ? P4 * P6 * P8 : P2 * P6 * P8;
				//const int32 C = (!P2 * (P3 | P4)) + (!P4 * (P5 | P6)) + (!P6 * (P7 | P8)) + (!P8 * (P9 | P2));
				//const int32 N1 = (P9 | P2) + (P3 | P4) + (P5 | P6) + (P7 | P8);
				//const int32 N2 = (P2 | P3) + (P4 | P5) + (P6 | P7) + (P8 | P9);
				//const int32 N = FMath::Min(N1, N2);
				//const int32 M3 = ((P2 | P3 | !P5) * P4);

				if (P1 * P4 * P5 * P6 == 1 && (P2 | P3 | P7 | P8 | P9) == 0)
				{
					if ((P43 | P44 | P54 | P55 | P56 | P66 | P67) == 0)
					{
						ToRemove.Add(P4C);
						ToRemove.Add(P5C);
						ToRemove.Add(P6C);
					}
				}
				if (A == 1 && B >= 3 && B <= 6 && M1 == 0 && M2 == 0)
				{
					ToRemove.Add(P1C);
				}
			}
			for (const FCoords Index : ToRemove)
			{
				IslandSet.Remove(Index);
				bPixelsRemoved = true;
			}
		}


		//Phase 2
		bPixelsRemoved = true;
		while (bPixelsRemoved)
		{
			bPixelsRemoved = false;
			TArray<FCoords> ToRemove;
			for (const FCoords Point : IslandSet)
			{
				const FCoords P1C = Point + FCoords{0, 0};
				const FCoords P2C = Point + FCoords{1, 0};
				const FCoords P3C = Point + FCoords{1, 1};
				const FCoords P4C = Point + FCoords{0, 1};
				const FCoords P5C = Point + FCoords{-1, 1};
				const FCoords P6C = Point + FCoords{-1, 0};
				const FCoords P7C = Point + FCoords{-1, -1};
				const FCoords P8C = Point + FCoords{0, -1};
				const FCoords P9C = Point + FCoords{1, -1};
				const FCoords P43C = Point + FCoords{1, 2};
				//const FCoords P44C = Point + FCoords{0, 2};
				const FCoords P54C = Point + FCoords{-1, 2};
				//const FCoords P55C = Point + FCoords{-2, 2};
				//const FCoords P56C = Point + FCoords{-2, 1};
				//const FCoords P66C = Point + FCoords{-2, 0};
				//const FCoords P67C = Point + FCoords{-2, -1};
				const int32 P1 = IslandSet.Contains(P1C);
				const int32 P2 = IslandSet.Contains(P2C);
				const int32 P3 = IslandSet.Contains(P3C);
				const int32 P4 = IslandSet.Contains(P4C);
				const int32 P5 = IslandSet.Contains(P5C);
				const int32 P6 = IslandSet.Contains(P6C);
				const int32 P7 = IslandSet.Contains(P7C);
				const int32 P8 = IslandSet.Contains(P8C);
				const int32 P9 = IslandSet.Contains(P9C);
				const int32 P43 = IslandSet.Contains(P43C);
				//const int32 P44 = IslandSet.Contains(P44C);
				const int32 P54 = IslandSet.Contains(P54C);
				//const int32 P55 = IslandSet.Contains(P55C);
				//const int32 P56 = IslandSet.Contains(P56C);
				//const int32 P66 = IslandSet.Contains(P66C);
				//const int32 P67 = IslandSet.Contains(P67C);

				//const int32 A = (!P2 && P3) + (!P3 && P4) + (!P4 && P5) + (!P5 && P6) + (!P6 && P7) + (!P7 && P8) + (!P8 && P9) + (!P9 && P2);
				//const int32 B = P2 + P3 + P4 + P5 + P6 + P7 + P8 + P9;
				//const int32 M1 = bIsEvenStep ? P2 * P4 * P6 : P2 * P4 * P8;
				//const int32 M2 = bIsEvenStep ? P4 * P6 * P8 : P2 * P6 * P8;
				const int32 C = (!P2 * (P3 | P4)) + (!P4 * (P5 | P6)) + (!P6 * (P7 | P8)) + (!P8 * (P9 | P2));
				const int32 N1 = (P9 | P2) + (P3 | P4) + (P5 | P6) + (P7 | P8);
				const int32 N2 = (P2 | P3) + (P4 | P5) + (P6 | P7) + (P8 | P9);
				const int32 N = FMath::Min(N1, N2);
				const int32 M3 = ((P2 | P3 | !P5) * P4);


				if (C == 1 && N >= 2 && N <= 3 && M3 == 0)
				{
					ToRemove.Add(P1C);
				}
				if ((P4 | P5 | P6) == 0)
				{
					if (P1 * P2 * P3 * P8 == 1 || P1 * P8 * !P3 == 1)
					{
						ToRemove.Add(P1C);
					}
				}
				if (P1 * P4 == 1 && (P2 | P3 | P5 | P6 | P7 | P8 | P9) == 0)
				{
					if ((P43 | P54) == 1)
					{
						ToRemove.Add(P1C);
					}
				}
			}

			for (const FCoords Index : ToRemove)
			{
				IslandSet.Remove(Index);
				bPixelsRemoved = true;
			}
		}
		FTileMapIndexSet Result;
		Result.Indices.Reserve(IslandSet.Num());
		for (const FCoords Coords : IslandSet)
		{
			Result.Indices.Add(CoordsToIndex(Coords, TileMap.Size));
		}
		return Result;
	}


	inline FTileMap GetDistanceField(const FTileMap& TileMap, const uint8 Foreground)
	{
		FTileMap Result;
		Result.Size = TileMap.Size;
		Result.Data.SetNum(TileMap.Data.Num());
		for (int32 i = 0; i < TileMap.Data.Num(); i++)
		{
			const bool bIsForeground = TileMap.Data[i] & Foreground;
			Result.Data[i] = bIsForeground ? 255 : 0;
		}

		//Forward Pass
		for (int32 i = 0; i < Result.Data.Num(); i++)
		{
			uint8 MinDist = Result.Data[i];
			for (int32 j = 0; j < 4; j++)
			{
				constexpr FCoords Offsets[4] = {{-1, -1}, {0, -1}, {1, -1}, {-1, 0}};
				constexpr uint8 NeighborDistance[4] = {4, 3, 4, 3};

				const uint8 Distance = NeighborDistance[j];
				const FCoords Coords = IndexToCoords(i, TileMap.Size) + Offsets[j];

				if (!IsValidCoordinate(TileMap, Coords))
				{
					MinDist = FMath::Min(MinDist, Distance);
					continue;
				}

				const int32 Index = CoordsToIndex(Coords, TileMap.Size);
				const uint8 NewDistance = Result.Data[Index] + Distance;
				MinDist = FMath::Min(MinDist, NewDistance);
			}
			Result.Data[i] = MinDist;
		}

		//Backward Pass
		for (int32 i = Result.Data.Num() - 1; i >= 0; i--)
		{
			uint8 MinDist = Result.Data[i];
			for (int32 j = 0; j < 4; j++)
			{
				constexpr FCoords Offsets[4] = {{1, 0}, {-1, 1}, {0, 1}, {1, 1}};
				constexpr uint8 NeighborDistance[4] = {3, 4, 3, 4};

				const uint8 Distance = NeighborDistance[j];
				const FCoords Coords = IndexToCoords(i, TileMap.Size) + Offsets[j];

				if (!IsValidCoordinate(TileMap, Coords))
				{
					MinDist = FMath::Min(MinDist, Distance);
					continue;
				}

				const int32 Index = CoordsToIndex(Coords, TileMap.Size);
				const uint8 NewDistance = Result.Data[Index] + Distance;
				MinDist = FMath::Min(MinDist, NewDistance);
			}
			Result.Data[i] = MinDist;
		}
		return Result;
	}


	inline void KeepOnlyBiggestIsland(FTileMap& TileMap, const uint8 ColorToReplace, const uint8 ColorToReplaceWith)
	{
		TArray<FTileMapIndexSet> Islands = SLTileMap::GetIslands(TileMap, ColorToReplace);
		int32 BiggestIslandIndex = -1;
		int32 BiggestIslandSize = 0;
		for (int32 i = 0; i < Islands.Num(); i++)
		{
			const int32 IslandSize = Islands[i].Indices.Num();
			if (IslandSize > BiggestIslandSize)
			{
				BiggestIslandIndex = i;
				BiggestIslandSize = IslandSize;
			}
		}
		if (BiggestIslandIndex >= 0)
		{
			for (int32 i = 0; i < Islands.Num(); i++)
			{
				if (i != BiggestIslandIndex)
				{
					for (const auto Index : Islands[i].Indices)
					{
						TileMap.Data[Index] = ColorToReplaceWith;
					}
				}
			}
		}
	}



	inline float SampleDistanceField(const FTileMap& DistanceField, const FVector& Location)
	{
		const FCoords NearestCoords = WorldLocationToCoords(DistanceField, Location);

		
		const uint8 RawValue = DistanceField.Data[CoordsToIndex(NearestCoords, DistanceField.Size)];
		return 3.0f * RawValue;;
	}








	
}
