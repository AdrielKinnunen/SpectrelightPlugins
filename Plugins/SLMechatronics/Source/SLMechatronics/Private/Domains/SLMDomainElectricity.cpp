// Copyright Spectrelight Studios, LLC

#include "Domains/SLMDomainElectricity.h"

FString FSLMDataElectricity::GetDebugString() const
{
	FString Result;
	Result += FString::Printf(TEXT("%f,%f"), StoredJoules, CapacityJoules);
	return Result;
}

uint32 FSLMDataElectricity::GetDebugHash() const
{
	uint32 Hash = 0;
	Hash = HashCombine(Hash, GetTypeHash(FMath::RoundToInt(StoredJoules * 100.0f)));
	Hash = HashCombine(Hash, GetTypeHash(FMath::RoundToInt(CapacityJoules * 100.0f)));
	return Hash;
}

USLMDomainElectricity::USLMDomainElectricity()
{
    DebugColor = FColor::Black;
}

int32 USLMDomainElectricity::AddPort(const FSLMPortElectricity& Port, const FSLMPortAddress& PortAddress)
{
	const int32 PortID = PortDefaults.Add(Port.PortData);
	const int32 ParticleID = Particles.Add(Port.PortData);
	PortIDToParticleID.EmplaceAt(PortID, ParticleID);
	PortAddressToPortID.Add(PortAddress, PortID);
	PortIDToPortAddress.EmplaceAt(PortID, PortAddress);
	PortMetaData.EmplaceAt(PortID, Port.PortMetaData);
	return PortID;
}

void USLMDomainElectricity::RunTests()
{
}

void USLMDomainElectricity::PreSimulate(const float DeltaTime)
{
}

void USLMDomainElectricity::Simulate(const float DeltaTime, const float SubstepScalar)
{
}

void USLMDomainElectricity::PostSimulate(const float DeltaTime)
{
}

uint32 USLMDomainElectricity::GetDebugHash()
{
	uint32 Result = 0;
	Result = HashCombine(Result, GetTypeHash(PortDefaults.Num()));
	Result = HashCombine(Result, GetTypeHash(PortIDToParticleID.Num()));
	Result = HashCombine(Result, GetTypeHash(Particles.Num()));
	for (const auto Entry : PortAddressToPortID)
	{
		const FSLMPortAddress PortAddress = Entry.Key;
		const FSLMDataElectricity& PortDefault = PortDefaults[Entry.Value];
		const FSLMDataElectricity& Particle = Particles[PortIDToParticleID[Entry.Value]];
		Result = Result ^ HashCombine(GetTypeHash(PortAddress), PortDefault.GetDebugHash(), Particle.GetDebugHash());
	}
	return Result;
}

FString USLMDomainElectricity::GetDebugString(const bool Verbose)
{
	FString Result;
	Result += "\n------------------DomainElectricity------------------";
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

FString USLMDomainElectricity::GetPortDebugString(const FSLMPortAddress& Address)
{
	FString Result;
	if (const int32* PortIDPtr = PortAddressToPortID.Find(Address))
	{
		const int32 PortID = *PortIDPtr;
		if (PortIDToParticleID.IsValidIndex(PortID))
		{
			const int32 ParticleID = PortIDToParticleID[PortID];
			const FSLMDataElectricity& PortDefault = PortDefaults[PortID];
			const FSLMDataElectricity& Particle = Particles[ParticleID];
			Result += FString::Format(TEXT("\nPort {0} maps to Particle {1}"), {PortID, ParticleID});
			Result += FString::Format(TEXT("\nPort Default: {0}"), {PortDefault.GetDebugString()});
			Result += FString::Format(TEXT("\nParticle: {0}"), {Particle.GetDebugString()});			
		}
	}
	return Result;
}

FSLMDataElectricity& USLMDomainElectricity::GetParticleRef(const int32 PortID)
{
	return Particles[PortIDToParticleID[PortID]];
}

void USLMDomainElectricity::CreateParticleForPorts(const TArray<int32> PortIDs)
{
    const int32 ParticleID = Particles.Add(FSLMDataElectricity());
    float SumStored = 0;
    float SumCapacity = 0;
    for (const auto& PortID : PortIDs)
    {
        SumStored += PortDefaults[PortID].StoredJoules;
        SumCapacity += PortDefaults[PortID].CapacityJoules;
        PortIDToParticleID[PortID] = ParticleID;
    }
    Particles[ParticleID].StoredJoules = SumStored;
    Particles[ParticleID].CapacityJoules = SumCapacity;
}

void USLMDomainElectricity::DissolveParticleIntoPort(const int32 ParticleID, const int32 PortID)
{
	const float PercentStored = Particles[ParticleID].StoredJoules / Particles[ParticleID].CapacityJoules;
	PortDefaults[PortID].StoredJoules = PercentStored * Particles[ParticleID].CapacityJoules;
}

void USLMDomainElectricity::RemovePortAtAddress(const FSLMPortAddress& PortAddress)
{
	const int32 PortID = PortAddressToPortID.FindChecked(PortAddress);
	Particles.RemoveAt(PortIDToParticleID[PortID]);
	PortIDToParticleID.RemoveAt(PortID);
	PortDefaults.RemoveAt(PortID);
	PortAddressToPortID.Remove(PortAddress);
	PortMetaData.RemoveAt(PortID);
	PortIDToPortAddress.RemoveAt(PortID);
}

void USLMDomainElectricity::RemoveParticleAtID(const int32 ParticleID)
{
	Particles.RemoveAt(ParticleID);
}