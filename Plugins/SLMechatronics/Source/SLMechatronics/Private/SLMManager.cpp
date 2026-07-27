// Copyright Spectrelight Studios, LLC

#include "SLMManager.h"

#include "SLMDeviceBase.h"
#include "SLMDomainBase.h"
#include "Net/UnrealNetwork.h"


DECLARE_CYCLE_STAT(TEXT("SLM Tick"), STAT_SLMTick, STATGROUP_SLMechatronics);




void FSLMechatronicsSubsystemTickFunction::ExecuteTick(float DeltaTime, ELevelTick TickType, ENamedThreads::Type CurrentThread, const FGraphEventRef& MyCompletionEventGraph)
{
	if (Target && IsValid(Target) && TickType != LEVELTICK_ViewportsOnly)
	{
		Target->Tick(DeltaTime);
	}
}

FString FSLMechatronicsSubsystemTickFunction::DiagnosticMessage()
{
	return TEXT("FSLDSubsystemTickFunction");
}

FName FSLMechatronicsSubsystemTickFunction::DiagnosticContext(bool bDetailed)
{
	return FName(TEXT("FSLDSubsystem"));
}





















void FSLMRepArrayDeviceDescriptors::PostReplicatedAdd(const TArrayView<int32>& AddedIndices, int32 FinalSize) const
{
	for (const auto Index : AddedIndices)
	{
		auto& Item = Items[Index];
		check(Manager);
		Manager->OnRepSetDescriptor(Item.DeviceAddress, Item.DeviceDescriptor);
	}
}

void FSLMRepArrayDeviceDescriptors::PostReplicatedChange(const TArrayView<int32>& ChangedIndices, int32 FinalSize) const
{
	for (const auto Index : ChangedIndices)
	{
		auto& Item = Items[Index];
		check(Manager);
		Manager->OnRepSetDescriptor(Item.DeviceAddress, Item.DeviceDescriptor);
	}
}

void FSLMRepArrayDeviceDescriptors::PreReplicatedRemove(const TArrayView<int32>& RemovedIndices, int32 FinalSize) const
{
	for (const auto Index : RemovedIndices)
	{
		auto& Item = Items[Index];
		check(Manager);
		Manager->OnRepRemoveDescriptor(Item.DeviceAddress);
	}
}

void FSLMRepArrayDeviceState::PostReplicatedAdd(const TArrayView<int32>& AddedIndices, int32 FinalSize) const
{
	for (const auto Index : AddedIndices)
	{
		auto& Item = Items[Index];
		check(Manager);
		Manager->OnRepSetState(Item.DeviceAddress, Item.DeviceState);
	}
}

void FSLMRepArrayDeviceState::PostReplicatedChange(const TArrayView<int32>& ChangedIndices, int32 FinalSize) const
{
	for (const auto Index : ChangedIndices)
	{
		auto& Item = Items[Index];
		check(Manager);
		Manager->OnRepSetState(Item.DeviceAddress, Item.DeviceState);
	}
}

void FSLMRepArrayDeviceState::PreReplicatedRemove(const TArrayView<int32>& RemovedIndices, int32 FinalSize) const
{
	for (const auto Index : RemovedIndices)
	{
		auto& Item = Items[Index];
		check(Manager);
		Manager->OnRepRemoveState(Item.DeviceAddress);
	}
}

void FSLMRepArrayConnections::PostReplicatedAdd(const TArrayView<int32>& AddedIndices, int32 FinalSize) const
{
	for (const auto Index : AddedIndices)
	{
		const FSLMRepItemConnection& Item = Items[Index];
		check(Manager);
		Manager->Local_AddConnection(Item.Connection);
	}
}

void FSLMRepArrayConnections::PostReplicatedChange(const TArrayView<int32>& ChangedIndices, int32 FinalSize) const
{
	for (const auto Index : ChangedIndices)
	{
		const FSLMRepItemConnection& Item = Items[Index];
		check(Manager);
		Manager->Local_AddConnection(Item.Connection);
	}
}

