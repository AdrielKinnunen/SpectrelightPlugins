// Copyright Spectrelight Studios, LLC

#pragma once

#include "CoreMinimal.h"
#include "SLMDeviceBase.h"
#include "Domains/SLMDomainSignal.h"
#include "SLMDeviceCockpit.generated.h"

class USLMDeviceSubsystemCockpit;

USTRUCT(BlueprintType)
struct FSLMCockpitValues
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	float Throttle = 0.0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	float Brake = 0.0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	float Steer = 0.0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	float Shift = 0.0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	float Fire = 0.0;
};

USTRUCT(BlueprintType)
struct FSLMDeviceModelCockpit
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	FSLMCockpitValues Values;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "SLMechatronics")
	int32 Index_Signal_Throttle = -1;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "SLMechatronics")
	int32 Index_Signal_Brake = -1;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "SLMechatronics")
	int32 Index_Signal_Steer = -1;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "SLMechatronics")
	int32 Index_Signal_Shift = -1;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "SLMechatronics")
	int32 Index_Signal_Fire = -1;
};

USTRUCT(BlueprintType)
struct FSLMDeviceCockpit
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	FSLMDeviceModelCockpit DeviceModel;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	FSLMPortSignal Port_Signal_Throttle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	FSLMPortSignal Port_Signal_Brake;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	FSLMPortSignal Port_Signal_Steer;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	FSLMPortSignal Port_Signal_Shift;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	FSLMPortSignal Port_Signal_Fire;
};


UCLASS(ClassGroup=("SLMechatronics"), meta=(BlueprintSpawnableComponent))
class SLMECHATRONICS_API USLMDeviceComponentCockpit : public USLMDeviceComponentBase
{
	GENERATED_BODY()
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	USLMDeviceSubsystemCockpit* Subsystem;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	FSLMDeviceCockpit DeviceSettings;

	UFUNCTION(BlueprintCallable, Category = "SLMechatronics")
	FSLMDeviceModelCockpit GetDeviceState();
	UFUNCTION(BlueprintCallable, Category = "SLMechatronics")
	void SetCockpitValues(FSLMCockpitValues Values);

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};


UCLASS()
class SLMECHATRONICS_API USLMDeviceSubsystemCockpit : public USLMDeviceSubsystemBase
{
	GENERATED_BODY()
public:
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	virtual void PreSimulate(const float DeltaTime) override;
	virtual void Simulate(const float DeltaTime) override;
	virtual void PostSimulate(const float DeltaTime) override;
	
	UFUNCTION(BlueprintCallable, Category = "SLMechatronics")
	int32 AddDevice(FSLMDeviceCockpit Device);
	UFUNCTION(BlueprintCallable, Category = "SLMechatronics")
	void RemoveDevice(const int32 DeviceIndex);
	UFUNCTION(BlueprintCallable, Category = "SLMechatronics")
	FSLMDeviceModelCockpit GetDeviceState(const int32 DeviceIndex);
	UFUNCTION(BlueprintCallable, Category = "SLMechatronics")
	void SetCockpitValues(const int32 DeviceIndex, FSLMCockpitValues Values);
private:
	TWeakObjectPtr<USLMDomainSignal> DomainSignal;
	TSparseArray<FSLMDeviceModelCockpit> DeviceModels;
};
