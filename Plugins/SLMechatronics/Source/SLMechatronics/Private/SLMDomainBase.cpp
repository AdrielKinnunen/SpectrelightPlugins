// Copyright Spectrelight Studios, LLC

#include "SLMDomainBase.h"
#include "SLMDeviceBase.h"


void USLMDomainSubsystemBase::RunTests()
{
}

void USLMDomainSubsystemBase::CreateParticleForPorts(const TArray<int32> PortIDs)
{
}

void USLMDomainSubsystemBase::DissolveParticleIntoPort(const int32 ParticleID, const int32 PortID)
{
}

void USLMDomainSubsystemBase::RemovePortAtAddress(const FSLMPortAddress& PortAddress)
{
}

void USLMDomainSubsystemBase::RemoveParticleAtID(const int32 ParticleID)
{
}

void USLMDomainSubsystemBase::AddPortMetaData(FSLMPortMetaData MetaData)
{
	const AActor* Actor = MetaData.AssociatedActor;
	const USceneComponent* SceneComp = nullptr;
	for (UActorComponent* Component : MetaData.AssociatedActor->GetComponents())
	{
		if (Component->GetFName() == MetaData.SceneComponentName)
		{
			SceneComp = Cast<USceneComponent>(Component);
		}
	}

	if (SceneComp)
	{
		MetaData.AssociatedSceneComponent = SceneComp;
	}
	const int32 ID = PortMetaData.Add(MetaData);
	ActorToPortIDs.Add(Actor, ID);
}

void USLMDomainSubsystemBase::AddConnection(const FSLMConnection& Connection)
{
	if (Connection.First.DomainClass != Connection.Second.DomainClass) return;
	if (Connection.First == Connection.Second) return;
	ConnectionsToAdd.Add(Connection);
	bNeedsCleanup = true;
}

void USLMDomainSubsystemBase::RemoveConnection(const FSLMConnection& Connection)
{
	if (Connection.First.DomainClass != Connection.Second.DomainClass) return;
	if (Connection.First == Connection.Second) return;
	ConnectionsToRemove.Add(Connection);
	bNeedsCleanup = true;
}

void USLMDomainSubsystemBase::CheckForCleanUp()
{
    if (bNeedsCleanup)
    {
    	CleanUpGraph();
    	bNeedsCleanup = false;
    }
    //Sanity checks
    check(PortsToRemove.Num() == 0);
    check(ConnectionsToAdd.Num() == 0);
    check(ConnectionsToRemove.Num() == 0);
}

bool USLMDomainSubsystemBase::WorldLocationToPortAddress(const FSLMPortMetaData& Filter, const FVector& WorldLocation, FSLMPortAddress& OutAddress)
{
	const int32 PortID = WorldLocationToPortID(Filter, WorldLocation);
	if (PortID != INDEX_NONE)
	{
		OutAddress = PortIDToPortAddress[PortID];
		return true;				
	}
	return false;
}

bool USLMDomainSubsystemBase::PortAddressToWorldLocation(const FSLMPortAddress& PortAddress, FVector& OutWorldLocation)
{
	if (const int32* PortIDPtr = PortAddressToPortID.Find(PortAddress))
	{
		OutWorldLocation = PortIDToWorldLocation(*PortIDPtr);
		return true;
	}
	return false;
}

void USLMDomainSubsystemBase::RemovePort(const FSLMPortAddress& PortAddress)
{
	PortsToRemove.Add(PortAddress);
	bNeedsCleanup = true;
}

void USLMDomainSubsystemBase::CleanUpGraph()
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
}

void USLMDomainSubsystemBase::CleanContainers()
{
	ConnectionsToAdd = ConnectionsToAdd.Difference(ConnectionsToRemove);
	ConnectionsPending = ConnectionsPending.Difference(ConnectionsToRemove);
	ConnectionsToAddByID.Empty(16);
	ConnectionsToRemoveByID.Empty(16);
	DirtyPortIDs.Empty(16);
}