void FSLMRepArrayConnections::PreReplicatedRemove(const TArrayView<int32>& RemovedIndices, int32 FinalSize) const
{
	for (const auto Index : RemovedIndices)
	{
		const FSLMRepItemConnection& Item = Items[Index];
		check(Manager);
		Manager->Local_RemoveConnection(Item.Connection);
	}
}













void USLMManager::Initialize(FSubsystemCollectionBase& Collection)
{
	PrimarySystemTick.Target = this;
	PrimarySystemTick.bCanEverTick = true;
	PrimarySystemTick.bHighPriority = true;
	PrimarySystemTick.bRunOnAnyThread = false;
	PrimarySystemTick.bTickEvenWhenPaused = false;
	PrimarySystemTick.TickGroup = TG_PostPhysics;
	PrimarySystemTick.TickInterval = 0.0;
	PrimarySystemTick.RegisterTickFunction(GetWorld()->PersistentLevel);
	
	for (const auto& Prototype : DomainSystemPrototypes)
	{
		DomainSystems.Add(Prototype->CreateInstance());
	}
	
	for (const auto& Prototype : DeviceSystemPrototypes)
	{
		DeviceSystems.Add(Prototype->CreateInstance());
	}
	
	for (const auto& DomainSystem : DomainSystems)
	{
		DomainSystem->Manager = this;
		DomainSystem->Initialize();
	}
	for (const auto& DeviceSystem : DeviceSystems)
	{
		DeviceSystem->Manager = this;
		DeviceSystem->Initialize();
	}

	Super::Initialize(Collection);
}

void USLMManager::OnWorldBeginPlay(UWorld& InWorld)
{
	bIsServer = GetWorld()->GetNetMode() != NM_Client;
	
	if (bIsServer)
	{
		Replicator = InWorld.SpawnActor<ASLMManagerReplicator>();
	}
	
	for (const auto& DomainSystem : DomainSystems)
	{
		DomainSystem->bIsServer = bIsServer;
	}
	for (const auto& DeviceSystem : DeviceSystems)
	{
		DeviceSystem->bIsServer = bIsServer;
	}
	
	Super::OnWorldBeginPlay(InWorld);
}

void USLMManager::Tick(const float DeltaTime)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(SLMSubsystem::Tick);
	SCOPE_CYCLE_COUNTER(STAT_SLMTick);

	//Graph maintenance
	{
		TRACE_CPUPROFILER_EVENT_SCOPE(SLMSubsystem::Tick::Cleanup);
		for (const auto& DeviceSystem : DeviceSystems)
		{
			DeviceSystem->CheckForCleanUp();
		}
		for (const auto& DomainSystem : DomainSystems)
		{
			DomainSystem->CheckForCleanUp();
		}
	}

	//PreSimulate, runs once per frame, sets up state for calculations
	{
		TRACE_CPUPROFILER_EVENT_SCOPE(SLMSubsystem::Tick::PreSimulate);
		for (const auto& DomainSystem : DomainSystems)
		{
			DomainSystem->PreSimulate(DeltaTime);
		}
		for (const auto& DeviceSystem : DeviceSystems)
		{
			DeviceSystem->PreSimulate(DeltaTime);
		}
	}

	//Simulate, substeps StepCount times per frame, this is where the bulk of simulation happens
	{
		TRACE_CPUPROFILER_EVENT_SCOPE(SLMSubsystem::Tick::Simulate);
		const float SubstepDeltaTime = DeltaTime / StepCount;
		const float SubstepScalar = 1.0 / StepCount;
		for (int32 i = 0; i < StepCount; i++)
		{
			for (const auto& DomainSystem : DomainSystems)
			{
				DomainSystem->Simulate(SubstepDeltaTime, SubstepScalar);
			}
			for (const auto& DeviceSystem : DeviceSystems)
			{
				DeviceSystem->Simulate(SubstepDeltaTime, SubstepScalar);
			}
		}
	}

	//PostSimulate, this is where results are ready for usage outside of SLMechatronics
	{
		TRACE_CPUPROFILER_EVENT_SCOPE(SLMSubsystem::Tick::PostSimulate);
		for (const auto& DomainSystem : DomainSystems)
		{
			DomainSystem->PostSimulate(DeltaTime);
		}
		for (const auto& DeviceSystem : DeviceSystems)
		{
			DeviceSystem->PostSimulate(DeltaTime);
		}
	}

	//Debug
	{
		TRACE_CPUPROFILER_EVENT_SCOPE(SLMSubsystem::Tick::Debug);
		if (bDebugDraw)
		{
			for (const auto& DomainSystem : DomainSystems)
			{
				DomainSystem->DebugDraw();
			}
			for (const auto& DeviceSystem : DeviceSystems)
			{
				DeviceSystem->DebugDraw();
			}			
		}
	}
}

