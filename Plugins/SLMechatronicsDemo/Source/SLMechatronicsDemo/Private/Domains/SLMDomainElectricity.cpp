// Copyright Spectrelight Studios, LLC

#include "Domains/SLMDomainElectricity.h"

UE_DEFINE_GAMEPLAY_TAG(TAG_SLM_System_Domain_Electricity, "SLM.System.Domain.Electricity");

FString FSLMDataElectricity::GetDebugString() const
{
	FString Result;
	Result += FString::Printf(TEXT("%f,%f"), StoredJoules, CapacityJoules);
	return Result;
}

uint32 FSLMDataElectricity::GetDebugHash(const bool bVerbose) const
{
	uint32 Hash = 0;
	if (bVerbose)
	{
		Hash = HashCombine(Hash, GetTypeHash(FMath::RoundToInt(StoredJoules * 100.0f)));
	}
	Hash = HashCombine(Hash, GetTypeHash(FMath::RoundToInt(CapacityJoules * 100.0f)));
	return Hash;
}

FSLMDomainSystemElectricity::FSLMDomainSystemElectricity()
{
	DebugColor = FColor::Yellow;
	SystemTag = TAG_SLM_System_Domain_Electricity;
}

FGameplayTag FSLMDomainSystemElectricity::GetSystemTagStatic()
{
	return TAG_SLM_System_Domain_Electricity;
}

void FSLMDomainSystemElectricity::Initialize()
{
}

void FSLMDomainSystemElectricity::RunTests()
{
}

void FSLMDomainSystemElectricity::PreSimulate(const float DeltaTime)
{
}

void FSLMDomainSystemElectricity::Simulate(const float DeltaTime, const float SubstepScalar)
{
}

void FSLMDomainSystemElectricity::PostSimulate(const float DeltaTime)
{
}