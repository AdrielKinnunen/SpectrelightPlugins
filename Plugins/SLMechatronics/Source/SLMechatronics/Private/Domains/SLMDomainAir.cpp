// Copyright Spectrelight Studios, LLC

#include "Domains/SLMDomainAir.h"

UE_DEFINE_GAMEPLAY_TAG(TAG_SPECTRELIGHTDYNAMICS_DOMAIN_AIR, "SpectrelightDynamics.Domain.Air")

USLMDomainAir::USLMDomainAir()
{
    DebugColor = FColor::Blue;
	DomainTag = TAG_SPECTRELIGHTDYNAMICS_DOMAIN_AIR;
}

int32 USLMDomainAir::AddPort(const FSLMPortAir& Port)
{
    const int32 PortIndex = Ports.Add(Port.PortData);
    AddPortMetaData(Port.PortMetaData);
    PortsRecentlyAdded.Add(PortIndex);
    PortIndexToNetworkIndex.Add(-1);
    bNeedsCleanup = true;
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

void USLMDomainAir::RunTests()
{
	const FSLMDataAir Original = FSLMDataAir(1.0, 1.0, 300, 0.3, false);
	FSLMDataAir A = Original;
	FSLMDataAir B = Original;
	check(A.NearlyEqualWith(B));		//null case
	A.CompressOrExpandToVolume(Original.Volume_l / 10);
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


	
	FSLMDataAir Air = Original;
	FString Out;
	
	Out += "Air domain test results\n";
	Out += "Original state was:\n" + Air.GetDebugString() + "................................\n";
	
	Air.CompressOrExpandToVolume(Original.Volume_l / 2);
	Out += "After compressing to half original volume:\n" + Air.GetDebugString() + "................................\n";
	
	Air.CompressOrExpandToVolume(Original.Volume_l * 2);
	Out += "After expanding to double original volume:\n" + Air.GetDebugString() + "................................\n";
	
	Air.CompressOrExpandToVolume(Original.Volume_l);
	Out += "After compressing back to original volume:\n" + Air.GetDebugString() + "................................\n";

	Air.CompressOrExpandToPressure(Original.Pressure_bar * 2);
	Out += "After compressing to double original pressure:\n" + Air.GetDebugString() + "................................\n";

	Air.CompressOrExpandToPressure(Original.Pressure_bar / 2);
	Out += "After compressing to half original pressure:\n" + Air.GetDebugString() + "................................\n";

	Air.CompressOrExpandToPressure(Original.Pressure_bar);
	Out += "After compressing back to original pressure:\n" + Air.GetDebugString() + "................................\n";
	
	UE_LOG(LogTemp, Warning, TEXT("%s"), *Out);
}

/*
void USLMDomainAir::MixAndCompressIntoByIndex(const int32 PortIndex, const FSLMDataAir AirToAdd)
{
    check(PortIndexToNetworkIndex.IsValidIndex(PortIndex));
    const int32 NetworkIndex = PortIndexToNetworkIndex[PortIndex];
    check(Networks.IsValidIndex(NetworkIndex));
	Networks[NetworkIndex].Inject(AirToAdd);
	//FSLMDataAir MixedAir = FSLMDataAir::Mix(Networks[NetworkIndex], AirToAdd);
	//MixedAir.ChangeVolumeIsentropically(Networks[NetworkIndex].Volume_l);
	//Networks[NetworkIndex] = MixedAir;
}

FSLMDataAir USLMDomainAir::RemoveAir(const int32 PortIndex, const float VolumeLiters)
{
	check(PortIndexToNetworkIndex.IsValidIndex(PortIndex));
	const int32 NetworkIndex = PortIndexToNetworkIndex[PortIndex];
	check(Networks.IsValidIndex(NetworkIndex));
	FSLMDataAir& Network = Networks[NetworkIndex];
    return Network.Extract(VolumeLiters);
	
	const float OriginalVolume = Network.Volume_l;
	const float ExpandedVolume = OriginalVolume + VolumeLiters;
	Network.ChangeVolumeIsentropically(ExpandedVolume);
	FSLMDataAir Result = Network;
	Network.Volume_l = OriginalVolume;
	Result.Volume_l = VolumeLiters;
	
}
*/

void USLMDomainAir::Simulate(const float DeltaTime, const float SubstepScalar)
{
	for (auto& Network : Networks)
	{
		if (Network.bConnectedToAtmosphere)
		{
			Network.Pressure_bar = 1;
			Network.Temp_K = 300;
			Network.OxygenRatio = 0.21;
			Network.UpdateMoles();
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
    Result += FString::Printf(TEXT("Pressure(bar) = %f\n"), Network.Pressure_bar);
    Result += FString::Printf(TEXT("Volume(l) = %f\n"), Network.Volume_l);
    Result += FString::Printf(TEXT("Temperature(K) = %f\n"), Network.Temp_K);
	Result += FString::Printf(TEXT("Moles = %f\n"), Network.N_Moles);
	Result += FString::Printf(TEXT("Oxygen(percent) = %f\n"), 100 * Network.OxygenRatio);
	Result += FString::Printf(TEXT("Connected To Atmosphere = %i\n"), Network.bConnectedToAtmosphere);
    return Result;
}

void USLMDomainAir::CreateNetworkForPorts(const TArray<int32> PortIndices)
{
    const int32 NetworkIndex = Networks.Add(FSLMDataAir());
    float SumVolume = 0.0;
    float SumPV = 0.0;
	float SumMoles = 0.0;
	float SumOxygen = 0.0;
    for (const auto& PortIndex : PortIndices)
    {
        const auto Data = Ports[PortIndex];
        SumVolume += Data.Volume_l;
        SumPV += Data.Pressure_bar * Data.Volume_l;
    	SumMoles += Data.N_Moles;
    	SumOxygen += Data.OxygenRatio * Data.N_Moles;
        PortIndexToNetworkIndex[PortIndex] = NetworkIndex;
    }
	const float FinalOxygen = 0.21;//SumOxygen / SumMoles;
	const float FinalPressure = 1;//SumPV / SumVolume;
	const float FinalTemp = 300;//(FinalPressure * SumVolume) / (SumMoles * SLMIdealGasConstant);
	const bool bConnectedToAtmosphere = PortIndices.Num() == 1;
	Networks[NetworkIndex] = FSLMDataAir(SumVolume, FinalPressure, FinalTemp, FinalOxygen, bConnectedToAtmosphere);
}

void USLMDomainAir::DissolveNetworkIntoPort(const int32 NetworkIndex, const int32 PortIndex)
{
    const FSLMDataAir Network = Networks[NetworkIndex];
    FSLMDataAir& PortData = Ports[PortIndex];
    PortData.Pressure_bar = Network.Pressure_bar;
    PortData.Temp_K = Network.Temp_K;
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
