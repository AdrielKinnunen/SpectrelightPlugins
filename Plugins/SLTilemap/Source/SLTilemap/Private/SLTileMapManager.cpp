// Copyright Spectrelight Studios, LLC


#include "SLTilemapManager.h"

#include "SLTilemapLevelGenerator.h"
#include "Engine/LevelStreamingDynamic.h"


ASLTilemapManager::ASLTilemapManager()
{
	PrimaryActorTick.bCanEverTick = true;
}


void ASLTilemapManager::BeginPlay()
{
	Super::BeginPlay();
}

void ASLTilemapManager::BeginGenerateLevel()
{
	if (GeneratedLevel)
	{
		DestroyLevel();
	}
	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("No World to put streaming level in"));
		return;
	}
	FTransform LevelTransform = FTransform::Identity;
	bool bSuccess = false;
	ULevelStreamingDynamic* StreamedLevel = ULevelStreamingDynamic::LoadLevelInstanceBySoftObjectPtr(World, TemplateLevel, LevelTransform, bSuccess);

	if (!StreamedLevel)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create streaming level"));
		return;
	}
	UE_LOG(LogTemp, Warning, TEXT("Streaming level created"));

	GeneratedLevel = StreamedLevel;
	StreamedLevel->OnLevelLoaded.AddDynamic(this, &ASLTilemapManager::OnStreamingLevelLoaded);
}


void ASLTilemapManager::OnStreamingLevelLoaded()
{
	FVector Location = FVector::ZeroVector;
	FRotator Rotation(0.f, 0.f, 0.f);
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = this->GetInstigator();
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.OverrideLevel = GeneratedLevel->GetLoadedLevel();
	
	ASLTilemapLevelGenerator* Generator = GetWorld()->SpawnActor<ASLTilemapLevelGenerator>(LevelGeneratorClass, Location, Rotation, SpawnParams);
	if (Generator)
	{
		UE_LOG(LogTemp, Log, TEXT("Spawned Generator: %s"), *Generator->GetName());
	}
	OnLevelGenerated();
}


void ASLTilemapManager::DestroyLevel()
{
	if (!GeneratedLevel)
	{
		return;
	}
	GeneratedLevel->SetIsRequestingUnloadAndRemoval(true);
	UE_LOG(LogTemp, Warning, TEXT("🧹 Generated sublevel unloaded"));
	GeneratedLevel = nullptr;
}
