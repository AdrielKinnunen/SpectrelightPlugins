// Copyright Spectrelight Studios, LLC

#include "Devices/SLMDeviceEngine.h"

USLMDeviceComponentEngine::USLMDeviceComponentEngine()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void USLMDeviceComponentEngine::BeginPlay()
{
	Super::BeginPlay();
	GetWorld()->GetSubsystem<USLMDeviceSubsystemEngine>()->RegisterDeviceComponent(this);
}

void USLMDeviceComponentEngine::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorld()->GetSubsystem<USLMDeviceSubsystemEngine>()->DeRegisterDeviceComponent(this);
	Super::EndPlay(EndPlayReason);
}

void USLMDeviceSubsystemEngine::OnWorldBeginPlay(UWorld& InWorld)
{
	DomainRotation = GetWorld()->GetSubsystem<USLMDomainRotation>();
	Super::OnWorldBeginPlay(InWorld);
}

void USLMDeviceSubsystemEngine::PreSimulate(const float DeltaTime)
{
}

void USLMDeviceSubsystemEngine::Simulate(const float DeltaTime)
{
	for (const auto& Model : DeviceModels)
	{
		const auto [CrankRPM,CrankMOI] = DomainRotation->GetNetworkData(Model.Index_Rotation_Crankshaft);
		const auto Throttle = DomainSignal->ReadData(Model.Index_Signal_Throttle);

		const float CrankMomentum = CrankRPM * CrankMOI;
		const float Torque = Model.MaxTorque * Throttle * (CrankRPM < Model.MaxRPM) - 0.2 * CrankRPM;

		const float CrankMomentum_Out = CrankMomentum + Torque * DeltaTime;
		const float CrankRPM_Out = CrankMomentum_Out / CrankMOI;

		DomainRotation->SetNetworkAngVel(Model.Index_Rotation_Crankshaft, CrankRPM_Out);
	}
}

void USLMDeviceSubsystemEngine::PostSimulate(const float DeltaTime)
{
}

void USLMDeviceSubsystemEngine::RegisterDeviceComponent(USLMDeviceComponentEngine* DeviceComponent)
{
	const auto Index = AddDevice(DeviceComponent->DeviceSettings);
	DeviceComponent->DeviceIndex = Index;
	DeviceComponents.Insert(Index, DeviceComponent);
}

void USLMDeviceSubsystemEngine::DeRegisterDeviceComponent(const USLMDeviceComponentEngine* DeviceComponent)
{
	const auto Index = DeviceComponent->DeviceIndex;
	RemoveDevice(Index);
	DeviceComponents.RemoveAt(Index);
}

int32 USLMDeviceSubsystemEngine::AddDevice(FSLMDeviceEngine Device)
{
	Device.DeviceModel.Index_Rotation_Crankshaft = DomainRotation->AddPort(Device.Port_Rotation_Crankshaft);
	Device.DeviceModel.Index_Signal_Throttle = DomainSignal->AddPort(Device.Port_Signal_Throttle);
	return DeviceModels.Add(Device.DeviceModel);
}

void USLMDeviceSubsystemEngine::RemoveDevice(const int32 DeviceIndex)
{
	DomainRotation->RemovePort(DeviceModels[DeviceIndex].Index_Rotation_Crankshaft);
	DomainSignal->RemovePort(DeviceModels[DeviceIndex].Index_Signal_Throttle);
	DeviceModels.RemoveAt(DeviceIndex);
}

FSLMDeviceModelEngine USLMDeviceSubsystemEngine::GetDeviceState(const int32 DeviceIndex)
{
	return DeviceModels[DeviceIndex];
}
