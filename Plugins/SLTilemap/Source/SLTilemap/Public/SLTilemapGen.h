// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SLTilemapCore.h"
#include "SLTilemapPaint.h"
#include "SLTilemapPatterns.h"
#include "SLTilemapWFC.h"
#include "CompGeom/Delaunay2.h"
#include "StructUtils/InstancedStruct.h"
#include "SLTilemapGen.generated.h"


USTRUCT(BlueprintType)
struct FTileGenBlackboard
{
	GENERATED_BODY()
	TMultiMap<FName, FTileMapCoords> NameToCoords;
};


USTRUCT(BlueprintType)
struct FTileGenOp
{
	GENERATED_BODY()
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FName Name = "Gen Op";
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 Color = 0;

	virtual ~FTileGenOp()
	{
	}

	virtual void Execute(FTileMap& TileMap, FTileGenBlackboard& Blackboard, FRandomStream& RandomStream)
	{
	};
};


USTRUCT(BlueprintType)
struct FTileGenOpsStack
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ExcludeBaseStruct))
	TArray<TInstancedStruct<FTileGenOp>> Ops;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTileGenBlackboard Blackboard;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRandomStream RandomStream;
};


USTRUCT(BlueprintType)
struct FTileGenOpRandomPoints : public FTileGenOp
{
	GENERATED_BODY()

	FTileGenOpRandomPoints()
	{
		Name = "RandomPoints";
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 XMin = 3;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 XMax = 30;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 YMin = 3;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 YMax = 30;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 NumPoints = 5;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName PointsName = "RandomPoints";
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bPaintPoints = true;


	virtual void Execute(FTileMap& TileMap, FTileGenBlackboard& Blackboard, FRandomStream& RandomStream) override
	{
		for (int32 i = 0; i<NumPoints; i++)
		{
			const int32 X = RandomStream.RandRange(XMin, XMax);
			const int32 Y = RandomStream.RandRange(YMin, YMax);
			Blackboard.NameToCoords.AddUnique(PointsName, FTileMapCoords(X, Y));
			if (bPaintPoints)
			{
				SLTileMap::SetTile(TileMap, Color, FTileMapCoords(X, Y));
			}
		}
	};
};


USTRUCT(BlueprintType)
struct FTileGenOpMinimumSpanningTree : public FTileGenOp
{
	GENERATED_BODY()

	FTileGenOpMinimumSpanningTree()
	{
		Name = "MinimumSpanningTree";
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName PointsName = "RandomPoints";

	virtual void Execute(FTileMap& TileMap, FTileGenBlackboard& Blackboard, FRandomStream& RandomStream) override
	{
		TArray<TTuple<FTileMapCoords, FTileMapCoords>> EdgesCoords;
		//TArray<FTileIndexPair> Edges;
		TArray<FTileMapCoords> Explored;
		TArray<FTileMapCoords> Unexplored;
		Blackboard.NameToCoords.MultiFind(PointsName, Unexplored);
		if (Unexplored.Num()<2)
		{
			return;
		}

		Explored.Add(Unexplored.Pop());
		while (!Unexplored.IsEmpty())
		{
			FTileMapCoords ClosestE = Explored[0];
			FTileMapCoords ClosestU = Unexplored[0];
			int32 MinDistSquared = INT_MAX;
			for (const auto E : Explored)
			{
				for (const auto U : Unexplored)
				{
					const int32 DistSquared = (E.X-U.X)*(E.X-U.X)+(E.Y-U.Y)*(E.Y-U.Y);
					if (DistSquared<MinDistSquared)
					{
						MinDistSquared = DistSquared;
						ClosestE = E;
						ClosestU = U;
					}
				}
			}
			Unexplored.Remove(ClosestU);
			Explored.Add(ClosestU);
			//const int32 IndexE = SLTileMap::CoordsToIndex(ClosestE, TileMap.Size);
			//const int32 IndexU = SLTileMap::CoordsToIndex(ClosestU, TileMap.Size);
			//Edges.Add({IndexE, IndexU});
			EdgesCoords.Add({ClosestE, ClosestU});
		}

		for (const auto& Edge : EdgesCoords)
		{
			SLTileMap::DrawLine(TileMap, Color, Edge.Key, Edge.Value);
		}
	}
};


USTRUCT(BlueprintType)
struct FTileGenOpDelaunay : public FTileGenOp
{
	GENERATED_BODY()

