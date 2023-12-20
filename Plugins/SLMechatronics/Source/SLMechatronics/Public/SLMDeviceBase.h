// Copyright Spectrelight Studios, LLC

#pragma once

#include "CoreMinimal.h"
#include "SLMDomainBase.h"
#include "Subsystems/WorldSubsystem.h"
#include "SLMDeviceBase.generated.h"

class USLMDomainSubsystemBase;

UCLASS(Abstract)
class SLMECHATRONICS_API USLMDeviceComponentBase : public UActorComponent
{
	GENERATED_BODY()
public:
	USLMDeviceComponentBase();

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSLMEventSignature, float, DeltaTime);


	UPROPERTY(BlueprintAssignable)
	FSLMEventSignature OnPreSimulate;
	UPROPERTY(BlueprintAssignable)
	FSLMEventSignature OnSimulate;
	UPROPERTY(BlueprintAssignable)
	FSLMEventSignature OnPostSimulate;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "SLMechatronics")
	int32 DeviceIndex = -1;

	
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};


UCLASS()
class SLMECHATRONICS_API USLMDeviceSubsystemBase : public UWorldSubsystem
{
	GENERATED_BODY()
public:
	virtual void PreSimulate(const float DeltaTime);
	virtual void Simulate(const float DeltaTime);
	virtual void PostSimulate(const float DeltaTime);
};
