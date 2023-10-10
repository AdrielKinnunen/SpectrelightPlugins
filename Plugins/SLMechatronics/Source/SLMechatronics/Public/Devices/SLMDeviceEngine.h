// Copyright Spectrelight Studios, LLC

#pragma once

/*
#include "CoreMinimal.h"
#include "SLMDeviceBase.h"
#include "Domains/SLMDomainMech.h"
#include "SLMDeviceEngine.generated.h"


USTRUCT(BlueprintType)
struct FSLMDeviceModelEngine
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	float MaxTorque = 1.0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	float MaxRPM = 1.0;
	int32 Index_Mech_Crankshaft = -1;
	//int32 Index_Signal_Throttle = -1;
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
	void AddInstance(FSLMDeviceModelEngine Instance);
private:
	UPROPERTY()
	USLMDomainMech* DomainMech;
	TArray<FSLMDeviceModelEngine> Instances;
}; 

UCLASS(ClassGroup=("SLMechatronics"), meta=(BlueprintSpawnableComponent))
class SLMECHATRONICS_API USLMDeviceComponentEngine : public USLMDeviceComponentBase
{
	GENERATED_BODY()
public:
	USLMDeviceComponentEngine();
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	FSLMDeviceModelEngine DeviceModel = FSLMDeviceModelEngine();
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	FSLMPortMech Port_Mech_Crankshaft;
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	//FSLMPort Port_Signal_Throttle;
protected:
	UPROPERTY()
	USLMDomainMech* DomainMech;

	
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};*/