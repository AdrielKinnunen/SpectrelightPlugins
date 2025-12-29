// Copyright Spectrelight Studios, LLC

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SLTilemapLevelGenerator.generated.h"

class ASLTileMapPOI;
struct FTileMapIndexSet;
struct FTileMap;
struct FTileGraph;
class USLTilemapDataAsset;

UCLASS(Blueprintable, Abstract)
class SLTILEMAP_API ASLTilemapLevelGenerator : public AActor
{
	GENERATED_BODY()

public:
	ASLTilemapLevelGenerator();
protected:
	virtual void BeginPlay() override;
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<USLTilemapDataAsset> TilemapDataAsset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Seed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRandomStream RandomStream;

	UFUNCTION(BlueprintCallable, Category = "SLTileMap | Debug")
	void DebugDrawGraph(UPARAM(ref) FTileGraph& Graph);

	UFUNCTION(BlueprintCallable, Category = "SLTileMap | POI")
	void SpawnPOIAtPoints(UPARAM(ref) TArray<FVector>& Points, TSubclassOf<ASLTileMapPOI> POIActor);
};
