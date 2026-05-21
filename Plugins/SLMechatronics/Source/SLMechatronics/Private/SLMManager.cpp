// Copyright Spectrelight Studios, LLC

#include "SLMManager.h"

#include "SLMDeviceBase.h"
#include "SLMDomainBase.h"
#include "Net/UnrealNetwork.h"


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
		Manager->Client_AddOrChangeDescriptor(Item.DeviceAddress, Item.DeviceDescriptor);
	}
}

void FSLMRepArrayDeviceDescriptors::PostReplicatedChange(const TArrayView<int32>& ChangedIndices, int32 FinalSize) const
{
	for (const auto Index : ChangedIndices)
	{
		auto& Item = Items[Index];
		check(Manager);
		Manager->Client_AddOrChangeDescriptor(Item.DeviceAddress, Item.DeviceDescriptor);
	}
}

void FSLMRepArrayDeviceDescriptors::PreReplicatedRemove(const TArrayView<int32>& RemovedIndices, int32 FinalSize) const
{
	for (const auto Index : RemovedIndices)
	{
		auto& Item = Items[Index];
		check(Manager);
		Manager->Client_RemoveDescriptor(Item.DeviceAddress);
	}
}

void FSLMRepArrayDeviceState::PostReplicatedAdd(const TArrayView<int32>& AddedIndices, int32 FinalSize) const
{
	for (const auto Index : AddedIndices)
	{
		auto& Item = Items[Index];
		check(Manager);
		Manager->Client_AddOrChangeState(Item.DeviceAddress, Item.DeviceState);
	}
}

void FSLMRepArrayDeviceState::PostReplicatedChange(const TArrayView<int32>& ChangedIndices, int32 FinalSize) const
{
	for (const auto Index : ChangedIndices)
	{
		auto& Item = Items[Index];
		check(Manager);
		Manager->Client_AddOrChangeState(Item.DeviceAddress, Item.DeviceState);
	}
}

void FSLMRepArrayDeviceState::PreReplicatedRemove(const TArrayView<int32>& RemovedIndices, int32 FinalSize) const
{
	for (const auto Index : RemovedIndices)
	{
		auto& Item = Items[Index];
		check(Manager);
		Manager->Client_RemoveState(Item.DeviceAddress);
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

	Super::Initialize(Collection);
}

void USLMManager::OnWorldBeginPlay(UWorld& InWorld)
{
	DeviceSubsystems = GetWorld()->GetSubsystemArrayCopy<USLMDeviceSubsystemBase>();
	DomainSubsystems = GetWorld()->GetSubsystemArrayCopy<USLMDomainSubsystemBase>();
	UE_LOG(LogTemp, Warning, TEXT("There are %i Device Subsystems and %i Domain Subsystems"), DeviceSubsystems.Num(), DomainSubsystems.Num());
	for (const auto DomainSubsystem : DomainSubsystems)
	{
		DomainSubsystem->RunTests();
	}
	if (GetWorld()->GetNetMode() != NM_Client)
	{
		Replicator = InWorld.SpawnActor<ASLMManagerReplicator>();
	}
	Super::OnWorldBeginPlay(InWorld);
}

void USLMManager::Tick(const float DeltaTime)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(SLMSubsystem::Tick);

	//Graph maintenance
	{
		TRACE_CPUPROFILER_EVENT_SCOPE(SLMSubsystem::Tick::Cleanup); 
		for (const auto DomainSubsystem : DomainSubsystems)
		{
			DomainSubsystem->CheckForCleanUp();
		}
	}

	//PreSimulate, runs once per frame, sets up state for calculations
	{
		TRACE_CPUPROFILER_EVENT_SCOPE(SLMSubsystem::Tick::PreSimulate);
		for (const auto DomainSubsystem : DomainSubsystems)
		{
			DomainSubsystem->PreSimulate(DeltaTime);
		}
		for (const auto DeviceSubsystem : DeviceSubsystems)
		{
			DeviceSubsystem->PreSimulate(DeltaTime);
		}
	}

	//Simulate, substeps StepCount times per frame, this is where the bulk of simulation happens
	{
		TRACE_CPUPROFILER_EVENT_SCOPE(SLMSubsystem::Tick::Simulate);
		const float SubstepDeltaTime = DeltaTime / StepCount;
		const float SubstepScalar = 1.0 / StepCount;
		for (int32 i = 0; i < StepCount; i++)
		{
			for (const auto DomainSubsystem : DomainSubsystems)
			{
				DomainSubsystem->Simulate(SubstepDeltaTime, SubstepScalar);
			}
			for (const auto DeviceSubsystem : DeviceSubsystems)
			{
				DeviceSubsystem->Simulate(SubstepDeltaTime, SubstepScalar);
			}
		}
	}

	//PostSimulate, this is where results are ready for usage outside of SLMechatronics
	{
		TRACE_CPUPROFILER_EVENT_SCOPE(SLMSubsystem::Tick::PostSimulate);
		for (const auto DomainSubsystem : DomainSubsystems)
		{
			DomainSubsystem->PostSimulate(DeltaTime);
		}
		for (const auto DeviceSubsystem : DeviceSubsystems)
		{
			DeviceSubsystem->PostSimulate(DeltaTime);
		}
	}

	//Debug
	{
		TRACE_CPUPROFILER_EVENT_SCOPE(SLMSubsystem::Tick::Debug);
		if (bDebugDraw)
		{
			for (const auto DomainSubsystem : DomainSubsystems)
			{
				DomainSubsystem->DebugDraw();
			}
			for (const auto DeviceSubsystem : DeviceSubsystems)
			{
				DeviceSubsystem->DebugDraw();
			}			
		}
	}
}

