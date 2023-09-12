// Copyright Spectrelight Studios, LLC


#include "SLMDomainBase.h"

void USLMDomainSubsystemBase::CheckForCleanUp()
{
	if (bNeedsCleanup)
	{
		CleanUpGraph();
		bNeedsCleanup = false;
	}
	//Sanity checks
	check(PortsToAdd.Num() == 0);
	check(PortsToRemove.Num() == 0);
	check(PortsDirty.Num() == 0);
}

void USLMDomainSubsystemBase::ConnectPorts(int32 FirstPortIndex, int32 SecondPortIndex)
{
	if (FirstPortIndex != SecondPortIndex)
	{
		Adjacencies.Add(FirstPortIndex, SecondPortIndex);
		Adjacencies.Add(SecondPortIndex, FirstPortIndex);
		PortsDirty.Add(FirstPortIndex);
		PortsDirty.Add(SecondPortIndex);
		bNeedsCleanup = true;
		UE_LOG(LogTemp, Warning, TEXT("Connecting Port %i to Port %i"), FirstPortIndex, SecondPortIndex);
	}
}

void USLMDomainSubsystemBase::DisconnectPorts(int32 FirstPortIndex, int32 SecondPortIndex)
{
	Adjacencies.Remove(FirstPortIndex, SecondPortIndex);
	Adjacencies.Remove(SecondPortIndex, FirstPortIndex);
	PortsDirty.Add(FirstPortIndex);
	PortsDirty.Add(SecondPortIndex);
	bNeedsCleanup = true;
	UE_LOG(LogTemp, Warning, TEXT("Disonnecting Port %i from Port %i"), FirstPortIndex, SecondPortIndex);
}

bool USLMDomainSubsystemBase::ArePortsConnected(int32 FirstPortIndex, int32 SecondPortIndex)
{
	return Adjacencies.FindPair(FirstPortIndex, SecondPortIndex) || Adjacencies.FindPair(SecondPortIndex, FirstPortIndex);
}

void USLMDomainSubsystemBase::CreateNetworkForPorts(TArray<int32> PortIndices)
{
}

void USLMDomainSubsystemBase::RemovePortAtIndex(int32 Index)
{
}

void USLMDomainSubsystemBase::RemoveNetworkAtIndex(int32 NetworkIndex)
{
}

void USLMDomainSubsystemBase::DissolveNetworkIntoPort(int32 NetworkIndex, int32 PortIndex)
{
}

void USLMDomainSubsystemBase::CleanUpGraph()
{
	//UE_LOG(LogTemp, Warning, TEXT("%i/%i ports are dirty at beginning of cleanup"), PortsDirty.Num(), Ports.Num());

	//Dirty all Neighbors of PortsToRemove
	const TArray<int32> Neighbors = GetConnectedPorts(PortsToRemove);
	PortsDirty.Append(Neighbors);
	
	//Remove PortsToRemove from Adjacencies, Ports, and PortIndexToNetworkIndex
	for (const auto& PortIndex : PortsToRemove)
	{
		TArray<int32> AdjacentPorts;
		Adjacencies.MultiFind(PortIndex, AdjacentPorts);
		for (const auto& Adjacent : AdjacentPorts)
		{
			Adjacencies.Remove(Adjacent, PortIndex);
		}
		Adjacencies.Remove(PortIndex);
		RemovePortAtIndex(PortIndex);
		PortIndexToNetworkIndex.RemoveAt(PortIndex);
	}
	PortsToRemove.Empty();
	//UE_LOG(LogTemp, Warning, TEXT("%i/%i ports are dirty afer PortsToRemove"), PortsDirty.Num(), Ports.Num());

	//Dirty all dirty ports connected neighbors
	PortsDirty = GetConnectedPorts(PortsDirty);
	//UE_LOG(LogTemp, Warning, TEXT("%i/%i ports are dirty after dirtying neighbors"), PortsDirty.Num(), Ports.Num());
	
	//Dissolve all network values back into port data
	TArray<int32> NetworkIndicesToRemove;
	for (const auto& PortIndex : PortsDirty)
	{
		const int32 NetworkIndex = PortIndexToNetworkIndex[PortIndex];
		if (NetworkIndex >= 0)
		{
			DissolveNetworkIntoPort(NetworkIndex, PortIndex);
			NetworkIndicesToRemove.AddUnique(NetworkIndex);
		}
	}
	//Remove all dirty port's Networks
	for (const auto& Index : NetworkIndicesToRemove)
	{
		RemoveNetworkAtIndex(Index);
	}
	//UE_LOG(LogTemp, Warning, TEXT("%i dirty  %i total  %i networks %i connections"), PortsDirty.Num(), Ports.Num(), Networks.Num(), Adjacencies.Num());

	//Create network for each set of connected dirty ports
	TSet<int32> Visited;
	for (const auto& Port : PortsDirty)
	{
		if (!Visited.Contains(Port) && !PortsToRemove.Contains(Port))
		{
			auto Connected = GetConnectedPorts(TArray<int32>{Port});
			Visited.Append(Connected);
			CreateNetworkForPorts(Connected);
		}
	}
	PortsDirty.Empty();
	//UE_LOG(LogTemp, Warning, TEXT("%i dirty  %i total  %i networks %i connections"), PortsDirty.Num(), Ports.Num(), Networks.Num(), Adjacencies.Num());
	//TestPrintALlPortData();
}

TArray<int32> USLMDomainSubsystemBase::GetConnectedPorts(const TArray<int32>& Roots) const
{
	TSet<int32> Visited;
	TArray<int32> Connected;
	TArray<int32> Stack;
	Visited.Append(Roots);
	Connected.Append(Roots);
	Stack.Append(Roots);

	TArray<int32> Neighbors;
	while (!Stack.IsEmpty())
	{
		const int32 Index = Stack.Pop(false);
		Adjacencies.MultiFind(Index, Neighbors);
		for (auto& Neighbor : Neighbors)
		{
			if (!Visited.Contains(Neighbor))
			{
				Visited.Add(Neighbor);
				Stack.Add(Neighbor);
				Connected.Add(Neighbor);
			}
		}
		Neighbors.Empty();
	}
	return Connected;
}
