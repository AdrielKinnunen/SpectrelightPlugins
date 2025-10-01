// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SLTilemapCore.h"
#include "SLTilemapPatterns.h"
#include "SLTilemapLevelGeneration.generated.h"


USTRUCT(BlueprintType)
struct FTileMapLevelRecipe
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLGame")
	int32 Seed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLGame")
	FTileMapCoords Size;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLGame")
	FTilePatternSet PatternSet;
};


namespace SLTileMap
{
	
}
