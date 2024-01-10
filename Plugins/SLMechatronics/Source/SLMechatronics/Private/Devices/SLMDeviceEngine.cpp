// Copyright Spectrelight Studios, LLC
/*
#include "Devices/SLMDeviceEngine.h"


FSLMDeviceModelEngine USLMDeviceComponentEngine::GetDeviceState()
{
	return Subsystem->GetDeviceState(DeviceIndex);
}

void USLMDeviceComponentEngine::BeginPlay()
{
	Super::BeginPlay();
	const AActor* OwningActor = GetOwner();
	DeviceSettings.Port_Rotation_Crankshaft.PortMetaData.AssociatedActor = OwningActor;
	DeviceSettings.Port_Signal_Throttle.PortMetaData.AssociatedActor = OwningActor;
	DeviceSettings.Port_Air_Intake.PortMetaData.AssociatedActor = OwningActor;
	DeviceSettings.Port_Air_Exhaust.PortMetaData.AssociatedActor = OwningActor;
	
	Subsystem = GetWorld()->GetSubsystem<USLMDeviceSubsystemEngine>();
	DeviceIndex = Subsystem->AddDevice(DeviceSettings);
}

void USLMDeviceComponentEngine::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Subsystem->RemoveDevice(DeviceIndex);
	Super::EndPlay(EndPlayReason);
}

void USLMDeviceSubsystemEngine::OnWorldBeginPlay(UWorld& InWorld)
{
	DomainRotation = GetWorld()->GetSubsystem<USLMDomainRotation>();
	DomainSignal = GetWorld()->GetSubsystem<USLMDomainSignal>();
	DomainAir = GetWorld()->GetSubsystem<USLMDomainAir>();
	Super::OnWorldBeginPlay(InWorld);
}

void USLMDeviceSubsystemEngine::PreSimulate(const float DeltaTime)
{
}

void USLMDeviceSubsystemEngine::Simulate(const float DeltaTime)
{
	for (const auto& Model : DeviceModels)
	{
		const FSLMDataRotation Crank = DomainRotation->GetData(Model.Index_Rotation_Crankshaft);
		const FSLMDataAir Intake = DomainAir->GetByPortIndex(Model.Index_Air_Intake);
		const FSLMDataAir Exhaust = DomainAir->GetByPortIndex(Model.Index_Air_Exhaust);
		const float Throttle = DomainSignal->ReadByPortIndex(Model.Index_Signal_Throttle);
		
		const float PressureDifference = Intake.Pressure_bar - Exhaust.Pressure_bar;
		const float PumpingTorque = PressureDifference * Model.DisplacementPerRev * SLMOneOverTwoPi;

		const float RotationDelta = Crank.AngularVelocity * DeltaTime;
		const float LitersIngested = FMath::Clamp(Throttle, 0, 1) * Model.DisplacementPerRev * FMath::Abs(RotationDelta);
		const bool bIsNormalDirection = Crank.AngularVelocity >= 0.0;
		const int32 FromPort = bIsNormalDirection ? Model.Index_Air_Intake : Model.Index_Air_Exhaust;
		const int32 ToPort = bIsNormalDirection ? Model.Index_Air_Exhaust : Model.Index_Air_Intake;
		
		FSLMDataAir Charge = DomainAir->RemoveAir(FromPort, LitersIngested);
		const float OxygenGrams = Charge.GetMassGrams() * Charge.OxygenRatio;
		const float FuelGrams = OxygenGrams * SLMFuelPerAirGrams;																			//TODO Add fuel consumption
		const float CombustionEnergy = SLMFuelJoulesPerGram * FuelGrams;
		const float CombustionWork = CombustionEnergy * Model.Efficiency;
		const float CombustionTorque = CombustionWork / RotationDelta;
		Charge.AddHeatJoules(CombustionEnergy - CombustionWork);
		DomainAir->AddAir(ToPort, Charge);

		const float TotalTorque = PumpingTorque + CombustionTorque;
		const float CrankRPS_Out = Crank.AngularVelocity + (TotalTorque * DeltaTime) / Crank.MomentOfInertia;
		DomainRotation->SetAngularVelocity(Model.Index_Rotation_Crankshaft, CrankRPS_Out);
	}
}

void USLMDeviceSubsystemEngine::PostSimulate(const float DeltaTime)
{
}

int32 USLMDeviceSubsystemEngine::AddDevice(FSLMDeviceEngine Device)
{
	Device.DeviceModel.Index_Rotation_Crankshaft = DomainRotation->AddPort(Device.Port_Rotation_Crankshaft);
	Device.DeviceModel.Index_Signal_Throttle = DomainSignal->AddPort(Device.Port_Signal_Throttle);
	Device.DeviceModel.Index_Air_Intake = DomainAir->AddPort(Device.Port_Air_Intake);
	Device.DeviceModel.Index_Air_Exhaust = DomainAir->AddPort(Device.Port_Air_Exhaust);
	return DeviceModels.Add(Device.DeviceModel);
}

void USLMDeviceSubsystemEngine::RemoveDevice(const int32 DeviceIndex)
{
	DomainRotation->RemovePort(DeviceModels[DeviceIndex].Index_Rotation_Crankshaft);
	DomainSignal->RemovePort(DeviceModels[DeviceIndex].Index_Signal_Throttle);
	DomainAir->RemovePort(DeviceModels[DeviceIndex].Index_Air_Intake);
	DomainAir->RemovePort(DeviceModels[DeviceIndex].Index_Air_Exhaust);
	DeviceModels.RemoveAt(DeviceIndex);
}

FSLMDeviceModelEngine USLMDeviceSubsystemEngine::GetDeviceState(const int32 DeviceIndex)
{
	return DeviceModels[DeviceIndex];
}
*/
