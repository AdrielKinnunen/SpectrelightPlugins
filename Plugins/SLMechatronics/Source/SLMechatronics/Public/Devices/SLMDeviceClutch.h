// Copyright Spectrelight Studios, LLC

#pragma once

#include "CoreMinimal.h"
#include "SLMDeviceBase.h"
#include "Domains/SLMDomainRotation.h"
#include "Domains/SLMDomainSignal.h"
#include "SLMDeviceClutch.generated.h"

class USLMDeviceSubsystemClutch;


USTRUCT(BlueprintType)
struct FSLMDeviceModelClutch
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
    float MaxTorque = 0.0;
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "SLMechatronics")
    int32 Index_Rotation_Input = -1;
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "SLMechatronics")
    int32 Index_Rotation_Output = -1;
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "SLMechatronics")
    int32 Index_Signal_Engage = -1;
};


USTRUCT(BlueprintType)
struct FSLMDeviceClutch
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
    FSLMDeviceModelClutch DeviceModel;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
    FSLMPortRotation Port_Rotation_Input;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
    FSLMPortRotation Port_Rotation_Output;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
    FSLMPortSignal Port_Signal_Engage;
};


UCLASS(ClassGroup=("SLMechatronics"), meta=(BlueprintSpawnableComponent))
class SLMECHATRONICS_API USLMDeviceComponentClutch : public USLMDeviceComponentBase
{
    GENERATED_BODY()
public:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
    USLMDeviceSubsystemClutch* Subsystem;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
    FSLMDeviceClutch DeviceSettings;

    UFUNCTION(BlueprintCallable, Category = "SLMechatronics")
    FSLMDeviceModelClutch GetDeviceState();

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};


UCLASS()
class SLMECHATRONICS_API USLMDeviceSubsystemClutch : public USLMDeviceSubsystemBase
{
    GENERATED_BODY()
public:
    virtual void OnWorldBeginPlay(UWorld& InWorld) override;
    virtual void PreSimulate(const float DeltaTime) override;
    virtual void Simulate(const float DeltaTime, const float SubstepScalar) override;
    virtual void PostSimulate(const float DeltaTime) override;

    UFUNCTION(BlueprintCallable, Category = "SLMechatronics")
    int32 AddDevice(FSLMDeviceClutch Device);
    UFUNCTION(BlueprintCallable, Category = "SLMechatronics")
    void RemoveDevice(const int32 DeviceIndex);
    UFUNCTION(BlueprintCallable, Category = "SLMechatronics")
    FSLMDeviceModelClutch GetDeviceState(const int32 DeviceIndex);

private:
    TWeakObjectPtr<USLMDomainRotation> DomainRotation;
    TWeakObjectPtr<USLMDomainSignal> DomainSignal;
    TSparseArray<FSLMDeviceModelClutch> DeviceModels;
};
