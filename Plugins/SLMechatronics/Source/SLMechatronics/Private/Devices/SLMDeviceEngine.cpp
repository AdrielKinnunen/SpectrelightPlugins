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
	for (const auto& Engine : DeviceModels)
	{
		//Get
		const FSLMDataRotation Crank = DomainRotation->GetByPortIndex(Engine.Index_Rotation_Crankshaft);
		const float Throttle = DomainSignal->ReadByPortIndex(Engine.Index_Signal_Throttle);
		
		//Thermodynamics
		const float LitersIngested = FMath::Clamp(Throttle, 0, 1) * Engine.DisplacementPerRev * Crank.RPS * DeltaTime;
		FSLMDataAir Charge = DomainAir->RemoveAir(Engine.Index_Air_Intake, LitersIngested);
		const float OxygenGrams = Charge.GetMassGrams() * Charge.OxygenRatio;
		const float FuelGrams = OxygenGrams * FuelPerAirGrams;
		const float CombustionEnergy = FuelJoulesPerGram * FuelGrams;
		const float Work = CombustionEnergy * Engine.Efficiency;
		Charge.AddHeatJoules(CombustionEnergy - Work);

		//Mechanics
		const float CrankMomentum = Crank.RPS * Crank.MOI;
		const float CrankMomentum_Out = CrankMomentum + Work / Crank.RPS * DeltaTime;
		const float CrankRPM_Out = CrankMomentum_Out / Crank.MOI;

		//Set
		DomainRotation->SetNetworkAngVel(Engine.Index_Rotation_Crankshaft, CrankRPM_Out);
		DomainAir->AddAir(Engine.Index_Air_Exhaust, Charge);

		//torque = pressure x volume / 2pi
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
