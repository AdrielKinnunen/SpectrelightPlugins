// Copyright Spectrelight Studios, LLC

#pragma once
/*
#include "CoreMinimal.h"
#include "SLTilemapCore.h"
#include "SLTilemapManager.h"
#include "SLTilemapPaint.h"
#include "SLTileMapPOI.h"
#include "Engine/LevelStreamingDynamic.h"
#include "SLTilemapOps.generated.h"



/*


USTRUCT(BlueprintType)
struct FOpMinimumSpanningTree : public FTilemapOp
{
	GENERATED_BODY()

	FOpMinimumSpanningTree()
	{
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName PointsName = "RandomPoints";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Bitmask, BitmaskEnum = "/Script/SLTilemap.ETileMapColor"))
	uint8 Color = 0;

	virtual bool Execute(ASLTilemapManager* Manager) override
	{
		TArray<TTuple<FCoords, FCoords>> EdgesCoords;
		//TArray<FTileIndexPair> Edges;
		TArray<FCoords> Explored;
		TArray<FCoords> Unexplored = Manager->NameToCoordsSet.FindOrAdd(PointsName).Coords;
		if (Unexplored.Num() < 2)
		{
			return true;
		}

		Explored.Add(Unexplored.Pop());
		while (!Unexplored.IsEmpty())
		{
			FCoords ClosestE = Explored[0];
			FCoords ClosestU = Unexplored[0];
			int32 MinDistSquared = INT_MAX;
			for (const auto E : Explored)
			{
				for (const auto U : Unexplored)
				{
					const int32 DistSquared = (E.X - U.X) * (E.X - U.X) + (E.Y - U.Y) * (E.Y - U.Y);
					if (DistSquared < MinDistSquared)
					{
						MinDistSquared = DistSquared;
						ClosestE = E;
						ClosestU = U;
					}
				}
			}
			Unexplored.Remove(ClosestU);
			Explored.Add(ClosestU);
			EdgesCoords.Add({ClosestE, ClosestU});
		}

		for (const auto& Edge : EdgesCoords)
		{
			SLTileMap::DrawLine(Manager->Tilemap, Color, Edge.Key, Edge.Value);
		}
		return true;
	}
};











USTRUCT(BlueprintType)
struct FOpDistanceField : public FTilemapOp
{
	GENERATED_BODY()

	FOpDistanceField()
	{
	}
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Bitmask, BitmaskEnum = "/Script/SLTilemap.ETileMapColor"))
	uint8 WalkableColor = 0;
	
	
	virtual bool Execute(ASLTilemapManager* Manager) override
	{
		Manager->DistanceField = SLTileMap::DistanceField(Manager->Tilemap, WalkableColor);
		return true;
	}
};







USTRUCT(BlueprintType)
struct FOpSpawnPOIs : public FTilemapOp
{
	GENERATED_BODY()

	FOpSpawnPOIs()
	{
	}

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSubclassOf<ASLTileMapPOI>> POIClassesToChooseFrom;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ExcludeBaseStruct))
	TArray<TInstancedStruct<FSpawnPointGenerator>> SpawnPointGenerators;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ExcludeBaseStruct))
	TArray<TInstancedStruct<FSpawnPointFilter>> SpawnPointFilters;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName PointsName = NAME_None;
	
	virtual bool Execute(ASLTilemapManager* Manager) override
	{
		//UWorld* World = Manager->GetWorld();
		UWorld* World = Manager->GeneratedLevel->GetLoadedLevel()->GetWorld();
		if (World)
		{
			TArray<int32> Candidates;

			for (auto Generator : SpawnPointGenerators)
			{
				if (FSpawnPointGenerator* G = Generator.GetMutablePtr<>())
				{
					G->Generate(Candidates, Manager);
				}
			}

			for (auto Filter : SpawnPointFilters)
			{
				if (FSpawnPointFilter* F = Filter.GetMutablePtr<>())
				{
					F->FilterAndSort(Candidates, Manager);
				}
			}

			if (Candidates.Num() == 0)
			{
				UE_LOG(LogTemp, Log, TEXT("There were no candidates"));
				return false;
			}

			Manager->NameToCoordsSet.FindOrAdd(PointsName).Coords.Append(Candidates);

			for (const auto Index : Candidates)
			{
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
			}
		}
		return true;
	}
};






*/