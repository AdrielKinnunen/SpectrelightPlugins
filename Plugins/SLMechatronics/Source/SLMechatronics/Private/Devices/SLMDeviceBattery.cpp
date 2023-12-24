// Copyright Spectrelight Studios, LLC

#include "Devices/SLMDeviceBattery.h"

FSLMDeviceModelBattery USLMDeviceComponentBattery::GetDeviceState() const
{
	return Subsystem->GetDeviceState(DeviceIndex);
}

void USLMDeviceComponentBattery::BeginPlay()
{
	Super::BeginPlay();
	const AActor* OwningActor = GetOwner();
	DeviceSettings.Port_Electricity.PortMetaData.AssociatedActor = OwningActor;
	DeviceSettings.Port_Signal_ChargePercent.PortMetaData.AssociatedActor = OwningActor;
	
	Subsystem = GetWorld()->GetSubsystem<USLMDeviceSubsystemBattery>();
	DeviceIndex = Subsystem->AddDevice(DeviceSettings);
}

void USLMDeviceComponentBattery::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Subsystem->RemoveDevice(DeviceIndex);
	Super::EndPlay(EndPlayReason);
}

void USLMDeviceSubsystemBattery::OnWorldBeginPlay(UWorld& InWorld)
{
	DomainElectricity = GetWorld()->GetSubsystem<USLMDomainElectricity>();
	DomainSignal = GetWorld()->GetSubsystem<USLMDomainSignal>();
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
		EnergyDemand = FMath::Clamp(EnergyDemand, Battery.EnergyJoules - Battery.CapacityJoules, Battery.EnergyJoules);

		const float NewJoules = Electricity.StoredJoules + EnergyDemand;
		Battery.EnergyJoules -= EnergyDemand;
		const float StateOfCharge = Battery.EnergyJoules / Battery.CapacityJoules;
		DomainElectricity->SetJoulesByPortIndex(Battery.Index_Electricity, NewJoules);
		DomainSignal->WriteByPortIndex(Battery.Index_Signal_ChargePercent, StateOfCharge);
	}
}

void USLMDeviceSubsystemBattery::PostSimulate(const float DeltaTime)
{
}

int32 USLMDeviceSubsystemBattery::AddDevice(FSLMDeviceBattery Device)
{
	Device.DeviceModel.Index_Electricity = DomainElectricity->AddPort(Device.Port_Electricity);
	Device.DeviceModel.Index_Signal_ChargePercent = DomainSignal->AddPort(Device.Port_Signal_ChargePercent);
	return DeviceModels.Add(Device.DeviceModel);
}

void USLMDeviceSubsystemBattery::RemoveDevice(const int32 DeviceIndex)
{
	DomainElectricity->RemovePort(DeviceModels[DeviceIndex].Index_Electricity);
	DomainSignal->RemovePort(DeviceModels[DeviceIndex].Index_Signal_ChargePercent);
	
	DeviceModels.RemoveAt(DeviceIndex);
}

FSLMDeviceModelBattery USLMDeviceSubsystemBattery::GetDeviceState(const int32 DeviceIndex)
{
	return DeviceModels[DeviceIndex];
}