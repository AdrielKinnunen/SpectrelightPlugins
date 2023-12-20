// Copyright Spectrelight Studios, LLC

#include "Domains/SLMDomainRotation.h"

int32 USLMDomainRotation::AddPort(const FSLMPortRotation& Port)
{
	const int32 PortIndex = PortsData.Add(Port.PortData);
	PortsMetaData.Add(Port.PortMetaData);
	PortsRecentlyAdded.Add(PortIndex);
	PortIndexToNetworkIndex.Add(-1);
	bNeedsCleanup = true;
	return PortIndex;
}

void USLMDomainRotation::RemovePort(const int32 PortIndex)
{
	PortsToRemove.Add(PortIndex);
	bNeedsCleanup = true;
}

FSLMDataRotation USLMDomainRotation::GetByPortIndex(const int32 PortIndex)
{
	const int32 NetworkIndex = PortIndexToNetworkIndex[PortIndex];
	return Networks[NetworkIndex];
}

void USLMDomainRotation::SetNetworkAngVel(const int32 PortIndex, const float NewAngVel)
{
	const int32 NetworkIndex = PortIndexToNetworkIndex[PortIndex];
	Networks[NetworkIndex].RPS = NewAngVel;
}

void USLMDomainRotation::CreateNetworkForPorts(const TArray<int32> PortIndices)
{
	//UE_LOG(LogTemp, Warning, TEXT("Creating network for %i ports"), PortIndices.Num());
	const int32 NetworkIndex = Networks.Add(FSLMDataRotation());
	float SumProduct = 0;
	float SumMOI = 0;
	for (const auto& PortIndex : PortIndices)
	{
		SumProduct += PortsData[PortIndex].RPS * PortsData[PortIndex].MOI;
		SumMOI += PortsData[PortIndex].MOI;
		PortIndexToNetworkIndex[PortIndex] = NetworkIndex;
	}
	Networks[NetworkIndex].RPS = SumProduct / SumMOI;
	Networks[NetworkIndex].MOI = SumMOI;
}

void USLMDomainRotation::DissolveNetworkIntoPort(const int32 NetworkIndex, const int32 PortIndex)
{
	const FSLMDataRotation Network = Networks[NetworkIndex];
	PortsData[PortIndex].RPS = Network.RPS;
}

void USLMDomainRotation::RemovePortAtIndex(const int32 PortIndex)
{
	PortsData.RemoveAt(PortIndex);
}

void USLMDomainRotation::RemoveNetworkAtIndex(const int32 NetworkIndex)
{
	Networks.RemoveAt(NetworkIndex);
}

void USLMDomainRotation::CreateNetworkForPort(const int32 Port)
{
	PortIndexToNetworkIndex[Port] = Networks.Add(PortsData[Port]);
}
