// Copyright Spectrelight Studios, LLC

#pragma once

#include "CoreMinimal.h"
#include "SLMDeviceBase.h"
#include "SLMTypes.h"
#include "SLMDeviceGearbox.generated.h"


USTRUCT(BlueprintType)
struct FSLMDeviceModelGearbox
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	float GearRatio = 1.0;
	int32 IndexShaftInput = -1;
	int32 IndexShaftOutput = -1;
};

UCLASS()
class SLMECHATRONICS_API USLMDeviceSubsystemGearbox : public USLMDeviceSubsystemBase
{
	GENERATED_BODY()
public:
	virtual void PreSimulate(float DeltaTime) override;
	virtual void Simulate(float DeltaTime) override;
	virtual void PostSimulate(float DeltaTime) override;
	void AddInstance(FSLMDeviceModelGearbox Instance);
private:
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
	FSLMPort PortShaftInput;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	FSLMPort PortShaftOutput;
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};