// Copyright Spectrelight Studios, LLC

#include "SLMDomainBase.h"
#include "SLMDeviceBase.h"
#include "SLMManager.h"



bool FSLMDomainSystemBase::DoesConnectionExist(const FSLMConnection& Connection)
{
	const int32* FirstPortIDPtr = PortAddressToPortID.Find(Connection.First);
	const int32* SecondPortIDPtr = PortAddressToPortID.Find(Connection.Second);
	if (!FirstPortIDPtr || !SecondPortIDPtr)
	{
		return false;
	}
	return ArePortsConnected(*FirstPortIDPtr, *SecondPortIDPtr);
}

void FSLMDomainSystemBase::AddConnection(const FSLMConnection& Connection)
{
	if (Connection.First.DomainTag != Connection.Second.DomainTag) return;
	if (Connection.First == Connection.Second) return;
	ConnectionsToAdd.Add(Connection);
	bNeedsCleanUp = true;
}

void FSLMDomainSystemBase::RemoveConnection(const FSLMConnection& Connection)
{
	if (Connection.First.DomainTag != Connection.Second.DomainTag) return;
	if (Connection.First == Connection.Second) return;
	ConnectionsToRemove.Add(Connection);
	bNeedsCleanUp = true;
}

bool FSLMDomainSystemBase::WorldLocationToPortAddress(const FSLMSpatialContextRuntime& Filter, const FVector& WorldLocation, FSLMPortAddress& OutAddress)
{
	const int32 PortID = WorldLocationToPortID(Filter, WorldLocation);
	if (PortID != INDEX_NONE)
	{
		OutAddress = PortIDToPortAddress[PortID];
		return true;
	}
	return false;
}

bool FSLMDomainSystemBase::PortAddressToWorldLocation(const FSLMPortAddress& PortAddress, FVector& OutWorldLocation)
{
	if (const int32* PortIDPtr = PortAddressToPortID.Find(PortAddress))
	{
		OutWorldLocation = PortIDToWorldLocation(*PortIDPtr);
		return true;
	}
	return false;
}

void FSLMDomainSystemBase::AppendPortAddressesForDevices(const TArray<FSLMDeviceAddress>& DeviceAddresses, TArray<FSLMPortAddress>& PortAddresses) const
{
	for (const auto& DeviceAddress : DeviceAddresses)
	{
		DeviceAddressToPortAddresses.MultiFind(DeviceAddress, PortAddresses);
	}
}

void FSLMDomainSystemBase::AppendConnectionsForPortAddresses(const TArray<FSLMPortAddress>& PortAddresses, TArray<FSLMConnection>& Connections) const
{
	const TSet<FSLMPortAddress> PortAddressSet(PortAddresses);
	for (const auto& PortAddress : PortAddresses)
	{
		if (const int32* PortIDPtr = PortAddressToPortID.Find(PortAddress))
		{
			const int32 PortID = *PortIDPtr;
			TArray<int32, TInlineAllocator<16>> NeighborIDs;
			AdjacencyList.MultiFind(PortID, NeighborIDs);
			for (const auto NeighborID : NeighborIDs)
			{
				const FSLMPortAddress NeighborAddress = PortIDToPortAddress[NeighborID];
				if (PortAddressSet.Contains(NeighborAddress) && PortID < NeighborID)
				{
					Connections.Add({PortAddress, NeighborAddress});
				}
			}
		}
	}
}

void FSLMDomainSystemBase::DebugDraw()
{
	for (const auto Pair : AdjacencyList)
	{
		const FVector Start = PortIDToWorldLocation(Pair.Key);
		const FVector End = PortIDToWorldLocation(Pair.Value);
		DrawDebugLine(Manager->GetWorld(), Start, End, DebugColor, false, -1, 0, 2);
	}
}

FString FSLMDomainSystemBase::GetPortDebugString(const FSLMPortAddress& Address)
{
	return FString();
}

void FSLMDomainSystemBase::RunTests()
{
}









TSet<int32> FSLMDomainSystemBase::GetConnectedPortIDs(const TSet<int32>& Roots) const
{
	TSet<int32> Visited;
	//TSet<int32> Connected;
	TArray<int32> Stack;
	Visited.Append(Roots);
	//Connected.Append(Roots);
	Stack.Append(Roots.Array());

	TArray<int32> Neighbors;
	while (!Stack.IsEmpty())
	{
		const int32 ID = Stack.Pop(EAllowShrinking::No);
		AdjacencyList.MultiFind(ID, Neighbors);
		for (auto& Neighbor : Neighbors)
		{
			if (!Visited.Contains(Neighbor))
			{
				Visited.Add(Neighbor);
				Stack.Add(Neighbor);
				//Connected.Add(Neighbor);
			}
		}
		Neighbors.Empty();
	}
	return Visited;
}

bool FSLMDomainSystemBase::ArePortsConnected(const int32 FirstPortID, const int32 SecondPortID)
{
	return AdjacencyList.FindPair(FirstPortID, SecondPortID) || AdjacencyList.FindPair(SecondPortID, FirstPortID);
}

void FSLMDomainSystemBase::CleanUp()
{
	CleanContainers();
	ProcessPortsToRemove();
	ProcessConnectionsPending();
	ProcessConnectionsToAdd();
	ProcessConnectionsToRemove();
	UpdateAdjacencyList();
	GatherDirtyPortIDs();
	DissolveAndRebuildParticles();
	DestroyPorts();
	
	check(PortsToRemove.Num() == 0);
	check(ConnectionsToAdd.Num() == 0);
	check(ConnectionsToRemove.Num() == 0);
}

