// Copyright Spectrelight Studios, LLC

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SLTileMapPOI.generated.h"


UCLASS(BlueprintType, Blueprintable, Abstract)
class SLTILEMAP_API ASLTileMapPOI : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASLTileMapPOI();
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 Radius = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 Weight = 1;
	
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