void USLMManager::OpenRemappingContext()
{
	check(!bRemappingContextOpen)
	check(RemappingContext.Num() == 0)
	bRemappingContextOpen = true;
}

void USLMManager::CloseRemappingContext()
{
	RemappingContext.Empty();
	bRemappingContextOpen = false;
}

void USLMManager::AddConnection(const FSLMConnection& Connection)
{
	if (GetWorld()->GetNetMode() < NM_Client)
	{
		if (bRemappingContextOpen)
		{
			FSLMConnection RemappedConnection = Connection;
			const FSLMDeviceAddress FirstDeviceAddress = Connection.First.DeviceAddress;
			if (RemappingContext.Contains(FirstDeviceAddress))
			{
				RemappedConnection.First.DeviceAddress = RemappingContext.FindChecked(FirstDeviceAddress);
			}
			const FSLMDeviceAddress SecondDeviceAddress = Connection.Second.DeviceAddress;
			if (RemappingContext.Contains(SecondDeviceAddress))
			{
				RemappedConnection.Second.DeviceAddress = RemappingContext.FindChecked(SecondDeviceAddress);
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
}

void USLMManager::RemoveConnection(const FSLMConnection& Connection)
{
	if (GetWorld()->GetNetMode() < NM_Client)
	{
		Local_RemoveConnection(Connection);
		Replicator->RemoveConnection(Connection);
	}
}

bool USLMManager::WorldLocationToPortAddress(const TSubclassOf<USLMDomainSubsystemBase> DomainClass, const FSLMPortMetaData& Filter, const FVector& WorldLocation, FSLMPortAddress& OutAddress)
{
	for (const auto DomainSubsystem : DomainSubsystems)
	{
		if (DomainSubsystem->GetClass() == DomainClass)
		{
			return DomainSubsystem->WorldLocationToPortAddress(Filter, WorldLocation, OutAddress);
		}
	}
	return false;
}

bool USLMManager::PortAddressToWorldLocation(const FSLMPortAddress& PortAddress, FVector& OutWorldLocation)
{
	for (const auto DomainSubsystem : DomainSubsystems)
	{
		if (DomainSubsystem->GetClass() == PortAddress.DomainClass)
		{
			return DomainSubsystem->PortAddressToWorldLocation(PortAddress, OutWorldLocation);
		}
	}
	return false;
}

bool USLMManager::DoesConnectionExist(const FSLMConnection& Connection)
{
	for (const auto DomainSubsystem : DomainSubsystems)
	{
		if (DomainSubsystem->GetClass() == Connection.First.DomainClass)
		{
			return DomainSubsystem->DoesConnectionExist(Connection);
		}
	}
	return false;
}

FString USLMManager::GetGlobalDebugString(const bool Verbose)
{
	FString Result;
	for (const auto DomainSubsystem : DomainSubsystems)
	{
		Result.Append(DomainSubsystem->GetDebugString(Verbose));
	}
	for (const auto DeviceSubsystem : DeviceSubsystems)
	{
		Result.Append(DeviceSubsystem->GetDebugString(Verbose));
	}
	return Result;
}

int32 USLMManager::GetGlobalDebugHash()
{
	int32 Hash = 0;
	for (const auto DomainSubsystem : DomainSubsystems)
	{
		Hash = Hash ^ DomainSubsystem->GetDebugHash();
	}
	for (const auto DeviceSubsystem : DeviceSubsystems)
	{
		Hash = Hash ^ DeviceSubsystem->GetDebugHash();
	}
	return Hash;
}

FString USLMManager::GetPortDebugString(const FSLMPortAddress& Address)
{
	for (const auto DomainSubsystem : DomainSubsystems)
	{
		if (DomainSubsystem->GetClass() == Address.DomainClass)
		{
			return DomainSubsystem->GetPortDebugString(Address);
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
	check(Connection.First.DomainClass == Connection.Second.DomainClass);
	for (const auto DomainSubsystem : DomainSubsystems)
	{
		if (DomainSubsystem->GetClass() == Connection.First.DomainClass)
		{
			DomainSubsystem->AddConnection(Connection);
		}
	}
}

void USLMManager::Local_RemoveConnection(const FSLMConnection& Connection)
{
	check(Connection.First.DomainClass == Connection.Second.DomainClass);
	for (const auto DomainSubsystem : DomainSubsystems)
	{
		if (DomainSubsystem->GetClass() == Connection.First.DomainClass)
		{
			DomainSubsystem->RemoveConnection(Connection);
		}
	}
}

void USLMManager::Client_AddOrChangeDescriptor(const FSLMDeviceAddress& DeviceAddress, const FInstancedStruct& Payload)
{
	for (const auto DeviceSubsystem : DeviceSubsystems)
	{
		if (DeviceSubsystem->GetClass() == DeviceAddress.DeviceClass)
		{
			DeviceSubsystem->Client_AddOrChangeDescriptor(DeviceAddress, Payload);
		}
	}
}

void USLMManager::Client_RemoveDescriptor(const FSLMDeviceAddress& DeviceAddress) const
{
	for (const auto DeviceSubsystem : DeviceSubsystems)
	{
		if (DeviceSubsystem->GetClass() == DeviceAddress.DeviceClass)
		{
			DeviceSubsystem->Client_RemoveDescriptor(DeviceAddress);
		}
	}
}

void USLMManager::Client_AddOrChangeState(const FSLMDeviceAddress& DeviceAddress, const FInstancedStruct& Payload)
{
	for (const auto DeviceSubsystem : DeviceSubsystems)
	{
		if (DeviceSubsystem->GetClass() == DeviceAddress.DeviceClass)
		{
			DeviceSubsystem->Client_AddOrChangeState(DeviceAddress, Payload);
		}
	}	
}

void USLMManager::Client_RemoveState(const FSLMDeviceAddress& DeviceAddress) const
{
	for (const auto DeviceSubsystem : DeviceSubsystems)
	{
		if (DeviceSubsystem->GetClass() == DeviceAddress.DeviceClass)
		{
			DeviceSubsystem->Client_RemoveState(DeviceAddress);
		}
	}
}






















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

void ASLMManagerReplicator::AddOrChangeDescriptor(const FSLMDeviceAddress& Address, const FInstancedStruct& Payload)
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

void ASLMManagerReplicator::AddOrChangeState(const FSLMDeviceAddress& Address, const FInstancedStruct& Payload)
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

bool USLMBlueprintFunctionLibrary::WorldLocationToPortAddress(const UObject* WorldContextObject, const TSubclassOf<USLMDomainSubsystemBase> Domain, const FSLMPortMetaData& Filter, const FVector& WorldLocation, FSLMPortAddress& OutAddress)
{
	check(WorldContextObject);
	return WorldContextObject->GetWorld()->GetSubsystem<USLMManager>()->WorldLocationToPortAddress(Domain, Filter, WorldLocation, OutAddress);
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

int32 USLMBlueprintFunctionLibrary::GetGlobalDebugHash(const UObject* WorldContextObject)
{
	check(WorldContextObject);
	return WorldContextObject->GetWorld()->GetSubsystem<USLMManager>()->GetGlobalDebugHash();
}

FString USLMBlueprintFunctionLibrary::GetGlobalDebugString(const UObject* WorldContextObject, const bool Verbose)
{
	check(WorldContextObject);
	return WorldContextObject->GetWorld()->GetSubsystem<USLMManager>()->GetGlobalDebugString(Verbose);
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
	//return WorldContextObject->GetWorld()->GetSubsystem<USLMManager>()->WorldLocationToPortAddress(Domain, Filter, WorldLocation, OutAddress);
}
