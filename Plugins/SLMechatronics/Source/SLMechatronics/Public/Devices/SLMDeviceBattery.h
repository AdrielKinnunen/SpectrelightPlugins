// Copyright Spectrelight Studios, LLC

#pragma once

#include "CoreMinimal.h"
#include "SLMDeviceBase.h"
#include "Domains/SLMDomainElectricity.h"
#include "Domains/SLMDomainSignal.h"
#include "SLMDeviceBattery.generated.h"

class USLMDeviceSubsystemBattery;


USTRUCT(BlueprintType)
struct FSLMDeviceModelBattery
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
    float PowerWatts = 10000;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
    float EnergyJoules = 5000;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
    float CapacityJoules = 10000;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
    float ChargeDischargeTriggerPercent = 0.5;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
    int32 Index_Electricity = -1;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
    int32 Index_Signal_ChargePercent = -1;
};


USTRUCT(BlueprintType)
struct FSLMDeviceBattery
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
    FSLMDeviceModelBattery DeviceModel;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
    FSLMPortElectricity Port_Electricity;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
    FSLMPortSignal Port_Signal_ChargePercent;
};


UCLASS(ClassGroup=("SLMechatronics"), meta=(BlueprintSpawnableComponent))
class SLMECHATRONICS_API USLMDeviceComponentBattery : public USLMDeviceComponentBase
{
    GENERATED_BODY()
public:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
    USLMDeviceSubsystemBattery* Subsystem;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
    FSLMDeviceBattery DeviceSettings;

    UFUNCTION(BlueprintCallable, Category = "SLMechatronics")
    FSLMDeviceModelBattery GetDeviceState();
protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};


UCLASS()
class SLMECHATRONICS_API USLMDeviceSubsystemBattery : public USLMDeviceSubsystemBase
{
    GENERATED_BODY()
public:
    virtual void OnWorldBeginPlay(UWorld& InWorld) override;
    virtual void PreSimulate(const float DeltaTime) override;
    virtual void Simulate(const float DeltaTime, const float SubstepScalar) override;
    virtual void PostSimulate(const float DeltaTime) override;

    UFUNCTION(BlueprintCallable, Category = "SLMechatronics")
    int32 AddDevice(FSLMDeviceBattery Device);
    UFUNCTION(BlueprintCallable, Category = "SLMechatronics")
    void RemoveDevice(const int32 DeviceIndex);
    UFUNCTION(BlueprintCallable, Category = "SLMechatronics")
    FSLMDeviceModelBattery GetDeviceState(const int32 DeviceIndex);
private:
    TWeakObjectPtr<USLMDomainElectricity> DomainElectricity;
    TWeakObjectPtr<USLMDomainSignal> DomainSignal;
    TSparseArray<FSLMDeviceModelBattery> DeviceModels;
};
