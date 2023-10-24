// Fill out your copyright notice in the Description page of Project Settings.


/*
#include "Devices/SLMDeviceDifferential.h"


void USLMDeviceSubsystemDifferential::OnWorldBeginPlay(UWorld& InWorld)
{
	DomainMech = GetWorld()->GetSubsystem<USLMDomainMech>();
	Super::OnWorldBeginPlay(InWorld);
}

void USLMDeviceSubsystemDifferential::PreSimulate(float DeltaTime)
{
}

void USLMDeviceSubsystemDifferential::Simulate(float DeltaTime)
{
	for (auto& [IndexShaftInput, IndexShaftLeft, IndexShaftRight] :Instances)
	{
		const auto [B,M] = DomainMech->GetNetworkData(IndexShaftInput);
		const auto [C,N] = DomainMech->GetNetworkData(IndexShaftLeft);
		const auto [D,O] = DomainMech->GetNetworkData(IndexShaftRight);
	
		const float Divisor = M*N + M*O + 4*N*O;

		const float LeftShaftVel_Out = (-2*B*M*O + C*M*N + 4*C*N*O + D*M*O) / Divisor;
		const float RightShaftVel_Out = (2*B*M*N + C*M*N + D*M*O + 4*D*N*O) / Divisor;
		const float InputShaftVel_Out = 0.5 * (RightShaftVel_Out - LeftShaftVel_Out);

		const FSLMDataMech Data_Input = FSLMDataMech(InputShaftVel_Out, M);
		const FSLMDataMech Data_Left = FSLMDataMech(LeftShaftVel_Out, N);
		const FSLMDataMech Data_Right = FSLMDataMech(RightShaftVel_Out, O);
		
		DomainMech->SetNetworkData(IndexShaftInput, Data_Input);
		DomainMech->SetNetworkData(IndexShaftLeft, Data_Left);
		DomainMech->SetNetworkData(IndexShaftRight, Data_Right);
	}
}

void USLMDeviceSubsystemDifferential::PostSimulate(float DeltaTime)
{
}

void USLMDeviceSubsystemDifferential::AddInstance(const FSLMDeviceModelDifferential Instance)
{
	Instances.Add(Instance);
}

USLMDeviceComponentDifferential::USLMDeviceComponentDifferential()
{
	PrimaryComponentTick.bCanEverTick = false;
	DomainMech = nullptr;
}

void USLMDeviceComponentDifferential::BeginPlay()
{
	Super::BeginPlay();
	DomainMech = GetWorld()->GetSubsystem<USLMDomainMech>();
	DeviceModel.IndexShaftInput = DomainMech->AddPort(PortShaftInput);
	DeviceModel.IndexShaftLeft = DomainMech->AddPort(PortShaftLeft);
	DeviceModel.IndexShaftRight = DomainMech->AddPort(PortShaftRight);
	GetWorld()->GetSubsystem<USLMDeviceSubsystemDifferential>()->AddInstance(DeviceModel);
}

void USLMDeviceComponentDifferential::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}
*/
