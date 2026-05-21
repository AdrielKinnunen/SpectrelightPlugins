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

void USLMDeviceSubsystemBase::Client_AddOrChangeDescriptor(const FSLMDeviceAddress& DeviceAddress, const FInstancedStruct& Payload)
{
}

void USLMDeviceSubsystemBase::Client_RemoveDescriptor(const FSLMDeviceAddress& DeviceAddress)
{
}

void USLMDeviceSubsystemBase::Client_AddOrChangeState(const FSLMDeviceAddress& DeviceAddress, const FInstancedStruct& Payload)
{
}

void USLMDeviceSubsystemBase::Client_RemoveState(const FSLMDeviceAddress& DeviceAddress)
{
}