void USLMManager::OpenRemappingContext()
{
	check(bIsServer);
	check(!bRemappingContextOpen)
	check(OldAddressToNewAddress.Num() == 0)
	
	bRemappingContextOpen = true;
}

void USLMManager::CloseRemappingContext()
{
	check(bIsServer);
	check(bRemappingContextOpen)
	
	OldAddressToNewAddress.Empty();
	bRemappingContextOpen = false;
}

FSLMDeviceSnapshot USLMManager::GetDeviceSnapshot(const FSLMDeviceAddress& DeviceAddress)
{
	check(bIsServer);
	
	const FSLMDeviceSystemBase* System = GetDeviceSystemByTag(DeviceAddress.DeviceTag);
	check(System);
	
	return System->GetDeviceSnapshot(DeviceAddress);
}

FSLMDeviceAddress USLMManager::AddDeviceBySnapshot(const FSLMDeviceSnapshot& Snapshot, AActor* AssociatedActor)
{
	check(bIsServer);
	check(bRemappingContextOpen);
	
	FSLMDeviceSystemBase* System = GetDeviceSystemByTag(Snapshot.DeviceAddress.DeviceTag);
	check(System);
	
	return System->AddDeviceBySnapshot(Snapshot, AssociatedActor);
}

TArray<FSLMDeviceSnapshot> USLMManager::GetDeviceSnapshotsByActor(AActor* Actor)
{
	check(bIsServer);
	check(Actor);
	
	TArray<FSLMDeviceSnapshot> Snapshots;
	TArray<USLMDeviceComponentBase*> DeviceComponents;
	Actor->GetComponents<USLMDeviceComponentBase>(DeviceComponents);
	
	for (const USLMDeviceComponentBase* DeviceComponent : DeviceComponents)
	{
		check(DeviceComponent->bDeviceResolved);
		FSLMDeviceSnapshot Snapshot = GetDeviceSnapshot(DeviceComponent->DeviceAddress);
		Snapshot.DeviceComponentName = DeviceComponent->GetFName();
		Snapshots.Add(Snapshot);
	}
	return Snapshots;
}

void USLMManager::ApplyDeviceSnapshotsByActor(const TArray<FSLMDeviceSnapshot>& Snapshots, AActor* Actor)
{
	check(bIsServer);
	check(bRemappingContextOpen);
	check(Actor);
	
	TArray<USLMDeviceComponentBase*> DeviceComponents;
	Actor->GetComponents<USLMDeviceComponentBase>(DeviceComponents);
	
	TMap<FName, USLMDeviceComponentBase*> ComponentsByName;
	for (USLMDeviceComponentBase* Comp : DeviceComponents)
	{
		ComponentsByName.Add(Comp->GetFName(), Comp);
	}

	for (const FSLMDeviceSnapshot& Snapshot : Snapshots)
	{
		USLMDeviceComponentBase** CompPtr = ComponentsByName.Find(Snapshot.DeviceComponentName);
		check(CompPtr);
		USLMDeviceComponentBase* Component = *CompPtr;
		Component->DeviceAddress = AddDeviceBySnapshot(Snapshot, Actor);
		Component->bDeviceResolved = true;
	}
}

