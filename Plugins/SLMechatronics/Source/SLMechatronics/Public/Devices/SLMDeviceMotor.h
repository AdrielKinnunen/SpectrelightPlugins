// Copyright Spectrelight Studios, LLC

#pragma once

#include "CoreMinimal.h"
#include "SLMDeviceBase.h"
#include "Domains/SLMDomainElectricity.h"
#include "Domains/SLMDomainRotation.h"
#include "Domains/SLMDomainSignal.h"
#include "SLMDeviceMotor.generated.h"

class USLMDeviceSubsystemMotor;


USTRUCT(BlueprintType)
struct FSLMDeviceModelMotor
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
    float MaxPowerWatts = 1000;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
    float ConstantTorqueAngVel = 60;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
    int32 Index_Rotation_Crankshaft = -1;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
    int32 Index_Signal_Throttle = -1;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
    int32 Index_Electricity = -1;
};


USTRUCT(BlueprintType)
struct FSLMDeviceMotor
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
    FSLMDeviceModelMotor DeviceModel;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
    FSLMPortRotation Port_Rotation_Crankshaft;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
    FSLMPortElectricity Port_Electricity;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
    FSLMPortSignal Port_Signal_Throttle;
};


UCLASS(ClassGroup=("SLMechatronics"), meta=(BlueprintSpawnableComponent))
class SLMECHATRONICS_API USLMDeviceComponentMotor : public USLMDeviceComponentBase
{
    GENERATED_BODY()
public:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
    USLMDeviceSubsystemMotor* Subsystem;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
    FSLMDeviceMotor DeviceSettings;

    UFUNCTION(BlueprintCallable, Category = "SLMechatronics")
    FSLMDeviceModelMotor GetDeviceState();
protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};


UCLASS()
class SLMECHATRONICS_API USLMDeviceSubsystemMotor : public USLMDeviceSubsystemBase
{
    GENERATED_BODY()
public:
    virtual void OnWorldBeginPlay(UWorld& InWorld) override;
    virtual void PreSimulate(const float DeltaTime) override;
    virtual void Simulate(const float DeltaTime,float SubstepScalar) override;
    virtual void PostSimulate(const float DeltaTime) override;

    UFUNCTION(BlueprintCallable, Category = "SLMechatronics")
    int32 AddDevice(FSLMDeviceMotor Device);
    UFUNCTION(BlueprintCallable, Category = "SLMechatronics")
    void RemoveDevice(const int32 DeviceIndex);
    UFUNCTION(BlueprintCallable, Category = "SLMechatronics")
    FSLMDeviceModelMotor GetDeviceState(const int32 DeviceIndex);
private:
    TWeakObjectPtr<USLMDomainRotation> DomainRotation;
    TWeakObjectPtr<USLMDomainSignal> DomainSignal;
    TWeakObjectPtr<USLMDomainElectricity> DomainElectricity;
    TSparseArray<FSLMDeviceModelMotor> DeviceModels;
};
