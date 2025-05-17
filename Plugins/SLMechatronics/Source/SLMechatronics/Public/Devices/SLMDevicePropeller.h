// Copyright Spectrelight Studios, LLC

#pragma once

#include "CoreMinimal.h"
#include "SLMDeviceBase.h"
#include "Domains/SLMDomainRotation.h"
#include "SLMDevicePropeller.generated.h"

class USLMDeviceSubsystemPropeller;


USTRUCT(BlueprintType)
struct FSLMDeviceModelPropeller
{
    GENERATED_BODY()

    //Settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
    float Radius_cm = 50;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
    float BladePitch_Deg = 30;
	
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
    UPrimitiveComponent* Primitive = nullptr;
	
    //PreSimulate
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
    FVector DirectionPropellerAxis = FVector(1,0,0);
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
    float AxialSpeed_ms = 0.0;

    //Simulate
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
    float ForceAccumulator_N = 0.0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
    int32 Index_Rotation_Drive = -1;
};


USTRUCT(BlueprintType)
struct FSLMDeviceCosmeticsPropeller
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
    float AngularVelocityDegrees = 0.0;
};


USTRUCT(BlueprintType)
struct FSLMDevicePropeller
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
    FSLMDeviceModelPropeller DeviceModel;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
    FSLMPortRotation Port_Rotation_Drive;
};


UCLASS(ClassGroup=("SLMechatronics"), meta=(BlueprintSpawnableComponent))
class SLMECHATRONICS_API USLMDeviceComponentPropeller : public USLMDeviceComponentBase
{
    GENERATED_BODY()
public:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
    USLMDeviceSubsystemPropeller* Subsystem;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
    FSLMDevicePropeller DeviceSettings;

    UFUNCTION(BlueprintCallable, Category = "SLMechatronics")
    FSLMDeviceModelPropeller GetDeviceState();
    UFUNCTION(BlueprintCallable, Category = "SLMechatronics")
    FSLMDeviceCosmeticsPropeller GetDeviceCosmetics();

    UFUNCTION(BlueprintCallable, Category = "SLMechatronics")
    void SetPrimitive(UPrimitiveComponent* Primitive);

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;
};


UCLASS()
class SLMECHATRONICS_API USLMDeviceSubsystemPropeller : public USLMDeviceSubsystemBase
{
    GENERATED_BODY()
public:
    virtual void OnWorldBeginPlay(UWorld& InWorld) override;
    virtual void PreSimulate(float DeltaTime) override;
    virtual void Simulate(float DeltaTime, float SubstepScalar) override;
    virtual void PostSimulate(float DeltaTime) override;

    UFUNCTION(BlueprintCallable, Category = "SLMechatronics")
    int32 AddDevice(FSLMDevicePropeller Device);
    UFUNCTION(BlueprintCallable, Category = "SLMechatronics")
    void RemoveDevice(int32 DeviceIndex);
    UFUNCTION(BlueprintCallable, Category = "SLMechatronics")
    FSLMDeviceModelPropeller GetDeviceState(int32 DeviceIndex);
    UFUNCTION(BlueprintCallable, Category = "SLMechatronics")
    FSLMDeviceCosmeticsPropeller GetDeviceCosmetics(int32 DeviceIndex);
	UFUNCTION(BlueprintCallable, Category = "SLMechatronics")
	void SetPrimitive(int32 DeviceIndex, UPrimitiveComponent* Primitive);

private:
    TWeakObjectPtr<USLMDomainRotation> DomainRotation;
    TSparseArray<FSLMDeviceModelPropeller> DeviceModels;
    TSparseArray<FSLMDeviceCosmeticsPropeller> DeviceCosmetics;
};
