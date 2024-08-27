// Copyright Spectrelight Studios, LLC

#pragma once

#include "CoreMinimal.h"
#include "SLMDeviceBase.h"
#include "Domains/SLMDomainRotation.h"
#include "SLMDeviceFluidCoupling.generated.h"

class USLMDeviceSubsystemFluidCoupling;


USTRUCT(BlueprintType)
struct FSLMDeviceModelFluidCoupling
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
    float TorquePerAngularDifference = 0.0;
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "SLMechatronics")
    int32 Index_Rotation_Input = -1;
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "SLMechatronics")
    int32 Index_Rotation_Output = -1;
};


USTRUCT(BlueprintType)
struct FSLMDeviceFluidCoupling
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
    FSLMDeviceModelFluidCoupling DeviceModel;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
    FSLMPortRotation Port_Rotation_Input;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
    FSLMPortRotation Port_Rotation_Output;
    
};


UCLASS(ClassGroup=("SLMechatronics"), meta=(BlueprintSpawnableComponent))
class SLMECHATRONICS_API USLMDeviceComponentFluidCoupling : public USLMDeviceComponentBase
{
    GENERATED_BODY()
public:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
    USLMDeviceSubsystemFluidCoupling* Subsystem;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
    FSLMDeviceFluidCoupling DeviceSettings;

    UFUNCTION(BlueprintCallable, Category = "SLMechatronics")
    FSLMDeviceModelFluidCoupling GetDeviceState();

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};


UCLASS()
class SLMECHATRONICS_API USLMDeviceSubsystemFluidCoupling : public USLMDeviceSubsystemBase
{
    GENERATED_BODY()
public:
    virtual void OnWorldBeginPlay(UWorld& InWorld) override;
    virtual void PreSimulate(const float DeltaTime) override;
    virtual void Simulate(const float DeltaTime, const float SubstepScalar) override;
    virtual void PostSimulate(const float DeltaTime) override;

    UFUNCTION(BlueprintCallable, Category = "SLMechatronics")
    int32 AddDevice(FSLMDeviceFluidCoupling Device);
    UFUNCTION(BlueprintCallable, Category = "SLMechatronics")
    void RemoveDevice(const int32 DeviceIndex);
    UFUNCTION(BlueprintCallable, Category = "SLMechatronics")
    FSLMDeviceModelFluidCoupling GetDeviceState(const int32 DeviceIndex);

private:
    TWeakObjectPtr<USLMDomainRotation> DomainRotation;
    TSparseArray<FSLMDeviceModelFluidCoupling> DeviceModels;
};
