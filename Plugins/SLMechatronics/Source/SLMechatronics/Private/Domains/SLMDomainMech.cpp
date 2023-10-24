// Copyright Spectrelight Studios, LLC


#include "Domains/SLMDomainMech.h"

int32 USLMDomainMech::AddPort(const FSLMPortMech& Port)
{
	const int32 PortIndex = Ports.Add(Port);
	PortsRecentlyAdded.Add(PortIndex);
	PortIndexToNetworkIndex.Add(-1);
	bNeedsCleanup = true;
	return PortIndex;
}

void USLMDomainMech::RemovePort(const int32 PortIndex)
{
	PortsToRemove.Add(PortIndex);
	bNeedsCleanup = true;
}

FSLMDataMech USLMDomainMech::GetNetworkData(const int32 PortIndex)
{
	const int32 NetworkIndex = PortIndexToNetworkIndex[PortIndex];
	return Networks[NetworkIndex];
}

void USLMDomainMech::SetNetworkAngVel(const int32 PortIndex, const float NewAngVel)
{
	const int32 NetworkIndex = PortIndexToNetworkIndex[PortIndex];
	Networks[NetworkIndex].AngVel = NewAngVel;
}

void USLMDomainMech::CreateNetworkForPorts(const TArray<int32> PortIndices)
{
	//UE_LOG(LogTemp, Warning, TEXT("Creating network for %i ports"), PortIndices.Num());
	const int32 NetworkIndex = Networks.Add(FSLMDataMech());
	float SumProduct = 0;
	float SumMOI = 0;
	for (const auto& PortIndex : PortIndices)
	{
		SumProduct += Ports[PortIndex].DefaultData.AngVel * Ports[PortIndex].DefaultData.MOI;
		SumMOI += Ports[PortIndex].DefaultData.MOI;
		PortIndexToNetworkIndex[PortIndex] = NetworkIndex;
	}
	Networks[NetworkIndex].AngVel = SumProduct / SumMOI;
	Networks[NetworkIndex].MOI = SumMOI;
}

void USLMDomainMech::DissolveNetworkIntoPort(const int32 NetworkIndex, const int32 PortIndex)
{
	const FSLMDataMech Network = Networks[NetworkIndex];
	Ports[PortIndex].DefaultData.AngVel = Network.AngVel;
}

void USLMDomainMech::RemovePortAtIndex(const int32 PortIndex)
{
	Ports.RemoveAt(PortIndex);
}

void USLMDomainMech::RemoveNetworkAtIndex(const int32 NetworkIndex)
{
	Networks.RemoveAt(NetworkIndex);
}

void USLMDomainMech::CreateNetworkForPort(const int32 Port)
{
	PortIndexToNetworkIndex[Port] = Networks.Add(Ports[Port].DefaultData);
}
