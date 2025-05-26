// Copyright Spectrelight Studios, LLC

#pragma once

#include "CoreMinimal.h"
#include "SLMDeviceBase.h"
#include "Domains/SLMDomainRotation.h"
#include "Domains/SLMDomainSignal.h"
#include "SLMDeviceWheel.generated.h"

class USLMDeviceSubsystemWheel;

UENUM(BlueprintType)
enum class ETireModel : uint8
{
	None,
	StaticFriction,
	Pacejka,
	Brush
};


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

    //Set by hit event
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
    UPrimitiveComponent* Collider = nullptr;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
    FVector ContactPatchLocation = FVector(0,0,0);
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
    FVector ContactPatchNormal = FVector(0,0,1);
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
    float NormalImpulseMagnitude = 0;

    //PreSimulate
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
    float SteerAngle = 0;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
    float WheelMass = 1;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
    float ImpulseBudget = 0;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
    FVector DirectionWheelAxis = FVector(0,1,0);
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
    FVector DirectionLong = FVector(1,0,0);
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
    FVector DirectionLat = FVector(0,1,0);
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
    FVector Velocity = FVector(0,0,0);

    //Simulate
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
    FVector ImpulseAccumulator = FVector(0,0,0);;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	float SlipSpeed = 0;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
    int32 Index_Mech_Drive = -1;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
    int32 Index_Signal_Steer = -1;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
    int32 Index_Signal_Brake = -1;
	
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	ETireModel TireModel = ETireModel::StaticFriction;
};


USTRUCT(BlueprintType)
struct FSLMDeviceCosmeticsWheel
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
    float AngularVelocityDegrees = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	float SlipRatio = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	float Load = 0;
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
    void OnComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, FHitResult& Hit);
    */

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;
};


UCLASS()
class SLMECHATRONICS_API USLMDeviceSubsystemWheel : public USLMDeviceSubsystemBase
{
    GENERATED_BODY()
public:
    virtual void OnWorldBeginPlay(UWorld& InWorld) override;
    virtual void PreSimulate(float DeltaTime) override;
    virtual void Simulate(float DeltaTime, float SubstepScalar) override;
    virtual void PostSimulate(float DeltaTime) override;

    UFUNCTION(BlueprintCallable, Category = "SLMechatronics")
    int32 AddDevice(FSLMDeviceWheel Device);
    UFUNCTION(BlueprintCallable, Category = "SLMechatronics")
    void RemoveDevice(int32 DeviceIndex);
    UFUNCTION(BlueprintCallable, Category = "SLMechatronics")
    FSLMDeviceModelWheel GetDeviceState(int32 DeviceIndex);
    UFUNCTION(BlueprintCallable, Category = "SLMechatronics")
    FSLMDeviceCosmeticsWheel GetDeviceCosmetics(int32 DeviceIndex);

    //TESTING
    UFUNCTION(BlueprintCallable, Category = "SLMechatronics")
    void SendHitData(int32 DeviceIndex, UPrimitiveComponent* Primitive, FVector Location, FVector Normal, FVector NormalImpulse);
private:
    TWeakObjectPtr<USLMDomainRotation> DomainRotation;
    TWeakObjectPtr<USLMDomainSignal> DomainSignal;
    TSparseArray<FSLMDeviceModelWheel> DeviceModels;
    TSparseArray<FSLMDeviceCosmeticsWheel> DeviceCosmetics;

	void DoStaticFriction(FSLMDeviceModelWheel& Wheel, float DeltaTime, float SubstepScalar);
	void DoPacejka(FSLMDeviceModelWheel& Wheel, float DeltaTime, float SubstepScalar);
	void DoBrush(FSLMDeviceModelWheel& Wheel, float DeltaTime, float SubstepScalar);

	//FTestThingCallback* TestThingCallback;

    //TMap<TWeakObjectPtr<UPrimitiveComponent>, int32> PrimitiveToIndex;
    //TArray<FHitResult> HitResultBuffer;

    //void AddHitResultToBuffer(FHitResult NewHitResult);
    //void ProcessHitResult(FHitResult HitResult);
};
