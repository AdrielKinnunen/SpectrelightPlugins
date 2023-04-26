// Fill out your copyright notice in the Description page of Project Settings.


#include "SLMechatronicsSubsystem.h"

#include "SLMechatronicsDeviceComponent.h"


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

void USLMechatronicsSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	PrimarySystemTick.Target = this;
	PrimarySystemTick.bCanEverTick = true;
	PrimarySystemTick.bHighPriority = true;
	PrimarySystemTick.bRunOnAnyThread = false;
	PrimarySystemTick.bTickEvenWhenPaused = false;
	PrimarySystemTick.TickGroup = ETickingGroup::TG_PrePhysics;
	PrimarySystemTick.TickInterval = 0.0;
	PrimarySystemTick.RegisterTickFunction(GetWorld()->PersistentLevel);

	Super::Initialize(Collection);
}


void USLMechatronicsSubsystem::Tick(float DeltaTime)
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
	//Tick steps
	/*
	OnPreSimulate.Broadcast(DeltaTime);
	for (int32 i = 0; i < StepCount; i++)
	{
		OnSimulate.Broadcast(DeltaTime / StepCount);
	}
	OnPostSimulate.Broadcast(DeltaTime);
	*/
	for (const auto& Device : DeviceComponents)
	{
		Device->PreSimulate(DeltaTime);
	}
	for (int32 i = 0; i < StepCount; i++)
	{
		for (const auto& Device : DeviceComponents)
		{
			Device->Simulate(DeltaTime);
		}
	}
	for (const auto& Device : DeviceComponents)
	{
		Device->PostSimulate(DeltaTime);
	}
}

int32 USLMechatronicsSubsystem::AddDevice(USLMechatronicsDeviceComponent* DeviceComponent)
{
	return DeviceComponents.Add(DeviceComponent);
}

void USLMechatronicsSubsystem::RemoveDevice(int32 DeviceIndex)
{
	DeviceComponents.RemoveAt(DeviceIndex);
}

int32 USLMechatronicsSubsystem::AddPort(FSLMPort Port)
{
	const int32 Index = Ports.Add(Port);
	CreateNetworkForPort(Index);
	//Adjacencies.FindOrAdd(Index, Index);
	//PortsToAdd.Add(Index);
	//bNeedsCleanup = true;
	return Index;
}

void USLMechatronicsSubsystem::RemovePort(int32 PortIndex)
{
	PortsToRemove.Add(PortIndex);
	bNeedsCleanup = true;
}

void USLMechatronicsSubsystem::ConnectPorts(int32 FirstPortIndex, int32 SecondPortIndex)
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

void USLMechatronicsSubsystem::DisconnectPorts(int32 FirstPortIndex, int32 SecondPortIndex)
{
	Adjacencies.Remove(FirstPortIndex, SecondPortIndex);
	Adjacencies.Remove(SecondPortIndex, FirstPortIndex);
	PortsDirty.Add(FirstPortIndex);
	PortsDirty.Add(SecondPortIndex);
	bNeedsCleanup = true;
	UE_LOG(LogTemp, Warning, TEXT("Disonnecting Port %i from Port %i"), FirstPortIndex, SecondPortIndex);
}

bool USLMechatronicsSubsystem::ArePortsConnected(int32 FirstPortIndex, int32 SecondPortIndex)
{
	return Adjacencies.FindPair(FirstPortIndex, SecondPortIndex) || Adjacencies.FindPair(SecondPortIndex, FirstPortIndex);
}

FSLMData USLMechatronicsSubsystem::GetNetworkData(int32 PortIndex)
{
	check(PortIndex >= 0);
	const int32 NetworkIndex = Ports[PortIndex].Index;
	check(NetworkIndex >= 0);
	return Networks[NetworkIndex];
}

void USLMechatronicsSubsystem::SetNetworkData(FSLMData NetworkData, int32 PortIndex)
{
	check(PortIndex >= 0);
	const int32 NetworkIndex = Ports[PortIndex].Index;
	check(NetworkIndex >= 0);
	Networks[NetworkIndex] = NetworkData;
}

