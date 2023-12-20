// Copyright Spectrelight Studios, LLC

#include "Domains/SLMDomainAir.h"

int32 USLMDomainAir::AddPort(const FSLMPortAir& Port)
{
	const int32 PortIndex = PortsData.Add(Port.PortData);
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
	check(PortIndex >= 0);
	const int32 NetworkIndex = PortIndexToNetworkIndex[PortIndex];
	check(NetworkIndex >= 0);
	return Networks[NetworkIndex];
}

FSLMDataAir USLMDomainAir::RemoveAir(const int32 PortIndex, const float VolumeLiters)
{
	return FSLMDataAir();
}

void USLMDomainAir::AddAir(const int32 PortIndex, const FSLMDataAir AirToAdd)
{
	
}


/*
void USLMDomainAir::WriteData(const int32 PortIndex, const float Data)
{
	check(PortIndex >= 0);
	const int32 NetworkIndex = PortIndexToNetworkIndex[PortIndex];
	check(NetworkIndex >= 0);
	Networks[NetworkIndex].Write = Data;
}

void USLMDomainAir::PostSimulate(const float DeltaTime)
{
	for (auto& Network : Networks)
	{
	}
}
*/

void USLMDomainAir::CreateNetworkForPorts(const TArray<int32> PortIndices)
{
	const int32 NetworkIndex = Networks.Add(FSLMDataAir());
	float SumVolume = 0.0;
	float SumTemp = 0.0;
	//float Oxygen = kgPerLiterAtSSL;
	for (const auto& PortIndex : PortIndices)
	{
		const auto Data = PortsData[PortIndex];
		
		SumVolume += Data.Volume_l;
		SumTemp += Data.Temp_K;
		
		PortIndexToNetworkIndex[PortIndex] = NetworkIndex;
	}
	//Networks[NetworkIndex].AngVel = SumProduct / Volume;
	//Networks[NetworkIndex].MOI = Volume;
}

void USLMDomainAir::DissolveNetworkIntoPort(const int32 NetworkIndex, const int32 PortIndex)
{
	const FSLMDataAir NetworkData = Networks[NetworkIndex];
	FSLMDataAir& PortData = PortsData[PortIndex];
	
	PortData.Pressure_bar = NetworkData.Pressure_bar;
	PortData.Temp_K = NetworkData.Temp_K;
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
