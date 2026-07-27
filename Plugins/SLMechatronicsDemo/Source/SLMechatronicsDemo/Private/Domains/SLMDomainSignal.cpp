// Copyright Spectrelight Studios, LLC

#include "Domains/SLMDomainSignal.h"

UE_DEFINE_GAMEPLAY_TAG(TAG_SLM_System_Domain_Signal, "SLM.System.Domain.Signal");

FString FSLMDataSignal::GetDebugString() const
{
	FString Result;
	Result += FString::Printf(TEXT("%f,%f"), ReadValue, WriteValue);
	return Result;
}

uint32 FSLMDataSignal::GetDebugHash(const bool bVerbose) const
{
	uint32 Hash = 0;
	
	Hash = HashCombine(Hash, GetTypeHash(FMath::RoundToInt(ReadValue * 100.0f)));
	Hash = HashCombine(Hash, GetTypeHash(FMath::RoundToInt(WriteValue * 100.0f)));
	
	return Hash;
}

FSLMDomainSystemSignal::FSLMDomainSystemSignal()
{
	DebugColor = FColor::White;
	SystemTag = TAG_SLM_System_Domain_Signal;
}

FGameplayTag FSLMDomainSystemSignal::GetSystemTagStatic()
{
	return TAG_SLM_System_Domain_Signal;
}

void FSLMDomainSystemSignal::Initialize()
{
}

void FSLMDomainSystemSignal::RunTests()
{
}

void FSLMDomainSystemSignal::PreSimulate(const float DeltaTime)
{
}

void FSLMDomainSystemSignal::Simulate(const float DeltaTime, const float SubstepScalar)
{
	for (auto& Particle : Particles)
	{
		Particle.Flush();
	}
}

void FSLMDomainSystemSignal::PostSimulate(const float DeltaTime)
{
}
/*
void FSLMDomainSystemSignal::Merge(const TArray<FSLMDataSignal>& Sources, FSLMDataSignal& Target)
{
	float SumRead = 0;
	float SumWrite = 0;
	for (const auto& Source : Sources)
	{
		SumRead += Source.ReadValue;
		SumWrite += Source.WriteValue;
	}
	Target.ReadValue = SumRead;
	Target.WriteValue = SumWrite;
}

void FSLMDomainSystemSignal::Dissolve(const FSLMDataSignal& Source, FSLMDataSignal& Target)
{
	Target.ReadValue = Source.ReadValue;
	Target.WriteValue = Source.WriteValue;
}
*/