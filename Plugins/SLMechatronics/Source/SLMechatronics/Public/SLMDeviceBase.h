// Copyright Spectrelight Studios, LLC

#pragma once

#include "CoreMinimal.h"
#include "SLMSubsystemBase.h"
#include "Subsystems/WorldSubsystem.h"
#include "SLMDeviceBase.generated.h"


class USLMDomainSubsystemBase;


UCLASS(Abstract)
class SLMECHATRONICS_API USLMDeviceComponentBase : public UActorComponent
{
    GENERATED_BODY()
public:
    USLMDeviceComponentBase();
protected:
    //FSLMHandle Handle;

    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};


UCLASS()
class SLMECHATRONICS_API USLMDeviceSubsystemBase : public USLMSubsystemBase
{
    GENERATED_BODY()
	friend class USLMManager;
};