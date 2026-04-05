// Copyright Spectrelight Studios, LLC

#include "Domains/SLMDomainRotation.h"

//UE_DEFINE_GAMEPLAY_TAG(TAG_SLMECHATRONICS_DOMAIN_ROTATION, "SLMechatronics.Domain.Rotation")

USLMDomainRotation::USLMDomainRotation()
{
    DebugColor = FColor::Black;
}

int32 USLMDomainRotation::AddPort(const FSLMPortRotation& Port, const FSLMPortAddress& Address)
{
	const int32 PortID = PortDefaults.Add(Port.PortData);
	PortAddressToID.Add(Address, PortID);
	const int32 ParticleID = Particles.Add(Port.PortData);
	PortIDToParticleID.EmplaceAt(PortID, ParticleID);
	return ParticleID;
}

void USLMDomainRotation::RemovePort(const FSLMPortAddress& Address)
{
	check(PortAddressToID.Contains(Address));
	const int32 PortID = PortAddressToID[Address];
	const int32 ParticleID = PortIDToParticleID[PortID];
	PortDefaults.RemoveAt(PortID);
	Particles.RemoveAt(ParticleID);
	PortIDToParticleID.RemoveAt(PortID);
	PortAddressToID.Remove(Address);
}

/*
void USLMDomainRotation::EnqueueRemovePort(const FSLMPortAddress& Address)
{
	PortsToRemove.Add(Address);
	bNeedsCleanup = true;
}
*/
uint32 USLMDomainRotation::GetDebugHash()
{
	uint32 Result = 0;
	Result += PortDefaults.Num();
	Result += PortAddressToID.Num();
	Result += Particles.Num();
	for (auto& Entry : PortAddressToID)
	{
		const uint32 AddressHash = GetTypeHash(Entry.Key);
		const uint32 DefaultHash = GetTypeHash(PortDefaults[Entry.Value]);
		const uint32 CombinedHash = HashCombine(AddressHash, DefaultHash);
		Result += CombinedHash;
	}
	return Result;
}

FString USLMDomainRotation::GetDebugString()
{
	FString Result;
	Result += "DomainRotation";
	Result += FString::Printf(TEXT("\nHas %i Port Address to ID Mappings"), PortAddressToID.Num());
	Result += FString::Printf(TEXT("\nHas %i Port Defaults"), PortDefaults.Num());
	Result += FString::Printf(TEXT("\nHas %i Port ID to Particle ID Mappings"), PortIDToParticleID.Num());
	Result += FString::Printf(TEXT("\nHas %i Particles"), Particles.Num());
	for (auto& Entry : PortAddressToID)
	{
		Result += "\n";
		Result += FString::Printf(TEXT("Port state: "));
		Result += Entry.Key.GetDebugString();
		Result += PortDefaults[Entry.Value].GetDebugString();
	}
	Result += "\n";
	return Result;
}

void USLMDomainRotation::ConnectPortsByAddress(const FSLMPortAddress First, const FSLMPortAddress Second)
{
	//Super::ConnectPortsByAddress(First, Second);
	//int32* FirstID = PortAddressToID.Find(First);
	int32* FirstIDPtr = PortAddressToID.Find(First);
	int32* SecondIDPtr = PortAddressToID.Find(Second);
	if (!FirstIDPtr || !SecondIDPtr || (*FirstIDPtr == *SecondIDPtr)) return;
	
	const int32 FirstID = *FirstIDPtr;
	const int32 SecondID = *SecondIDPtr;
	
	FSLMDataRotation FirstParticle = Particles[FirstID];
	FSLMDataRotation SecondParticle = Particles[SecondID];
	const float FirstMomentum = FirstParticle.AngularVelocity * FirstParticle.MomentOfInertia;
	const float SecondMomentum = SecondParticle.AngularVelocity * SecondParticle.MomentOfInertia;
	const float SumMomentum = FirstMomentum + SecondMomentum;
	const float SumMOI = FirstParticle.MomentOfInertia + SecondParticle.MomentOfInertia;
	const float FinalAngularVelocity = SumMomentum / SumMOI;
	const FSLMDataRotation NewParticle = FSLMDataRotation(FinalAngularVelocity, SumMOI);
	const int32 NewParticleID = Particles.Add(NewParticle);
}

