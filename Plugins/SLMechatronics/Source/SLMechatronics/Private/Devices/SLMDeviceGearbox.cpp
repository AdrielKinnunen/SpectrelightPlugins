// Fill out your copyright notice in the Description page of Project Settings.


#include "Devices/SLMDeviceGearbox.h"
#include "SLMSubsystem.h"


void USLMDeviceSubsystemGearbox::PreSimulate(float DeltaTime)
{
}

void USLMDeviceSubsystemGearbox::Simulate(float DeltaTime)
{
	for (auto& [GearRatio, IndexShaft1, IndexShaft2] :Instances)
	{
		const auto [AngVelShaftInput,MOIShaftInput] = Subsystem->GetNetworkData(IndexShaft1);
		const auto [AngVelShaftOutput,MOIShaftOutput] = Subsystem->GetNetworkData(IndexShaft2);
		
		const float MOIShaftInputEffective = GearRatio * GearRatio * MOIShaftInput;
		const float AngVelShaftInputEffective = AngVelShaftInput / GearRatio;
		const float AngVelShaftOutput_Out = (AngVelShaftInputEffective * MOIShaftInputEffective + AngVelShaftOutput * MOIShaftOutput) / (MOIShaftInputEffective + MOIShaftOutput);
		const float AngVelShaftInput_Out = AngVelShaftOutput_Out * GearRatio;

		Subsystem->SetNetworkValue(IndexShaft1, AngVelShaftInput_Out);
		Subsystem->SetNetworkValue(IndexShaft2, AngVelShaftOutput_Out);
	}
}

void USLMDeviceSubsystemGearbox::PostSimulate(float DeltaTime)
{
}

void USLMDeviceSubsystemGearbox::AddInstance(const FSLMDeviceModelGearbox Instance)
{
	Instances.Add(Instance);
}

USLMDeviceComponentGearbox::USLMDeviceComponentGearbox()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void USLMDeviceComponentGearbox::BeginPlay()
{
	Super::BeginPlay();
	DeviceModel.IndexShaftInput = Subsystem->AddPort(PortShaftInput);
	DeviceModel.IndexShaftOutput = Subsystem->AddPort(PortShaftOutput);
	GetWorld()->GetSubsystem<USLMDeviceSubsystemGearbox>()->AddInstance(DeviceModel);
}

void USLMDeviceComponentGearbox::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

