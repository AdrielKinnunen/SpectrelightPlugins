// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "SLFluidDynamics.generated.h"


USTRUCT(BlueprintType)
struct FSLFluidDynamicsProbeSettings
{
    GENERATED_BODY()
    FSLFluidDynamicsProbeSettings()
    {
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLFluidDynamics")
    FVector AirDrag = FVector(0.f, 0.f, 0.f);
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLFluidDynamics")
    FVector WaterDrag = FVector(0.f, 0.f, 0.f);
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLFluidDynamics")
    float MaxBuoyancy = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLFluidDynamics")
    float MaxBuoyancyDepth = 1.f;
};


class FSLFluidDynamicsModule : public IModuleInterface
{
public:
    /** IModuleInterface implementation */
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};
