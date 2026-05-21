// Copyright Spectrelight Studios, LLC

#include "Domains/SLMDomainAir.h"

FString FSLMDataAir::GetDebugString() const
{
	FString Result;
	Result += FString::Printf(TEXT("Volume(m3) = %f\n"), Volume);
	Result += FString::Printf(TEXT("Pressure(Pa) = %f\n"), GetPressure());
	Result += FString::Printf(TEXT("Temperature(K) = %f\n"), GetTemperature());
	Result += FString::Printf(TEXT("Oxygen(percent) = %f\n"), 100 * OxygenRatio);
	Result += FString::Printf(TEXT("Connected To Atmosphere = %i\n"), bConnectedToAtmosphere);
	return Result;
}

uint32 FSLMDataAir::GetDebugHash() const
{
	uint32 Hash = 0;
	Hash = HashCombine(Hash, GetTypeHash(FMath::RoundToInt(Volume * 100.0f)));
	Hash = HashCombine(Hash, GetTypeHash(FMath::RoundToInt(Energy * 100.0f)));
	Hash = HashCombine(Hash, GetTypeHash(FMath::RoundToInt(Moles * 100.0f)));
	Hash = HashCombine(Hash, GetTypeHash(FMath::RoundToInt(OxygenRatio * 100.0f)));
	Hash = HashCombine(Hash, GetTypeHash(bConnectedToAtmosphere));
	return Hash;
}

USLMDomainAir::USLMDomainAir()
{
	DebugColor = FColor::Cyan;
}

int32 USLMDomainAir::AddPort(const FSLMPortAir& Port, const FSLMPortAddress& PortAddress)
{
	const int32 PortID = PortDefaults.Add(Port.PortData);
	const int32 ParticleID = Particles.Add(Port.PortData);
	PortIDToParticleID.EmplaceAt(PortID, ParticleID);
	PortAddressToPortID.Add(PortAddress, PortID);
	PortIDToPortAddress.EmplaceAt(PortID, PortAddress);
	PortMetaData.EmplaceAt(PortID, Port.PortMetaData);
	return PortID;
}

void USLMDomainAir::RunTests()
{
	//const FSLMDataAir Original = FSLMDataAir();
	//FSLMDataAir A = Original;
	//FSLMDataAir B = Original;
	/*
	check(A.NearlyEqualWith(B));		//null case
	A.CompressOrExpandToVolume(Original.Volume / 10);
	B.CompressOrExpandToPressure(A.Pressure_bar);
	check(A.NearlyEqualWith(B));
	A = Original;
	B = Original;
	A.CompressOrExpandToVolume(Original.Volume_l * 10);
	B.CompressOrExpandToPressure(A.Pressure_bar);
	check(A.NearlyEqualWith(B));
	A.UpdateVolume();
	check(A.NearlyEqualWith(B));
	A.UpdatePressure();
	check(A.NearlyEqualWith(B));
	A.UpdateTemperature();
	check(A.NearlyEqualWith(B));
	A.UpdateMoles();
	check(A.NearlyEqualWith(B));
	*/
	
	FSLMDataAir Original = FSLMDataAir();
	Original.SetToSTP();
	FSLMDataAir Air = Original;
	Air.SetToSTP();
	
	FString Out;
	
	Out += "Air domain test results\n";
	Out += "Original state was:\n" + Air.GetDebugString() + "................................\n";
	
	Air.CompressOrExpandToVolume(Original.Volume / 2);
	Out += "After compressing to half original volume:\n" + Air.GetDebugString() + "................................\n";
	
	Air.CompressOrExpandToVolume(Original.Volume * 2);
	Out += "After expanding to double original volume:\n" + Air.GetDebugString() + "................................\n";
	
	Air.CompressOrExpandToVolume(Original.Volume);
	Out += "After compressing back to original volume:\n" + Air.GetDebugString() + "................................\n";
	
	Air.CompressOrExpandToPressure(Original.GetPressure() * 2);
	Out += "After compressing to double original pressure:\n" + Air.GetDebugString() + "................................\n";

	Air.CompressOrExpandToPressure(Original.GetPressure() / 2);
	Out += "After compressing to half original pressure:\n" + Air.GetDebugString() + "................................\n";

	Air.CompressOrExpandToPressure(Original.GetPressure());
	Out += "After compressing back to original pressure:\n" + Air.GetDebugString() + "................................\n";
	
	UE_LOG(LogTemp, Warning, TEXT("%s"), *Out);
}

void USLMDomainAir::PreSimulate(const float DeltaTime)
{
	Super::PreSimulate(DeltaTime);
}

