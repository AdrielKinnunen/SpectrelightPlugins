// Copyright Spectrelight Studios, LLC

#include "Domains/SLMDomainRotation.h"

FString FSLMDataRotation::GetDebugString() const
{
	FString Result;
	Result += FString::Printf(TEXT("%f,%f"), AngularVelocity, MomentOfInertia);
	return Result;
}

uint32 FSLMDataRotation::GetDebugHash() const
{
	uint32 Hash = 0;
	Hash = HashCombine(Hash, GetTypeHash(FMath::RoundToInt(AngularVelocity * 100.0f)));
	Hash = HashCombine(Hash, GetTypeHash(FMath::RoundToInt(MomentOfInertia * 100.0f)));
	//Hash = HashCombine(Hash, GetTypeHash(FMath::RoundToInt(StaticFriction * 1.0f)));
	//Hash = HashCombine(Hash, GetTypeHash(FMath::RoundToInt(DynamicFriction * 1.0f)));
	return Hash;
}

USLMDomainRotation::USLMDomainRotation()
{
    DebugColor = FColor::Black;
}

int32 USLMDomainRotation::AddPort(const FSLMPortRotation& Port, const FSLMPortAddress& PortAddress)
{
	const int32 PortID = PortDefaults.Add(Port.PortData);
	const int32 ParticleID = Particles.Add(Port.PortData);
	PortIDToParticleID.EmplaceAt(PortID, ParticleID);
	PortAddressToPortID.Add(PortAddress, PortID);
	PortIDToPortAddress.EmplaceAt(PortID, PortAddress);
	PortMetaData.EmplaceAt(PortID, Port.PortMetaData);
	return PortID;
}

void USLMDomainRotation::RunTests()
{
}

void USLMDomainRotation::PreSimulate(const float DeltaTime)
{
}

void USLMDomainRotation::Simulate(const float DeltaTime, const float SubstepScalar)
{
}

void USLMDomainRotation::PostSimulate(const float DeltaTime)
{
}

uint32 USLMDomainRotation::GetDebugHash()
{
	uint32 Result = 0;
	Result = HashCombine(Result, GetTypeHash(PortDefaults.Num()));
	Result = HashCombine(Result, GetTypeHash(PortIDToParticleID.Num()));
	Result = HashCombine(Result, GetTypeHash(Particles.Num()));
	for (const auto Entry : PortAddressToPortID)
	{
		const FSLMPortAddress PortAddress = Entry.Key;
		const FSLMDataRotation& PortDefault = PortDefaults[Entry.Value];
		const FSLMDataRotation& Particle = Particles[PortIDToParticleID[Entry.Value]];
		Result = Result ^ HashCombine(GetTypeHash(PortAddress), PortDefault.GetDebugHash(), Particle.GetDebugHash());
	}
	return Result;
}

FString USLMDomainRotation::GetDebugString(const bool Verbose)
{
	FString Result;
	Result += "\n------------------DomainRotation------------------";
	Result += FString::Format(TEXT("\nHas {0} PortDefaults"), {PortDefaults.Num()});
	Result += FString::Format(TEXT("\nHas {0} PortIDToParticleID"), {PortIDToParticleID.Num()});
	Result += FString::Format(TEXT("\nHas {0} Particles"), {Particles.Num()});
	if (Verbose)
	{
		for (int32 PortID = 0; PortID < PortIDToParticleID.Num(); PortID++)
		{
			if (PortIDToParticleID.IsValidIndex(PortID))
			{
				Result += FString::Format(TEXT("\nPort {0} maps to Particle {1} with state {2}"), {PortID, PortIDToParticleID[PortID], Particles[PortIDToParticleID[PortID]].GetDebugString()});
			}
		}
	}
	return Result;
}

FString USLMDomainRotation::GetPortDebugString(const FSLMPortAddress& Address)
{
	FString Result;
	if (const int32* PortIDPtr = PortAddressToPortID.Find(Address))
	{
		const int32 PortID = *PortIDPtr;
		if (PortIDToParticleID.IsValidIndex(PortID))
		{
			const int32 ParticleID = PortIDToParticleID[PortID];
			const FSLMDataRotation& PortDefault = PortDefaults[PortID];
			const FSLMDataRotation& Particle = Particles[ParticleID];
			Result += FString::Format(TEXT("\nPort {0} maps to Particle {1}"), {PortID, ParticleID});
			Result += FString::Format(TEXT("\nPort Default: {0}"), {PortDefault.GetDebugString()});
			Result += FString::Format(TEXT("\nParticle: {0}"), {Particle.GetDebugString()});			
		}
	}
	return Result;
}

FSLMDataRotation& USLMDomainRotation::GetParticleRef(const int32 PortID)
{
	return Particles[PortIDToParticleID[PortID]];
}

void USLMDomainRotation::CreateParticleForPorts(const TArray<int32> PortIDs)
{
    const int32 ParticleID = Particles.Add(FSLMDataRotation());
    float SumMomentum = 0;
    float SumMOI = 0;
    for (const auto& PortID : PortIDs)
    {
        SumMomentum += PortDefaults[PortID].AngularVelocity * PortDefaults[PortID].MomentOfInertia;
        SumMOI += PortDefaults[PortID].MomentOfInertia;
        PortIDToParticleID[PortID] = ParticleID;
    }
    Particles[ParticleID].AngularVelocity = SumMomentum / SumMOI;
    Particles[ParticleID].MomentOfInertia = SumMOI;
}

void USLMDomainRotation::DissolveParticleIntoPort(const int32 ParticleID, const int32 PortID)
{
	PortDefaults[PortID].AngularVelocity = Particles[ParticleID].AngularVelocity;
}

void USLMDomainRotation::RemovePortAtAddress(const FSLMPortAddress& PortAddress)
{
	const int32 PortID = PortAddressToPortID.FindChecked(PortAddress);
	Particles.RemoveAt(PortIDToParticleID[PortID]);
	PortIDToParticleID.RemoveAt(PortID);
	PortDefaults.RemoveAt(PortID);
	PortAddressToPortID.Remove(PortAddress);
	PortMetaData.RemoveAt(PortID);
	PortIDToPortAddress.RemoveAt(PortID);
}

void USLMDomainRotation::RemoveParticleAtID(const int32 ParticleID)
{
	Particles.RemoveAt(ParticleID);
}