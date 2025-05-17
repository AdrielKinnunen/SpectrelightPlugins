// Copyright Spectrelight Studios, LLC

#include "Domains/SLMDomainAir.h"

UE_DEFINE_GAMEPLAY_TAG(TAG_SPECTRELIGHTDYNAMICS_DOMAIN_AIR, "SpectrelightDynamics.Domain.Air")

USLMDomainAir::USLMDomainAir()
{
	DebugColor = FColor::Cyan;
	DomainTag = TAG_SPECTRELIGHTDYNAMICS_DOMAIN_AIR;
}

int32 USLMDomainAir::AddPort(const FSLMPortAir& Port)
{
	const int32 PortIndex = Ports.Add(Port.PortData);
	AddPortMetaData(Port.PortMetaData);
	const int32 NetworkIndex = Networks.Add(Port.PortData);
	PortIndexToNetworkIndex.Add(NetworkIndex);
	return PortIndex;
}

void USLMDomainAir::RemovePort(const int32 PortIndex)
{
    PortsToRemove.Add(PortIndex);
    bNeedsCleanup = true;
}

FSLMDataAir USLMDomainAir::GetData(const int32 PortIndex)
{
    check(PortIndexToNetworkIndex.IsValidIndex(PortIndex));
    const int32 NetworkIndex = PortIndexToNetworkIndex[PortIndex];
    check(Networks.IsValidIndex(NetworkIndex));
    return Networks[NetworkIndex];
}



void USLMDomainAir::SetData(const int32 PortIndex, const FSLMDataAir Data)
{
	check(PortIndexToNetworkIndex.IsValidIndex(PortIndex));
	const int32 NetworkIndex = PortIndexToNetworkIndex[PortIndex];
	check(Networks.IsValidIndex(NetworkIndex));
	Networks[NetworkIndex] = Data;
}

void USLMDomainAir::AddEnergyAndMoles(const int32 PortIndex, const float Energy, const float Moles)
{
	check(PortIndexToNetworkIndex.IsValidIndex(PortIndex));
	const int32 NetworkIndex = PortIndexToNetworkIndex[PortIndex];
	check(Networks.IsValidIndex(NetworkIndex));
	Networks[NetworkIndex].Energy += Energy;
	Networks[NetworkIndex].Moles += Moles;
}

void USLMDomainAir::RunTests()
{
	const FSLMDataAir Original = FSLMDataAir();
	FSLMDataAir A = Original;
	FSLMDataAir B = Original;
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

	
	FSLMDataAir Air = Original;
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

void USLMDomainAir::Simulate(const float DeltaTime, const float SubstepScalar)
{
	for (auto& Network : Networks)
	{
		if (Network.bConnectedToAtmosphere)
		{
			Network.SetToSTP();
		}
	}
}

FString USLMDomainAir::GetDebugString(const int32 PortIndex)
{
    check(PortIndexToNetworkIndex.IsValidIndex(PortIndex));
    const int32 NetworkIndex = PortIndexToNetworkIndex[PortIndex];
    check(Networks.IsValidIndex(NetworkIndex));
    const auto Network = Networks[NetworkIndex];
    FString Result;
    Result += "Air\n";
    Result += FString::Printf(TEXT("Port %i : Network %i\n"), PortIndex, NetworkIndex);
    Result += FString::Printf(TEXT("Pressure(Pa) = %f\n"), Network.GetPressure());
    Result += FString::Printf(TEXT("Volume(m3) = %f\n"), Network.Volume);
    Result += FString::Printf(TEXT("Temperature(K) = %f\n"), Network.GetTemperature());
	Result += FString::Printf(TEXT("Moles = %f\n"), Network.Moles);
	Result += FString::Printf(TEXT("Oxygen(percent) = %f\n"), 100 * Network.OxygenRatio);
	Result += FString::Printf(TEXT("Connected To Atmosphere = %i\n"), Network.bConnectedToAtmosphere);
    return Result;
}

void USLMDomainAir::CreateNetworkForPorts(const TArray<int32> PortIndices)
{
    const int32 NetworkIndex = Networks.Add(FSLMDataAir());
    float SumVolume = 0.0;
    float SumEnergy = 0.0;
	float SumMoles = 0.0;
	float SumOxygen = 0.0;
    for (const auto& PortIndex : PortIndices)
    {
        const auto Data = Ports[PortIndex];
        SumVolume += Data.Volume;
        SumEnergy += Data.Energy;
    	SumMoles += Data.Moles;
    	SumOxygen += Data.OxygenRatio * Data.Moles;
        PortIndexToNetworkIndex[PortIndex] = NetworkIndex;
    }
	FSLMDataAir Result;
	Result.Energy = SumEnergy;
	Result.Moles = SumMoles;
	Result.Volume = SumVolume;
	Result.OxygenRatio = SumOxygen / SumMoles;
	Result.bConnectedToAtmosphere = PortIndices.Num() == 1;
	Networks[NetworkIndex] = Result;
}

void USLMDomainAir::DissolveNetworkIntoPort(const int32 NetworkIndex, const int32 PortIndex)
{
    const FSLMDataAir Network = Networks[NetworkIndex];
    FSLMDataAir& PortData = Ports[PortIndex];
	const float Ratio = PortData.Volume / Network.Volume;
	PortData.Energy = Ratio * Network.Energy;
	PortData.Moles = Ratio * Network.Moles;
    PortData.OxygenRatio = Network.OxygenRatio;
}

void USLMDomainAir::RemovePortAtIndex(const int32 PortIndex)
{
    Ports.RemoveAt(PortIndex);
}

void USLMDomainAir::RemoveNetworkAtIndex(const int32 NetworkIndex)
{
    Networks.RemoveAt(NetworkIndex);
}

void USLMDomainAir::CreateNetworkForPort(const int32 Port)
{
    PortIndexToNetworkIndex[Port] = Networks.Add(Ports[Port]);
}
