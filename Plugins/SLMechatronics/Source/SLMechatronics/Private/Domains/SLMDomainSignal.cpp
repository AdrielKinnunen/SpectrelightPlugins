// Copyright Spectrelight Studios, LLC

#include "Domains/SLMDomainSignal.h"

int32 USLMDomainSignal::AddPort(const FSLMPortSignal& Port)
{
	const int32 PortIndex = PortsData.Add(Port.PortData);
	PortsRecentlyAdded.Add(PortIndex);
	PortIndexToNetworkIndex.Add(-1);
	bNeedsCleanup = true;
	return PortIndex;
}

void USLMDomainSignal::RemovePort(const int32 PortIndex)
{
	PortsToRemove.Add(PortIndex);
	bNeedsCleanup = true;
}

float USLMDomainSignal::ReadByPortIndex(const int32 PortIndex)
{
	check(PortIndex >= 0);
	const int32 NetworkIndex = PortIndexToNetworkIndex[PortIndex];
	check(NetworkIndex >= 0);
	return Networks[NetworkIndex].Read;
}

void USLMDomainSignal::WriteData(const int32 PortIndex, const float Data)
{
	check(PortIndex >= 0);
	const int32 NetworkIndex = PortIndexToNetworkIndex[PortIndex];
	check(NetworkIndex >= 0);
	Networks[NetworkIndex].Write = Data;
}

void USLMDomainSignal::PostSimulate(const float DeltaTime)
{
	for (auto& Network : Networks)
	{
		Network.Read = Network.Write;
	}
}

void USLMDomainSignal::CreateNetworkForPorts(const TArray<int32> PortIndices)
{
	UE_LOG(LogTemp, Warning, TEXT("Creating network for %i ports"), PortIndices.Num());
	const int32 NetworkIndex = Networks.Add(FSLMDataSignal());
	for (const auto& PortIndex : PortIndices)
	{
		PortIndexToNetworkIndex[PortIndex] = NetworkIndex;
	}
}

void USLMDomainSignal::DissolveNetworkIntoPort(const int32 NetworkIndex, const int32 PortIndex)
{
	PortsData[PortIndex] = FSLMDataSignal();
}

void USLMDomainSignal::RemovePortAtIndex(const int32 PortIndex)
{
	PortsData.RemoveAt(PortIndex);
}

void USLMDomainSignal::RemoveNetworkAtIndex(const int32 NetworkIndex)
{
	Networks.RemoveAt(NetworkIndex);
}

void USLMDomainSignal::CreateNetworkForPort(const int32 Port)
{
	PortIndexToNetworkIndex[Port] = Networks.Add(PortsData[Port]);
}
