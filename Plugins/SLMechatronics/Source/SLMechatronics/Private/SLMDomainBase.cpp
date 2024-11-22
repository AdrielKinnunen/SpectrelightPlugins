// Copyright Spectrelight Studios, LLC

#include "SLMDomainBase.h"

UE_DEFINE_GAMEPLAY_TAG(TAG_SPECTRELIGHTDYNAMICS_DOMAIN, "SpectrelightDynamics.Domain")


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

void USLMDomainSubsystemBase::Simulate(const float DeltaTime, const float SubstepScalar)
{
}

void USLMDomainSubsystemBase::PostSimulate(const float DeltaTime)
{
}

FString USLMDomainSubsystemBase::GetDebugString(int32 PortIndex)
{
	return "Hello World!";
}

void USLMDomainSubsystemBase::RunTests()
{
}

void USLMDomainSubsystemBase::DebugDraw()
{
	for (const auto Pair : Adjacencies)
	{
		const FVector Start = PortIndexToWorldLocation(Pair.Key);
		const FVector End = PortIndexToWorldLocation(Pair.Value);
		DrawDebugLine(GetWorld(), Start, End, DebugColor, false, -1, 0, 2);
	}
}
void USLMDomainSubsystemBase::DebugPrint()
{
	const auto Max = PortsMetaData.GetMaxIndex();
	for (int32 i = 0; i < Max; i++)
	{
		if (PortsMetaData.IsValidIndex(i))
		{
			const auto MetaData = PortsMetaData[i];
			const auto Transform = PortMetaDataToWorldTransform(MetaData);
			const auto Location = Transform.GetLocation();
			const FRotator Rotation = Transform.Rotator();
			DrawDebugCoordinateSystem(GetWorld(), Location, Rotation, 200);
			DrawDebugString(GetWorld(), Location, GetDebugString(i), nullptr, FColor::White, 0.0, true, 1.5);
		}
	}
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 0.0f, FColor::Red, TEXT("-----------------------------------------------"), false);
	const auto ObjectName = this->GetName();
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 0.0f, FColor::Red, ObjectName, false);
	TArray<int32> AdjacencyKeys;
	Adjacencies.GetKeys(AdjacencyKeys);
	for (const auto Key : AdjacencyKeys)
	{
		TArray<int32> Values;
		Adjacencies.MultiFind(Key, Values);
		for (const auto Value : Values)
		{
			GEngine->AddOnScreenDebugMessage(INDEX_NONE, 0.0f, FColor::Red, FString::Printf(TEXT("Port %i is adjacent to port %i"), Key, Value), false);
		}
	}
	const auto Max2 = PortIndexToNetworkIndex.GetMaxIndex();
	for (int32 i = 0; i < Max2; i++)
	{
		if (PortIndexToNetworkIndex.IsValidIndex(i))
		{
			const auto Network = PortIndexToNetworkIndex[i];
			GEngine->AddOnScreenDebugMessage(INDEX_NONE, 0.0f, FColor::Red, FString::Printf(TEXT(
												 "Port %i maps to network %i"
											 ), i, Network), false);
		}
	}
}

FVector USLMDomainSubsystemBase::PortIndexToWorldLocation(const int32 PortIndex)
{
    if (PortsMetaData.IsValidIndex(PortIndex))
    {
        return PortMetaDataToWorldTransform(PortsMetaData[PortIndex]).GetLocation();
    }
    return FVector();
}

int32 USLMDomainSubsystemBase::GetPortIndex(const FSLMPortMetaData Filter, const FVector WorldLocation)
{
	int32 PortIndex = -1;
	float DistanceSquared = UE_BIG_NUMBER;
	const auto Max = PortsMetaData.GetMaxIndex();
	
	for (int32 i = 0; i < Max; i++)
	{
		if (PortsMetaData.IsValidIndex(i))
		{
			const FSLMPortMetaData PortData = PortsMetaData[i];
			
			const bool bActor = Filter.AssociatedActor == PortData.AssociatedActor || Filter.AssociatedActor == nullptr;
			const bool bScene = Filter.AssociatedSceneComponent == PortData.AssociatedSceneComponent || Filter.AssociatedSceneComponent == nullptr;
			const bool bDevice = Filter.DeviceComponent == PortData.DeviceComponent || Filter.DeviceComponent == nullptr;
			const bool bName = Filter.PortName == PortData.PortName || Filter.PortName.IsNone();
			
			if (bActor && bScene && bDevice && bName)
			{
				const FVector PortLocation = PortMetaDataToWorldTransform(PortsMetaData[i]).GetLocation();
				const float ThisPortDistanceSquared = FVector::DistSquared(WorldLocation, PortLocation);
				if (ThisPortDistanceSquared < DistanceSquared)
				{
					DistanceSquared = ThisPortDistanceSquared;
					PortIndex = i;
				}
			}
		}
	}
	return PortIndex;
}

FTransform USLMDomainSubsystemBase::PortMetaDataToWorldTransform(const FSLMPortMetaData MetaData)
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

TArray<FSLMConnectionByMetaData> USLMDomainSubsystemBase::GetAllConnections()
{
	TArray<FSLMConnectionByMetaData> Out;
	for (const auto Pair : Adjacencies)
	{
		FSLMConnectionByMetaData Entry;
		Entry.DomainTag = DomainTag;
		Entry.FirstMetaData = PortsMetaData[Pair.Key];
		Entry.SecondMetaData = PortsMetaData[Pair.Value];
		Out.Add(Entry);
	}
	return Out;
}

void USLMDomainSubsystemBase::ConnectPorts(const int32 FirstPortIndex, const int32 SecondPortIndex)
{
    if (FirstPortIndex != SecondPortIndex)
    {
        ConnectionsToAdd.Add(FSLMConnection(FirstPortIndex, SecondPortIndex));
        bNeedsCleanup = true;
    }
}

void USLMDomainSubsystemBase::DisconnectPorts(const int32 FirstPortIndex, const int32 SecondPortIndex)
{
    if (FirstPortIndex != SecondPortIndex)
    {
        ConnectionsToRemove.Add(FSLMConnection(FirstPortIndex, SecondPortIndex));
        bNeedsCleanup = true;
    }
}

bool USLMDomainSubsystemBase::ArePortsConnected(const int32 FirstPortIndex, const int32 SecondPortIndex)
{
    return Adjacencies.FindPair(FirstPortIndex, SecondPortIndex) || Adjacencies.FindPair(SecondPortIndex, FirstPortIndex);
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
    const int32 Index = PortsMetaData.Add(MetaData);
    ActorToPorts.Add(Actor, Index);
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
        Adjacencies.AddUnique(FirstIndex, SecondIndex);
        Adjacencies.AddUnique(SecondIndex, FirstIndex);
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
        const auto Actor = PortsMetaData[PortIndex].AssociatedActor;
        ActorToPorts.Remove(Actor, PortIndex);
        PortsMetaData.RemoveAt(PortIndex);
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
        //const int32 Index = Stack.Pop(false);
    	const int32 Index = Stack.Pop(EAllowShrinking::No);
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
