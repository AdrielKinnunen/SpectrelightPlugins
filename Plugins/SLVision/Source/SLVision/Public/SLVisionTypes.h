// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//#include "CoreMinimal.h"
#include "SLVisionTypes.generated.h"


USTRUCT(BlueprintType)
struct FVisionPolygon
{
	GENERATED_BODY()
	FVisionPolygon()
	{
	}

	FVisionPolygon(const FVector2D NewOrigin, const TArray<FVector2D> NewVertices)
	{
		Origin = NewOrigin;
		Vertices = NewVertices;
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLVision")
	FVector2D Origin;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLVision")
	TArray<FVector2D> Vertices;
};




UENUM(BlueprintType)
enum class EVisionShape : uint8
{
	Circle,
	Directional
};
