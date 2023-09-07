// Copyright Spectrelight Studios, LLC

#pragma once

#include "CoreMinimal.h"
#include "SLMDeviceBase.h"
#include "SLMTypes.h"
#include "SLMDeviceDoubleDifferential.generated.h"


USTRUCT(BlueprintType)
struct FSLMDeviceModelDoubleDifferential
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	float GearRatio_Drive = 1.0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	float GearRatio_Steer = 1.0;
	
	int32 Index_Mech_Drive = -1;
	int32 Index_Mech_Steer = -1;
	int32 Index_Mech_Left = -1;
	int32 Index_Mech_Right = -1;
};

UCLASS()
class SLMECHATRONICS_API USLMDeviceSubsystemDoubleDifferential : public USLMDeviceSubsystemBase
{
	GENERATED_BODY()
public:
	virtual void PreSimulate(float DeltaTime) override;
	virtual void Simulate(float DeltaTime) override;
	virtual void PostSimulate(float DeltaTime) override;
	void AddInstance(FSLMDeviceModelDoubleDifferential Instance);
private:
	TArray<FSLMDeviceModelDoubleDifferential> Instances;
}; 

UCLASS(ClassGroup=("SLMechatronics"), meta=(BlueprintSpawnableComponent))
class SLMECHATRONICS_API USLMDeviceComponentDoubleDifferential : public USLMDeviceComponentBase
{
	GENERATED_BODY()
public:
	USLMDeviceComponentDoubleDifferential();
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	FSLMDeviceModelDoubleDifferential DeviceModel = FSLMDeviceModelDoubleDifferential();
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	FSLMPort Port_Mech_Drive;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	FSLMPort Port_Mech_Steer;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	FSLMPort Port_Mech_Left;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	FSLMPort Port_Mech_Right;
	
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};