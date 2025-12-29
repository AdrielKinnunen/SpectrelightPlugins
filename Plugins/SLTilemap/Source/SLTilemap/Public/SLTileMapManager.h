// Copyright Spectrelight Studios, LLC

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SLTilemapManager.generated.h"


class ASLTilemapLevelGenerator;
class ULevelStreamingDynamic;
class USLTilemapRecipe;

UCLASS(BlueprintType, Blueprintable)
class SLTILEMAP_API ASLTilemapManager : public AActor
{
	GENERATED_BODY()

public:
	ASLTilemapManager();
protected:
	virtual void BeginPlay() override;
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UWorld> TemplateLevel;
	UPROPERTY(Transient)
	ULevelStreamingDynamic* GeneratedLevel;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<ASLTilemapLevelGenerator> LevelGeneratorClass;
	
	UFUNCTION(BlueprintCallable)
	void BeginGenerateLevel();
	UFUNCTION()
	void OnStreamingLevelLoaded();
	UFUNCTION(BlueprintImplementableEvent)
	void OnLevelGenerated();
	UFUNCTION(BlueprintCallable)
	void DestroyLevel();
};