// Copyright Spectrelight Studios, LLC

#include "SLMDeviceBase.h"

USLMDeviceComponentBase::USLMDeviceComponentBase()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void USLMDeviceComponentBase::BeginPlay()
{
    Super::BeginPlay();
}

void USLMDeviceComponentBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
}
