// Copyright Spectrelight Studios, LLC

/*
#pragma once

#include "CoreMinimal.h"
#include "SLMDeviceBase.h"
#include "Domains/SLMDomainMech.h"
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
	
	int32 Index_Mech_Drive = -1;
	//int32 Index_Signal_Steer = -1;
	//int32 Index_Signal_Brake = -1;
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
	USLMDomainMech* DomainMech;
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
	FSLMPortMech Port_Mech_Drive;
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
	USLMDomainMech* DomainMech;
	
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};




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
	int32 RotationIndex;
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
*/
