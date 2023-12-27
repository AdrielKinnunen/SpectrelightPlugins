// Copyright Spectrelight Studios, LLC

#include "Domains/SLMDomainRotation.h"

USLMDomainRotation::USLMDomainRotation()
{
	DomainColor = FColor::Black;
}

int32 USLMDomainRotation::AddPort(const FSLMPortRotation& Port)
{
	const int32 PortIndex = Ports.Add(Port.PortData);
	AddPortMetaData(Port.PortMetaData);
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

FSLMDataRotation USLMDomainRotation::GetData(const int32 PortIndex)
{
	check(PortIndexToNetworkIndex.IsValidIndex(PortIndex));
	const int32 NetworkIndex = PortIndexToNetworkIndex[PortIndex];
	check(Networks.IsValidIndex(NetworkIndex));
	return Networks[NetworkIndex];
}

void USLMDomainRotation::SetAngularVelocity(const int32 PortIndex, const float NewAngVel)
{
	check(PortIndexToNetworkIndex.IsValidIndex(PortIndex));
	const int32 NetworkIndex = PortIndexToNetworkIndex[PortIndex];
	check(Networks.IsValidIndex(NetworkIndex));
	Networks[NetworkIndex].AngularVelocity = NewAngVel;
}

void USLMDomainRotation::Simulate(const float DeltaTime)
{
	//for (auto& Network : Networks)
	//{
		//Network.RPS *= 1 - 0.1 * DeltaTime;
	//}
}

FString USLMDomainRotation::GetDebugString(const int32 PortIndex)
{
	check(PortIndexToNetworkIndex.IsValidIndex(PortIndex));
	const int32 NetworkIndex = PortIndexToNetworkIndex[PortIndex];
	check(Networks.IsValidIndex(NetworkIndex));
	const auto Network =  Networks[NetworkIndex];
	FString Result;
	Result += "Rotation\n";
	Result += FString::Printf(TEXT("Port %i : Network %i\n"), PortIndex, NetworkIndex);
	Result += FString::Printf(TEXT("MOI = %f\n"), Network.MomentOfInertia);
	Result += FString::Printf(TEXT("Rad/s = %f\n"), Network.AngularVelocity);
	return Result;
}

void USLMDomainRotation::CreateNetworkForPorts(const TArray<int32> PortIndices)
{
	const int32 NetworkIndex = Networks.Add(FSLMDataRotation());
	float SumProduct = 0;
	float SumMOI = 0;
	for (const auto& PortIndex : PortIndices)
	{
		SumProduct += Ports[PortIndex].AngularVelocity * Ports[PortIndex].MomentOfInertia;
		SumMOI += Ports[PortIndex].MomentOfInertia;
		PortIndexToNetworkIndex[PortIndex] = NetworkIndex;
	}
	Networks[NetworkIndex].AngularVelocity = SumProduct / SumMOI;
	Networks[NetworkIndex].MomentOfInertia = SumMOI;
}

void USLMDomainRotation::DissolveNetworkIntoPort(const int32 NetworkIndex, const int32 PortIndex)
{
	const FSLMDataRotation Network = Networks[NetworkIndex];
	Ports[PortIndex].AngularVelocity = Network.AngularVelocity;
}

void USLMDomainRotation::RemovePortAtIndex(const int32 PortIndex)
{
	Ports.RemoveAt(PortIndex);
}

void USLMDomainRotation::RemoveNetworkAtIndex(const int32 NetworkIndex)
{
	Networks.RemoveAt(NetworkIndex);
}

void USLMDomainRotation::CreateNetworkForPort(const int32 Port)
{
	PortIndexToNetworkIndex[Port] = Networks.Add(Ports[Port]);
}
