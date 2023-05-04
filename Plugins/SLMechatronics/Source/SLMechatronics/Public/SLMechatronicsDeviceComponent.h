// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SLMechatronicsTypes.h"
#include "Components/ActorComponent.h"
#include "SLMechatronicsDeviceComponent.generated.h"

class USLMechatronicsSubsystem;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SLMECHATRONICS_API USLMechatronicsDeviceComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USLMechatronicsDeviceComponent();

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSimulateSignature, float, DeltaTime);
	UPROPERTY(BlueprintAssignable)
	FOnSimulateSignature OnPreSimulate;
	UPROPERTY(BlueprintAssignable)
	FOnSimulateSignature OnSimulate;
	UPROPERTY(BlueprintAssignable)
	FOnSimulateSignature OnPostSimulate;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SLMechatronics")
	TArray<FSLMPort> Ports;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "SLMechatronics")
	USLMechatronicsSubsystem* Subsystem;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "SLMechatronics")
	int32 DeviceIndex;
	
	virtual void PreSimulate(float DeltaTime);
	virtual void Simulate(float DeltaTime);
	virtual void PostSimulate(float DeltaTime);
	
	UFUNCTION(BlueprintCallable, Category = "SLMechatronics")
	void SelectClosestPort(FVector WorldLocation, FGameplayTag Domain, bool& Success, FSLMPort& OutPort);
	UFUNCTION(BlueprintPure, Category = "SLMechatronics")
	static FVector PortToWorldLocation(FSLMPort Port);


	void UpdatePortLocationData();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};
