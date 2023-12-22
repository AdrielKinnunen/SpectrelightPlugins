// Copyright Spectrelight Studios, LLC

#include "Devices/SLMDeviceBattery.h"

USLMDeviceComponentBattery::USLMDeviceComponentBattery()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void USLMDeviceComponentBattery::BeginPlay()
{
	Super::BeginPlay();
	GetWorld()->GetSubsystem<USLMDeviceSubsystemBattery>()->RegisterDeviceComponent(this);
}

void USLMDeviceComponentBattery::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorld()->GetSubsystem<USLMDeviceSubsystemBattery>()->DeRegisterDeviceComponent(this);
	Super::EndPlay(EndPlayReason);
}

void USLMDeviceSubsystemBattery::OnWorldBeginPlay(UWorld& InWorld)
{
	DomainElectricity = GetWorld()->GetSubsystem<USLMDomainElectricity>();
	Super::OnWorldBeginPlay(InWorld);
}

void USLMDeviceSubsystemBattery::PreSimulate(const float DeltaTime)
{
}

void USLMDeviceSubsystemBattery::Simulate(const float DeltaTime)
{
	for (auto& Battery : DeviceModels)
	{
		const FSLMDataElectricity Electricity = DomainElectricity->GetByPortIndex(Battery.Index_Electricity);

		float EnergyDemand = Electricity.CapacityJoules * Battery.ChargeDischargeTriggerPercent - Electricity.StoredJoules;
		const float UpperBoundDueToPower = Battery.PowerWatts * DeltaTime;
		EnergyDemand = FMath::Clamp(EnergyDemand, -1 * UpperBoundDueToPower, UpperBoundDueToPower);
		EnergyDemand = FMath::Clamp(EnergyDemand, Battery.CapacityJoules - Battery.EnergyJoules, Battery.EnergyJoules);

		const float NewJoules = Electricity.StoredJoules + EnergyDemand;
		Battery.EnergyJoules -= EnergyDemand;
		DomainElectricity->SetJoulesByPortIndex(Battery.Index_Electricity, NewJoules);
	}
}

void USLMDeviceSubsystemBattery::PostSimulate(const float DeltaTime)
{
}

void USLMDeviceSubsystemBattery::RegisterDeviceComponent(USLMDeviceComponentBattery* DeviceComponent)
{
	const auto Index = AddDevice(DeviceComponent->DeviceSettings);
	DeviceComponent->DeviceIndex = Index;
	DeviceComponents.Insert(Index, DeviceComponent);
}

void USLMDeviceSubsystemBattery::DeRegisterDeviceComponent(const USLMDeviceComponentBattery* DeviceComponent)
{
	const auto Index = DeviceComponent->DeviceIndex;
	RemoveDevice(Index);
	DeviceComponents.RemoveAt(Index);
}

int32 USLMDeviceSubsystemBattery::AddDevice(FSLMDeviceBattery Device)
{
	Device.DeviceModel.Index_Electricity = DomainElectricity->AddPort(Device.Port_Electricity);
	return DeviceModels.Add(Device.DeviceModel);
}

void USLMDeviceSubsystemBattery::RemoveDevice(const int32 DeviceIndex)
{
	DomainElectricity->RemovePort(DeviceModels[DeviceIndex].Index_Electricity);
	DeviceModels.RemoveAt(DeviceIndex);
}

FSLMDeviceModelBattery USLMDeviceSubsystemBattery::GetDeviceState(const int32 DeviceIndex)
{
	return DeviceModels[DeviceIndex];
}