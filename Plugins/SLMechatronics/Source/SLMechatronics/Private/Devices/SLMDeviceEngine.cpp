// Copyright Spectrelight Studios, LLC

#include "Devices/SLMDeviceEngine.h"
#include "SLMSubsystem.h"


void USLMDeviceSubsystemEngine::PreSimulate(float DeltaTime)
{
}

void USLMDeviceSubsystemEngine::Simulate(float DeltaTime)
{
	for (auto& [MaxTorque, MaxRPM, Index_Mech_Crankshaft, Index_Signal_Throttle] :Instances)
	{
		const auto [CrankRPM,CrankMOI] = Subsystem->GetNetworkData(Index_Mech_Crankshaft);
		const auto Throttle = Subsystem->GetNetworkValue(Index_Signal_Throttle);
	
		const float CrankMomentum = CrankRPM * CrankMOI;
		const float Torque = MaxTorque * Throttle * (CrankRPM < MaxRPM) - 0.2 * CrankRPM;

		const float CrankMomentum_Out = CrankMomentum + Torque * DeltaTime;
		const float CrankRPM_Out = CrankMomentum_Out / CrankMOI;

		Subsystem->SetNetworkValue(Index_Mech_Crankshaft, CrankRPM_Out);
	}
}

void USLMDeviceSubsystemEngine::PostSimulate(float DeltaTime)
{
}

void USLMDeviceSubsystemEngine::AddInstance(const FSLMDeviceModelEngine Instance)
{
	Instances.Add(Instance);
}

USLMDeviceComponentEngine::USLMDeviceComponentEngine()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void USLMDeviceComponentEngine::BeginPlay()
{
	Super::BeginPlay();
	DeviceModel.Index_Mech_Crankshaft = Subsystem->AddPort(Port_Mech_Crankshaft);
	DeviceModel.Index_Signal_Throttle = Subsystem->AddPort(Port_Signal_Throttle);
	GetWorld()->GetSubsystem<USLMDeviceSubsystemEngine>()->AddInstance(DeviceModel);
}

void USLMDeviceComponentEngine::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

