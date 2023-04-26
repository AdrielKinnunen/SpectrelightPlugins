// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SLFluidDynamicsProbeComponent.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "Subsystems/WorldSubsystem.h"
#include "SLFluidDynamicsSubsystem.generated.h"

UCLASS()
class SLFLUIDDYNAMICS_API USLFluidDynamicsSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:

	// FTickableGameObject implementation Begin
	//virtual ETickableTickType GetTickableTickType() const override;
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;
	// FTickableGameObject implementation End

	// USubsystem implementation Begin
	virtual bool DoesSupportWorldType(EWorldType::Type WorldType) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	// USubsystem implementation End
	
	//properties
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLFluidDynamics")
	TArray<USLFluidDynamicsProbeComponent*> ProbeComponentsArray;

	//functions
	UFUNCTION(BlueprintCallable, Category="SLFluidDynamics")
	void RegisterProbeComponent(USLFluidDynamicsProbeComponent* ProbeComponentToRegister);
	UFUNCTION(BlueprintCallable, Category="SLFluidDynamics")
	void DeRegisterProbeComponent(USLFluidDynamicsProbeComponent* ProbeComponentToDeRegister);


};
