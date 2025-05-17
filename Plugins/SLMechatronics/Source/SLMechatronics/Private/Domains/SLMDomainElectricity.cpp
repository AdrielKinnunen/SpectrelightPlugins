// Copyright Spectrelight Studios, LLC

#include "Domains/SLMDomainElectricity.h"

UE_DEFINE_GAMEPLAY_TAG(TAG_SPECTRELIGHTDYNAMICS_DOMAIN_ELECTRICITY, "SpectrelightDynamics.Domain.Electricity")

USLMDomainElectricity::USLMDomainElectricity()
{
    DebugColor = FColor::Yellow;
	DomainTag = TAG_SPECTRELIGHTDYNAMICS_DOMAIN_ELECTRICITY;
}

int32 USLMDomainElectricity::AddPort(const FSLMPortElectricity& Port)
{
	const int32 PortIndex = Ports.Add(Port.PortData);
	AddPortMetaData(Port.PortMetaData);
	const int32 NetworkIndex = Networks.Add(Port.PortData);
	PortIndexToNetworkIndex.Add(NetworkIndex);
	return PortIndex;
}

void USLMDomainElectricity::RemovePort(const int32 PortIndex)
{
    PortsToRemove.Add(PortIndex);
    bNeedsCleanup = true;
}

FSLMDataElectricity USLMDomainElectricity::GetByPortIndex(const int32 PortIndex)
{
    check(PortIndexToNetworkIndex.IsValidIndex(PortIndex));
    const int32 NetworkIndex = PortIndexToNetworkIndex[PortIndex];
    check(Networks.IsValidIndex(NetworkIndex));
    return Networks[NetworkIndex];
}

void USLMDomainElectricity::SetJoulesByPortIndex(int32 PortIndex, float NewJoules)
{
    check(PortIndexToNetworkIndex.IsValidIndex(PortIndex));
    const int32 NetworkIndex = PortIndexToNetworkIndex[PortIndex];
    check(Networks.IsValidIndex(NetworkIndex));
    Networks[NetworkIndex].StoredJoules = NewJoules;
}

FString USLMDomainElectricity::GetDebugString(const int32 PortIndex)
{
    check(PortIndexToNetworkIndex.IsValidIndex(PortIndex));
    const int32 NetworkIndex = PortIndexToNetworkIndex[PortIndex];
    check(Networks.IsValidIndex(NetworkIndex));
    const auto Network = Networks[NetworkIndex];
    FString Result;
    Result += "Electricity\n";
    Result += FString::Printf(TEXT("Port %i : Network %i\n"), PortIndex, NetworkIndex);
    Result += FString::Printf(TEXT("Stored = %f\n"), Network.StoredJoules);
    Result += FString::Printf(TEXT("Capacity = %f\n"), Network.CapacityJoules);
    return Result;
}

void USLMDomainElectricity::CreateNetworkForPorts(const TArray<int32> PortIndices)
{
    const int32 NetworkIndex = Networks.Add(FSLMDataElectricity());
    float SumStored = 0;
    float SumCapacity = 0;
    for (const auto& PortIndex : PortIndices)
    {
        SumStored += Ports[PortIndex].StoredJoules;
        SumCapacity += Ports[PortIndex].CapacityJoules;
        PortIndexToNetworkIndex[PortIndex] = NetworkIndex;
    }
    Networks[NetworkIndex].StoredJoules = SumStored;
    Networks[NetworkIndex].CapacityJoules = SumCapacity;
}

void USLMDomainElectricity::DissolveNetworkIntoPort(const int32 NetworkIndex, const int32 PortIndex)
{
    const FSLMDataElectricity Network = Networks[NetworkIndex];
    const float NetworkPercent = Network.StoredJoules / Network.CapacityJoules;
    Ports[PortIndex].StoredJoules = NetworkPercent * Ports[PortIndex].CapacityJoules;
}

void USLMDomainElectricity::RemovePortAtIndex(const int32 PortIndex)
{
    Ports.RemoveAt(PortIndex);
}

void USLMDomainElectricity::RemoveNetworkAtIndex(const int32 NetworkIndex)
{
    Networks.RemoveAt(NetworkIndex);
}

void USLMDomainElectricity::CreateNetworkForPort(const int32 Port)
{
    PortIndexToNetworkIndex[Port] = Networks.Add(Ports[Port]);
}
