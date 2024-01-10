// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SLFluidDynamics.h"
#include "Components/SceneComponent.h"
#include "SLFluidDynamicsProbeComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SLFLUIDDYNAMICS_API USLFluidDynamicsProbeComponent : public USceneComponent
{
    GENERATED_BODY()
public:
    // Sets default values for this component's properties
    USLFluidDynamicsProbeComponent();
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FSLFluidDynamicsProbeSettings FluidDynamicsProbeSettings;
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    //virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
