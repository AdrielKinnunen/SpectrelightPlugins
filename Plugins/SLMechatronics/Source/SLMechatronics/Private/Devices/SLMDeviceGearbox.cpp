// Fill out your copyright notice in the Description page of Project Settings.


#include "Devices/SLMDeviceGearbox.h"


void USLMDeviceSubsystemGearbox::OnWorldBeginPlay(UWorld& InWorld)
{
	DomainMech = GetWorld()->GetSubsystem<USLMDomainMech>();
	Super::OnWorldBeginPlay(InWorld);
}

void USLMDeviceSubsystemGearbox::PreSimulate(float DeltaTime)
{
}

void USLMDeviceSubsystemGearbox::Simulate(float DeltaTime)
{
	for (auto& [GearRatio, Index_Mech_Input, Index_Mech_Output] :DeviceModels)
	{
		const FSLMDataMech In = DomainMech->GetNetworkData(Index_Mech_Input);
		const FSLMDataMech Out = DomainMech->GetNetworkData(Index_Mech_Output);
		
		const float MOIShaftInputEffective = GearRatio * GearRatio * In.MOI;
		const float AngVelShaftInputEffective = In.AngVel / GearRatio;

		const float OutAngVel = (AngVelShaftInputEffective * MOIShaftInputEffective + Out.AngVel * Out.MOI) / (MOIShaftInputEffective + Out.MOI);
		const float InAngVel = OutAngVel * GearRatio;

		DomainMech->SetNetworkAngVel(Index_Mech_Input, InAngVel);
		DomainMech->SetNetworkAngVel(Index_Mech_Output, OutAngVel);
	}
}

void USLMDeviceSubsystemGearbox::PostSimulate(float DeltaTime)
{
}

int32 USLMDeviceSubsystemGearbox::AddDevice(FSLMDeviceGearbox Device)
{
	Device.DeviceModel.Index_Mech_Input = DomainMech->AddPort(Device.Port_Mech_Input);
	Device.DeviceModel.Index_Mech_Output = DomainMech->AddPort(Device.Port_Mech_Output);
	return DeviceModels.Add(Device.DeviceModel);
}

void USLMDeviceSubsystemGearbox::RemoveDevice(const int32 DeviceIndex)
{
	DomainMech->RemovePort(DeviceModels[DeviceIndex].Index_Mech_Input);
	DomainMech->RemovePort(DeviceModels[DeviceIndex].Index_Mech_Output);
	DeviceModels.RemoveAt(DeviceIndex);
}

FSLMDeviceModelGearbox USLMDeviceSubsystemGearbox::GetDeviceState(const int32 DeviceIndex)
{
	return DeviceModels[DeviceIndex];
}

void USLMDeviceSubsystemGearbox::SetGearRatio(const int32 DeviceIndex, const float GearRatio)
{
	DeviceModels[DeviceIndex].GearRatio = GearRatio;
}


USLMDeviceComponentGearbox::USLMDeviceComponentGearbox()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void USLMDeviceComponentGearbox::BeginPlay()
{
	Super::BeginPlay();
	DeviceIndex = GetWorld()->GetSubsystem<USLMDeviceSubsystemGearbox>()->AddDevice(DeviceSettings);
}

void USLMDeviceComponentGearbox::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorld()->GetSubsystem<USLMDeviceSubsystemGearbox>()->RemoveDevice(DeviceIndex);
	Super::EndPlay(EndPlayReason);
}