void USLMManager::AddConnection(const FSLMConnection& Connection)
{
	check (bIsServer);
	
	if (bRemappingContextOpen)
	{
		FSLMConnection RemappedConnection = Connection;
		const FSLMDeviceAddress FirstDeviceAddress = Connection.First.DeviceAddress;
		if (OldAddressToNewAddress.Contains(FirstDeviceAddress))
		{
			RemappedConnection.First.DeviceAddress = OldAddressToNewAddress.FindChecked(FirstDeviceAddress);
		}
		const FSLMDeviceAddress SecondDeviceAddress = Connection.Second.DeviceAddress;
		if (OldAddressToNewAddress.Contains(SecondDeviceAddress))
		{
			RemappedConnection.Second.DeviceAddress = OldAddressToNewAddress.FindChecked(SecondDeviceAddress);
		}
		Local_AddConnection(RemappedConnection);
		Replicator->AddConnection(RemappedConnection);
	}
	else
	{
		Local_AddConnection(Connection);
		Replicator->AddConnection(Connection);			
	}
}

void USLMManager::RemoveConnection(const FSLMConnection& Connection)
{
	check (bIsServer);
	
	Local_RemoveConnection(Connection);
	Replicator->RemoveConnection(Connection);
}

bool USLMManager::WorldLocationToPortAddress(const FGameplayTag DomainTag, const FSLMSpatialContextRuntime& Filter, const FVector& WorldLocation, FSLMPortAddress& OutAddress)
{
	for (const auto& DomainSystem : DomainSystems)
	{
		if (DomainSystem->SystemTag == DomainTag)
		{
			return DomainSystem->WorldLocationToPortAddress(Filter, WorldLocation, OutAddress);
		}
	}
	return false;
}

bool USLMManager::PortAddressToWorldLocation(const FSLMPortAddress& PortAddress, FVector& OutWorldLocation)
{
	for (const auto& DomainSystem : DomainSystems)
	{
		if (DomainSystem->SystemTag == PortAddress.DomainTag)
		{
			return DomainSystem->PortAddressToWorldLocation(PortAddress, OutWorldLocation);
		}
	}
	return false;
}

bool USLMManager::DoesConnectionExist(const FSLMConnection& Connection)
{
	for (const auto& DomainSystem : DomainSystems)
	{
		if (DomainSystem->SystemTag == Connection.First.DomainTag)
		{
			return DomainSystem->DoesConnectionExist(Connection);
		}
	}
	return false;
}

TArray<FSLMPortAddress> USLMManager::GetPortAddressesForDevices(const TArray<FSLMDeviceAddress>& DeviceAddresses) const
{
	TArray<FSLMPortAddress> Result;
	for (const auto& DomainSystem : DomainSystems)
	{
		DomainSystem->AppendPortAddressesForDevices(DeviceAddresses, Result);
	}
	return Result;
	
}

TArray<FSLMConnection> USLMManager::GetConnectionsWithinPortAddresses(const TArray<FSLMPortAddress>& PortAddresses) const
{
	const TSet<FSLMPortAddress> PortAddressSet(PortAddresses);
	TArray<FSLMConnection> Result;
	for (const auto& DomainSystem : DomainSystems)
	{
		DomainSystem->AppendConnectionsForPortAddresses(PortAddresses, Result);
	}
	
	return Result;
}

