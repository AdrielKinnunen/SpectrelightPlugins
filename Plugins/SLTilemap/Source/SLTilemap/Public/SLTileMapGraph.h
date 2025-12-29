// Fill out your copyright notice in the Description page of Project Options.

#pragma once

#include "CoreMinimal.h"
#include "SLTilemapCore.h"
#include "SLTilemapPaint.h"
#include "SLTilemapQuery.h"
#include "SLTilemapGraph.generated.h"


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
	return A.A == B.A && A.B == B.B;
}

inline uint32 GetTypeHash(const FTileIndexPair& In)
{
	return HashCombine(GetTypeHash(In.A), GetTypeHash(In.B));
}


USTRUCT(BlueprintType)
struct FTileGraphEdgeOptions
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tilemap")
	bool bPreferDiagonals = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tilemap")
	bool bAllowInterior = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tilemap")
	bool bSharpenCorners = false;
};

inline static constexpr FTileGraphEdgeOptions MainPath = {false, false, false};
inline static constexpr FTileGraphEdgeOptions Walls = {true, false, true};


USTRUCT(BlueprintType)
struct FTileGraph
{
	GENERATED_BODY()
	//Raw Data
	TArray<int32> GraphIndexToTileMapIndex;
	TArray<FVector> GraphIndexToPosition;

	//Structure
	TMap<int32, int32> TileMapIndexToGraphIndex;
	TArray<FTileNeighborIndices8> GraphIndexToNeighbors;

	//Memoized helpers
	TArray<uint8> GraphIndexToDegree;
	TArray<FTileMapIndexSet> DegreeToGraphIndex;
	TArray<FTileMapIndexSet> ChainsByGraphIndex;
};

