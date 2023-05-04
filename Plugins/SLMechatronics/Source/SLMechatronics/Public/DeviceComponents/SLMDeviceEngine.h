// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SLMechatronicsDeviceComponent.h"
#include "SLMDeviceEngine.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SLMECHATRONICS_API USLMDeviceEngine : public USLMechatronicsDeviceComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	USLMDeviceEngine();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	float MaxTorque = 100;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	float Throttle = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	float MaxRPM = 1000;
	
	int32 CrankIndex;
	

	virtual void Simulate(float DeltaTime) override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
};
