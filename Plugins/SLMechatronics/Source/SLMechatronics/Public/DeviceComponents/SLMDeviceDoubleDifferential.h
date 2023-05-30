// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SLMechatronicsDeviceComponent.h"
#include "SLMDeviceDoubleDifferential.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SLMECHATRONICS_API USLMDeviceDoubleDifferential : public USLMechatronicsDeviceComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	USLMDeviceDoubleDifferential();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	float DriveRatio;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	float SteerRatio;

	int32 DriveShaftIndex;
	int32 SteerShaftIndex;
	int32 LeftShaftIndex;
	int32 RightShaftIndex;

	virtual void Simulate(float DeltaTime) override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
};