TArray<FSLMConnection> USLMManager::GetConnectionsWithinDevices(const TArray<FSLMDeviceAddress>& DeviceAddresses) const
{
	const TArray<FSLMPortAddress>& PortAddresses = GetPortAddressesForDevices(DeviceAddresses);
	TArray<FSLMConnection> Result = GetConnectionsWithinPortAddresses(PortAddresses);
	return Result;
}

FString USLMManager::GetGlobalDebugString(const bool Verbose)
{
	FString Result;
	for (const auto& DomainSystem : DomainSystems)
	{
		Result.Append(DomainSystem->GetDebugString(Verbose));
	}
	for (const auto& DeviceSystem : DeviceSystems)
	{
		Result.Append(DeviceSystem->GetDebugString(Verbose));
	}
	return Result;
}

int32 USLMManager::GetGlobalDebugHash(const bool bVerbose)
{
	int32 Hash = 0;
	for (const auto& DomainSystem : DomainSystems)
	{
		Hash = Hash ^ DomainSystem->GetDebugHash(bVerbose);
	}
	for (const auto& DeviceSystem : DeviceSystems)
	{
		Hash = Hash ^ DeviceSystem->GetDebugHash(bVerbose);
	}
	return Hash;
}

FString USLMManager::GetPortDebugString(const FSLMPortAddress& Address)
{
	for (const auto& DomainSystem : DomainSystems)
	{
		if (DomainSystem->SystemTag == Address.DomainTag)
		{
			return DomainSystem->GetPortDebugString(Address);
		}
	}
	return FString();
}

FString USLMManager::GetDeviceDebugString(const FSLMPortAddress& Address)
{
	return FString();
}

void USLMManager::Local_AddConnection(const FSLMConnection& Connection)
{
	check(Connection.First.DomainTag == Connection.Second.DomainTag);
	for (const auto& DomainSystem : DomainSystems)
	{
		if (DomainSystem->SystemTag == Connection.First.DomainTag)
		{
			DomainSystem->AddConnection(Connection);
		}
	}
}

void USLMManager::Local_RemoveConnection(const FSLMConnection& Connection)
{
	check(Connection.First.DomainTag == Connection.Second.DomainTag);
	for (const auto& DomainSystem : DomainSystems)
	{
		if (DomainSystem->SystemTag == Connection.First.DomainTag)
		{
			DomainSystem->RemoveConnection(Connection);
		}
	}
}

void USLMManager::OnRepSetDescriptor(const FSLMDeviceAddress& DeviceAddress, const FInstancedStruct& Payload)
{
	for (const auto& DeviceSystem : DeviceSystems)
	{
		if (DeviceSystem->SystemTag == DeviceAddress.DeviceTag)
		{
			DeviceSystem->OnRepSetDescriptor(DeviceAddress, Payload);
		}
	}
}

void USLMManager::OnRepRemoveDescriptor(const FSLMDeviceAddress& DeviceAddress) const
{
	for (const auto& DeviceSystem : DeviceSystems)
	{
		if (DeviceSystem->SystemTag == DeviceAddress.DeviceTag)
		{
			DeviceSystem->OnRepRemoveDescriptor(DeviceAddress);
		}
	}
}

void USLMManager::OnRepSetState(const FSLMDeviceAddress& DeviceAddress, const FInstancedStruct& Payload)
{
	for (const auto& DeviceSystem : DeviceSystems)
	{
		if (DeviceSystem->SystemTag == DeviceAddress.DeviceTag)
		{
			DeviceSystem->OnRepSetState(DeviceAddress, Payload);
		}
	}	
}

void USLMManager::OnRepRemoveState(const FSLMDeviceAddress& DeviceAddress) const
{
	for (const auto& DeviceSystem : DeviceSystems)
	{
		if (DeviceSystem->SystemTag == DeviceAddress.DeviceTag)
		{
			DeviceSystem->OnRepRemoveState(DeviceAddress);
		}
	}
}





