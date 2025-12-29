// Copyright Spectrelight Studios, LLC


#include "SLTilemapLevelGenerator.h"
#include "SLTilemapGraph.h"
#include "SLTileMapPOI.h"


// Sets default values
ASLTilemapLevelGenerator::ASLTilemapLevelGenerator()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void ASLTilemapLevelGenerator::BeginPlay()
{
	Super::BeginPlay();
}

void ASLTilemapLevelGenerator::DebugDrawGraph(FTileGraph& Graph)
{
	UWorld* World = GetWorld();
	if (World)
	{
		SLTileMap::DebugDrawGraph(Graph, World);
	}
}

void ASLTilemapLevelGenerator::SpawnPOIAtPoints(TArray<FVector>& Points, TSubclassOf<ASLTileMapPOI> POIActor)
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}
	for (const FVector& Point : Points)
	{
		FVector Location = Point;
		FRotator Rotation(0.f, 0.f, 0.f);
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = this->GetInstigator();
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		//SpawnParams.OverrideLevel = Manager->GeneratedLevel->GetLoadedLevel();

		//const int32 ClassIndex = Manager->RandomStream.RandRange(0, POIClassesToChooseFrom.Num() - 1);
		//ASLTileMapPOI* Actor = World->SpawnActor<ASLTileMapPOI>(POIClassesToChooseFrom[ClassIndex], Location, Rotation, SpawnParams);
		ASLTileMapPOI* Actor = World->SpawnActor<ASLTileMapPOI>(POIActor, Location, Rotation, SpawnParams);
		if (Actor)
		{
			UE_LOG(LogTemp, Log, TEXT("Spawned actor: %s"), *Actor->GetName());
		}
	}
}



/*
 *
UE_LOG(LogTemp, Warning, TEXT("Streaming level loaded, generating now."));
const double StartTime = FPlatformTime::Seconds();


const FCoords Coords = SLTileMap::IndexToCoords(Index, Manager->Tilemap.Size);
FVector Location = SLTileMap::CoordsToWorldLocation(Manager->Tilemap, Coords);
FRotator Rotation(0.f, 0.f, 0.f);
FActorSpawnParameters SpawnParams;
SpawnParams.Owner = Manager;
SpawnParams.Instigator = Manager->GetInstigator();
SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
SpawnParams.OverrideLevel = Manager->GeneratedLevel->GetLoadedLevel();

const int32 ClassIndex = Manager->RandomStream.RandRange(0, POIClassesToChooseFrom.Num() - 1);
ASLTileMapPOI* Actor = World->SpawnActor<ASLTileMapPOI>(POIClassesToChooseFrom[ClassIndex], Location, Rotation, SpawnParams);
if (Actor)
{
UE_LOG(LogTemp, Log, TEXT("Spawned actor: %s"), *Actor->GetName());
}


UE_LOG(LogTemp, Warning, TEXT("Generation finished."));
const double EndTime = FPlatformTime::Seconds();
const double TotalTimems = 1000 * (EndTime - StartTime);
UE_LOG(LogTemp, Warning, TEXT("Level Generation took %f ms"), TotalTimems);
OnLevelGenerated();
*/
