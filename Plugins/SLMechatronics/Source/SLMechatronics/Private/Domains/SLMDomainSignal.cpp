// Copyright Spectrelight Studios, LLC

#include "Domains/SLMDomainSignal.h"

USLMDomainSignal::USLMDomainSignal()
{
    DebugColor = FColor::White;
}

int32 USLMDomainSignal::AddPort(const FSLMPortSignal& Port, const FSLMPortAddress& PortAddress)
{
	const int32 PortID = PortDefaults.Add(Port.PortData);
	const int32 ParticleID = Particles.Add(Port.PortData);
	PortIDToParticleID.EmplaceAt(PortID, ParticleID);
	PortAddressToPortID.Add(PortAddress, PortID);
	PortIDToPortAddress.EmplaceAt(PortID, PortAddress);
	PortMetaData.EmplaceAt(PortID, Port.PortMetaData);
	return PortID;
}

void USLMDomainSignal::RemovePort(const FSLMPortAddress& PortAddress)
{
	PortsToRemove.Add(PortAddress);
	bNeedsCleanup = true;
}

float USLMDomainSignal::ReadValue(const int32 PortID)
{
	if (PortIDToParticleID.IsValidIndex(PortID))
	{
		const int32 ParticleID = PortIDToParticleID[PortID];
		check(Particles.IsValidIndex(ParticleID));
		return Particles[ParticleID].Read;
	}
	return 0.0;
}

void USLMDomainSignal::WriteValue(const int32 PortID, const float Value)
{
	if (PortIDToParticleID.IsValidIndex(PortID))
	{
		const int32 ParticleID = PortIDToParticleID[PortID];
		check(Particles.IsValidIndex(ParticleID));
		Particles[ParticleID].Write += Value;
	}
}

void USLMDomainSignal::RunTests()
{
}


void USLMDomainSignal::CreateParticleForPorts(const TArray<int32> PortIDs)
{
    const int32 ParticleID = Particles.Add(FSLMDataSignal());
    for (const auto& PortID : PortIDs)
    {        
        PortIDToParticleID[PortID] = ParticleID;
    }
}

void USLMDomainSignal::DissolveParticleIntoPort(const int32 ParticleID, const int32 PortID)
{
	PortDefaults[PortID] = FSLMDataSignal();
}

void USLMDomainSignal::RemovePortAtAddress(const FSLMPortAddress& PortAddress)
{
	const int32 PortID = PortAddressToPortID.FindChecked(PortAddress);
	Particles.RemoveAt(PortIDToParticleID[PortID]);
	PortIDToParticleID.RemoveAt(PortID);
	PortDefaults.RemoveAt(PortID);
	PortAddressToPortID.Remove(PortAddress);
	PortMetaData.RemoveAt(PortID);
	PortIDToPortAddress.RemoveAt(PortID);
}

void USLMDomainSignal::RemoveParticleAtID(const int32 ParticleID)
{
	Particles.RemoveAt(ParticleID);
}

uint32 USLMDomainSignal::GetDebugHash()
{
	uint32 Result = 0;
	Result = HashCombine(Result, GetTypeHash(PortDefaults.Num()));
	Result = HashCombine(Result, GetTypeHash(PortIDToParticleID.Num()));
	Result = HashCombine(Result, GetTypeHash(Particles.Num()));
	for (const auto Entry : PortAddressToPortID)
	{
		const FSLMPortAddress PortAddress = Entry.Key;
		const FSLMDataSignal& PortDefault = PortDefaults[Entry.Value];
		const FSLMDataSignal& Particle = Particles[PortIDToParticleID[Entry.Value]];
		Result = Result ^ HashCombine(GetTypeHash(PortAddress), GetTypeHash(PortDefault), GetTypeHash(Particle));
	}
	return Result;
}

FString USLMDomainSignal::GetDebugString(const bool Verbose)
{
	FString Result;
	Result += "\n------------------DomainSignal------------------";
	Result += FString::Format(TEXT("\nHas {0} PortDefaults"), {PortDefaults.Num()});
	Result += FString::Format(TEXT("\nHas {0} PortIDToParticleID"), {PortIDToParticleID.Num()});
	Result += FString::Format(TEXT("\nHas {0} Particles"), {Particles.Num()});
	if (Verbose)
	{
		for (int32 PortID = 0; PortID < PortIDToParticleID.Num(); PortID++)
		{
			if (PortIDToParticleID.IsValidIndex(PortID))
			{
				Result += FString::Format(TEXT("\nPort {0} maps to Particle {1}"), {PortID, PortIDToParticleID[PortID]});
			}
		}
	}
	return Result;
}

FString USLMDomainSignal::GetPortDebugString(const FSLMPortAddress& Address)
{
	FString Result;
	if (const int32* PortIDPtr = PortAddressToPortID.Find(Address))
	{
		const int32 PortID = *PortIDPtr;
		if (PortIDToParticleID.IsValidIndex(PortID))
		{
			const int32 ParticleID = PortIDToParticleID[PortID];
			const FSLMDataSignal& PortDefault = PortDefaults[PortID];
			const FSLMDataSignal& Particle = Particles[ParticleID];
			Result += FString::Format(TEXT("\nPort {0} maps to Particle {1}"), {PortID, ParticleID});
			Result += FString::Format(TEXT("\nPort Default: {0}"), {PortDefault.GetDebugString()});
			Result += FString::Format(TEXT("\nParticle: {0}"), {Particle.GetDebugString()});			
		}
	}
	return Result;
}
