// Copyright Spectrelight Studios, LLC

/*

#include "Devices/SLMDevicePositiveDisplacementAirPump.h"



void USLMDeviceComponentPositiveDisplacementAirPump::BeginPlay()
{
	Super::BeginPlay();
	GetWorld()->GetSubsystem<USLMDeviceSubsystemPositiveDisplacementAirPump>()->RegisterDeviceComponent(this);
}

void USLMDeviceComponentPositiveDisplacementAirPump::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorld()->GetSubsystem<USLMDeviceSubsystemPositiveDisplacementAirPump>()->DeRegisterDeviceComponent(this);
	Super::EndPlay(EndPlayReason);
}

void USLMDeviceSubsystemPositiveDisplacementAirPump::OnWorldBeginPlay(UWorld& InWorld)
{
	DomainRotation = GetWorld()->GetSubsystem<USLMDomainRotation>();
	Super::OnWorldBeginPlay(InWorld);
}

void USLMDeviceSubsystemPositiveDisplacementAirPump::PreSimulate(const float DeltaTime)
{
}

void USLMDeviceSubsystemPositiveDisplacementAirPump::Simulate(const float DeltaTime)
{
	for (const auto& Model : DeviceModels)
	{
		const FSLMDataRotation Crank = DomainRotation->GetByPortIndex(Model.Index_Rotation_Crankshaft);
		const FSLMDataAir Intake = DomainAir->GetByPortIndex(Model.Index_Air_Intake);
		const FSLMDataAir Exhaust = DomainAir->GetByPortIndex(Model.Index_Air_Exhaust);
		
		const float PressureDifference = Intake.Pressure_bar - Exhaust.Pressure_bar;
		const float PumpingTorque = PressureDifference * Model.DisplacementPerRev * OneOverTwoPi;
		
		const float LitersMoved = Model.DisplacementPerRev * FMath::Abs(Crank.RPS) * DeltaTime;
		const bool bIsNormalDirection = Crank.RPS >= 0.0;
		const int32 FromPort = bIsNormalDirection ? Model.Index_Air_Intake : Model.Index_Air_Exhaust;
		const int32 ToPort = bIsNormalDirection ? Model.Index_Air_Exhaust : Model.Index_Air_Intake;
		const FSLMDataAir Charge = DomainAir->RemoveAir(FromPort, LitersMoved);
		DomainAir->AddAir(ToPort, Charge);
		
		const float CrankRPS_Out = Crank.RPS + (PumpingTorque * DeltaTime) / Crank.MOI;
		DomainRotation->SetAngVelByPortIndex(Model.Index_Rotation_Crankshaft, CrankRPS_Out);
	}
}

void USLMDeviceSubsystemPositiveDisplacementAirPump::PostSimulate(const float DeltaTime)
{
}

void USLMDeviceSubsystemPositiveDisplacementAirPump::RegisterDeviceComponent(USLMDeviceComponentPositiveDisplacementAirPump* DeviceComponent)
{
	const auto Index = AddDevice(DeviceComponent->DeviceSettings);
	DeviceComponent->DeviceIndex = Index;
	DeviceComponents.Insert(Index, DeviceComponent);
}

void USLMDeviceSubsystemPositiveDisplacementAirPump::DeRegisterDeviceComponent(const USLMDeviceComponentPositiveDisplacementAirPump* DeviceComponent)
{
	const auto Index = DeviceComponent->DeviceIndex;
	RemoveDevice(Index);
	DeviceComponents.RemoveAt(Index);
}

int32 USLMDeviceSubsystemPositiveDisplacementAirPump::AddDevice(FSLMDevicePositiveDisplacementAirPump Device)
{
	Device.DeviceModel.Index_Rotation_Crankshaft = DomainRotation->AddPort(Device.Port_Rotation_Crankshaft);
	return DeviceModels.Add(Device.DeviceModel);
}

void USLMDeviceSubsystemPositiveDisplacementAirPump::RemoveDevice(const int32 DeviceIndex)
{
	DomainRotation->RemovePort(DeviceModels[DeviceIndex].Index_Rotation_Crankshaft);
	DeviceModels.RemoveAt(DeviceIndex);
}

FSLMDeviceModelPositiveDisplacementAirPump USLMDeviceSubsystemPositiveDisplacementAirPump::GetDeviceState(const int32 DeviceIndex)
{
	return DeviceModels[DeviceIndex];
}
*/