// Copyright Spectrelight Studios, LLC

#pragma once

#include "CoreMinimal.h"
#include "SLMDeviceBase.h"
#include "Domains/SLMDomainRotation.h"
#include "Domains/SLMDomainSignal.h"
#include "SLMDeviceWheel.generated.h"

class USLMDeviceSubsystemWheel;

USTRUCT(BlueprintType)
struct FSLMDeviceModelWheel
{
	GENERATED_BODY()

	//Settings
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	float Radius = 50;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	float FrictionCoefficient = 1.0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	float MaxSteerAngle = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	float SteerRate = 30;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	float BrakeMaxTorque = 5000;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	float TestImpulseMultiplier = -0.0001;

	//Set by hit event
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	UPrimitiveComponent* Collider;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	FVector ContactPatchLocation;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	FVector ContactPatchNormal;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	float NormalImpulseMagnitude;

	//PreSimulate
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	float SteerAngle;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	float WheelMass;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	float ImpulseBudget;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	FVector DirectionWheelAxis;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	FVector DirectionLong;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	FVector DirectionLat;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	FVector Velocity;

	//Simulate
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	FVector ImpulseAccumulator;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	int32 Index_Mech_Drive = -1;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	int32 Index_Signal_Steer = -1;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	int32 Index_Signal_Brake = -1;
};

USTRUCT(BlueprintType)
struct FSLMDeviceCosmeticsWheel
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	float AngularVelocityDegrees;

	
/*
	void PreSimulate()
	{
		ImpulseBudget = NormalImpulseMagnitude * FrictionCoefficient;
		//DirectionWheelAxis = Collider->GetRightVector();
		DirectionWheelAxis = FVector(0,1,0);
		DirectionLong = FVector::CrossProduct(DirectionWheelAxis, ContactPatchNormal);
		DirectionLat = FVector::CrossProduct(ContactPatchNormal, DirectionLong);
		//WheelVelocity = WheelColliderComponent->GetComponentVelocity();
		Velocity = FVector(100,0,0);
	}
*/
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


UCLASS(ClassGroup=("SLMechatronics"), meta=(BlueprintSpawnableComponent))
class SLMECHATRONICS_API USLMDeviceComponentWheel : public USLMDeviceComponentBase
{
	GENERATED_BODY()
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	USLMDeviceSubsystemWheel* Subsystem;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	FSLMDeviceWheel DeviceSettings;

	UFUNCTION(BlueprintCallable, Category = "SLMechatronics")
	FSLMDeviceModelWheel GetDeviceState();
	UFUNCTION(BlueprintCallable, Category = "SLMechatronics")
	FSLMDeviceCosmeticsWheel GetDeviceCosmetics();

	UFUNCTION(BlueprintCallable, Category = "SLMechatronics")
	void SendHitData(UPrimitiveComponent* Primitive, FVector Location, FVector Normal, FVector NormalImpulse);

	/*
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	FName WheelColliderComponentName;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	UPrimitiveComponent* WheelColliderComponent = nullptr;

	UFUNCTION(Category = "SLMechatronics")
	void OnComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	*/

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
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

	UFUNCTION(BlueprintCallable, Category = "SLMechatronics")
	int32 AddDevice(FSLMDeviceWheel Device);
	UFUNCTION(BlueprintCallable, Category = "SLMechatronics")
	void RemoveDevice(const int32 DeviceIndex);
	UFUNCTION(BlueprintCallable, Category = "SLMechatronics")
	FSLMDeviceModelWheel GetDeviceState(const int32 DeviceIndex);
	UFUNCTION(BlueprintCallable, Category = "SLMechatronics")
	FSLMDeviceCosmeticsWheel GetDeviceCosmetics(const int32 DeviceIndex);

	//TESTING
	UFUNCTION(BlueprintCallable, Category = "SLMechatronics")
	void SendHitData(const int32 DeviceIndex, UPrimitiveComponent* Primitive, FVector Location, FVector Normal, FVector NormalImpulse);

	
private:
	TWeakObjectPtr<USLMDomainRotation> DomainRotation;
	TWeakObjectPtr<USLMDomainSignal> DomainSignal;
	TSparseArray<FSLMDeviceModelWheel> DeviceModels;
	TSparseArray<FSLMDeviceCosmeticsWheel> DeviceCosmetics;


	//TMap<TWeakObjectPtr<UPrimitiveComponent>, int32> PrimitiveToIndex;
	//TArray<FHitResult> HitResultBuffer;

	//void AddHitResultToBuffer(FHitResult NewHitResult);
	//void ProcessHitResult(FHitResult HitResult);
};
