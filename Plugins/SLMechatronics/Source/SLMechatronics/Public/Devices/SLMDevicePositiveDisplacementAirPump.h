// Copyright Spectrelight Studios, LLC

#pragma once

#include "CoreMinimal.h"
#include "SLMDeviceBase.h"
#include "Domains/SLMDomainAir.h"
#include "Domains/SLMDomainRotation.h"
#include "SLMDevicePositiveDisplacementAirPump.generated.h"


USTRUCT(BlueprintType)
struct FSLMDeviceModelPositiveDisplacementAirPump
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	float DisplacementPerRev = 1.0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	int32 Index_Rotation_Crankshaft = -1;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	int32 Index_Air_Intake = -1;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	int32 Index_Air_Exhaust = -1;
};


USTRUCT(BlueprintType)
struct FSLMDevicePositiveDisplacementAirPump
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	FSLMDeviceModelPositiveDisplacementAirPump DeviceModel;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	FSLMPortRotation Port_Rotation_Crankshaft;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	FSLMPortAir Port_Air_Intake;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	FSLMPortAir Port_Air_Exhaust;
};


UCLASS(ClassGroup=("SLMechatronics"), meta=(BlueprintSpawnableComponent))
class SLMECHATRONICS_API USLMDeviceComponentPositiveDisplacementAirPump : public USLMDeviceComponentBase
{
	GENERATED_BODY()
public:
	USLMDeviceComponentPositiveDisplacementAirPump();
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	FSLMDevicePositiveDisplacementAirPump DeviceSettings;
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};


UCLASS()
class SLMECHATRONICS_API USLMDeviceSubsystemPositiveDisplacementAirPump : public USLMDeviceSubsystemBase
{
	GENERATED_BODY()
public:
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	virtual void PreSimulate(float DeltaTime) override;
	virtual void Simulate(float DeltaTime) override;
	virtual void PostSimulate(float DeltaTime) override;

	void RegisterDeviceComponent(USLMDeviceComponentPositiveDisplacementAirPump* DeviceComponent);
	void DeRegisterDeviceComponent(const USLMDeviceComponentPositiveDisplacementAirPump* DeviceComponent);

	UFUNCTION(BlueprintCallable, Category = "SLMechatronics")
	int32 AddDevice(FSLMDevicePositiveDisplacementAirPump Device);
	UFUNCTION(BlueprintCallable, Category = "SLMechatronics")
	void RemoveDevice(const int32 DeviceIndex);
	UFUNCTION(BlueprintCallable, Category = "SLMechatronics")
	FSLMDeviceModelPositiveDisplacementAirPump GetDeviceState(const int32 DeviceIndex);
	
private:
	TWeakObjectPtr<USLMDomainRotation> DomainRotation;
	TWeakObjectPtr<USLMDomainAir> DomainAir;

	TSparseArray<FSLMDeviceModelPositiveDisplacementAirPump> DeviceModels;
	TSparseArray<USLMDeviceComponentPositiveDisplacementAirPump*> DeviceComponents;
};