void USLMDomainSubsystemBase::ProcessPortsToRemove()
{
	TSet<int32> PortIDsToRemove;
	for (const auto& Address : PortsToRemove)
	{
		UE_LOG(LogTemp, Warning, TEXT("ProcessPortsToRemove: NetMode=%d"), (int32)GetWorld()->GetNetMode());
		UE_LOG(LogTemp, Warning, TEXT("ProcessPortsToRemove: Address %s"), *Address.GetDebugString());
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

void USLMDomainSubsystemBase::ProcessConnectionsPending()
{
	ConnectionsToAdd.Append(ConnectionsPending);
	ConnectionsPending.Empty(16);
}

void USLMDomainSubsystemBase::ProcessConnectionsToAdd()
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

void USLMDomainSubsystemBase::ProcessConnectionsToRemove()
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

void USLMDomainSubsystemBase::UpdateAdjacencyList()
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

void USLMDomainSubsystemBase::GatherDirtyPortIDs()
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

void USLMDomainSubsystemBase::DissolveAndRebuildParticles()
{
	//Dissolve all Particle values back into port data
	TSet<int32> ParticleIDsToRemove;
	for (const auto& PortID : DirtyPortIDs)
	{
		const int32 ParticleID = PortIDToParticleID[PortID];
		DissolveParticleIntoPort(ParticleID, PortID);
		ParticleIDsToRemove.Add(ParticleID);
	}

	//Remove all dirty port's Particles
	for (const auto& ParticleID : ParticleIDsToRemove)
	{
		RemoveParticleAtID(ParticleID);
	}

	//Create a Particle for each set of connected dirty ports
	TSet<int32> VisitedIDs;
	for (const auto& PortID : DirtyPortIDs)
	{
		if (!VisitedIDs.Contains(PortID))
		{
			auto ConnectedIDs = GetConnectedPortIDs({PortID});
			VisitedIDs.Append(ConnectedIDs);
			CreateParticleForPorts(ConnectedIDs.Array());
		}
	}
}

void USLMDomainSubsystemBase::DestroyPorts()
{
	for (const auto& Address : PortsToRemove)
	{
		RemovePortAtAddress(Address);
	}
	PortsToRemove.Empty(16);
}

TSet<int32> USLMDomainSubsystemBase::GetConnectedPortIDs(const TSet<int32>& Roots) const
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

bool USLMDomainSubsystemBase::ArePortsConnected(const int32 FirstPortID, const int32 SecondPortID)
{
	return AdjacencyList.FindPair(FirstPortID, SecondPortID) || AdjacencyList.FindPair(SecondPortID, FirstPortID);
}

FVector USLMDomainSubsystemBase::PortIDToWorldLocation(const int32 PortID)
{
	if (PortMetaData.IsValidIndex(PortID))
	{
		return PortMetaDataToWorldTransform(PortMetaData[PortID]).GetLocation();
	}
	return FVector();
}

int32 USLMDomainSubsystemBase::WorldLocationToPortID(const FSLMPortMetaData& Filter, const FVector& WorldLocation)
{
	int32 PortID = INDEX_NONE;
	float DistanceSquared = UE_BIG_NUMBER;
	
	for (int32 i = 0; i < PortMetaData.GetMaxIndex(); i++)
	{
		if (PortMetaData.IsValidIndex(i))
		{
			const FSLMPortMetaData& PortData = PortMetaData[i];
			
			const bool bActor = Filter.AssociatedActor == PortData.AssociatedActor || Filter.AssociatedActor == nullptr;
			const bool bScene = Filter.AssociatedSceneComponent == PortData.AssociatedSceneComponent || Filter.AssociatedSceneComponent == nullptr;
			const bool bDevice = Filter.DeviceName == PortData.DeviceName || Filter.DeviceName.IsNone();
			const bool bName = Filter.PortName == PortData.PortName || Filter.PortName.IsNone();
			
			if (bActor && bScene && bDevice && bName)
			{
				const FVector PortLocation = PortMetaDataToWorldTransform(PortMetaData[i]).GetLocation();
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

FTransform USLMDomainSubsystemBase::PortMetaDataToWorldTransform(const FSLMPortMetaData& MetaData)
{
	FTransform Result;
	if (const USceneComponent* Scene = MetaData.AssociatedSceneComponent)
	{
		Result = Scene->GetSocketTransform(MetaData.SocketName);
		Result.SetLocation(Result.TransformPosition(MetaData.OffsetLocal));
		return Result;
	}
	if (const AActor* Actor = MetaData.AssociatedActor)
	{
		Result = Actor->GetTransform();
		Result.SetLocation(Result.TransformPosition(MetaData.OffsetLocal));
		return Result;
	}
	Result.SetLocation(Result.TransformPosition(MetaData.OffsetLocal));
	return Result;
}



void USLMDomainSubsystemBase::DebugDraw()
{
	for (const auto Pair : AdjacencyList)
	{
		const FVector Start = PortIDToWorldLocation(Pair.Key);
		const FVector End = PortIDToWorldLocation(Pair.Value);
		DrawDebugLine(GetWorld(), Start, End, DebugColor, false, -1, 0, 2);
	}
}

FString USLMDomainSubsystemBase::GetPortDebugString(const FSLMPortAddress& Address)
{
	return FString();
}

bool USLMDomainSubsystemBase::DoesConnectionExist(const FSLMConnection& Connection)
{
	const int32* FirstPortIDPtr = PortAddressToPortID.Find(Connection.First);
	const int32* SecondPortIDPtr = PortAddressToPortID.Find(Connection.Second);
	if (!FirstPortIDPtr || !SecondPortIDPtr)
	{
		return false;
	}
	return ArePortsConnected(*FirstPortIDPtr, *SecondPortIDPtr);
}
