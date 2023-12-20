// Copyright Spectrelight Studios, LLC

/*
#pragma once

#include "CoreMinimal.h"
#include "SLMDeviceBase.h"
#include "Domains/SLMDomainRotation.h"
#include "Domains/SLMDomainSignal.h"
#include "SLMDeviceWheel.generated.h"


USTRUCT(BlueprintType)
struct FSLMDeviceModelWheel
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	float WheelRadiuscm = 50;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	float FrictionCoefficient = 1.0;

	//bool bIsTouching = false;
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
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	int32 Index_Mech_Drive = -1;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	int32 Index_Signal_Steer = -1;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	int32 Index_Signal_Brake = -1;
};

USTRUCT(BlueprintType)
struct FSLMDeviceWheel
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	FSLMDeviceModelWheel DeviceModel;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	FSLMPortRotation Port_Rotation_Drive;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	FSLMPortSignal Port_Signal_Steer;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	FSLMPortSignal Port_Signal_Brake;
};



UCLASS()
class SLMECHATRONICS_API USLMDeviceSubsystemWheel : public USLMDeviceSubsystemBase
{
	GENERATED_BODY()
public:
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	virtual void PreSimulate(float DeltaTime) override;
	virtual void Simulate(float DeltaTime) override;
	virtual void PostSimulate(float DeltaTime) override;
	void AddInstance(const FSLMDeviceModelWheel& Instance);
private:
	UPROPERTY()
	USLMDomainRotation* DomainRotation;
	TArray<FSLMDeviceModelWheel> Instances;

}; 

UCLASS(ClassGroup=("SLMechatronics"), meta=(BlueprintSpawnableComponent))
class SLMECHATRONICS_API USLMDeviceComponentWheel : public USLMDeviceComponentBase
{
	GENERATED_BODY()
public:
	USLMDeviceComponentWheel();
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	FSLMDeviceModelWheel DeviceModel = FSLMDeviceModelWheel();
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	FSLMPortRotation Port_Mech_Drive;
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	//FSLMPort Port_Signal_Steer;
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	//FSLMPort Port_Signal_Brake;

	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	FName WheelColliderComponentName;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	UPrimitiveComponent* WheelColliderComponent = nullptr;

	UFUNCTION(Category = "SLMechatronics")
	void OnComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	
	

protected:
	UPROPERTY()
	USLMDomainRotation* DomainRotation;
	
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};
*/