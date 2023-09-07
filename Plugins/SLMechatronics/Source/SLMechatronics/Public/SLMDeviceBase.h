// Copyright Spectrelight Studios, LLC

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "SLMDeviceBase.generated.h"

class USLMechatronicsSubsystem;

UCLASS()
class SLMECHATRONICS_API USLMDeviceSubsystemBase : public UWorldSubsystem
{
	GENERATED_BODY()
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	USLMechatronicsSubsystem* Subsystem;
	
	virtual void PreSimulate(float DeltaTime);
	virtual void Simulate(float DeltaTime);
	virtual void PostSimulate(float DeltaTime);
};

UCLASS(Abstract)
class SLMECHATRONICS_API USLMDeviceComponentBase : public UActorComponent
{
	GENERATED_BODY()

public:
	USLMDeviceComponentBase();

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSimulateSignature, float, DeltaTime);
	UPROPERTY(BlueprintAssignable)
	FOnSimulateSignature OnPreSimulate;
	UPROPERTY(BlueprintAssignable)
	FOnSimulateSignature OnSimulate;
	UPROPERTY(BlueprintAssignable)
	FOnSimulateSignature OnPostSimulate;
	
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "SLMechatronics")
	USLMechatronicsSubsystem* Subsystem;
	//UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "SLMechatronics")
	//int32 DeviceIndex;
	
	//UFUNCTION(BlueprintCallable, Category = "SLMechatronics")
	//void SelectClosestPort(FVector WorldLocation, FGameplayTag Domain, bool& Success, FSLMPort& OutPort);
	//UFUNCTION(BlueprintPure, Category = "SLMechatronics")
	//static FVector PortToWorldLocation(FSLMPort Port);
	//void UpdatePortLocationData();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};

