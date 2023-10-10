// Fill out your copyright notice in the Description page of Project Settings.


#include "Devices/SLMDeviceDoubleDifferential.h"


/*
void USLMDeviceSubsystemDoubleDifferential::OnWorldBeginPlay(UWorld& InWorld)
{
	DomainMech = GetWorld()->GetSubsystem<USLMDomainMech>();
	Super::OnWorldBeginPlay(InWorld);
}

void USLMDeviceSubsystemDoubleDifferential::PreSimulate(float DeltaTime)
{
}

void USLMDeviceSubsystemDoubleDifferential::Simulate(float DeltaTime)
{
	for (auto& [GearRatio_Drive, GearRatio_Steer, Index_Mech_Drive, Index_Mech_Steer, Index_Mech_Left, Index_Mech_Right] :Instances)
	{
		const auto [A,L] = DomainMech->GetNetworkData(Index_Mech_Drive);
		const auto [B,M] = DomainMech->GetNetworkData(Index_Mech_Steer);
		const auto [C,N] = DomainMech->GetNetworkData(Index_Mech_Left);
		const auto [D,O] = DomainMech->GetNetworkData(Index_Mech_Right);
	
		const float Divisor = L*(M+N+O) + M*(N+O) + 4*N*O;
	
		const float W = (A*L*(M+N+O) + B*M*(N-O) - C*M*N - 2*C*N*O + D*M*O + 2*D*N*O) / Divisor;
		const float X = (A*L*(N-O) + B*M*(L+N+O) + C*L*N + 2*C*N*O + D*L*O + 2*D*N*O) / Divisor;
		const float Y = X - W;
		const float Z = X + W;

		const FSLMDataMech Data_Drive = FSLMDataMech(W, L);
		const FSLMDataMech Data_Steer = FSLMDataMech(X, M);
		const FSLMDataMech Data_Left = FSLMDataMech(Y, N);
		const FSLMDataMech Data_Right = FSLMDataMech(Z, O);
		
		DomainMech->SetNetworkData(Index_Mech_Drive, Data_Drive);
		DomainMech->SetNetworkData(Index_Mech_Steer, Data_Steer);
		DomainMech->SetNetworkData(Index_Mech_Left, Data_Left);
		DomainMech->SetNetworkData(Index_Mech_Right, Data_Right);
	}
}

void USLMDeviceSubsystemDoubleDifferential::PostSimulate(float DeltaTime)
{
}

void USLMDeviceSubsystemDoubleDifferential::AddInstance(const FSLMDeviceModelDoubleDifferential& Instance)
{
	Instances.Add(Instance);
}

USLMDeviceComponentDoubleDifferential::USLMDeviceComponentDoubleDifferential()
{
	PrimaryComponentTick.bCanEverTick = false;
	DomainMech = nullptr;
}

void USLMDeviceComponentDoubleDifferential::BeginPlay()
{
	Super::BeginPlay();
	DomainMech = GetWorld()->GetSubsystem<USLMDomainMech>();
	DeviceModel.Index_Mech_Drive = DomainMech->AddPort(Port_Mech_Drive);
	DeviceModel.Index_Mech_Steer = DomainMech->AddPort(Port_Mech_Steer);
	DeviceModel.Index_Mech_Left = DomainMech->AddPort(Port_Mech_Left);
	DeviceModel.Index_Mech_Right = DomainMech->AddPort(Port_Mech_Right);
	GetWorld()->GetSubsystem<USLMDeviceSubsystemDoubleDifferential>()->AddInstance(DeviceModel);
}

void USLMDeviceComponentDoubleDifferential::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}
*/

