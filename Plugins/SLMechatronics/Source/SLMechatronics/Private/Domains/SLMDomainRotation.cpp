// Copyright Spectrelight Studios, LLC

#include "Domains/SLMDomainRotation.h"

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

void USLMDomainRotation::RemovePort(const FSLMPortAddress& PortAddress)
{
	PortsToRemove.Add(PortAddress);
	bNeedsCleanup = true;
}

FSLMDataRotation USLMDomainRotation::GetData(const int32 PortID)
{
	if (PortIDToParticleID.IsValidIndex(PortID))
	{
		const int32 ParticleID = PortIDToParticleID[PortID];
		check(Particles.IsValidIndex(ParticleID));
		return Particles[ParticleID];
	}
	return FSLMDataRotation();
}

void USLMDomainRotation::AddTorque(const int32 PortID, const float Torque, const float DeltaTime)
{
	check(PortIDToParticleID.IsValidIndex(PortID));
	const int32 ParticleIndex = PortIDToParticleID[PortID];
	check(Particles.IsValidIndex(ParticleIndex));
	Particles[ParticleIndex].AddTorque(Torque, DeltaTime);
}

/*
void USLMDomainRotation::SetAngularVelocity(const int32 PortIndex, const float NewAngVel)
{
    check(PortIndexToParticleIndex.IsValidIndex(PortIndex));
    const int32 ParticleIndex = PortIndexToParticleIndex[PortIndex];
    check(Particles.IsValidIndex(ParticleIndex));
    Particles[ParticleIndex].AngularVelocity = NewAngVel;
}

void USLMDomainRotation::AddAngularImpulse(const int32 PortIndex, const float Impulse)
{
	check(PortIndexToParticleIndex.IsValidIndex(PortIndex));
	const int32 ParticleIndex = PortIndexToParticleIndex[PortIndex];
	check(Particles.IsValidIndex(ParticleIndex));
	Particles[ParticleIndex].AddImpulse(Impulse);
}

void USLMDomainRotation::AddTorque(const int32 PortIndex, const float Torque, const float DeltaTime)
{
	check(PortIndexToParticleIndex.IsValidIndex(PortIndex));
	const int32 ParticleIndex = PortIndexToParticleIndex[PortIndex];
	check(Particles.IsValidIndex(ParticleIndex));
	Particles[ParticleIndex].AddTorque(Torque, DeltaTime);
}
*/
void USLMDomainRotation::RunTests()
{
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
	const FSLMDataRotation& Particle = Particles[ParticleID];
	PortDefaults[PortID].AngularVelocity = Particle.AngularVelocity;
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
		Result = Result ^ HashCombine(GetTypeHash(PortAddress), GetTypeHash(PortDefault), GetTypeHash(Particle));
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
