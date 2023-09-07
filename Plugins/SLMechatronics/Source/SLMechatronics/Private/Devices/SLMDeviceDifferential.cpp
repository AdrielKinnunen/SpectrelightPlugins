// Fill out your copyright notice in the Description page of Project Settings.


#include "Devices/SLMDeviceDifferential.h"
#include "SLMSubsystem.h"


void USLMDeviceSubsystemDifferential::PreSimulate(float DeltaTime)
{
}

void USLMDeviceSubsystemDifferential::Simulate(float DeltaTime)
{
	for (auto& [IndexShaftInput, IndexShaftLeft, IndexShaftRight] :Instances)
	{
		const auto [B,M] = Subsystem->GetNetworkData(IndexShaftInput);
		const auto [C,N] = Subsystem->GetNetworkData(IndexShaftLeft);
		const auto [D,O] = Subsystem->GetNetworkData(IndexShaftRight);
	
		const float Divisor = M*N + M*O + 4*N*O;

		const float LeftShaftVel_Out = (-2*B*M*O + C*M*N + 4*C*N*O + D*M*O) / Divisor;
		const float RightShaftVel_Out = (2*B*M*N + C*M*N + D*M*O + 4*D*N*O) / Divisor;
		const float InputShaftVel_Out = 0.5 * (RightShaftVel_Out - LeftShaftVel_Out);

		Subsystem->SetNetworkValue(IndexShaftInput, InputShaftVel_Out);
		Subsystem->SetNetworkValue(IndexShaftLeft, LeftShaftVel_Out);
		Subsystem->SetNetworkValue(IndexShaftRight, RightShaftVel_Out);
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
}

void USLMDeviceComponentDifferential::BeginPlay()
{
	Super::BeginPlay();
	DeviceModel.IndexShaftInput = Subsystem->AddPort(PortShaftInput);
	DeviceModel.IndexShaftLeft = Subsystem->AddPort(PortShaftLeft);
	DeviceModel.IndexShaftRight = Subsystem->AddPort(PortShaftRight);
	GetWorld()->GetSubsystem<USLMDeviceSubsystemDifferential>()->AddInstance(DeviceModel);
}

void USLMDeviceComponentDifferential::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