void USLMManager::RegisterDomainSystem(TUniquePtr<FSLMDomainSystemBase> Prototype)
{
	DomainSystemPrototypes.Add(MoveTemp(Prototype));
}
void USLMManager::RegisterDeviceSystem(TUniquePtr<FSLMDeviceSystemBase> Prototype)
{
	DeviceSystemPrototypes.Add(MoveTemp(Prototype));
}

FSLMDeviceSystemBase* USLMManager::GetDeviceSystemByTag(const FGameplayTag& Tag)
{
	for (const auto& System : DeviceSystems)
	{
		if (System->SystemTag == Tag) return System.Get();
	}
	checkNoEntry();
	return nullptr;
}

TArray<TUniquePtr<FSLMDomainSystemBase>> USLMManager::DomainSystemPrototypes;

TArray<TUniquePtr<FSLMDeviceSystemBase>> USLMManager::DeviceSystemPrototypes;











ASLMManagerReplicator::ASLMManagerReplicator()
{
	bReplicates = true;
	bAlwaysRelevant = true;
	SetReplicatingMovement(false);
}

void ASLMManagerReplicator::PostInitializeComponents()
{
	RepArrayDescriptors.Manager = GetWorld()->GetSubsystem<USLMManager>();
	RepArrayState.Manager = GetWorld()->GetSubsystem<USLMManager>();
	RepArrayConnections.Manager = GetWorld()->GetSubsystem<USLMManager>();
	Super::PostInitializeComponents();
}

void ASLMManagerReplicator::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASLMManagerReplicator, RepArrayDescriptors);
	DOREPLIFETIME(ASLMManagerReplicator, RepArrayState);
	DOREPLIFETIME(ASLMManagerReplicator, RepArrayConnections);
}

void ASLMManagerReplicator::AddConnection(const FSLMConnection& Connection)
{
	if (ConnectionToIndex.Contains(Connection))
	{
		return;
	}
	FSLMRepItemConnection& Item = RepArrayConnections.Items.AddDefaulted_GetRef();
	Item.Connection = Connection;
	RepArrayConnections.MarkItemDirty(Item);
	ConnectionToIndex.Add(Connection, RepArrayConnections.Items.Num() - 1);
}

void ASLMManagerReplicator::RemoveConnection(const FSLMConnection& Connection)
{
	if (const int32* Index = ConnectionToIndex.Find(Connection))
	{
		const int32 RemovedIndex = *Index;
		const int32 LastIndex = RepArrayConnections.Items.Num() - 1;
		if (RemovedIndex != LastIndex)
		{
			ConnectionToIndex[RepArrayConnections.Items[LastIndex].Connection] = RemovedIndex;
		}
		RepArrayConnections.Items.RemoveAtSwap(RemovedIndex);
		RepArrayConnections.MarkArrayDirty();
		ConnectionToIndex.Remove(Connection);
	}
}

void ASLMManagerReplicator::SetDescriptor(const FSLMDeviceAddress& Address, const FInstancedStruct& Payload)
{
	if (const int32* IndexPtr = DeviceAddressToDescriptorIndex.Find(Address))
	{
		auto& Item = RepArrayDescriptors.Items[*IndexPtr];
		Item.DeviceDescriptor = Payload;
		RepArrayDescriptors.MarkItemDirty(Item);
		return;
	}
	FSLMRepItemDeviceDescriptor& Item = RepArrayDescriptors.Items.AddDefaulted_GetRef();
	Item.DeviceAddress = Address;
	Item.DeviceDescriptor = Payload;
	RepArrayDescriptors.MarkItemDirty(Item);
	DeviceAddressToDescriptorIndex.Add(Address, RepArrayDescriptors.Items.Num() - 1);
}

