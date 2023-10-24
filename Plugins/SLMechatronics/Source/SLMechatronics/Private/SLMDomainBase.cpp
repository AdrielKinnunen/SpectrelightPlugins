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
	check(PortsRecentlyAdded.Num() == 0);
	check(PortsToRemove.Num() == 0);
	check(ConnectionsToAdd.Num() == 0);
	check(ConnectionsToRemove.Num() == 0);
}

void USLMDomainSubsystemBase::PreSimulate(const float DeltaTime)
{
}

void USLMDomainSubsystemBase::Simulate(const float DeltaTime)
{
}

void USLMDomainSubsystemBase::PostSimulate(const float DeltaTime)
{
}

void USLMDomainSubsystemBase::TestPrintAllData()
{
	UE_LOG(LogTemp, Warning, TEXT("-----------------------------------------------"));
	UE_LOG(LogTemp, Warning, TEXT("    Printing all data for SLDomainBase"));
	TArray<int32> AdjacencyKeys;
	Adjacencies.GetKeys(AdjacencyKeys);
	UE_LOG(LogTemp, Warning, TEXT("Adjacency List:"));
	for (const auto Key : AdjacencyKeys)
	{
		TArray<int32> Values;
		Adjacencies.MultiFind(Key, Values);
		for (const auto Value : Values)
		{
			UE_LOG(LogTemp, Warning, TEXT("    Port %i is adjacent to port %i"), Key, Value);
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("Port to Network mapping:"));
	const auto Max = PortIndexToNetworkIndex.GetMaxIndex();
	for (int32 i = 0; i < Max; i++)
	{
		if (PortIndexToNetworkIndex.IsValidIndex(i))
		{
			const auto Network = PortIndexToNetworkIndex[i];
			UE_LOG(LogTemp, Warning, TEXT("    Port %i maps to network %i"), i, Network);
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("-----------------------------------------------"));
}

void USLMDomainSubsystemBase::ConnectPorts(int32 FirstPortIndex, int32 SecondPortIndex)
{
	ConnectionsToAdd.Add(FSLMConnection(FirstPortIndex, SecondPortIndex));
	bNeedsCleanup = true;

	/*if (FirstPortIndex != SecondPortIndex)
	{
		Adjacencies.Add(FirstPortIndex, SecondPortIndex);
		Adjacencies.Add(SecondPortIndex, FirstPortIndex);
		PortsDirty.Add(FirstPortIndex);
		PortsDirty.Add(SecondPortIndex);
		bNeedsCleanup = true;
		UE_LOG(LogTemp, Warning, TEXT("Connecting Port %i to Port %i"), FirstPortIndex, SecondPortIndex);
	}*/
}

void USLMDomainSubsystemBase::DisconnectPorts(int32 FirstPortIndex, int32 SecondPortIndex)
{
	ConnectionsToRemove.Add(FSLMConnection(FirstPortIndex, SecondPortIndex));
	bNeedsCleanup = true;

	/*
	Adjacencies.Remove(FirstPortIndex, SecondPortIndex);
	Adjacencies.Remove(SecondPortIndex, FirstPortIndex);
	PortsDirty.Add(FirstPortIndex);
	PortsDirty.Add(SecondPortIndex);
	bNeedsCleanup = true;
	UE_LOG(LogTemp, Warning, TEXT("Disonnecting Port %i from Port %i"), FirstPortIndex, SecondPortIndex);
	*/
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
	TSet<int32> PortsDirty;
	//UE_LOG(LogTemp, Warning, TEXT("%i ports are dirty at beginning of cleanup"), PortsDirty.Num());


	//Handle ConnectionsToAdd
	for (const auto& [FirstIndex, SecondIndex] : ConnectionsToAdd)
	{
		Adjacencies.Add(FirstIndex, SecondIndex);
		Adjacencies.Add(SecondIndex, FirstIndex);
		PortsDirty.Add(FirstIndex);
		PortsDirty.Add(SecondIndex);
	}
	ConnectionsToAdd.Empty();


	//Handle ConnectionsToRemove
	for (const auto& [FirstIndex, SecondIndex] : ConnectionsToRemove)
	{
		Adjacencies.Remove(FirstIndex, SecondIndex);
		Adjacencies.Remove(SecondIndex, FirstIndex);
		PortsDirty.Add(FirstIndex);
		PortsDirty.Add(SecondIndex);
	}
	ConnectionsToRemove.Empty();


	//Handle PortsRecentlyAdded
	PortsDirty.Append(PortsRecentlyAdded);
	PortsRecentlyAdded.Empty();


	//Handle PortsToRemove
	PortsDirty.Append(GetConnectedPorts(PortsToRemove).Difference(PortsToRemove));
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


	//Dirty all of PortsDirty's connected neighbors
	PortsDirty = GetConnectedPorts(PortsDirty);


	//Dissolve all network values back into port data
	TSet<int32> NetworkIndicesToRemove;
	for (const auto& PortIndex : PortsDirty)
	{
		if (PortIndexToNetworkIndex[PortIndex] > -1)
		{
			const int32 NetworkIndex = PortIndexToNetworkIndex[PortIndex];
			DissolveNetworkIntoPort(NetworkIndex, PortIndex);
			NetworkIndicesToRemove.Add(NetworkIndex);
		}
	}


	//Remove all dirty port's Networks
	for (const auto& Index : NetworkIndicesToRemove)
	{
		RemoveNetworkAtIndex(Index);
	}


	//Create network for each set of connected dirty ports
	TSet<int32> Visited;
	for (const auto& Port : PortsDirty)
	{
		if (!Visited.Contains(Port))
		{
			auto Connected = GetConnectedPorts(TSet<int32>{Port});
			Visited.Append(Connected);
			CreateNetworkForPorts(Connected.Array());
		}
	}
}

TSet<int32> USLMDomainSubsystemBase::GetConnectedPorts(const TSet<int32>& Roots) const
{
	TSet<int32> Visited;
	TSet<int32> Connected;
	TArray<int32> Stack;
	Visited.Append(Roots);
	Connected.Append(Roots);
	Stack.Append(Roots.Array());

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