void USLMDomainAir::Simulate(const float DeltaTime, const float SubstepScalar)
{
	for (auto& Particle : Particles)
	{
		if (Particle.bConnectedToAtmosphere)
		{
			Particle.SetToSTP();
		}
	}
}

void USLMDomainAir::PostSimulate(const float DeltaTime)
{
	Super::PostSimulate(DeltaTime);
}

uint32 USLMDomainAir::GetDebugHash()
{
	uint32 Result = 0;
	Result = HashCombine(Result, GetTypeHash(PortDefaults.Num()));
	Result = HashCombine(Result, GetTypeHash(PortIDToParticleID.Num()));
	Result = HashCombine(Result, GetTypeHash(Particles.Num()));
	for (const auto Entry : PortAddressToPortID)
	{
		const FSLMPortAddress PortAddress = Entry.Key;
		const FSLMDataAir& PortDefault = PortDefaults[Entry.Value];
		const FSLMDataAir& Particle = Particles[PortIDToParticleID[Entry.Value]];
		Result = Result ^ HashCombine(GetTypeHash(PortAddress), PortDefault.GetDebugHash(), Particle.GetDebugHash());
	}
	return Result;}

FString USLMDomainAir::GetDebugString(const bool Verbose)
{
	FString Result;
	Result += "\n------------------DomainAir------------------";
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
	return Result;}

FString USLMDomainAir::GetPortDebugString(const FSLMPortAddress& Address)
{
	FString Result;
	if (const int32* PortIDPtr = PortAddressToPortID.Find(Address))
	{
		const int32 PortID = *PortIDPtr;
		if (PortIDToParticleID.IsValidIndex(PortID))
		{
			const int32 ParticleID = PortIDToParticleID[PortID];
			const FSLMDataAir& PortDefault = PortDefaults[PortID];
			const FSLMDataAir& Particle = Particles[ParticleID];
			Result += FString::Format(TEXT("\nPort {0} maps to Particle {1}"), {PortID, ParticleID});
			Result += FString::Format(TEXT("\nPort Default: {0}"), {PortDefault.GetDebugString()});
			Result += FString::Format(TEXT("\nParticle: {0}"), {Particle.GetDebugString()});			
		}
	}
	return Result;}

FSLMDataAir& USLMDomainAir::GetParticleRef(const int32 PortID)
{
	return Particles[PortIDToParticleID[PortID]];
}

void USLMDomainAir::CreateParticleForPorts(const TArray<int32> PortIDs)
{
	const int32 ParticleID = Particles.Add(FSLMDataAir());
	float SumVolume = 0.0;
	float SumEnergy = 0.0;
	float SumMoles = 0.0;
	float SumOxygen = 0.0;
	for (const auto& PortID : PortIDs)
	{
		SumVolume += PortDefaults[PortID].Volume;
		SumEnergy += PortDefaults[PortID].Energy;
		SumMoles += PortDefaults[PortID].Moles;
		SumOxygen += PortDefaults[PortID].OxygenRatio * PortDefaults[PortID].Moles;
		PortIDToParticleID[PortID] = ParticleID;
	}
	Particles[ParticleID].Energy = SumEnergy;
	Particles[ParticleID].Moles = SumMoles;
	Particles[ParticleID].Volume = SumVolume;
	Particles[ParticleID].OxygenRatio = SumOxygen / SumMoles;
	Particles[ParticleID].bConnectedToAtmosphere = PortIDs.Num() == 1;
}

void USLMDomainAir::DissolveParticleIntoPort(const int32 ParticleID, const int32 PortID)
{
	const float Ratio = PortDefaults[PortID].Volume / Particles[ParticleID].Volume;
	PortDefaults[PortID].Energy = Ratio * Particles[ParticleID].Energy;
	PortDefaults[PortID].Moles = Ratio * Particles[ParticleID].Moles;
	PortDefaults[PortID].OxygenRatio = Particles[ParticleID].OxygenRatio;
}

void USLMDomainAir::RemovePortAtAddress(const FSLMPortAddress& PortAddress)
{
	const int32 PortID = PortAddressToPortID.FindChecked(PortAddress);
	Particles.RemoveAt(PortIDToParticleID[PortID]);
	PortIDToParticleID.RemoveAt(PortID);
	PortDefaults.RemoveAt(PortID);
	PortAddressToPortID.Remove(PortAddress);
	PortMetaData.RemoveAt(PortID);
	PortIDToPortAddress.RemoveAt(PortID);}

void USLMDomainAir::RemoveParticleAtID(const int32 ParticleID)
{
	Particles.RemoveAt(ParticleID);
}