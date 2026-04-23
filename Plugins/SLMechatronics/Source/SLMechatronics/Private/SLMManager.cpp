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

void FSLMRepArrayConnections::PostReplicatedAdd(const TArrayView<int32>& AddedIndices, int32 FinalSize) const
{
	for (const auto Index : AddedIndices)
	{
		const FSLMRepItemConnection& Item = Items[Index];
		Manager->AddConnection(Item.Connection);
	}
}

void FSLMRepArrayConnections::PostReplicatedChange(const TArrayView<int32>& ChangedIndices, int32 FinalSize) const
{
	for (const auto Index : ChangedIndices)
	{
		const FSLMRepItemConnection& Item = Items[Index];
		Manager->AddConnection(Item.Connection);
	}
}

void FSLMRepArrayConnections::PreReplicatedRemove(const TArrayView<int32>& RemovedIndices, int32 FinalSize) const
{
	for (const auto Index : RemovedIndices)
	{
		const FSLMRepItemConnection& Item = Items[Index];
		Manager->RemoveConnection(Item.Connection);
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

bool USLMManager::WorldLocationToPortAddress(const TSubclassOf<USLMDomainSubsystemBase> Domain, const FSLMPortMetaData& Filter, const FVector& WorldLocation, FSLMPortAddress& OutAddress)
{
	for (const auto DomainSubsystem : DomainSubsystems)
	{
		if (DomainSubsystem->GetClass() == Domain)
		{
			const int32 PortID = DomainSubsystem->WorldLocationToPortID(Filter, WorldLocation);
			if (PortID != INDEX_NONE)
			{
				OutAddress = DomainSubsystem->PortIDToPortAddress[PortID];
				return true;				
			}
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
			if (const int32* PortIDPtr = DomainSubsystem->PortAddressToPortID.Find(PortAddress))
			{
				OutWorldLocation = DomainSubsystem->PortIDToWorldLocation(*PortIDPtr);
				return true;
			}
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

void USLMManager::AddConnection(const FSLMConnection& Connection)
{
	if (GetWorld()->GetNetMode() < NM_Client && Replicator)
	{
		Replicator->AddConnection(Connection);
	}
	check(Connection.First.DomainClass == Connection.Second.DomainClass);
	for (const auto DomainSubsystem : DomainSubsystems)
	{
		if (DomainSubsystem->GetClass() == Connection.First.DomainClass)
		{
			DomainSubsystem->AddConnection(Connection);
		}
	}
}

void USLMManager::RemoveConnection(const FSLMConnection& Connection)
{
	if (GetWorld()->GetNetMode() < NM_Client && Replicator)
	{
		Replicator->RemoveConnection(Connection);
	}
	check(Connection.First.DomainClass == Connection.Second.DomainClass);
	for (const auto DomainSubsystem : DomainSubsystems)
	{
		if (DomainSubsystem->GetClass() == Connection.First.DomainClass)
		{
			DomainSubsystem->RemoveConnection(Connection);
		}
	}
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

ASLMManagerReplicator::ASLMManagerReplicator()
{
	bReplicates = true;
	bAlwaysRelevant = true;
	SetReplicatingMovement(false);
}

void ASLMManagerReplicator::PostInitializeComponents()
{
	FastArray.Manager = GetWorld()->GetSubsystem<USLMManager>();
	Super::PostInitializeComponents();
}

void ASLMManagerReplicator::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASLMManagerReplicator, FastArray);
}

void ASLMManagerReplicator::AddConnection(const FSLMConnection& Connection)
{
	FSLMRepItemConnection& Item = FastArray.Items.AddDefaulted_GetRef();
	Item.Connection = Connection;
	FastArray.MarkItemDirty(Item);
}

void ASLMManagerReplicator::RemoveConnection(const FSLMConnection& Connection)
{
	for (int32 i = 0; i < FastArray.Items.Num(); ++i)
	{
		if (FastArray.Items[i].Connection == Connection)
		{
			FastArray.Items.RemoveAt(i);
			FastArray.MarkArrayDirty();
			break;
		}
	}
}
