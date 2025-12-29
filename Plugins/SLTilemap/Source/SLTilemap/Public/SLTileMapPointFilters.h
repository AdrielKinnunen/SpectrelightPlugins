// Copyright Spectrelight Studios, LLC

#pragma once

#include "CoreMinimal.h"
#include "SLTilemapQuery.h"


namespace SLTileMap
{
	inline void FilterPointsByDistanceField(TArray<FVector>& Points, const FTileMap& DistanceField, const float DesiredDistance, const float MinDistance, const float MaxDistance)
	{		
		//Filter
		for (auto It = Points.CreateIterator(); It; ++It)
		{
			const float Distance = SampleDistanceField(DistanceField, *It);
			if (Distance < MinDistance || Distance > MaxDistance)
			{
				It.RemoveCurrent();
			}
		}

		//Sort
		Points.Sort([DistanceField, DesiredDistance](const FVector A, const FVector B)
		{
			const float DistanceA = SampleDistanceField(DistanceField, A);
			const float DistanceB = SampleDistanceField(DistanceField, B);
			const int32 DifferenceA = FMath::Abs(DesiredDistance - DistanceA);
			const int32 DifferenceB = FMath::Abs(DesiredDistance - DistanceB);
			return DifferenceA < DifferenceB;
		});
	}

	

	inline void ShuffleAndLimitPointCount(TArray<FVector>& Points, const FRandomStream& RandomStream, const bool bShuffle, const int32 MinCount, const int32 MaxCount)
	{
		if (bShuffle)
		{
			const int32 LastIndex = Points.Num() - 1;
			for (int32 i = 0; i <= LastIndex; ++i)
			{
				const int32 Index = RandomStream.RandRange(i, LastIndex);
				if (i != Index)
				{
					Points.Swap(i, Index);
				}
			}
		}
		const int32 RandomCount = RandomStream.RandRange(MinCount, MaxCount);
		const int32 Count = FMath::Min(Points.Num(), RandomCount);
		Points.SetNum(Count);
	}


	inline void FilterPointsByDistance(TArray<FVector>& Points, const FVector& TargetPoint, const float DesiredDistance, const float MinDistance, const float MaxDistance)
	{
		//Filter
		for (auto It = Points.CreateIterator(); It; ++It)
		{
			const float Distance = FVector::Distance(*It, TargetPoint);
			if (Distance < MinDistance || Distance > MaxDistance)
			{
				It.RemoveCurrent();
			}
		}

		//Sort
		Points.Sort([DesiredDistance, TargetPoint](const FVector& A, const FVector& B)
		{
			const float DistanceA = FVector::Distance(A, TargetPoint);
			const float DistanceB = FVector::Distance(B, TargetPoint);
			const int32 DifferenceA = FMath::Abs(DesiredDistance - DistanceA);
			const int32 DifferenceB = FMath::Abs(DesiredDistance - DistanceB);
			return DifferenceA < DifferenceB;
		});
	}
}