namespace SLTileMap
{
	inline TArray<FTileIndexPair> GetEdges(const FTileMap& TileMap, const uint8 Foreground, const FTileGraphEdgeOptions Options)
	{
		TSet<FTileIndexPair> Result;
		TSet<int32> PossibleCorners;
		for (int32 y = 0; y < TileMap.Size.Y - 1; y++)
		{
			for (int32 x = 0; x < TileMap.Size.X - 1; x++)
			{
				//		B D					x+1,y	x+1,y+1					2,8
				//		A C					x,y		x,y+1					1,4
				const int32 A = CoordsToIndex(FCoords(x, y), TileMap.Size);
				const int32 B = CoordsToIndex(FCoords(x + 1, y), TileMap.Size);
				const int32 C = CoordsToIndex(FCoords(x, y + 1), TileMap.Size);
				const int32 D = CoordsToIndex(FCoords(x + 1, y + 1), TileMap.Size);
				const uint8 AMask = (TileMap.Data[A] == Foreground) << 0;
				const uint8 BMask = (TileMap.Data[B] == Foreground) << 1;
				const uint8 CMask = (TileMap.Data[C] == Foreground) << 2;
				const uint8 DMask = (TileMap.Data[D] == Foreground) << 3;
				const uint8 Mask = AMask | BMask | CMask | DMask;

				switch (Mask)
				{
					case 0: //0000
						break;
					case 1: //1000
						Result.Add(FTileIndexPair(A, A));
						break;
					case 2: //0100
						Result.Add(FTileIndexPair(B, B));
						break;
					case 3: //1100
						Result.Add(FTileIndexPair(A, B));
						break;
					case 4: //0010
						Result.Add(FTileIndexPair(C, C));
						break;
					case 5: //1010
						Result.Add(FTileIndexPair(A, C));
						break;
					case 6: //0110
						Result.Add(FTileIndexPair(B, C));
						break;
					case 7: //1110
						if (Options.bPreferDiagonals)
						{
							Result.Add(FTileIndexPair(B, C));
						}
						else
						{
							Result.Add(FTileIndexPair(A, B));
							Result.Add(FTileIndexPair(A, C));
						}
						PossibleCorners.Add(A);
						break;
					case 8: //0001
						Result.Add(FTileIndexPair(D, D));
						break;
					case 9: //1001
						Result.Add(FTileIndexPair(A, D));
						break;
					case 10: //0101
						Result.Add(FTileIndexPair(B, D));
						break;
					case 11: //1101
						if (Options.bPreferDiagonals)
						{
							Result.Add(FTileIndexPair(A, D));
						}
						else
						{
							Result.Add(FTileIndexPair(A, B));
							Result.Add(FTileIndexPair(B, D));
						}
						PossibleCorners.Add(B);
						break;
					case 12: //0011
						Result.Add(FTileIndexPair(C, D));
						break;
					case 13: //1011
						if (Options.bPreferDiagonals)
						{
							Result.Add(FTileIndexPair(A, D));
						}
						else
						{
							Result.Add(FTileIndexPair(A, C));
							Result.Add(FTileIndexPair(C, D));
						}
						PossibleCorners.Add(C);
						break;
					case 14: //0111
						if (Options.bPreferDiagonals)
						{
							Result.Add(FTileIndexPair(B, C));
						}
						else
						{
							Result.Add(FTileIndexPair(B, D));
							Result.Add(FTileIndexPair(C, D));
						}
						PossibleCorners.Add(D);
						break;
					case 15: //1111
						if (Options.bAllowInterior)
						{
							Result.Add(FTileIndexPair(A, B));
							Result.Add(FTileIndexPair(B, C));
							Result.Add(FTileIndexPair(C, D));
							Result.Add(FTileIndexPair(D, A));
						}
						break;
					default:
						break;
				}
			}
		}

		if (Options.bSharpenCorners)
		{
			TArray<FTileIndexPair> ConnectionsToRemove;
			TArray<FTileIndexPair> ConnectionsToAdd;
			for (const int32 Corner : PossibleCorners)
			{
				//Figure out if this corner should be sharp
				bool bSharp = true;
				const FTileNeighborIndices8 NeighborsAdjacent = GetNeighborsAdjacent8(TileMap, Corner);
				for (int i = 0; i < 4; ++i)
				{
					const int Neighbor = NeighborsAdjacent.Indices[i];
					const int Behind = NeighborsAdjacent.Indices[i + 4];
					if (Neighbor == -1 || Behind == -1)
					{
						continue;
					}
					const bool bNeighborMatches = TileMap.Data[Neighbor] == Foreground;
					const bool bBehindMatches = TileMap.Data[Behind] == Foreground;
					bSharp &= !bNeighborMatches || (bNeighborMatches && bBehindMatches);
					//bSharp &= Result.Contains({Neighbor, Behind});
				}
				if (!bSharp)
				{
					continue;
				}

				//Aight we're sharp, update connections
				for (int i = 0; i < 4; ++i)
				{
					const int Neighbor = NeighborsAdjacent.Indices[i];
					const int Left = NeighborsAdjacent.Indices[((i - 1) % 4 + 4) % 4];
					const int Right = NeighborsAdjacent.Indices[((i + 1) % 4 + 4) % 4];

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
		}


		//Remove pairs that include invalid indices
		TArray<FTileIndexPair> ResultArray;
		ResultArray.Reserve(Result.Num());
		for (const FTileIndexPair Pair : Result)
		{
			if (Pair.A > -1 && Pair.B > -1)
			{
				ResultArray.Add(Pair);
			}
		}
		return Result.Array();
	}


	inline FTileMapIndexSet TraceChain(const FTileGraph& Graph, const int32 StartIndex, const int32 NextIndex, TSet<FTileIndexPair>& ConnectionsVisited)
	{
		//check(StartIndex != NextIndex);
		int32 Current = NextIndex;
		int32 Previous = StartIndex;
		FTileMapIndexSet Result;
		Result.Indices.Add(Previous);
		if (Current == Previous)
		{
			return Result;
		}
		bool bFinished = false;
		while (!bFinished)
		{
			ConnectionsVisited.Add(FTileIndexPair(Previous, Current));
			Result.Indices.Add(Current);

			int32 Next = INDEX_NONE;
			int32 NumValidNeighbors = 0;
			for (int32 i = 0; i < 8; i++)
			{
				const int32 Neighbor = Graph.GraphIndexToNeighbors[Current].Indices[i];
				if (Neighbor != INDEX_NONE && Neighbor != Previous)
				{
					Next = Neighbor;
					NumValidNeighbors++;
				}
			}
			switch (NumValidNeighbors)
			{
				case 0: //Dead end
					bFinished = true;
					break;
				case 1: //Following a path
					Previous = Current;
					Current = Next;
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

	inline TArray<FTileMapIndexSet> GetChains(FTileGraph& Graph)
	{
		TArray<FTileMapIndexSet> Results;
		TSet<FTileIndexPair> Visited;
		static const int32 DegreeOrder[9] = {0, 1, 3, 4, 5, 6, 7, 8, 2};
		for (const int32 Degree : DegreeOrder)
		{
			for (const int32 StartIndex : Graph.DegreeToGraphIndex[Degree].Indices)
			{
				for (int32 j = 0; j < 8; j++)
				{
					const int32 NextIndex = Graph.GraphIndexToNeighbors[StartIndex].Indices[j];
					const bool EdgeVisited = Visited.Contains({StartIndex, NextIndex});
					if (NextIndex != INDEX_NONE && !EdgeVisited)
					{
						const FTileMapIndexSet Trace = TraceChain(Graph, StartIndex, NextIndex, Visited);
						Results.Add(Trace);
					}
				}
			}
		}
		return Results;
	}


	inline FTileGraph TileMapToGraph(const FTileMap& TileMap, const uint8 Foreground, const FTileGraphEdgeOptions Options)
	{
		FTileGraph Result;

		//Get Edges
		const TArray<FTileIndexPair> Edges = GetEdges(TileMap, Foreground, Options);

		//Collect unique vertices and map them to indices
		for (const FTileIndexPair& E : Edges)
		{
			if (!Result.TileMapIndexToGraphIndex.Contains(E.A))
			{
				int32 Index = Result.GraphIndexToTileMapIndex.Add(E.A);
				Result.TileMapIndexToGraphIndex.Add(E.A, Index);
			}
			if (!Result.TileMapIndexToGraphIndex.Contains(E.B))
			{
				int32 Index = Result.GraphIndexToTileMapIndex.Add(E.B);
				Result.TileMapIndexToGraphIndex.Add(E.B, Index);
			}
		}

		//Compute Adjacencies and Degree
		const int32 Num = Result.GraphIndexToTileMapIndex.Num();
		Result.GraphIndexToPosition.SetNum(Num);
		for (int32 i = 0; i < Num; i++)
		{
			Result.GraphIndexToPosition[i] = CoordsToWorldLocation(TileMap, IndexToCoords(Result.GraphIndexToTileMapIndex[i], TileMap.Size));
		}
		Result.GraphIndexToNeighbors.SetNum(Num);
		Result.GraphIndexToDegree.SetNumZeroed(Num);
		for (const FTileIndexPair& Edge : Edges)
		{
			check(Edge.A <= Edge.B);
			if (Edge.A == Edge.B)
			{
				continue;
			}

			const int32 GraphIndexA = Result.TileMapIndexToGraphIndex[Edge.A];
			const int32 GraphIndexB = Result.TileMapIndexToGraphIndex[Edge.B];

			FTileNeighborIndices8& NeighborsA = Result.GraphIndexToNeighbors[GraphIndexA];
			uint8& DegreeA = Result.GraphIndexToDegree[GraphIndexA];
			if (DegreeA < 8)
			{
				NeighborsA.Indices[DegreeA] = GraphIndexB;
				DegreeA++;
			}

			FTileNeighborIndices8& NeighborsB = Result.GraphIndexToNeighbors[GraphIndexB];
			uint8& DegreeB = Result.GraphIndexToDegree[GraphIndexB];
			if (DegreeB < 8)
			{
				NeighborsB.Indices[DegreeB] = GraphIndexA;
				DegreeB++;
			}
		}
		Result.DegreeToGraphIndex.SetNum(9);
		for (int32 i = 0; i < Num; i++)
		{
			const int32 Degree = Result.GraphIndexToDegree[i];
			Result.DegreeToGraphIndex[Degree].Indices.Add(i);
		}

		Result.ChainsByGraphIndex = GetChains(Result);
		
		return Result;
	}


	inline void DebugPrintGraph(const FTileGraph& Graph)
	{
		UE_LOG(LogTemp, Warning, TEXT("----------------"));
		UE_LOG(LogTemp, Warning, TEXT("---Graph Debug--"));
		for (int32 i = 0; i < Graph.DegreeToGraphIndex.Num(); i++)
		{
			UE_LOG(LogTemp, Warning, TEXT("There are %d degree %d"), Graph.DegreeToGraphIndex[i].Indices.Num(), i);
		}
		UE_LOG(LogTemp, Warning, TEXT("There are %d chains"), Graph.ChainsByGraphIndex.Num());
		UE_LOG(LogTemp, Warning, TEXT("----------------"));
	}


	inline void DebugDrawGraph(const FTileGraph& Graph, const UWorld* World)
	{
		if (!World)
		{
			return;
		}

		const FVector Offset = {0, 0, 20};

		TSet<FTileIndexPair> DrawnConnections;
		for (const auto Chain : Graph.ChainsByGraphIndex)
		{
			const FColor Color = FColor::MakeRandomColor();
			for (int32 i = 0; i < Chain.Indices.Num() - 1; i++)
			{
				const FVector Start = Graph.GraphIndexToPosition[Chain.Indices[i]] + Offset;
				const FVector End = Graph.GraphIndexToPosition[Chain.Indices[i+1]] + Offset;
				DrawDebugDirectionalArrow(World, Start, End, 500.0f, Color, false, 5.0f, 0, 5.0f);
			}
		}
	}


	inline int32 GetVertexWithHighestDistanceFromWall(const FTileGraph& Graph, const FTileMap& DistanceField, const int32 MinDegree, const int32 MaxDegree, const TArray<int32>& VerticesToIgnore)
	{
		int32 Result = -1;

		TSet<int32> Candidates;
		if (0 >= MinDegree && 0 <= MaxDegree)
		{
			//Candidates = Candidates.Union(Graph.Degree0);
		}
		if (1 >= MinDegree && 1 <= MaxDegree)
		{
			//Candidates = Candidates.Union(Graph.Degree1);
		}
		if (2 >= MinDegree && 2 <= MaxDegree)
		{
			//Candidates = Candidates.Union(Graph.Degree2);
		}
		if (3 >= MinDegree && 3 <= MaxDegree)
		{
			//Candidates = Candidates.Union(Graph.Degree3Plus);
		}

		int32 MaxDistance = 0;
		for (const int32 Index : Candidates)
		{
			const int32 Distance = DistanceField.Data[Index];
			if (Distance > MaxDistance && !VerticesToIgnore.Contains(Index))
			{
				MaxDistance = Distance;
				Result = Index;
			}
		}

		if (Result == -1)
		{
			UE_LOG(LogTemp, Warning, TEXT("No candidate vertex was found for GetVertexWithHighestDistanceFromWall"));
		}
		return Result;
	}


	
	inline int32 GetDistance(const FTileGraph& Graph, const int32 Start, const int32 End)
	{
		// If Start or End vertex doesn't exist in the graph, return -1
		if (!Graph.TileMapIndexToGraphIndex.Contains(Start) || !Graph.TileMapIndexToGraphIndex.Contains(End))
		{
			return -1;
		}

		// If Start and End are the same vertex, return 0
		if (Start == End)
		{
			return 0;
		}

		const int32 StartInternal = Graph.TileMapIndexToGraphIndex.FindRef(Start);
		const int32 EndInternal = Graph.TileMapIndexToGraphIndex.FindRef(End);
		// Queue for BFS
		TQueue<int32> Queue;
		// Keep track of visited vertices and their distances
		TMap<int32, int32> Distances;

		// Initialize with start vertex
		Queue.Enqueue(StartInternal);
		Distances.Add(StartInternal, 0);

		// BFS
		while (!Queue.IsEmpty())
		{
			int32 CurrentVertex;
			Queue.Dequeue(CurrentVertex);

			// Get all neighbors of current vertex
			const auto Neighbors = Graph.GraphIndexToNeighbors[CurrentVertex];

			// Process each neighbor
			for (const int32 Neighbor : Neighbors.Indices)
			{
				if (Neighbor == -1)
				{
					continue;
				}
				// If we haven't visited this neighbor yet
				if (!Distances.Contains(Neighbor))
				{
					// Calculate and store its distance
					Distances.Add(Neighbor, Distances[CurrentVertex] + 1);

					// If this is our target, return the distance
					if (Neighbor == EndInternal)
					{
						return Distances[Neighbor];
					}

					// Add to queue to explore its neighbors later
					Queue.Enqueue(Neighbor);
				}
			}
		}
		// If we get here, no path was found
		return -1;
	}


	inline FTileGraph TileMapToSkeletonGraph(FTileMap& TileMap, const uint8 ColorToSkeletonize, const FPaintOptions PaintOptions)
	{
		TArray<FTileMapIndexSet> Islands = GetIslands(TileMap, ColorToSkeletonize);
		for (auto& Island : Islands)
		{
			Island = ShrinkIsland3(Island, TileMap);
		}
		FTileMap TempTileMap = FTileMap(TileMap.Size, 0);
		TempTileMap.Origin = TileMap.Origin;
		TempTileMap.TileSizeUU = TileMap.TileSizeUU;
		for (const auto& Island : Islands)
		{
			for (const auto Index : Island.Indices)
			{
				if (PaintOptions.bDrawColor)
				{
					TileMap.Data[Index] = PaintOptions.Color;
				}
				TempTileMap.Data[Index] = 1;
			}
		}
		return TileMapToGraph(TempTileMap, 1, MainPath);
	}


	inline FTileGraph CoordsSetToGraph(FTilemapCoordsSet& CoordsSet)
	{
		FTileGraph Result;
		return Result;
	}

	inline void DrawGraph(FTileMap& TileMap, const FTileGraph& Graph, const uint8 Color)
	{
		//DrawLine()
	}

	inline void SmoothGraph(FTileGraph& Graph, const FTileMap& TileMap, const int32 Iterations)
	{
		for (const auto& Chain : Graph.ChainsByGraphIndex)
		{
			const int32 Num = Chain.Indices.Num();
			if (Num < 3 || Iterations < 1) {continue;}
			const bool bIsLoop = Chain.Indices[0] == Chain.Indices.Last();
			
			const int32 StartIndex = bIsLoop ? 0 : 1;
			const int32 EndIndex = Num - 2;
			const int32 Mod = bIsLoop ? Num - 1 : Num;
			for (int32 I = 0; I < Iterations; I++)
			{
				for (int32 i = StartIndex; i <= EndIndex; i++)
				{
					const int32 PrevIndex = (i - 1 + Mod) % Mod;
					const int32 NextIndex = (i + 1 + Mod) % Mod;
					const FVector Prev = Graph.GraphIndexToPosition[Chain.Indices[PrevIndex]];
					const FVector This = Graph.GraphIndexToPosition[Chain.Indices[i]];
					const FVector Next = Graph.GraphIndexToPosition[Chain.Indices[NextIndex]];
					const FVector Average = (Prev + Next) * 0.5f;
					const FVector Delta = This - Average;
					const FVector NewPositionUnclamped = This + Delta * 0.5f * 0.5f;
					const FVector OriginalPosition = CoordsToWorldLocation(TileMap, IndexToCoords(Graph.GraphIndexToTileMapIndex[Chain.Indices[i]], TileMap.Size));
					const FVector UnclampedOffsetFromOriginal = NewPositionUnclamped - OriginalPosition;
					const FVector ClampedOffset = UnclampedOffsetFromOriginal.GetClampedToMaxSize(TileMap.TileSizeUU * 0.5f);
					//Graph.GraphIndexToPosition[Chain.Indices[i]] = OriginalPosition + ClampedOffset;
					Graph.GraphIndexToPosition[Chain.Indices[i]] = NewPositionUnclamped;
				}
				for (int32 i = StartIndex; i <= EndIndex; i++)
				{
					const int32 PrevIndex = (i - 1 + Mod) % Mod;
					const int32 NextIndex = (i + 1 + Mod) % Mod;
					const FVector Prev = Graph.GraphIndexToPosition[Chain.Indices[PrevIndex]];
					const FVector This = Graph.GraphIndexToPosition[Chain.Indices[i]];
					const FVector Next = Graph.GraphIndexToPosition[Chain.Indices[NextIndex]];
					const FVector Average = (Prev + Next) * 0.5f;
					const FVector Delta = This - Average;
					const FVector NewPositionUnclamped = This - Delta * 0.53f * 0.5f;
					const FVector OriginalPosition = CoordsToWorldLocation(TileMap, IndexToCoords(Graph.GraphIndexToTileMapIndex[Chain.Indices[i]], TileMap.Size));
					const FVector UnclampedOffsetFromOriginal = NewPositionUnclamped - OriginalPosition;
					//Graph.GraphIndexToPosition[Chain.Indices[i]] = OriginalPosition + ClampedOffset;
					Graph.GraphIndexToPosition[Chain.Indices[i]] = NewPositionUnclamped;
				}
			}
		}
	}


	inline TArray<FTileMapIndexSet> GetChains(const FTileGraph& Graph)
	{
		return Graph.ChainsByGraphIndex;
	}
	
	inline TArray<FVector> GetChainPositions(const FTileGraph& Graph, const FTileMapIndexSet& Chain)
	{
		TArray<FVector> Result;
		for (const int32 Index : Chain.Indices)
		{
			Result.Add(Graph.GraphIndexToPosition[Index]);
		}
		return Result;
	}
}
