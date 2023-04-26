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
	// Sets default values for this component's properties
	USLMechatronicsDeviceComponent();

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSimulateSignature, float, DeltaTime);
	UPROPERTY(BlueprintAssignable)
	FOnSimulateSignature OnPreSimulate;
	UPROPERTY(BlueprintAssignable)
	FOnSimulateSignature OnSimulate;
	UPROPERTY(BlueprintAssignable)
	FOnSimulateSignature OnPostSimulate;	
	
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "SLMechatronics")
	int32 DeviceIndex;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SLMechatronics")
	TArray<FSLMPort> Ports;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "SLMechatronics")
	USLMechatronicsSubsystem* Subsystem;



	//UFUNCTION(Category = "SLMechatronics")
	void PreSimulate(float DeltaTime);
	void Simulate(float DeltaTime);
	void PostSimulate(float DeltaTime);



	
	UFUNCTION(BlueprintPure, Category = "SLMechatronics")
	void GetLocationData(int32 PortIndex, USceneComponent*& OutSceneComponent, FName& OutSocket, FVector& OutOffset);
	//FVector GetWorldLocationForPort(int32 PortIndex);

	
	UFUNCTION(BlueprintPure, Category = "SLMechatronics")
	FSLMData GetNetworkData(int32 PortIndex);
	UFUNCTION(Blueprintcallable, Category = "SLMechatronics")
	void SetNetworkData(FSLMData Data, int32 PortIndex);
	
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};
