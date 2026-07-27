// Copyright Spectrelight Studios, LLC

#include "Domains/SLMDomainRotation.h"

UE_DEFINE_GAMEPLAY_TAG(TAG_SLM_System_Domain_Rotation, "SLM.System.Domain.Rotation");

FString FSLMDataRotation::GetDebugString() const
{
	FString Result;
	Result += FString::Printf(TEXT("%f,%f"), AngularVelocity, MomentOfInertia);
	return Result;
}

uint32 FSLMDataRotation::GetDebugHash(const bool bVerbose) const
{
	uint32 Hash = 0;
	if (bVerbose)
	{
		Hash = HashCombine(Hash, GetTypeHash(FMath::RoundToInt(AngularVelocity * 100.0f)));
	}
	Hash = HashCombine(Hash, GetTypeHash(FMath::RoundToInt(MomentOfInertia * 100.0f)));
	//Hash = HashCombine(Hash, GetTypeHash(FMath::RoundToInt(StaticFriction * 1.0f)));
	//Hash = HashCombine(Hash, GetTypeHash(FMath::RoundToInt(DynamicFriction * 1.0f)));
	return Hash;
}

FSLMDomainSystemRotation::FSLMDomainSystemRotation()
{
	DebugColor = FColor::Black;
	SystemTag = TAG_SLM_System_Domain_Rotation;
}

FGameplayTag FSLMDomainSystemRotation::GetSystemTagStatic()
{
	return TAG_SLM_System_Domain_Rotation;
}

void FSLMDomainSystemRotation::Initialize()
{
}

void FSLMDomainSystemRotation::RunTests()
{
}

void FSLMDomainSystemRotation::PreSimulate(const float DeltaTime)
{
}

void FSLMDomainSystemRotation::Simulate(const float DeltaTime, const float SubstepScalar)
{
}

void FSLMDomainSystemRotation::PostSimulate(const float DeltaTime)
{
}