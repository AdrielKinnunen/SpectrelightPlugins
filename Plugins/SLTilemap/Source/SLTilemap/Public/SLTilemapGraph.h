// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SLTilemapCore.h"
#include "SLTilemapGraph.generated.h"



USTRUCT(BlueprintType)
struct FTileGraph
{
	GENERATED_BODY()
	TMultiMap<int32, int32> Adjacencies;
};

namespace SLTileMap
{
	
}