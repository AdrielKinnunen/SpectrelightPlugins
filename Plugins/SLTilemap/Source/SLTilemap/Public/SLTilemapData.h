// Copyright Spectrelight Studios, LLC

#pragma once

#include "CoreMinimal.h"
#include "SLTilemapGen.h"
#include "Engine/DataAsset.h"
#include "SLTilemapData.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class SLTILEMAP_API USLTilemapData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLTilemap")
	FTileGenOpsStack Recipe;
};
