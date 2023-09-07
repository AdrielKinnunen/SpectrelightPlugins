// Fill out your copyright notice in the Description page of Project Settings.


#include "Devices/SLMDeviceDoubleDifferential.h"
#include "SLMSubsystem.h"


void USLMDeviceSubsystemDoubleDifferential::PreSimulate(float DeltaTime)
{
}

void USLMDeviceSubsystemDoubleDifferential::Simulate(float DeltaTime)
{
	for (auto& [GearRatio_Drive, GearRatio_Steer, Index_Mech_Drive, Index_Mech_Steer, Index_Mech_Left, Index_Mech_Right] :Instances)
	{
		const auto [A,L] = Subsystem->GetNetworkData(Index_Mech_Drive);
		const auto [B,M] = Subsystem->GetNetworkData(Index_Mech_Steer);
		const auto [C,N] = Subsystem->GetNetworkData(Index_Mech_Left);
		const auto [D,O] = Subsystem->GetNetworkData(Index_Mech_Right);
	
		const float Divisor = L*(M+N+O) + M*(N+O) + 4*N*O;
	
		const float W = (A*L*(M+N+O) + B*M*(N-O) - C*M*N - 2*C*N*O + D*M*O + 2*D*N*O) / Divisor;
		const float X = (A*L*(N-O) + B*M*(L+N+O) + C*L*N + 2*C*N*O + D*L*O + 2*D*N*O) / Divisor;
		const float Y = X - W;
		const float Z = X + W;

		Subsystem->SetNetworkValue(Index_Mech_Drive, W);
		Subsystem->SetNetworkValue(Index_Mech_Steer, X);
		Subsystem->SetNetworkValue(Index_Mech_Left, Y);
		Subsystem->SetNetworkValue(Index_Mech_Right, Z);
	}
}

void USLMDeviceSubsystemDoubleDifferential::PostSimulate(float DeltaTime)
{
}

void USLMDeviceSubsystemDoubleDifferential::AddInstance(const FSLMDeviceModelDoubleDifferential Instance)
{
	Instances.Add(Instance);
}

USLMDeviceComponentDoubleDifferential::USLMDeviceComponentDoubleDifferential()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void USLMDeviceComponentDoubleDifferential::BeginPlay()
{
	Super::BeginPlay();
	DeviceModel.Index_Mech_Drive = Subsystem->AddPort(Port_Mech_Drive);
	DeviceModel.Index_Mech_Steer = Subsystem->AddPort(Port_Mech_Steer);
	DeviceModel.Index_Mech_Left = Subsystem->AddPort(Port_Mech_Left);
	DeviceModel.Index_Mech_Right = Subsystem->AddPort(Port_Mech_Right);
	GetWorld()->GetSubsystem<USLMDeviceSubsystemDoubleDifferential>()->AddInstance(DeviceModel);
}

void USLMDeviceComponentDoubleDifferential::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