	FTileGenOpDelaunay()
	{
		Name = "Delaunay";
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName PointsName = "RandomPoints";


	virtual void Execute(FTileMap& TileMap, FTileGenBlackboard& Blackboard, FRandomStream& RandomStream) override
	{
		TArray<FTileMapCoords> Points;
		Blackboard.NameToCoords.MultiFind(PointsName, Points);
		if (Points.Num()<3)
		{
			return;
		}

		TArray<UE::Math::TVector2<float>> PointsF;
		PointsF.Reserve(Points.Num());
		for (const auto& Point : Points)
		{
			PointsF.Add({static_cast<float>(Point.X), static_cast<float>(Point.Y)});
		}

		UE::Geometry::FDelaunay2 Del;
		Del.Triangulate(PointsF);

		TSet<FTileIndexPair> Edges;

		const TArray<UE::Geometry::FIndex3i> Tris = Del.GetTriangles();
		for (const auto& Tri : Tris)
		{
			Edges.Add({Tri.A, Tri.B});
			Edges.Add({Tri.B, Tri.C});
			Edges.Add({Tri.C, Tri.A});
		}


		TArray<TTuple<FTileMapCoords, FTileMapCoords>> EdgesCoords;
		for (const auto& Edge : Edges)
		{
			EdgesCoords.Add({Points[Edge.A], Points[Edge.B]});
		}

		for (const auto& Edge : EdgesCoords)
		{
			SLTileMap::DrawLine(TileMap, Color, Edge.Key, Edge.Value);
		}
	}
};


USTRUCT(BlueprintType)
struct FTileGenOpInitialize : public FTileGenOp
{
	GENERATED_BODY()

	FTileGenOpInitialize()
	{
		Name = "Initialize";
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 SizeX = 150;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 SizeY = 75;

	virtual void Execute(FTileMap& TileMap, FTileGenBlackboard& Blackboard, FRandomStream& RandomStream) override
	{
		TileMap = FTileMap({SizeX, SizeY}, Color);
	};
};


USTRUCT(BlueprintType)
struct FTileGenOpBorder : public FTileGenOp
{
	GENERATED_BODY()

	FTileGenOpBorder()
	{
		Name = "Border";
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Thickness = 1;

	virtual void Execute(FTileMap& TileMap, FTileGenBlackboard& Blackboard, FRandomStream& RandomStream) override
	{
		SLTileMap::SetBorder(TileMap, Color, Thickness);
	};
};


USTRUCT(BlueprintType)
struct FTileGenOpRectangle : public FTileGenOp
{
	GENERATED_BODY()

	FTileGenOpRectangle()
	{
		Name = "Rectangle";
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTileMapCoords Size = {5, 5};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTileMapCoords Center = {10, 10};


	virtual void Execute(FTileMap& TileMap, FTileGenBlackboard& Blackboard, FRandomStream& RandomStream) override
	{
		const int32 StartX = Center.X-Size.X/2;
		const int32 StartY = Center.Y-Size.Y/2;
		for (int32 j = 0; j<Size.Y; j++)
		{
			for (int32 i = 0; i<Size.X; i++)
			{
				const FTileMapCoords Coords = FTileMapCoords(StartX+i, StartY+j);
				SLTileMap::SetTileChecked(TileMap, Color, Coords);
			}
		}
	};
};


USTRUCT(BlueprintType)
struct FTileGenOpEllipse : public FTileGenOp
{
	GENERATED_BODY()

	FTileGenOpEllipse()
	{
		Name = "Ellipse";
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTileMapCoords Radius = {5, 5};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTileMapCoords Center = {20, 20};

	virtual void Execute(FTileMap& TileMap, FTileGenBlackboard& Blackboard, FRandomStream& RandomStream) override
	{
		const int32 RadiusX = Radius.X;
		const int32 RadiusY = Radius.Y;
		const int32 CenterX = Center.X;
		const int32 CenterY = Center.Y;


		for (int32 i = 0; i<TileMap.Data.Num(); i++)
		{
			const FTileMapCoords Coords = SLTileMap::IndexToCoords(i, TileMap.Size);
			const int32 dx = Coords.X-CenterX;
			const int32 dy = Coords.Y-CenterY;
			const int32 lhs = dx*dx*RadiusY*RadiusY+dy*dy*RadiusX*RadiusX+1;
			const int32 rhs = RadiusX*RadiusX*RadiusY*RadiusY;
			if (lhs<=rhs)
			{
				SLTileMap::SetTile(TileMap, Color, Coords);
			}
		}
	}
};


USTRUCT(BlueprintType)
struct FTileGenOpWFC : public FTileGenOp
{
	GENERATED_BODY()

	FTileGenOpWFC()
	{
		Name = "WFC";
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTilePatternSet PatternSet;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FWFCOptions Options;

	virtual void Execute(FTileMap& TileMap, FTileGenBlackboard& Blackboard, FRandomStream& RandomStream) override
	{
		Options.RandomSeed = RandomStream.GetUnsignedInt();
		USLWave* Wave = NewObject<USLWave>();
		Wave->InitializeWithOptions(TileMap, PatternSet, Options);
		Wave->Run();
		TileMap = Wave->OutputTileMap;
	}
};


namespace SLTileMap
{
	inline void ApplyGenOpsStack(FTileMap& TileMap, FTileGenOpsStack& GenOpsStack, const int32 Seed)
	{
		GenOpsStack.RandomStream.Initialize(Seed);
		GenOpsStack.Blackboard.NameToCoords.Empty();
		for (auto Op : GenOpsStack.Ops)
		{
			if (FTileGenOp* P = Op.GetMutablePtr<>())
			{
				P->Execute(TileMap, GenOpsStack.Blackboard, GenOpsStack.RandomStream);
			}
		}
	}
}
