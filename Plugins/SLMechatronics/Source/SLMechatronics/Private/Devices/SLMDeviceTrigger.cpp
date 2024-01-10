// Copyright Spectrelight Studios, LLC

/*

#include "Devices/SLMDeviceTrigger.h"

USLMDeviceComponentTrigger::USLMDeviceComponentTrigger()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void USLMDeviceComponentTrigger::BeginPlay()
{
	Super::BeginPlay();
	GetWorld()->GetSubsystem<USLMDeviceSubsystemTrigger>()->RegisterDeviceComponent(this);
}

void USLMDeviceComponentTrigger::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorld()->GetSubsystem<USLMDeviceSubsystemTrigger>()->DeRegisterDeviceComponent(this);
	Super::EndPlay(EndPlayReason);
}

void USLMDeviceSubsystemTrigger::OnWorldBeginPlay(UWorld& InWorld)
{
	DomainSignal = GetWorld()->GetSubsystem<USLMDomainSignal>();
	Super::OnWorldBeginPlay(InWorld);
}

void USLMDeviceSubsystemTrigger::PreSimulate(const float DeltaTime)
{
}

void USLMDeviceSubsystemTrigger::Simulate(const float DeltaTime)
{
	for (int32 i = 0; i < DeviceModels.Num(); i++)
	{
		auto& [TriggerValue, OldValue, Index_Signal_Input] = DeviceModels[i];
		const float NewValue = DomainSignal->ReadByPortIndex(Index_Signal_Input);
		if (NewValue >= TriggerValue && OldValue < TriggerValue)
		{
			//UE_LOG(LogTemp, Warning, TEXT("!!!!!!!TRIGGERED!!!!!!!"));
			DeviceComponents[i]->OnTrigger.Broadcast(DeltaTime);
		}
		OldValue = NewValue;
	}
}

void USLMDeviceSubsystemTrigger::PostSimulate(const float DeltaTime)
{
}

void USLMDeviceSubsystemTrigger::RegisterDeviceComponent(USLMDeviceComponentTrigger* DeviceComponent)
{
	const auto Index = AddDevice(DeviceComponent->DeviceSettings);
	DeviceComponent->DeviceIndex = Index;
	DeviceComponents.Insert(Index, DeviceComponent);
}

void USLMDeviceSubsystemTrigger::DeRegisterDeviceComponent(const USLMDeviceComponentTrigger* DeviceComponent)
{
	const auto Index = DeviceComponent->DeviceIndex;
	RemoveDevice(Index);
	DeviceComponents.RemoveAt(Index);
}

int32 USLMDeviceSubsystemTrigger::AddDevice(FSLMDeviceTrigger Device)
{
	Device.DeviceModel.Index_Signal_Input = DomainSignal->AddPort(Device.Port_Signal_Input);
	return DeviceModels.Add(Device.DeviceModel);
}

void USLMDeviceSubsystemTrigger::RemoveDevice(const int32 DeviceIndex)
{
	DomainSignal->RemovePort(DeviceModels[DeviceIndex].Index_Signal_Input);
	DeviceModels.RemoveAt(DeviceIndex);
}

FSLMDeviceModelTrigger USLMDeviceSubsystemTrigger::GetDeviceState(const int32 DeviceIndex)
{
	return DeviceModels[DeviceIndex];
}

void USLMDeviceSubsystemTrigger::SetSignal(const int32 DeviceIndex, const float NewSignal)
{
	const auto PortIndex = DeviceModels[DeviceIndex].Index_Signal_Input;
	DomainSignal->WriteByPortIndex(PortIndex, NewSignal);
}
*/