void USLMechatronicsSubsystem::CleanUpGraph()
{
	UE_LOG(LogTemp, Warning, TEXT("%i/%i ports are dirty at beginning of cleanup"), PortsDirty.Num(), Ports.Num());

	/*
	//Handle PortsToAdd
	for (auto& PortIndex : PortsToAdd)
	{
		Adjacencies.FindOrAdd(PortIndex, PortIndex);
	}
	PortsDirty.Append(PortsToAdd);
	PortsToAdd.Empty();
	UE_LOG(LogTemp, Warning, TEXT("%i/%i ports are dirty afer PortsToAdd"), PortsDirty.Num(), Ports.Num());
	*/

	
	//Handle PortsToRemove
	const TArray<int32> Neighbors = GetConnectedPorts(PortsToRemove);
	PortsDirty.Append(Neighbors);
	for (const auto& PortIndex : PortsToRemove)
	{
		TArray<int32> AdjacentPorts;
		Adjacencies.MultiFind(PortIndex, AdjacentPorts);
		for (const auto& Adjacent : AdjacentPorts)
		{
			Adjacencies.Remove(Adjacent, PortIndex);
		}
		Adjacencies.Remove(PortIndex);
		Ports.RemoveAt(PortIndex);
	}
	PortsToRemove.Empty();
	UE_LOG(LogTemp, Warning, TEXT("%i/%i ports are dirty afer PortsToRemove"), PortsDirty.Num(), Ports.Num());

	//Dirty all dirty ports connected neighbors
	PortsDirty = GetConnectedPorts(PortsDirty);
	UE_LOG(LogTemp, Warning, TEXT("%i/%i ports are dirty after dirtying neighbors"), PortsDirty.Num(), Ports.Num());

	//Dissolve all network values back into port data
	TArray<int32> NetworkIndicesToRemove;
	for (const auto& PortIndex : PortsDirty)
	{
		int32 NetworkIndex = Ports[PortIndex].Index;
		if (NetworkIndex >= 0)
		{
			const FSLMData Network = Networks[NetworkIndex];
			Ports[PortIndex].Data.Value = Ports[PortIndex].Data.Capacity * Network.Value / Network.Capacity;
			NetworkIndicesToRemove.AddUnique(NetworkIndex);
			UE_LOG(LogTemp, Warning, TEXT("Port %i has value %f and capacity %f after network dissolve"), PortIndex, Ports[PortIndex].Data.Value, Ports[PortIndex].Data.Capacity);
		}
	}
	//Remove all dirty port's Networks
	for (const auto& Index : NetworkIndicesToRemove)
	{
		Networks.RemoveAt(Index);
	}
	UE_LOG(LogTemp, Warning, TEXT("%i dirty  %i total  %i networks %i connections"), PortsDirty.Num(), Ports.Num(), Networks.Num(), Adjacencies.Num());

	//Create network for each set of connected dirty ports
	TSet<int32> Visited;
	for (const auto& Port : PortsDirty)
	{
		if (!Visited.Contains(Port) && Ports.IsValidIndex(Port))
		{
			auto Connected = GetConnectedPorts(TArray<int32>{Port});
			Visited.Append(Connected);
			CreateNetworkForPorts(Connected);
		}
	}
	PortsDirty.Empty();
	UE_LOG(LogTemp, Warning, TEXT("%i dirty  %i total  %i networks %i connections"), PortsDirty.Num(), Ports.Num(), Networks.Num(), Adjacencies.Num());
	TestPrintALlPortData();
}

TArray<int32> USLMechatronicsSubsystem::TestGetAllConnectedPortsMulti(TArray<int32> Roots)
{
	return GetConnectedPorts(Roots);
}

void USLMechatronicsSubsystem::TestPrintALlPortData()
{
	for (int32 i = 0; i < Ports.Num(); i++)
	{
		const FSLMPort Port = Ports[i];
		int32 NetworkIndex = Port.Index;
		const FSLMData Network = Networks[NetworkIndex];
		UE_LOG(LogTemp, Warning, TEXT("Port %i has Network %i which has Value %f and Capacity %f"), i, NetworkIndex,
		       Network.Value, Network.Capacity);
	}
	UE_LOG(LogTemp, Warning, TEXT("------------------------------------------------------------------------------------------------------------"));
}

TArray<int32> USLMechatronicsSubsystem::GetConnectedPorts(TArray<int32> Roots) const
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

void USLMechatronicsSubsystem::CreateNetworkForPorts(TArray<int32> PortIndices)
{
	UE_LOG(LogTemp, Warning, TEXT("Creating network for %i ports"), PortIndices.Num());
	const int32 NetworkIndex = Networks.Add(FSLMData());
	float SumValue = 0;
	float SumCapacity = 0;
	for (const auto& PortIndex : PortIndices)
	{
		SumValue += Ports[PortIndex].Data.Value;
		SumCapacity += Ports[PortIndex].Data.Capacity;
		Ports[PortIndex].Index = NetworkIndex;
	}
	Networks[NetworkIndex].Value = SumValue;
	Networks[NetworkIndex].Capacity = SumCapacity;
}

void USLMechatronicsSubsystem::CreateNetworkForPort(int32 Port)
{
	Ports[Port].Index = Networks.Add(Ports[Port].Data);
}


/*
void USLMechatronicsSubsystem::AddConnection(FSLDConnection Connection)
{
	Adjacencies.Add(Connection.FirstPortComponent, Connection.SecondPortComponent);
	Adjacencies.Add(Connection.SecondPortComponent, Connection.FirstPortComponent);
}
void USLMechatronicsSubsystem::RemoveConnection(FSLDConnection Connection)
{
	Adjacencies.Remove(Connection.FirstPortComponent, Connection.SecondPortComponent);
	Adjacencies.Remove(Connection.SecondPortComponent, Connection.FirstPortComponent);
}

void USLMechatronicsSubsystem::DoThing(FPhysScene_Chaos* PhysScene, float DeltaTime)
{
	UE_LOG(LogTemp, Warning, TEXT("SubStep DeltaTime = %f"), DeltaTime);

}
void USLMechatronicsSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{

	FPhysScene* PhysScene = GetWorld()->GetPhysicsScene();
	PhysScene->OnPhysSceneStep.AddUObject(this, &USLMechatronicsSubsystem::DoThing);

	
	Super::OnWorldBeginPlay(InWorld);
}
*/
