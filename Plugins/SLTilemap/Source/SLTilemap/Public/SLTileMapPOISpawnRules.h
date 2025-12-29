// Copyright Spectrelight Studios, LLC

#pragma once

#include "CoreMinimal.h"
#include "SLTilemapManager.h"
#include "SLTileMapPOISpawnRules.generated.h"

/*
USTRUCT(BlueprintType)
struct FPointsOnGraph : public FSpawnPointGenerator
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName GraphName = "Main Path";
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MinDegree = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxDegree = 8;

	virtual void Generate(TArray<int32>& Candidates, ASLTilemapManager* Manager) override
	{
		const FTileGraph* Graph = Manager->NameToGraph.Find(GraphName);
		if (Graph == nullptr)
		{
			UE_LOG(LogTemp, Log, TEXT("Graph did not exist."));
			return;
		}

		Candidates.Reserve(Graph->Vertices.Num() + Candidates.Num());
		for (const auto Vertex : Graph->Vertices)
		{
			const int32 Degree = Graph->IndexToDegree.FindChecked(Vertex);
			if (Degree >= MinDegree && Degree <= MaxDegree)
			{
				Candidates.Add(Vertex);
			}
		}
	};
};



USTRUCT(BlueprintType)
struct FPointsInDistanceField : public FSpawnPointGenerator
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MinDistance = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxDistance = 4;

	virtual void Generate(TArray<int32>& Candidates, ASLTilemapManager* Manager) override
	{
		const FTilemap& DistanceField = Manager->DistanceField;
		for (int32 i = 0; i < DistanceField.Data.Num(); ++i)
		{
			const int32 Distance = DistanceField.Data[i];
			if (Distance >= MinDistance && Distance <= MaxDistance)
			{
				Candidates.Add(i);
			}
		}
	};
};


USTRUCT(BlueprintType)
struct FPointsByColor : public FSpawnPointGenerator
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Bitmask, BitmaskEnum = "/Script/SLTilemap.ETileMapColor"))
	uint8 Color = 255;

	virtual void Generate(TArray<int32>& Candidates, ASLTilemapManager* Manager) override
	{
		const FTilemap& Tilemap = Manager->Tilemap;
		for (int32 i = 0; i < Tilemap.Data.Num(); ++i)
		{
			if (Tilemap.Data[i] & Color)
			{
				Candidates.Add(i);
			}
		}
	};
};





USTRUCT(BlueprintType)
struct FRuleGraphDegree : public FSpawnPointFilter
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName GraphName = "Main Path";
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 DesiredDegree = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MinDegree = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxDegree = 8;

	virtual void FilterAndSort(TArray<int32>& Candidates, ASLTilemapManager* Manager) override
	{
		const FTileGraph* Graph = Manager->NameToGraph.Find(GraphName);
		if (Graph == nullptr)
		{
			UE_LOG(LogTemp, Log, TEXT("Graph did not exist."));
			return;
		}
		
		//Filter
		for (auto It = Candidates.CreateIterator(); It; ++It)
		{
			const int32 Degree = Graph->IndexToDegree.FindChecked(*It);
			if (Degree < MinDegree || Degree > MaxDegree)
			{
				It.RemoveCurrent();
			}
		}

		//Sort
		Candidates.Sort([Graph, this](int32 A, int32 B)
		{
			const int32 DegreeA = Graph->IndexToDegree.FindChecked(A);
			const int32 DegreeB = Graph->IndexToDegree.FindChecked(B);
			const int32 DistanceA = FMath::Abs(DesiredDegree - DegreeA);
			const int32 DistanceB = FMath::Abs(DesiredDegree - DegreeB);
			return DistanceA < DistanceB;
		});
	};
};



USTRUCT(BlueprintType)
struct FRuleLinearDistanceFrom : public FSpawnPointFilter
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName TargetPoint = "Boss";
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 DesiredDistance = 999;

	virtual void FilterAndSort(TArray<int32>& Candidates, ASLTilemapManager* Manager) override
	{
		const auto& CoordsSet = Manager->NameToCoordsSet.FindOrAdd(TargetPoint);
		if (CoordsSet.Coords.Num() == 0)
		{
			UE_LOG(LogTemp, Log, TEXT("There were no coords to get distance from"));
			return;
		}
		
		const int32 TargetIndex = SLTileMap::CoordsToIndex(CoordsSet.Coords[0], Manager->Tilemap.Size);
		//Sort
		Candidates.Sort([TargetIndex, Manager, this](int32 A, int32 B)
		{
			const FCoords CoordsTarget = SLTileMap::IndexToCoords(TargetIndex, Manager->Tilemap.Size);
			const FCoords CoordsA = SLTileMap::IndexToCoords(A, Manager->Tilemap.Size);
			const FCoords CoordsB = SLTileMap::IndexToCoords(B, Manager->Tilemap.Size);
			const auto TargetLocation = SLTileMap::CoordsToWorldLocation(Manager->Tilemap, CoordsTarget);
			const auto LocationA = SLTileMap::CoordsToWorldLocation(Manager->Tilemap, CoordsA);
			const auto LocationB = SLTileMap::CoordsToWorldLocation(Manager->Tilemap, CoordsB);
			const float DistanceA = FVector::Distance(LocationA, TargetLocation);
			const float DistanceB = FVector::Distance(LocationB, TargetLocation);
			const float ErrorA = FMath::Abs(DesiredDistance - DistanceA);
			const float ErrorB = FMath::Abs(DesiredDistance - DistanceB);
			return ErrorA < ErrorB;
		});
	};
};

USTRUCT(BlueprintType)
struct FRuleGraphDistanceFrom : public FSpawnPointFilter
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName TargetPoint = "Boss";
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName GraphName = "Main Path";
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 DesiredDistance = 999;

	virtual void FilterAndSort(TArray<int32>& Candidates, ASLTilemapManager* Manager) override
	{
		const auto& CoordsSet = Manager->NameToCoordsSet.FindOrAdd(TargetPoint);
		if (CoordsSet.Coords.Num() == 0)
		{
			UE_LOG(LogTemp, Log, TEXT("There were no coords to get distance from"));
			return;
		}

		if (Manager->NameToGraph.Find(GraphName) == nullptr)
		{
			UE_LOG(LogTemp, Log, TEXT("Graph did not exist."));
			return;
		}
		
		const FTileGraph& Graph = Manager->NameToGraph.FindOrAdd(GraphName);
		
		const int32 TargetIndex = SLTileMap::CoordsToIndex(CoordsSet.Coords[0], Manager->Tilemap.Size);
		if (!Graph.Vertices.Contains(TargetIndex))
		{
			UE_LOG(LogTemp, Log, TEXT("Graph did not contain target index."));
			return;
		}
		
		//Sort
		Candidates.Sort([TargetIndex, this, Graph](int32 A, int32 B)
		{
			const int32 DistanceAToTarget = SLTileMap::GetDistance(Graph, A, TargetIndex);
			const int32 DistanceBToTarget = SLTileMap::GetDistance(Graph, B, TargetIndex);
			const float ErrorA = FMath::Abs(DesiredDistance - DistanceAToTarget);
			const float ErrorB = FMath::Abs(DesiredDistance - DistanceBToTarget);
			return ErrorA < ErrorB;
		});
	};
};




USTRUCT(BlueprintType)
struct FRuleDistanceField : public FSpawnPointFilter
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 DesiredDistance = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MinDistance = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxDistance = 8;

	virtual void FilterAndSort(TArray<int32>& Candidates, ASLTilemapManager* Manager) override
	{
		const FTilemap& DistanceField = Manager->DistanceField;
		if (DistanceField.Size != Manager->Tilemap.Size)
		{
			UE_LOG(LogTemp, Log, TEXT("Distance Field did not match manager's tilemap."));
			return;
		}

		//Filter
		for (auto It = Candidates.CreateIterator(); It; ++It)
		{
			const int32 Distance = DistanceField.Data[*It];
			if (Distance < MinDistance || Distance > MaxDistance)
			{
				It.RemoveCurrent();
			}
		}

		//Sort
		Candidates.Sort([DistanceField, this](const int32 A, const int32 B)
		{
			const int32 DistanceA = DistanceField.Data[A];
			const int32 DistanceB = DistanceField.Data[B];
			const int32 DifferenceA = FMath::Abs(DesiredDistance - DistanceA);
			const int32 DifferenceB = FMath::Abs(DesiredDistance - DistanceB);
			return DifferenceA < DifferenceB;
		});
	};
};



*/
