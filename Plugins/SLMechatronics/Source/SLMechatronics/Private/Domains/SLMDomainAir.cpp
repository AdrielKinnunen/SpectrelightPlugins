// Copyright Spectrelight Studios, LLC

#include "Domains/SLMDomainAir.h"

USLMDomainAir::USLMDomainAir()
{
	DomainColor = FColor::Blue;
}

int32 USLMDomainAir::AddPort(const FSLMPortAir& Port)
{
	const int32 PortIndex = PortsData.Add(Port.PortData);
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

FSLMDataAir USLMDomainAir::GetByPortIndex(const int32 PortIndex)
{
	check(PortIndexToNetworkIndex.IsValidIndex(PortIndex));
	const int32 NetworkIndex = PortIndexToNetworkIndex[PortIndex];
	check(Networks.IsValidIndex(NetworkIndex));
	return Networks[NetworkIndex];
}

void USLMDomainAir::AddAir(const int32 PortIndex, const FSLMDataAir AirToAdd)
{
	
}

FSLMDataAir USLMDomainAir::RemoveAir(const int32 PortIndex, const float VolumeLiters)
{
	return FSLMDataAir();
}

void USLMDomainAir::Simulate(const float DeltaTime)
{
	Super::Simulate(DeltaTime);
}

void USLMDomainAir::CreateNetworkForPorts(const TArray<int32> PortIndices)
{
	const int32 NetworkIndex = Networks.Add(FSLMDataAir());
	float SumVolume = 0.0;
	float SumTemp = 0.0;
	for (const auto& PortIndex : PortIndices)
	{
		const auto Data = PortsData[PortIndex];
		
		SumVolume += Data.Volume_l;
		SumTemp += Data.Temp_K;
		
		PortIndexToNetworkIndex[PortIndex] = NetworkIndex;
	}
}

void USLMDomainAir::DissolveNetworkIntoPort(const int32 NetworkIndex, const int32 PortIndex)
{
	const FSLMDataAir Network = Networks[NetworkIndex];
	FSLMDataAir& PortData = PortsData[PortIndex];
	
	PortData.Pressure_bar = Network.Pressure_bar;
	PortData.Temp_K = Network.Temp_K;
	//PortData.Oxygen = NetworkData.Oxygen;
}

void USLMDomainAir::RemovePortAtIndex(const int32 PortIndex)
{
	PortsData.RemoveAt(PortIndex);
}

void USLMDomainAir::RemoveNetworkAtIndex(const int32 NetworkIndex)
{
	Networks.RemoveAt(NetworkIndex);
}

void USLMDomainAir::CreateNetworkForPort(const int32 Port)
{
	PortIndexToNetworkIndex[Port] = Networks.Add(PortsData[Port]);
}
