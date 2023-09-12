// Copyright Spectrelight Studios, LLC

#include "Devices/SLMDeviceEngine.h"


void USLMDeviceSubsystemEngine::OnWorldBeginPlay(UWorld& InWorld)
{
	DomainMech = GetWorld()->GetSubsystem<USLMDomainMech>();
	Super::OnWorldBeginPlay(InWorld);
}

void USLMDeviceSubsystemEngine::PreSimulate(float DeltaTime)
{
}

void USLMDeviceSubsystemEngine::Simulate(float DeltaTime)
{
	for (auto& [MaxTorque, MaxRPM, Index_Mech_Crankshaft] :Instances)
	{
		const auto [CrankRPM,CrankMOI] = DomainMech->GetNetworkData(Index_Mech_Crankshaft);
		//const auto Throttle = DomainMech->GetNetworkValue(Index_Signal_Throttle);
		const float Throttle = 1.0;
		const float CrankMomentum = CrankRPM * CrankMOI;
		const float Torque = MaxTorque * Throttle * (CrankRPM < MaxRPM) - 0.2 * CrankRPM;

		const float CrankMomentum_Out = CrankMomentum + Torque * DeltaTime;
		const float CrankRPM_Out = CrankMomentum_Out / CrankMOI;

		const FSLMDataMech OutData = FSLMDataMech(CrankRPM_Out, CrankMOI);
		
		DomainMech->SetNetworkData(Index_Mech_Crankshaft, OutData);
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
	DomainMech = nullptr;
}

void USLMDeviceComponentEngine::BeginPlay()
{
	Super::BeginPlay();
	DomainMech = GetWorld()->GetSubsystem<USLMDomainMech>();
	DeviceModel.Index_Mech_Crankshaft = DomainMech->AddPort(Port_Mech_Crankshaft);
	//DeviceModel.Index_Signal_Throttle = Subsystem->AddPort(Port_Signal_Throttle);
	GetWorld()->GetSubsystem<USLMDeviceSubsystemEngine>()->AddInstance(DeviceModel);
}

void USLMDeviceComponentEngine::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