void ASLMManagerReplicator::RemoveDescriptor(const FSLMDeviceAddress& Address)
{
	if (const int32* IndexPtr = DeviceAddressToDescriptorIndex.Find(Address))
	{
		const int32 IndexToRemove = *IndexPtr;
		const int32 LastIndex = RepArrayDescriptors.Items.Num() - 1;
		if (IndexToRemove != LastIndex)
		{
			const auto& LastItem = RepArrayDescriptors.Items[LastIndex];
			DeviceAddressToDescriptorIndex[LastItem.DeviceAddress] = IndexToRemove;
		}
		RepArrayDescriptors.Items.RemoveAtSwap(IndexToRemove);
		RepArrayDescriptors.MarkArrayDirty();
		DeviceAddressToDescriptorIndex.Remove(Address);
	}
}

void ASLMManagerReplicator::SetState(const FSLMDeviceAddress& Address, const FInstancedStruct& Payload)
{
	if (const int32* IndexPtr = DeviceAddressToStateIndex.Find(Address))
	{
		auto& Item = RepArrayState.Items[*IndexPtr];
		Item.DeviceState = Payload;
		RepArrayState.MarkItemDirty(Item);
		return;
	}
	FSLMRepItemDeviceState& Item = RepArrayState.Items.AddDefaulted_GetRef();
	Item.DeviceAddress = Address;
	Item.DeviceState = Payload;
	RepArrayState.MarkItemDirty(Item);
	DeviceAddressToStateIndex.Add(Address, RepArrayState.Items.Num() - 1);
}

void ASLMManagerReplicator::RemoveState(const FSLMDeviceAddress& Address)
{
	if (const int32* IndexPtr = DeviceAddressToStateIndex.Find(Address))
	{
		const int32 IndexToRemove = *IndexPtr;
		const int32 LastIndex = RepArrayState.Items.Num() - 1;
		if (IndexToRemove != LastIndex)
		{
			const auto& LastItem = RepArrayState.Items[LastIndex];
			DeviceAddressToStateIndex[LastItem.DeviceAddress] = IndexToRemove;
		}
		RepArrayState.Items.RemoveAtSwap(IndexToRemove);
		RepArrayState.MarkArrayDirty();
		DeviceAddressToStateIndex.Remove(Address);
	}
}

bool USLMBlueprintFunctionLibrary::IsValidAddress(const FSLMPortAddress& Address)
{
	return Address.IsValid();
}

bool USLMBlueprintFunctionLibrary::IsValidConnection(const FSLMConnection& Connection)
{
	return Connection.IsValid();
}

bool USLMBlueprintFunctionLibrary::DoesConnectionExist(const UObject* WorldContextObject, const FSLMConnection Connection)
{
	check(WorldContextObject);
	return WorldContextObject->GetWorld()->GetSubsystem<USLMManager>()->DoesConnectionExist(Connection);
}

bool USLMBlueprintFunctionLibrary::WorldLocationToPortAddress(const UObject* WorldContextObject, const FGameplayTag DomainTag, const FSLMSpatialContextRuntime& Filter, const FVector& WorldLocation, FSLMPortAddress& OutAddress)
{
	check(WorldContextObject);
	return WorldContextObject->GetWorld()->GetSubsystem<USLMManager>()->WorldLocationToPortAddress(DomainTag, Filter, WorldLocation, OutAddress);
}

bool USLMBlueprintFunctionLibrary::PortAddressToWorldLocation(const UObject* WorldContextObject, const FSLMPortAddress& PortAddress, FVector& OutWorldLocation)
{
	check(WorldContextObject);
	return WorldContextObject->GetWorld()->GetSubsystem<USLMManager>()->PortAddressToWorldLocation(PortAddress, OutWorldLocation);
}

void USLMBlueprintFunctionLibrary::AddConnection(const UObject* WorldContextObject, const FSLMConnection Connection)
{
	check(WorldContextObject);
	WorldContextObject->GetWorld()->GetSubsystem<USLMManager>()->AddConnection(Connection);
}

