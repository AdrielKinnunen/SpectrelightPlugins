// Copyright Spectrelight Studios, LLC

#include "SLMDeviceBase.h"

USLMDeviceComponentBase::USLMDeviceComponentBase()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void USLMDeviceComponentBase::BeginPlay()
{
	Super::BeginPlay();
}

void USLMDeviceComponentBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void USLMDeviceSubsystemBase::PreSimulate(const float DeltaTime)
{
}

void USLMDeviceSubsystemBase::Simulate(const float DeltaTime)
{
}

void USLMDeviceSubsystemBase::PostSimulate(const float DeltaTime)
{
}

/*
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

void USLMechatronicsDeviceComponent::SelectClosestPort(FVector WorldLocation, FGameplayTag Domain, bool& Success, FSLMPort& OutPort)
{
	double DistanceSquared = UE_BIG_NUMBER;
	Success = false;
	for (const auto& Port : Ports)
	{
		const FVector PortLocation = PortToWorldLocation(Port);
		const double ThisPortDistanceSquared = FVector::DistSquared(WorldLocation, PortLocation);
		if (ThisPortDistanceSquared < DistanceSquared)
		{
			DistanceSquared = ThisPortDistanceSquared;
			OutPort = Port;
			Success = true;
		}
	}
}

void USLMechatronicsDeviceComponent::UpdatePortLocationData()
{
	const AActor* Owner = GetOwner();
	
	for (auto& Port : Ports)
	{
		USceneComponent* SceneComponent = nullptr;
		for(UActorComponent* Component : Owner->GetComponents())
		{
			if(Component->GetFName() == Port.PortLocationData.ComponentName)
			{
				SceneComponent = Cast<USceneComponent>(Component);
			}
		}
		if (!SceneComponent)
		{
			SceneComponent = Owner->GetRootComponent();
		}
		Port.PortLocationData.SceneComponent = SceneComponent;
	}
}


FVector USLMechatronicsDeviceComponent::PortToWorldLocation(FSLMPort Port)
{
	const USceneComponent* SceneComponent = Port.PortLocationData.SceneComponent;
	const FName SocketName = Port.PortLocationData.SocketName;
	const FVector Offset = Port.PortLocationData.Offset;
	const FTransform SocketTransform = SceneComponent->GetSocketTransform(SocketName, RTS_World);
	const FVector OutLocation = SocketTransform.TransformPosition(Offset);
	return OutLocation;
}


// Called when the game starts
void USLMechatronicsDeviceComponent::BeginPlay()
{
	Super::BeginPlay();
	UpdatePortLocationData();
	Subsystem = GetWorld()->GetSubsystem<USLMechatronicsSubsystem>();
	DeviceIndex = Subsystem->AddDevice(this);
	for (auto& Port : Ports)
	{
		Port.PortIndex = Subsystem->AddPort(Port);
	}
}


void USLMechatronicsDeviceComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	for (const auto& Port : Ports)
	{
		Subsystem->RemovePort(Port.PortIndex);
	}
	Super::EndPlay(EndPlayReason);
}
*/