/*
void USLMDomainRotation::RemovePort(const int32 PortID)
{
    PortsToRemove.Add(PortID);
    bNeedsCleanup = true;
}
/*
FSLMDataRotation USLMDomainRotation::GetData(const int32 PortIndex)
{
    check(PortIndexToParticleIndex.IsValidIndex(PortIndex));
    const int32 ParticleIndex = PortIndexToParticleIndex[PortIndex];
    check(Particles.IsValidIndex(ParticleIndex));
    return Particles[ParticleIndex];
}

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


void USLMDomainRotation::Simulate(const float DeltaTime, const float SubstepScalar)
{
    for (auto& Particle : Particles)
    {
        //Particle.AngularVelocity = FMath::FInterpConstantTo(Particle.AngularVelocity, 0.0, DeltaTime, Particle.StaticFriction + Particle.DynamicFriction * Particle.AngularVelocity);
        //const float Min = (Particle.AngularVelocity > 0.0) ? 0.0 : Particle.AngularVelocity;
        //const float Max = (Particle.AngularVelocity > 0.0) ? Particle.AngularVelocity : 0.0;
        //const float Desired = Particle.AngularVelocity - Particle.AngularVelocity * Particle.FrictionCoefficient * DeltaTime;
        //const float Clamped = FMath::Clamp(Desired, Min, Max);
        //Particle.AngularVelocity = Clamped;
    }
}
/*
FString USLMDomainRotation::GetDebugString(const int32 PortIndex)
{
    check(PortIndexToParticleIndex.IsValidIndex(PortIndex));
    const int32 ParticleIndex = PortIndexToParticleIndex[PortIndex];
    check(Particles.IsValidIndex(ParticleIndex));
    const auto Particle = Particles[ParticleIndex];
    FString Result;
    Result += "Rotation\n";
    Result += FString::Printf(TEXT("Port %i : Particle %i\n"), PortIndex, ParticleIndex);
    Result += FString::Printf(TEXT("MOI = %f\n"), Particle.MomentOfInertia);
    Result += FString::Printf(TEXT("Rad/s = %f\n"), Particle.AngularVelocity);
    return Result;
}

void USLMDomainRotation::CreateParticleForPorts(const TArray<int32> PortIndices)
{
    const int32 ParticleIndex = Particles.Add(FSLMDataRotation());
    float SumProduct = 0;
    float SumMOI = 0;
    for (const auto& PortIndex : PortIndices)
    {
        SumProduct += Ports[PortIndex].AngularVelocity * Ports[PortIndex].MomentOfInertia;
        SumMOI += Ports[PortIndex].MomentOfInertia;
        PortIndexToParticleIndex[PortIndex] = ParticleIndex;
    }
    Particles[ParticleIndex].AngularVelocity = SumProduct / SumMOI;
    Particles[ParticleIndex].MomentOfInertia = SumMOI;
}

void USLMDomainRotation::DissolveParticleIntoPort(const int32 ParticleIndex, const int32 PortIndex)
{
    const FSLMDataRotation Particle = Particles[ParticleIndex];
    Ports[PortIndex].AngularVelocity = Particle.AngularVelocity;
}

void USLMDomainRotation::RemovePortAtIndex(const int32 PortIndex)
{
    Ports.RemoveAt(PortIndex);
}

void USLMDomainRotation::RemoveParticleAtIndex(const int32 ParticleIndex)
{
    Particles.RemoveAt(ParticleIndex);
}

void USLMDomainRotation::CreateParticleForPort(const int32 Port)
{
    PortIndexToParticleIndex[Port] = Particles.Add(Ports[Port]);
}
*/