void USLMBlueprintFunctionLibrary::RemoveConnection(const UObject* WorldContextObject, const FSLMConnection Connection)
{
	check(WorldContextObject);
	WorldContextObject->GetWorld()->GetSubsystem<USLMManager>()->RemoveConnection(Connection);
}

TArray<FSLMConnection> USLMBlueprintFunctionLibrary::GetConnectionsWithinDevices(const UObject* WorldContextObject, const TArray<FSLMDeviceAddress>& DeviceAddresses)
{
	check(WorldContextObject);
	return WorldContextObject->GetWorld()->GetSubsystem<USLMManager>()->GetConnectionsWithinDevices(DeviceAddresses);
}

int32 USLMBlueprintFunctionLibrary::GetGlobalDebugHash(const UObject* WorldContextObject, const bool bVerbose)
{
	check(WorldContextObject);
	return WorldContextObject->GetWorld()->GetSubsystem<USLMManager>()->GetGlobalDebugHash(bVerbose);
}

FString USLMBlueprintFunctionLibrary::GetGlobalDebugString(const UObject* WorldContextObject, const bool bVerbose)
{
	check(WorldContextObject);
	return WorldContextObject->GetWorld()->GetSubsystem<USLMManager>()->GetGlobalDebugString(bVerbose);
}

FString USLMBlueprintFunctionLibrary::GetPortDebugString(const UObject* WorldContextObject, const FSLMPortAddress& Address)
{
	check(WorldContextObject);
	return WorldContextObject->GetWorld()->GetSubsystem<USLMManager>()->GetPortDebugString(Address);
}

FString USLMBlueprintFunctionLibrary::GetDeviceDebugString(const UObject* WorldContextObject, const FSLMPortAddress& Address)
{
	check(WorldContextObject);
	return WorldContextObject->GetWorld()->GetSubsystem<USLMManager>()->GetDeviceDebugString(Address);
}

FString USLMBlueprintFunctionLibrary::DiffDebugStrings(const FString Server, const FString Client)
{
	TArray<FString> ServerLines;
	TArray<FString> ClientLines;
	Server.ParseIntoArrayLines(ServerLines);
	Client.ParseIntoArrayLines(ClientLines);
	
	FString Result;
	Result += TEXT("\nIn Server but not Client: \n");
	for (const FString& Line : ServerLines)
	{
		if (!ClientLines.Contains(Line))
		{
			Result += Line + TEXT("\n");
		}
	}
	Result += TEXT("In Client but not Server: \n");
	for (const FString& Line : ClientLines)
	{
		if (!ServerLines.Contains(Line))
		{
			Result += Line + TEXT("\n");
		}
	}
	return Result;
}

void USLMBlueprintFunctionLibrary::OpenRemappingContext(const UObject* WorldContextObject)
{
	check(WorldContextObject);
	return WorldContextObject->GetWorld()->GetSubsystem<USLMManager>()->OpenRemappingContext();
}

void USLMBlueprintFunctionLibrary::CloseRemappingContext(const UObject* WorldContextObject)
{
	check(WorldContextObject);
	return WorldContextObject->GetWorld()->GetSubsystem<USLMManager>()->CloseRemappingContext();
}

TArray<FSLMDeviceSnapshot> USLMBlueprintFunctionLibrary::GetDeviceSnapshotsByActor(const UObject* WorldContextObject, AActor* Actor)
{
	check(WorldContextObject);
	return WorldContextObject->GetWorld()->GetSubsystem<USLMManager>()->GetDeviceSnapshotsByActor(Actor);
}

void USLMBlueprintFunctionLibrary::ApplyDeviceSnapshotsByActor(const UObject* WorldContextObject, AActor* Actor, const TArray<FSLMDeviceSnapshot>& Snapshots)
{
	check(WorldContextObject);
	WorldContextObject->GetWorld()->GetSubsystem<USLMManager>()->ApplyDeviceSnapshotsByActor(Snapshots, Actor);
}
