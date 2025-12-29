// Copyright Spectrelight Studios, LLC

#pragma once

#include "CoreMinimal.h"
#include "SLTilemapCore.h"
#include "Engine/DataAsset.h"
#include "SLTilemapDataAsset.generated.h"


UCLASS(BlueprintType)
class SLTILEMAP_API USLTilemapDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTileMap Tilemap;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
