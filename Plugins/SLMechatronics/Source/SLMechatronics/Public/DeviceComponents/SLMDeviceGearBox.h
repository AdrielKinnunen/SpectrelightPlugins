// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SLMechatronicsDeviceComponent.h"
#include "Components/ActorComponent.h"
#include "SLMDeviceGearBox.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SLMECHATRONICS_API USLMDeviceGearBox : public USLMechatronicsDeviceComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	USLMDeviceGearBox();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	float GearRatio;

	int32 FirstShaftIndex;
	int32 SecondShaftIndex;
	

	virtual void Simulate(float DeltaTime) override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
};
