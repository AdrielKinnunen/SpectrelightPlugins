// Fill out your copyright notice in the Description page of Project Settings.


#include "SLMechatronicsDeviceComponent.h"
#include "SLMechatronicsSubsystem.h"


// Sets default values for this component's properties
USLMechatronicsDeviceComponent::USLMechatronicsDeviceComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void USLMechatronicsDeviceComponent::PreSimulate(float DeltaTime)
{
	OnPreSimulate.Broadcast(DeltaTime);
}

void USLMechatronicsDeviceComponent::Simulate(float DeltaTime)
{
	OnSimulate.Broadcast(DeltaTime);
}

void USLMechatronicsDeviceComponent::PostSimulate(float DeltaTime)
{
	OnPostSimulate.Broadcast(DeltaTime);
}

void USLMechatronicsDeviceComponent::GetLocationData(int32 PortIndex, USceneComponent*& OutSceneComponent, FName& OutSocket, FVector& OutOffset)
{
	const AActor* Owner = GetOwner();
	check(Ports.IsValidIndex(PortIndex));
	const FSLMPort Port = Ports[PortIndex];
	for(UActorComponent* Component : Owner->GetComponents())
	{
		if(Component->GetFName() == Port.ComponentName)
		{
			OutSceneComponent = Cast<USceneComponent>(Component);
		}
	}
	if (!OutSceneComponent)
	{
		OutSceneComponent = Owner->GetRootComponent();
	}
	OutSocket = Port.SocketName;
	OutOffset = Port.Offset;
}

FSLMData USLMechatronicsDeviceComponent::GetNetworkData(int32 PortIndex)
{
	return Subsystem->GetNetworkData(Ports[PortIndex].Index);
}

void USLMechatronicsDeviceComponent::SetNetworkData(FSLMData Data, int32 PortIndex)
{
	Subsystem->SetNetworkData(Data, Ports[PortIndex].Index);
}




// Called when the game starts
void USLMechatronicsDeviceComponent::BeginPlay()
{
	Super::BeginPlay();
	Subsystem = GetWorld()->GetSubsystem<USLMechatronicsSubsystem>();
	DeviceIndex = Subsystem->AddDevice(this);
	for (auto& Port : Ports)
	{
		Port.Index = Subsystem->AddPort(Port);
	}
}


void USLMechatronicsDeviceComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	for (const auto& Port : Ports)
	{
		Subsystem->RemovePort(Port.Index);
	}
	Super::EndPlay(EndPlayReason);
}

