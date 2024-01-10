// Copyright Spectrelight Studios, LLC
/*
#pragma once

#include "CoreMinimal.h"
#include "SLMDeviceBase.h"
#include "Domains/SLMDomainAir.h"
#include "Domains/SLMDomainRotation.h"
#include "Domains/SLMDomainSignal.h"
#include "SLMDeviceEngine.generated.h"

class USLMDeviceSubsystemEngine;

USTRUCT(BlueprintType)
struct FSLMDeviceModelEngine
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	float DisplacementPerRev = 1.0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	float Efficiency = 0.35;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	int32 Index_Rotation_Crankshaft = -1;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	int32 Index_Signal_Throttle = -1;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	int32 Index_Air_Intake = -1;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	int32 Index_Air_Exhaust = -1;
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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	FSLMPortAir Port_Air_Intake;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	FSLMPortAir Port_Air_Exhaust;
};


UCLASS(ClassGroup=("SLMechatronics"), meta=(BlueprintSpawnableComponent))
class SLMECHATRONICS_API USLMDeviceComponentEngine : public USLMDeviceComponentBase
{
	GENERATED_BODY()
	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	USLMDeviceSubsystemEngine* Subsystem;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	FSLMDeviceEngine DeviceSettings;

	UFUNCTION(BlueprintCallable, Category = "SLMechatronics")
	FSLMDeviceModelEngine GetDeviceState();
	
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

	UFUNCTION(BlueprintCallable, Category = "SLMechatronics")
	int32 AddDevice(FSLMDeviceEngine Device);
	UFUNCTION(BlueprintCallable, Category = "SLMechatronics")
	void RemoveDevice(const int32 DeviceIndex);
	UFUNCTION(BlueprintCallable, Category = "SLMechatronics")
	FSLMDeviceModelEngine GetDeviceState(const int32 DeviceIndex);
	
private:
	TWeakObjectPtr<USLMDomainRotation> DomainRotation;
	TWeakObjectPtr<USLMDomainSignal> DomainSignal;
	TWeakObjectPtr<USLMDomainAir> DomainAir;
	TSparseArray<FSLMDeviceModelEngine> DeviceModels;
};
*/
