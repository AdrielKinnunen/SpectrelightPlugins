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

void USLMDeviceSubsystemBase::PreSimulate(const float DeltaTime)
{
}

void USLMDeviceSubsystemBase::Simulate(const float DeltaTime, const float SubstepScalar)
{
}

void USLMDeviceSubsystemBase::PostSimulate(const float DeltaTime)
{
}

FString USLMDeviceSubsystemBase::GetDebugString(int32 PortIndex)
{
	return "Hello World!";
}

void USLMDeviceSubsystemBase::DebugDraw()
{
}

void USLMDeviceSubsystemBase::DebugPrint()
{
}