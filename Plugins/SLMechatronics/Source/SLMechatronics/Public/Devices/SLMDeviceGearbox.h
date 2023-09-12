// Copyright Spectrelight Studios, LLC

#pragma once

#include "CoreMinimal.h"
#include "SLMDeviceBase.h"
#include "Domains/SLMDomainMech.h"
#include "SLMDeviceGearbox.generated.h"


USTRUCT(BlueprintType)
struct FSLMDeviceModelGearbox
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	float GearRatio = 1.0;
	int32 Index_Mech_Input = -1;
	int32 Index_Mech_Output = -1;
};

UCLASS()
class SLMECHATRONICS_API USLMDeviceSubsystemGearbox : public USLMDeviceSubsystemBase
{
	GENERATED_BODY()
public:
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	virtual void PreSimulate(float DeltaTime) override;
	virtual void Simulate(float DeltaTime) override;
	virtual void PostSimulate(float DeltaTime) override;
	void AddInstance(FSLMDeviceModelGearbox Instance);
private:
	UPROPERTY()
	USLMDomainMech* DomainMech;
	TArray<FSLMDeviceModelGearbox> Instances;
}; 

UCLASS(ClassGroup=("SLMechatronics"), meta=(BlueprintSpawnableComponent))
class SLMECHATRONICS_API USLMDeviceComponentGearbox : public USLMDeviceComponentBase
{
	GENERATED_BODY()
public:
	USLMDeviceComponentGearbox();
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	FSLMDeviceModelGearbox DeviceModel = FSLMDeviceModelGearbox();
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	FSLMPortMech Port_Mech_Input;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	FSLMPortMech Port_Mech_Output;
protected:
	UPROPERTY()
	USLMDomainMech* DomainMech;	
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};