void FSLMDomainSystemBase::CleanContainers()
{
	ConnectionsToAdd = ConnectionsToAdd.Difference(ConnectionsToRemove);
	ConnectionsPending = ConnectionsPending.Difference(ConnectionsToRemove);
	ConnectionsToAddByID.Empty(16);
	ConnectionsToRemoveByID.Empty(16);
	DirtyPortIDs.Empty(16);
}

void FSLMDomainSystemBase::ProcessPortsToRemove()
{
	TSet<int32> PortIDsToRemove;
	for (const auto& Address : PortsToRemove)
	{
		if (!PortAddressToPortID.Contains(Address))
		{
			check(false);
		}
		PortIDsToRemove.Add(PortAddressToPortID.FindChecked(Address));
	}
	for (const auto& PortID : PortIDsToRemove)
	{
		TArray<int32, TInlineAllocator<16>> AdjacentPortIDs;
		AdjacencyList.MultiFind(PortID, AdjacentPortIDs);
		for (const auto& AdjacentPortID : AdjacentPortIDs)
		{
			ConnectionsToRemoveByID.Add({PortID, AdjacentPortID});
		}
	}
}

void FSLMDomainSystemBase::ProcessConnectionsPending()
{
	ConnectionsToAdd.Append(ConnectionsPending);
	ConnectionsPending.Empty(16);
}

void FSLMDomainSystemBase::ProcessConnectionsToAdd()
{
	for (const FSLMConnection& Connection : ConnectionsToAdd)
	{
		const int32* FirstPortIDPtr = PortAddressToPortID.Find(Connection.First);
		const int32* SecondPortIDPtr = PortAddressToPortID.Find(Connection.Second);
		if (FirstPortIDPtr && SecondPortIDPtr)
		{
			const int32 FirstPortID = *FirstPortIDPtr;
			const int32 SecondPortID = *SecondPortIDPtr;
			ConnectionsToAddByID.Add({FirstPortID, SecondPortID});
		}
		else
		{
			ConnectionsPending.Add(Connection);
		}
	}
	ConnectionsToAdd.Empty(16);
}

void FSLMDomainSystemBase::ProcessConnectionsToRemove()
{
	for (const FSLMConnection& Connection : ConnectionsToRemove)
	{
		const int32* FirstPortIDPtr = PortAddressToPortID.Find(Connection.First);
		const int32* SecondPortIDPtr = PortAddressToPortID.Find(Connection.Second);
		if (FirstPortIDPtr && SecondPortIDPtr)
		{
			const int32 FirstPortID = *FirstPortIDPtr;
			const int32 SecondPortID = *SecondPortIDPtr;
			ConnectionsToRemoveByID.Add({FirstPortID, SecondPortID});
		}
		else
		{
			checkNoEntry();
		}
	}
	ConnectionsToRemove.Empty(16);
}

void FSLMDomainSystemBase::UpdateAdjacencyList()
{
	for (const auto Entry : ConnectionsToAddByID)
	{
		AdjacencyList.Add(Entry.Key, Entry.Value);
		AdjacencyList.Add(Entry.Value, Entry.Key);
	}
	for (const auto Entry : ConnectionsToRemoveByID)
	{
		AdjacencyList.Remove(Entry.Key, Entry.Value);
		AdjacencyList.Remove(Entry.Value, Entry.Key);
	}
}

void FSLMDomainSystemBase::GatherDirtyPortIDs()
{
	for (const auto Entry : ConnectionsToAddByID)
	{
		DirtyPortIDs.Add(Entry.Key);
		DirtyPortIDs.Add(Entry.Value);
	}
	for (const auto Entry : ConnectionsToRemoveByID)
	{
		DirtyPortIDs.Add(Entry.Key);
		DirtyPortIDs.Add(Entry.Value);
	}
	DirtyPortIDs = GetConnectedPortIDs(DirtyPortIDs);
}

FVector FSLMDomainSystemBase::PortIDToWorldLocation(const int32 PortID)
{
	if (PortSpatialContextRuntime.IsValidIndex(PortID))
	{
		return PortSpatialContextRuntime[PortID].GetWorldTransform().GetLocation();
	}
	return FVector();
}

int32 FSLMDomainSystemBase::WorldLocationToPortID(const FSLMSpatialContextRuntime& Filter, const FVector& WorldLocation)
{
	int32 PortID = INDEX_NONE;
	float DistanceSquared = UE_BIG_NUMBER;
	
	for (int32 i = 0; i < PortSpatialContextRuntime.GetMaxIndex(); i++)
	{
		if (PortSpatialContextRuntime.IsValidIndex(i))
		{
			const auto& PortContext = PortSpatialContextRuntime[i];
			
			const bool bActor = Filter.AssociatedActor == PortContext.AssociatedActor || !Filter.AssociatedActor.IsValid();
			const bool bScene = Filter.AssociatedSceneComponent == PortContext.AssociatedSceneComponent || !Filter.AssociatedSceneComponent.IsValid();
			
			if (bActor && bScene)
			{
				const FVector PortLocation = PortContext.GetWorldTransform().GetLocation();
				const float ThisPortDistanceSquared = FVector::DistSquared(WorldLocation, PortLocation);
				if (ThisPortDistanceSquared < DistanceSquared)
				{
					DistanceSquared = ThisPortDistanceSquared;
					PortID = i;
				}
			}
		}
	}
	return PortID;
}
