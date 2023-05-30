// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SLMechatronicsDeviceComponent.h"
#include "SLMDeviceDifferential.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SLMECHATRONICS_API USLMDeviceDifferential : public USLMechatronicsDeviceComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	USLMDeviceDifferential();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	float FinalDriveRatio;

	int32 InputShaftIndex;
	int32 LeftShaftIndex;
	int32 RightShaftIndex;

	virtual void Simulate(float DeltaTime) override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
};
