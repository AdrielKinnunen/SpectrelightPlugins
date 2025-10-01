// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
//#include "SLTilemapCore.h"
//#include "SLTilemapSpline.generated.h"



namespace SLTileMap
{
	inline void SmoothPoints(TArray<FVector>& Points, const float Alpha)
	{
		const int32 N = Points.Num();
		for (int32 i = 0; i < N; i++)
		{
			const int32 Prev = (i - 1 + N) % N;
			const int32 Next = (i + 1) % N;
			const FVector& P = Points[i];
			const FVector& PPrev = Points[Prev];
			const FVector& PNext = Points[Next];
			Points[i] = P*(1-Alpha) + 0.5*Alpha*(PPrev + PNext);
		}
	}
	
}