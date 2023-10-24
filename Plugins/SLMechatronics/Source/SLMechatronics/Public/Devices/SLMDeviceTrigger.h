// Copyright Spectrelight Studios, LLC

#pragma once

#include "CoreMinimal.h"
#include "SLMDeviceBase.h"
#include "Domains/SLMDomainSignal.h"
#include "SLMDeviceTrigger.generated.h"


USTRUCT(BlueprintType)
struct FSLMDeviceModelTrigger
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	float TriggerValue = 1.0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	float OldValue = 0.0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	int32 Index_Signal_Input = -1;
};


USTRUCT(BlueprintType)
struct FSLMDeviceTrigger
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	FSLMDeviceModelTrigger DeviceModel;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	FSLMPortSignal Port_Signal_Input;
};


UCLASS(ClassGroup=("SLMechatronics"), meta=(BlueprintSpawnableComponent))
class SLMECHATRONICS_API USLMDeviceComponentTrigger : public USLMDeviceComponentBase
{
	GENERATED_BODY()
public:
	USLMDeviceComponentTrigger();
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	FSLMDeviceTrigger DeviceSettings;
	UPROPERTY(BlueprintAssignable)
	FSLMEventSignature OnTrigger;
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};


UCLASS()
class SLMECHATRONICS_API USLMDeviceSubsystemTrigger : public USLMDeviceSubsystemBase
{
	GENERATED_BODY()
public:
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	virtual void PreSimulate(const float DeltaTime) override;
	virtual void Simulate(const float DeltaTime) override;
	virtual void PostSimulate(const float DeltaTime) override;

	void RegisterDeviceComponent(USLMDeviceComponentTrigger* DeviceComponent);
	void DeRegisterDeviceComponent(const USLMDeviceComponentTrigger* DeviceComponent);

	UFUNCTION(BlueprintCallable, Category = "SLMechatronics")
	int32 AddDevice(FSLMDeviceTrigger Device);
	UFUNCTION(BlueprintCallable, Category = "SLMechatronics")
	void RemoveDevice(const int32 DeviceIndex);
	UFUNCTION(BlueprintCallable, Category = "SLMechatronics")
	FSLMDeviceModelTrigger GetDeviceState(const int32 DeviceIndex);
	UFUNCTION(BlueprintCallable, Category = "SLMechatronics")
	void SetSignal(const int32 DeviceIndex, const float NewSignal);
private:
	TWeakObjectPtr<USLMDomainSignal> DomainSignal;
	TSparseArray<FSLMDeviceModelTrigger> DeviceModels;
	TSparseArray<USLMDeviceComponentTrigger*> DeviceComponents;
};
