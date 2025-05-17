// Copyright Spectrelight Studios, LLC

#include "Domains/SLMDomainSignal.h"

UE_DEFINE_GAMEPLAY_TAG(TAG_SPECTRELIGHTDYNAMICS_DOMAIN_SIGNAL, "SpectrelightDynamics.Domain.Signal")

USLMDomainSignal::USLMDomainSignal()
{
    DebugColor = FColor::White;
	DomainTag = TAG_SPECTRELIGHTDYNAMICS_DOMAIN_SIGNAL;
}

int32 USLMDomainSignal::AddPort(const FSLMPortSignal& Port)
{
    const int32 PortIndex = Ports.Add(Port.PortData);
    AddPortMetaData(Port.PortMetaData);
	const int32 NetworkIndex = Networks.Add(Port.PortData);
	PortIndexToNetworkIndex.Add(NetworkIndex);
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

FString USLMDomainSignal::GetDebugString(const int32 PortIndex)
{
    check(PortIndexToNetworkIndex.IsValidIndex(PortIndex));
    const int32 NetworkIndex = PortIndexToNetworkIndex[PortIndex];
    check(Networks.IsValidIndex(NetworkIndex));
    const auto Network = Networks[NetworkIndex];
    FString Result;
	Result += PortsMetaData[PortIndex].PortName.ToString();
    Result += "Signal\n";
    Result += FString::Printf(TEXT("Port %i : Network %i\n"), PortIndex, NetworkIndex);
    Result += FString::Printf(TEXT("Read = %f\n"), Network.Read);
    Result += FString::Printf(TEXT("Write = %f\n"), Network.Write);
    return Result;
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
    Ports[PortIndex] = FSLMDataSignal();
}

void USLMDomainSignal::RemovePortAtIndex(const int32 PortIndex)
{
    Ports.RemoveAt(PortIndex);
}

void USLMDomainSignal::RemoveNetworkAtIndex(const int32 NetworkIndex)
{
    Networks.RemoveAt(NetworkIndex);
}

void USLMDomainSignal::CreateNetworkForPort(const int32 Port)
{
    PortIndexToNetworkIndex[Port] = Networks.Add(Ports[Port]);
}
