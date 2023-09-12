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
	for (auto& [GearRatio, Index_Mech_Input, Index_Mech_Output] :Instances)
	{
		const auto [AngVelShaftInput,MOIShaftInput] = DomainMech->GetNetworkData(Index_Mech_Input);
		const auto [AngVelShaftOutput,MOIShaftOutput] = DomainMech->GetNetworkData(Index_Mech_Output);
		
		const float MOIShaftInputEffective = GearRatio * GearRatio * MOIShaftInput;
		const float AngVelShaftInputEffective = AngVelShaftInput / GearRatio;
		const float AngVelShaftOutput_Out = (AngVelShaftInputEffective * MOIShaftInputEffective + AngVelShaftOutput * MOIShaftOutput) / (MOIShaftInputEffective + MOIShaftOutput);
		const float AngVelShaftInput_Out = AngVelShaftOutput_Out * GearRatio;

		const FSLMDataMech Shaft1 = FSLMDataMech(AngVelShaftInput_Out, MOIShaftInput);
		const FSLMDataMech Shaft2 = FSLMDataMech(AngVelShaftOutput_Out, MOIShaftOutput);
		
		DomainMech->SetNetworkData(Index_Mech_Input, Shaft1);
		DomainMech->SetNetworkData(Index_Mech_Output, Shaft2);
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
	DomainMech = nullptr;
}

void USLMDeviceComponentGearbox::BeginPlay()
{
	Super::BeginPlay();
	DomainMech = GetWorld()->GetSubsystem<USLMDomainMech>();
	DeviceModel.Index_Mech_Input = DomainMech->AddPort(Port_Mech_Input);
	DeviceModel.Index_Mech_Output = DomainMech->AddPort(Port_Mech_Output);
	GetWorld()->GetSubsystem<USLMDeviceSubsystemGearbox>()->AddInstance(DeviceModel);
}

void USLMDeviceComponentGearbox::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

