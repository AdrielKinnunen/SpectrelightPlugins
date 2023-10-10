// Copyright Spectrelight Studios, LLC

#pragma once

#include "CoreMinimal.h"
#include "SLMDeviceBase.h"
#include "Domains/SLMDomainMech.h"
#include "SLMDeviceDifferential.generated.h"


/*
USTRUCT(BlueprintType)
struct FSLMDeviceModelDifferential
{
	GENERATED_BODY()
	
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	//float GearRatio = 1.0;
	int32 IndexShaftInput = -1;
	int32 IndexShaftLeft = -1;
	int32 IndexShaftRight = -1;
};

UCLASS()
class SLMECHATRONICS_API USLMDeviceSubsystemDifferential : public USLMDeviceSubsystemBase
{
	GENERATED_BODY()
public:
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	virtual void PreSimulate(float DeltaTime) override;
	virtual void Simulate(float DeltaTime) override;
	virtual void PostSimulate(float DeltaTime) override;
	void AddInstance(FSLMDeviceModelDifferential Instance);
private:
	UPROPERTY()
	USLMDomainMech* DomainMech;
	TArray<FSLMDeviceModelDifferential> Instances;
}; 

UCLASS(ClassGroup=("SLMechatronics"), meta=(BlueprintSpawnableComponent))
class SLMECHATRONICS_API USLMDeviceComponentDifferential : public USLMDeviceComponentBase
{
	GENERATED_BODY()
public:
	USLMDeviceComponentDifferential();
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	FSLMDeviceModelDifferential DeviceModel = FSLMDeviceModelDifferential();
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	FSLMPortMech PortShaftInput;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	FSLMPortMech PortShaftLeft;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	FSLMPortMech PortShaftRight;
protected:
	UPROPERTY()
	USLMDomainMech* DomainMech;
	
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};*/