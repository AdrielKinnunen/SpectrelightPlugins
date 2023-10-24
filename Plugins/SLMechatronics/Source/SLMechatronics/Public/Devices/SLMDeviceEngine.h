// Copyright Spectrelight Studios, LLC

#pragma once

#include "CoreMinimal.h"
#include "SLMDeviceBase.h"
#include "Domains/SLMDomainRotation.h"
#include "Domains/SLMDomainSignal.h"
#include "SLMDeviceEngine.generated.h"


USTRUCT(BlueprintType)
struct FSLMDeviceModelEngine
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	float MaxTorque = 1.0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	float MaxRPM = 1.0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	int32 Index_Rotation_Crankshaft = -1;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	int32 Index_Signal_Throttle = -1;
};


USTRUCT(BlueprintType)
struct FSLMDeviceEngine
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	FSLMDeviceModelEngine DeviceModel;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	FSLMPortRotation Port_Rotation_Crankshaft;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	FSLMPortSignal Port_Signal_Throttle;
};


UCLASS(ClassGroup=("SLMechatronics"), meta=(BlueprintSpawnableComponent))
class SLMECHATRONICS_API USLMDeviceComponentEngine : public USLMDeviceComponentBase
{
	GENERATED_BODY()
public:
	USLMDeviceComponentEngine();
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	FSLMDeviceEngine DeviceSettings;
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};


UCLASS()
class SLMECHATRONICS_API USLMDeviceSubsystemEngine : public USLMDeviceSubsystemBase
{
	GENERATED_BODY()
public:
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	virtual void PreSimulate(float DeltaTime) override;
	virtual void Simulate(float DeltaTime) override;
	virtual void PostSimulate(float DeltaTime) override;

	void RegisterDeviceComponent(USLMDeviceComponentEngine* DeviceComponent);
	void DeRegisterDeviceComponent(const USLMDeviceComponentEngine* DeviceComponent);

	UFUNCTION(BlueprintCallable, Category = "SLMechatronics")
	int32 AddDevice(FSLMDeviceEngine Device);
	UFUNCTION(BlueprintCallable, Category = "SLMechatronics")
	void RemoveDevice(const int32 DeviceIndex);
	UFUNCTION(BlueprintCallable, Category = "SLMechatronics")
	FSLMDeviceModelEngine GetDeviceState(const int32 DeviceIndex);
private:
	TWeakObjectPtr<USLMDomainRotation> DomainRotation;
	TWeakObjectPtr<USLMDomainSignal> DomainSignal;
	TSparseArray<FSLMDeviceModelEngine> DeviceModels;
	TSparseArray<USLMDeviceComponentEngine*> DeviceComponents;
};
