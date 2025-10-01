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
struct FTileGraph
{
	GENERATED_BODY()
	TSet<int32> Indices;
	TMultiMap<int32, int32> Adjacencies;
	TSet<int32> Degree0;
	TSet<int32> Degree1;
	TSet<int32> Degree2;
	TSet<int32> Degree3Plus;
};

USTRUCT(BlueprintType)
struct FTileIndexPair
{
	GENERATED_BODY()
	FTileIndexPair() : A(-1), B(-1)
	{
	}

	FTileIndexPair(int32 InA, int32 InB)
	{
		A = FMath::Min(InA, InB);
		B = FMath::Max(InA, InB);
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tilemap")
	int32 A;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tilemap")
	int32 B;
};

FORCEINLINE bool operator ==(const FTileIndexPair& A, const FTileIndexPair& B)
{
	return A.A==B.A&&A.B==B.B;
}

inline uint32 GetTypeHash(const FTileIndexPair& In)
{
	return HashCombine(GetTypeHash(In.A), GetTypeHash(In.B));
}


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
		const FTileMapCoords Coords = IndexToCoords(Index, TileMap.Size);
		const int32 x = Coords.X;
		const int32 y = Coords.Y;
		const FTileMapCoords Offsets[4] = {{x, y-1}, {x+1, y}, {x, y+1}, {x-1, y}};
		FTileNeighborIndices4 Result;
		for (int32 i = 0; i<4; i++)
		{
			int32 NeighborIndex = -1;
			const FTileMapCoords Offset = Offsets[i];
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
		const FTileMapCoords Coords = IndexToCoords(Index, TileMap.Size);
		const int32 x = Coords.X;
		const int32 y = Coords.Y;
		const FTileMapCoords Offsets[8] = {{x, y-1}, {x+1, y}, {x, y+1}, {x-1, y}, {x, y-2}, {x+2, y}, {x, y+2}, {x-2, y}};
		FTileNeighborIndices8 Result;
		for (int32 i = 0; i<8; i++)
		{
			int32 NeighborIndex = -1;
			const FTileMapCoords Offset = Offsets[i];
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
		const FTileMapCoords Coords = IndexToCoords(Index, TileMap.Size);
		const int32 x = Coords.X;
		const int32 y = Coords.Y;
		const FTileMapCoords Offsets[4] = {{x-1, y-1}, {x+1, y-1}, {x+1, y+1}, {x-1, y+1}};
		FTileNeighborIndices4 Result;
		for (int32 i = 0; i<4; i++)
		{
			int32 NeighborIndex = -1;
			const FTileMapCoords Offset = Offsets[i];
			if (IsValidCoordinate(TileMap, Offset))
			{
				NeighborIndex = CoordsToIndex(Offset, TileMap.Size);
			}
			Result.Indices[i] = NeighborIndex;
		}
		return Result;
	}


	inline FTileNeighborIndices8 GetNeighborsAll8(const FTileMap& TileMap, const int32 Index)
	{
		const FTileMapCoords Coords = IndexToCoords(Index, TileMap.Size);
		const int32 x = Coords.X;
		const int32 y = Coords.Y;
		const FTileMapCoords Offsets[8] = {{x-1, y-1}, {x, y-1}, {x+1, y-1}, {x+1, y}, {x+1, y+1}, {x, y+1}, {x-1, y+1}, {x-1, y}};
		FTileNeighborIndices8 Result;
		for (int32 i = 0; i<8; i++)
		{
			int32 NeighborIndex = -1;
			const FTileMapCoords Offset = Offsets[i];
			if (IsValidCoordinate(TileMap, Offset))
			{
				NeighborIndex = CoordsToIndex(Offset, TileMap.Size);
			}
			Result.Indices[i] = NeighborIndex;
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
			const FTileNeighborIndices4 NeighborIndices = GetNeighborsAdjacent4(TileMap, ThisIndex);
			for (int32 i = 0; i<4; i++)
			{
				const int32 NeighborIndex = NeighborIndices.Indices[i];
				if (NeighborIndex<0)
				{
					continue;
				}
				const uint8 NeighborTile = TileMap.Data[NeighborIndex];
				if (NeighborTile==Tile&&!Connected.Contains(NeighborIndex))
				{
					Buffer.Add(NeighborIndex);
				}
			}
		}
		FTileIndexSet Result;
		Result.Indices = Connected.Array();
		return Result;
	}


	inline TArray<FTileIndexSet> GetIslands(const FTileMap& TileMap, const uint8 Foreground)
	{
		TArray<FTileIndexSet> Islands;

		for (int32 i = 0; i<TileMap.Data.Num(); i++)
		{
			const uint8 Tile = TileMap.Data[i];
			if (Tile!=Foreground)
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


	inline TArray<FTileIndexPair> GetEdgeConnectionsByMarchingSquares(const FTileMap& TileMap, const uint8 Foreground)
	{
		TSet<FTileIndexPair> Result;
		TSet<int32> PossibleCorners;
		for (int32 y = 0; y<TileMap.Size.Y-1; y++)
		{
			for (int32 x = 0; x<TileMap.Size.X-1; x++)
			{
				//		B D					x+1,y	x+1,y+1					2,8
				//		A C					x,y		x,y+1					1,4
				const int32 AIndex = CoordsToIndex(FTileMapCoords(x, y), TileMap.Size);
				const int32 BIndex = CoordsToIndex(FTileMapCoords(x+1, y), TileMap.Size);
				const int32 CIndex = CoordsToIndex(FTileMapCoords(x, y+1), TileMap.Size);
				const int32 DIndex = CoordsToIndex(FTileMapCoords(x+1, y+1), TileMap.Size);
				const uint8 A = TileMap.Data[AIndex];
				const uint8 B = TileMap.Data[BIndex];
				const uint8 C = TileMap.Data[CIndex];
				const uint8 D = TileMap.Data[DIndex];

				switch ((A==Foreground)<<0|(B==Foreground)<<1|(C==Foreground)<<2|(D==Foreground)<<3)
				{
					case 0: //0000
						break;
					case 1: //1000
						Result.Add(FTileIndexPair(AIndex, AIndex));
						break;
					case 2: //0100
						Result.Add(FTileIndexPair(BIndex, BIndex));
						break;
					case 3: //1100
						Result.Add(FTileIndexPair(AIndex, BIndex));
						break;
					case 4: //0010
						Result.Add(FTileIndexPair(CIndex, CIndex));
						break;
					case 5: //1010
						Result.Add(FTileIndexPair(AIndex, CIndex));
						break;
					case 6: //0110
						Result.Add(FTileIndexPair(BIndex, CIndex));
						break;
					case 7: //1110
						Result.Add(FTileIndexPair(BIndex, CIndex));
						PossibleCorners.Add(AIndex);
						break;
					case 8: //0001
						Result.Add(FTileIndexPair(DIndex, DIndex));
						break;
					case 9: //1001
						Result.Add(FTileIndexPair(AIndex, DIndex));
						break;
					case 10: //0101
						Result.Add(FTileIndexPair(BIndex, DIndex));
						break;
					case 11: //1101
						Result.Add(FTileIndexPair(AIndex, DIndex));
						PossibleCorners.Add(BIndex);
						break;
					case 12: //0011
						Result.Add(FTileIndexPair(CIndex, DIndex));
						break;
					case 13: //1011
						Result.Add(FTileIndexPair(AIndex, DIndex));
						PossibleCorners.Add(CIndex);
						break;
					case 14: //0111
						Result.Add(FTileIndexPair(BIndex, CIndex));
						PossibleCorners.Add(DIndex);
						break;
					case 15: //1111
						break;
					default:
						break;
				}
			}
		}


		// Fix up loops
		TArray<FTileIndexPair> ConnectionsToRemove;
		TArray<FTileIndexPair> ConnectionsToAdd;
		for (const int32 Corner : PossibleCorners)
		{
			//Figure out if this corner should be sharp
			bool bSharp = true;
			const FTileNeighborIndices8 NeighborsAdjacent = GetNeighborsAdjacent8(TileMap, Corner);
			for (int i = 0; i<4; ++i)
			{
				const int Neighbor = NeighborsAdjacent.Indices[i];
				const int Behind = NeighborsAdjacent.Indices[i+4];
				if (Neighbor==-1||Behind==-1)
				{
					continue;
				}
				const bool bNeighborMatches = TileMap.Data[Neighbor] == Foreground;
				const bool bBehindMatches = TileMap.Data[Behind] ==  Foreground;
				bSharp &= !bNeighborMatches || (bNeighborMatches && bBehindMatches);
				//bSharp &= Result.Contains({Neighbor, Behind});
			}
			if (!bSharp)
			{
				continue;
			}

			//Aight we're sharp, update connections
			for (int i = 0; i<4; ++i)
			{
				const int Neighbor = NeighborsAdjacent.Indices[i];
				const int Left = NeighborsAdjacent.Indices[((i-1)%4+4)%4];
				const int Right = NeighborsAdjacent.Indices[((i+1)%4+4)%4];

				if (Result.Contains({Neighbor, Left}))
				{
					ConnectionsToRemove.Add({Neighbor, Left});
					ConnectionsToAdd.Add({Corner, Left});
					ConnectionsToAdd.Add({Neighbor, Corner});
				}
				if (Result.Contains({Neighbor, Right}))
				{
					ConnectionsToRemove.Add({Neighbor, Right});
					ConnectionsToAdd.Add({Corner, Right});
					ConnectionsToAdd.Add({Neighbor, Corner});
				}
			}
		}
		for (const auto Pair : ConnectionsToRemove)
		{
			Result.Remove(Pair);
		}
		for (const auto Pair : ConnectionsToAdd)
		{
			Result.Add(Pair);
		}


		//Remove pairs that include invalid indices
		TArray<FTileIndexPair> ResultArray;
		ResultArray.Reserve(Result.Num());
		for (const FTileIndexPair Pair : Result)
		{
			if (Pair.A>-1&&Pair.B>-1)
			{
				ResultArray.Add(Pair);
			}
		}
		return Result.Array();
	}


	inline FTileGraph GetBorderGraph(const FTileMap& TileMap, const uint8 Foreground)
	{
		const TArray<FTileIndexPair> Connections = GetEdgeConnectionsByMarchingSquares(TileMap, Foreground);
		FTileGraph Result;
		Result.Adjacencies.Reserve(Connections.Num()*2);

		for (int32 i = 0; i<Connections.Num(); i++)
		{
			const FTileIndexPair Connection = Connections[i];
			Result.Indices.Add(Connection.A);
			Result.Indices.Add(Connection.B);
			if (Connection.A!=Connection.B)
			{
				Result.Adjacencies.Add(Connection.A, Connection.B);
				Result.Adjacencies.Add(Connection.B, Connection.A);
			}
		}
		for (const auto Index : Result.Indices)
		{
			switch (Result.Adjacencies.Num(Index))
			{
				case 0:
					Result.Degree0.Add(Index);
					break;
				case 1:
					Result.Degree1.Add(Index);
					break;
				case 2:
					Result.Degree2.Add(Index);
					break;
				default:
					Result.Degree3Plus.Add(Index);
					break;
			}
		}
		return Result;
	}


	inline void DebugPrintGraph(const FTileGraph& Graph)
	{
		UE_LOG(LogTemp, Warning, TEXT("----------------"));
		UE_LOG(LogTemp, Warning, TEXT("---Graph Debug--"));
		UE_LOG(LogTemp, Warning, TEXT("There are %d degree 0"), Graph.Degree0.Num());
		UE_LOG(LogTemp, Warning, TEXT("There are %d degree 1"), Graph.Degree1.Num());
		UE_LOG(LogTemp, Warning, TEXT("There are %d degree 2"), Graph.Degree2.Num());
		UE_LOG(LogTemp, Warning, TEXT("There are %d degree 3+"), Graph.Degree3Plus.Num());
		for (const auto Index : Graph.Indices)
		{
			UE_LOG(LogTemp, Warning, TEXT("----------------"));
			UE_LOG(LogTemp, Warning, TEXT("Index %d"), Index);
			TArray<int32> Values;
			const int32 Degree = Graph.Adjacencies.Num(Index);
			UE_LOG(LogTemp, Warning, TEXT("Has degree %d"), Degree);
			Graph.Adjacencies.MultiFind(Index, Values);
			for (const auto Value : Values)
			{
				UE_LOG(LogTemp, Warning, TEXT("Is connected to %d"), Value);
			}
		}
	}

	//Traces along Graph, starting at StartIndex, stopping when it encounters a junction or end or loops.
	inline FTileIndexSet TraceBorder(const FTileGraph& Graph, const int32 StartIndex, const int32 NextIndex, TSet<FTileIndexPair>& ConnectionsVisited)
	{
		check(StartIndex != NextIndex);
		int32 Current = NextIndex;
		int32 Previous = StartIndex;
		FTileIndexSet Result;
		Result.Indices.Add(Previous);
		bool bFinished = false;
		while (!bFinished)
		{
			ConnectionsVisited.Add(FTileIndexPair(Previous, Current));
			Result.Indices.Add(Current);
			TArray<int32, TInlineAllocator<8>> Neighbors;
			Graph.Adjacencies.MultiFind(Current, Neighbors);
			Neighbors.RemoveSwap(Previous);
			switch (Neighbors.Num())
			{
				case 0: //Dead end
					bFinished = true;
					break;
				case 1: //Following a path
					Previous = Current;
					Current = Neighbors[0];
					if (ConnectionsVisited.Contains(FTileIndexPair(Previous, Current)))
					{
						bFinished = true;
					}
					break;
				default: //Hit a junction
					bFinished = true;
					break;
			}
		}
		return Result;
	}

	inline TArray<FTileIndexSet> GetBorderSets(const FTileMap& TileMap, const uint8 Foreground)
	{
		const FTileGraph Graph = GetBorderGraph(TileMap, Foreground);
		DebugPrintGraph(Graph);

		TArray<FTileIndexSet> Results;

		//Add degree 0 as single member sets
		for (const int32 Index : Graph.Degree0)
		{
			FTileIndexSet Set;
			Set.Indices.Add(Index);
			Results.Add(Set);
		}

		//Now we need to keep track of visited
		TSet<FTileIndexPair> Visited;

		//Search along paths from degree 1
		for (const int32 StartIndex : Graph.Degree1)
		{
			TArray<int32, TInlineAllocator<8>> Neighbors;
			Graph.Adjacencies.MultiFind(StartIndex, Neighbors);
			const int32 NextIndex = Neighbors[0];
			const bool EdgeVisited = Visited.Contains({StartIndex, NextIndex});
			if (!EdgeVisited)
			{
				const FTileIndexSet Trace = TraceBorder(Graph, StartIndex, NextIndex, Visited);
				Results.Add(Trace);
			}
		}


		//Search along paths from degree 3+
		for (const int32 StartIndex : Graph.Degree3Plus)
		{
			TArray<int32, TInlineAllocator<8>> Neighbors;
			Graph.Adjacencies.MultiFind(StartIndex, Neighbors);
			for (const int32 NextIndex : Neighbors)
			{
				const bool EdgeVisited = Visited.Contains(FTileIndexPair(StartIndex, NextIndex))||Visited.Contains(FTileIndexPair(NextIndex, StartIndex));
				if (!EdgeVisited)
				{
					const FTileIndexSet Trace = TraceBorder(Graph, StartIndex, NextIndex, Visited);
					Results.Add(Trace);
				}
			}
		}

		//Finally find 2-connected loops
		//const TSet<FTileIndexPair> asdf = Graph.Degree2.Difference()
		for (const int32 StartIndex : Graph.Degree2)
		{
			TArray<int32, TInlineAllocator<8>> Neighbors;
			Graph.Adjacencies.MultiFind(StartIndex, Neighbors);
			const int32 NextIndex = Neighbors[0];
			const bool EdgeVisited = Visited.Contains({StartIndex, NextIndex});
			if (!EdgeVisited)
			{
				const FTileIndexSet Trace = TraceBorder(Graph, StartIndex, NextIndex, Visited);
				Results.Add(Trace);
			}
		}
		return Results;
	}
}
