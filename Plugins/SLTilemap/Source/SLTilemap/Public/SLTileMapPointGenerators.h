// Copyright Spectrelight Studios, LLC

#pragma once

#include "CoreMinimal.h"
#include "SLTileMapGraph.h"



namespace SLTileMap
{
	inline TArray<FVector> GeneratePointsFromGraph(const FTileGraph& Graph, const int32 MinDegree, const int32 MaxDegree)
	{
		TArray<FVector> Result;

		for (int32 i = MinDegree; i <= MaxDegree; i++)
		{
			if (Graph.DegreeToGraphIndex.IsValidIndex(i))
			{
				for (const auto Index : Graph.DegreeToGraphIndex[i].Indices)
				{
					Result.Add(Graph.GraphIndexToPosition[Index]);
				}
			}
		}
		return Result;
	}
}
