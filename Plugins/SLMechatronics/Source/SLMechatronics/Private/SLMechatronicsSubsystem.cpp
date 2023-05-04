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
	DECLARE_SCOPE_CYCLE_COUNTER(TEXT("Entire Tick"), STAT_EntireTick, STATGROUP_SLMechatronics)

	//Graph Maintenance
	if (bNeedsCleanup)
	{
		DECLARE_SCOPE_CYCLE_COUNTER(TEXT("Cleanup"), STAT_Cleanup, STATGROUP_SLMechatronics)
		CleanUpGraph();
		bNeedsCleanup = false;
	}
	
	//Sanity checks
	check(PortsToAdd.Num() == 0);
	check(PortsToRemove.Num() == 0);
	check(PortsDirty.Num() == 0);
	
	//Tick steps
	{
		DECLARE_SCOPE_CYCLE_COUNTER(TEXT("PreSimulate"), STAT_PreSimulate, STATGROUP_SLMechatronics)
		for (const auto& Device : DeviceComponents)
		{
			Device->PreSimulate(DeltaTime);
		}
	}
	{
		DECLARE_SCOPE_CYCLE_COUNTER(TEXT("Simulate"), STAT_Simulate, STATGROUP_SLMechatronics)
		for (int32 i = 0; i < StepCount; i++)
		{
			for (const auto& Device : DeviceComponents)
			{
				Device->Simulate(DeltaTime);
			}
		}
	}
	{
		DECLARE_SCOPE_CYCLE_COUNTER(TEXT("PostSimulate"), STAT_PostSimulate, STATGROUP_SLMechatronics)
		for (const auto& Device : DeviceComponents)
		{
			Device->PostSimulate(DeltaTime);
		}
	}
}

int32 USLMechatronicsSubsystem::AddDevice(USLMechatronicsDeviceComponent* DeviceComponent)
{
	return DeviceComponents.Add(DeviceComponent);
}

void USLMechatronicsSubsystem::RemoveDevice(const int32 DeviceIndex)
{
	DeviceComponents.RemoveAt(DeviceIndex);
}

int32 USLMechatronicsSubsystem::AddPort(FSLMPort Port)
{
	const int32 Index = Ports.Add(Port);
	PortIndexToNetworkIndex.Add(-1);
	CreateNetworkForPort(Index);
	return Index;
}

void USLMechatronicsSubsystem::RemovePort(const int32 PortIndex)
{
	PortsToRemove.Add(PortIndex);
	bNeedsCleanup = true;
}

void USLMechatronicsSubsystem::ConnectPorts(const int32 FirstPortIndex, const int32 SecondPortIndex)
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

void USLMechatronicsSubsystem::DisconnectPorts(const int32 FirstPortIndex, const int32 SecondPortIndex)
{
	Adjacencies.Remove(FirstPortIndex, SecondPortIndex);
	Adjacencies.Remove(SecondPortIndex, FirstPortIndex);
	PortsDirty.Add(FirstPortIndex);
	PortsDirty.Add(SecondPortIndex);
	bNeedsCleanup = true;
	UE_LOG(LogTemp, Warning, TEXT("Disonnecting Port %i from Port %i"), FirstPortIndex, SecondPortIndex);
}

bool USLMechatronicsSubsystem::ArePortsConnected(const int32 FirstPortIndex, const int32 SecondPortIndex)
{
	return Adjacencies.FindPair(FirstPortIndex, SecondPortIndex) || Adjacencies.FindPair(SecondPortIndex, FirstPortIndex);
}

FSLMData USLMechatronicsSubsystem::GetNetworkData(const int32 PortIndex)
{
	check(PortIndex >= 0);
	const int32 NetworkIndex = PortIndexToNetworkIndex[PortIndex];
	check(NetworkIndex >= 0);
	return Networks[NetworkIndex];
}

float USLMechatronicsSubsystem::GetNetworkValue(const int32 PortIndex)
{
	check(PortIndex >= 0);
	const int32 NetworkIndex = PortIndexToNetworkIndex[PortIndex];
	check(NetworkIndex >= 0);
	return Networks[NetworkIndex].Value;
}

float USLMechatronicsSubsystem::GetNetworkCapacity(const int32 PortIndex)
{
	check(PortIndex >= 0);
	const int32 NetworkIndex = PortIndexToNetworkIndex[PortIndex];
	check(NetworkIndex >= 0);
	return Networks[NetworkIndex].Capacity;
}

void USLMechatronicsSubsystem::SetNetworkValue(const int32 PortIndex, const float NetworkValue)
{
	check(PortIndex >= 0);
	const int32 NetworkIndex = PortIndexToNetworkIndex[PortIndex];
	check(NetworkIndex >= 0);
	Networks[NetworkIndex].Value = NetworkValue;
}

void USLMechatronicsSubsystem::CleanUpGraph()
{
	UE_LOG(LogTemp, Warning, TEXT("%i/%i ports are dirty at beginning of cleanup"), PortsDirty.Num(), Ports.Num());

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
		PortIndexToNetworkIndex.RemoveAt(PortIndex);
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
		const int32 NetworkIndex = PortIndexToNetworkIndex[PortIndex];
		if (NetworkIndex >= 0)
		{
			const FSLMData Network = Networks[NetworkIndex];
			Ports[PortIndex].DefaultData.Value = Network.Value;
			NetworkIndicesToRemove.AddUnique(NetworkIndex);
			UE_LOG(LogTemp, Warning, TEXT("Port %i has value %f and capacity %f after network dissolve"), PortIndex, Ports[PortIndex].DefaultData.Value, Ports[PortIndex].DefaultData.Capacity);
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
		const int32 NetworkIndex = PortIndexToNetworkIndex[i];
		const FSLMData Network = Networks[NetworkIndex];
		UE_LOG(LogTemp, Warning, TEXT("Port %i has Network %i which has Value %f and Capacity %f"), i, NetworkIndex, Network.Value, Network.Capacity);
	}
	UE_LOG(LogTemp, Warning, TEXT("------------------------------------------------------------------------------------------------------------"));
}

TArray<int32> USLMechatronicsSubsystem::GetConnectedPorts(const TArray<int32>& Roots) const
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
	float SumProduct = 0;
	float SumCapacity = 0;
	for (const auto& PortIndex : PortIndices)
	{
		SumProduct += Ports[PortIndex].DefaultData.Value * Ports[PortIndex].DefaultData.Capacity;
		SumCapacity += Ports[PortIndex].DefaultData.Capacity;
		PortIndexToNetworkIndex[PortIndex] = NetworkIndex;
	}
	Networks[NetworkIndex].Value = SumProduct / SumCapacity;
	Networks[NetworkIndex].Capacity = SumCapacity;
}

void USLMechatronicsSubsystem::CreateNetworkForPort(int32 Port)
{
	PortIndexToNetworkIndex[Port] = Networks.Add(Ports[Port].DefaultData);;
}

