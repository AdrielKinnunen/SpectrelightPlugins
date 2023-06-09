﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SLMechatronicsDeviceComponent.h"
#include "SLMDeviceSimpleWheel.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SLMECHATRONICS_API USLMDeviceSimpleWheel : public USLMechatronicsDeviceComponent
{
	GENERATED_BODY()

public:
	USLMDeviceSimpleWheel();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	float WheelRadiuscm = 50;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	float FrictionCoefficient = 1.0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	FName WheelColliderComponentName;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	UPrimitiveComponent* WheelColliderComponent = nullptr;
	
	virtual void PreSimulate(float DeltaTime) override;
	virtual void Simulate(float DeltaTime) override;
	virtual void PostSimulate(float DeltaTime) override;
	UFUNCTION(Category = "SLMechatronics")
	void OnComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	int32 DrivetrainIndex;
	int32 SteerIndex;
	int32 BrakeIndex;

	bool bIsTouching = false;
	FVector ContactPatchLocation;
	FVector ContactPatchNormal;
	float NormalImpulseMagnitude;

	float WheelMass;
	float ImpulseBudget;

	FVector DirectionWheelAxis;
	FVector DirectionLong;
	FVector DirectionLat;

	FVector WheelVelocity;
	FVector SlipVelocityWorld;
	FVector SlipVelocityLocal;

	FRotator ContactPatchOrientation;
};
