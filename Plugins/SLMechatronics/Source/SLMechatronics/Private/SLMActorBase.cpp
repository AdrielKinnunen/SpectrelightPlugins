// Copyright Spectrelight Studios, LLC. All Rights Reserved.


#include "SLMActorBase.h"


ASLMActorBase::ASLMActorBase()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ASLMActorBase::PreSimulate(const float DeltaTime)
{
}

void ASLMActorBase::Simulate(const float DeltaTime, const float SubstepScalar)
{
}

void ASLMActorBase::PostSimulate(const float DeltaTime)
{
}

FString ASLMActorBase::GetDebugString(int32 PortIndex)
{
	return "SLMActorBase says hello";
}

void ASLMActorBase::DebugDraw()
{
}

void ASLMActorBase::DebugPrint()
{
}

void ASLMActorBase::BeginPlay()
{
	Super::BeginPlay();
}
