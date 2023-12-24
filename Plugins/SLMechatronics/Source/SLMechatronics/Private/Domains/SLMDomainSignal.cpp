// Copyright Spectrelight Studios, LLC

#include "Domains/SLMDomainSignal.h"

USLMDomainSignal::USLMDomainSignal()
{
	DomainColor = FColor::White;
}

int32 USLMDomainSignal::AddPort(const FSLMPortSignal& Port)
{
	const int32 PortIndex = PortsData.Add(Port.PortData);
	AddPortMetaData(Port.PortMetaData);
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
	check(PortIndexToNetworkIndex.IsValidIndex(PortIndex));
	const int32 NetworkIndex = PortIndexToNetworkIndex[PortIndex];
	check(Networks.IsValidIndex(NetworkIndex));
	return Networks[NetworkIndex].Read;
}

void USLMDomainSignal::WriteByPortIndex(const int32 PortIndex, const float Data)
{
	check(PortIndexToNetworkIndex.IsValidIndex(PortIndex));
	const int32 NetworkIndex = PortIndexToNetworkIndex[PortIndex];
	check(Networks.IsValidIndex(NetworkIndex));
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
