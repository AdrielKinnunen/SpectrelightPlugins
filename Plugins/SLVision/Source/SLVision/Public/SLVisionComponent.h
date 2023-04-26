// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "SLVisionTypes.h"
#include "SLVisionComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SLVISION_API USLVisionComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vision")
	EVisionShape VisionShape = EVisionShape::Circle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vision")
	float VisionRadius = 2000;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vision")
	float VisionCloseRadius = 200;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vision")
	float VisionSlope = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vision")
	float DistanceBetweenPoints = 100;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vision")
	TArray<FVector> RelativeTargetPoints;

	UFUNCTION(Blueprintcallable, Category = "Vision")
	void CalculateRelativeTargetPoints();

	// Sets default values for this component's properties
	USLVisionComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;


public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
};
