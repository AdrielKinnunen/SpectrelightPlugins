// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SLTilemapLib.h"
#include "SLWave.h"
#include "Subsystems/WorldSubsystem.h"
#include "SLTilemapSubsystem.generated.h"



UCLASS()
class SLTILEMAP_API USLTilemapSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()


public:
	//Begin Subsystem
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	//End Subsystem


	UPROPERTY(BlueprintReadWrite, Category = "SLTilemap")
	FTileMap InputTileMap;
	UPROPERTY(BlueprintReadWrite, Category = "SLTilemap")
	FTileMap OutputTileMap;
	UPROPERTY(BlueprintReadOnly, Category = "SLTilemap")
	USLWave* Wave;

	
	
	
private:
	
};
