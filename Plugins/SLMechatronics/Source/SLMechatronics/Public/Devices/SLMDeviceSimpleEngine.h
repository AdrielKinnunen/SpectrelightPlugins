// Copyright Spectrelight Studios, LLC

#pragma once

#include "CoreMinimal.h"
#include "SLMDeviceBase.h"
#include "Domains/SLMDomainRotation.h"
#include "Domains/SLMDomainSignal.h"
#include "SLMDeviceSimpleEngine.generated.h"

class USLMDeviceSubsystemSimpleEngine;


USTRUCT(BlueprintType)
struct FSLMDeviceModelSimpleEngine
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
    float MaxTorque = 1000;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
    float DragTorque = 500;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
    int32 Index_Rotation_Crankshaft = -1;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
    int32 Index_Signal_Throttle = -1;
};


USTRUCT(BlueprintType)
struct FSLMDeviceSimpleEngine
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
    FSLMDeviceModelSimpleEngine DeviceModel;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
    FSLMPortRotation Port_Rotation_Crankshaft;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
    FSLMPortSignal Port_Signal_Throttle;
};


UCLASS(ClassGroup=("SLMechatronics"), meta=(BlueprintSpawnableComponent))
class SLMECHATRONICS_API USLMDeviceComponentSimpleEngine : public USLMDeviceComponentBase
{
    GENERATED_BODY()
public:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
    USLMDeviceSubsystemSimpleEngine* Subsystem;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
    FSLMDeviceSimpleEngine DeviceSettings;

    UFUNCTION(BlueprintCallable, Category = "SLMechatronics")
    FSLMDeviceModelSimpleEngine GetDeviceState();
protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};


UCLASS()
class SLMECHATRONICS_API USLMDeviceSubsystemSimpleEngine : public USLMDeviceSubsystemBase
{
    GENERATED_BODY()
public:
    virtual void OnWorldBeginPlay(UWorld& InWorld) override;
    virtual void PreSimulate(const float DeltaTime) override;
    virtual void Simulate(const float DeltaTime,float SubstepScalar) override;
    virtual void PostSimulate(const float DeltaTime) override;

    UFUNCTION(BlueprintCallable, Category = "SLMechatronics")
    int32 AddDevice(FSLMDeviceSimpleEngine Device);
    UFUNCTION(BlueprintCallable, Category = "SLMechatronics")
    void RemoveDevice(const int32 DeviceIndex);
    UFUNCTION(BlueprintCallable, Category = "SLMechatronics")
    FSLMDeviceModelSimpleEngine GetDeviceState(const int32 DeviceIndex);
private:
    TWeakObjectPtr<USLMDomainRotation> DomainRotation;
    TWeakObjectPtr<USLMDomainSignal> DomainSignal;
    TSparseArray<FSLMDeviceModelSimpleEngine> DeviceModels;
};
