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
		const FSLMDataRotation Crank = DomainRotation->GetByPortIndex(Model.Index_Rotation_Crankshaft);
		const FSLMDataAir Intake = DomainAir->GetByPortIndex(Model.Index_Air_Intake);
		const FSLMDataAir Exhaust = DomainAir->GetByPortIndex(Model.Index_Air_Exhaust);
		const float Throttle = DomainSignal->ReadByPortIndex(Model.Index_Signal_Throttle);
		
		const float PressureDifference = Intake.Pressure_bar - Exhaust.Pressure_bar;
		const float PumpingTorque = PressureDifference * Model.DisplacementPerRev * OneOverTwoPi;

		const float RotationDelta = Crank.RPS * DeltaTime;
		const float LitersIngested = FMath::Clamp(Throttle, 0, 1) * Model.DisplacementPerRev * FMath::Abs(RotationDelta);
		const bool bIsNormalDirection = Crank.RPS >= 0.0;
		const int32 FromPort = bIsNormalDirection ? Model.Index_Air_Intake : Model.Index_Air_Exhaust;
		const int32 ToPort = bIsNormalDirection ? Model.Index_Air_Exhaust : Model.Index_Air_Intake;
		
		FSLMDataAir Charge = DomainAir->RemoveAir(FromPort, LitersIngested);
		const float OxygenGrams = Charge.GetMassGrams() * Charge.OxygenRatio;
		const float FuelGrams = OxygenGrams * FuelPerAirGrams;																			//TODO Add fuel consumption
		const float CombustionEnergy = FuelJoulesPerGram * FuelGrams;
		const float CombustionWork = CombustionEnergy * Model.Efficiency;
		const float CombustionTorque = CombustionWork / RotationDelta;
		Charge.AddHeatJoules(CombustionEnergy - CombustionWork);
		DomainAir->AddAir(ToPort, Charge);

		const float TotalTorque = PumpingTorque + CombustionTorque;
		const float CrankRPS_Out = Crank.RPS + (TotalTorque * DeltaTime) / Crank.MOI;
		DomainRotation->SetNetworkAngVel(Model.Index_Rotation_Crankshaft, CrankRPS_Out);
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