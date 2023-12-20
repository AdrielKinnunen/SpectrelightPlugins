// Copyright Spectrelight Studios, LLC

#include "Devices/SLMDeviceMotor.h"

USLMDeviceComponentMotor::USLMDeviceComponentMotor()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void USLMDeviceComponentMotor::BeginPlay()
{
	Super::BeginPlay();
	GetWorld()->GetSubsystem<USLMDeviceSubsystemMotor>()->RegisterDeviceComponent(this);
}

void USLMDeviceComponentMotor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorld()->GetSubsystem<USLMDeviceSubsystemMotor>()->DeRegisterDeviceComponent(this);
	Super::EndPlay(EndPlayReason);
}

void USLMDeviceSubsystemMotor::OnWorldBeginPlay(UWorld& InWorld)
{
	DomainRotation = GetWorld()->GetSubsystem<USLMDomainRotation>();
	Super::OnWorldBeginPlay(InWorld);
}

void USLMDeviceSubsystemMotor::PreSimulate(const float DeltaTime)
{
}

void USLMDeviceSubsystemMotor::Simulate(const float DeltaTime)
{
	for (const auto& Model : DeviceModels)
	{
		const FSLMDataRotation Crank = DomainRotation->GetByPortIndex(Model.Index_Rotation_Crankshaft);
		const FSLMDataElectricity Electricity = DomainElectricity->GetByPortIndex(Model.Index_Electricity);
		const float Throttle = FMath::Clamp(DomainSignal->ReadByPortIndex(Model.Index_Signal_Throttle), -1.0, 1.0);

		const float MaxTorque = Model.MaxPowerkW / Model.ConstantTorqueRPS;			//max torque from engine stats
		const float AvailableTorque = FMath::Abs(Crank.RPS) < Model.ConstantTorqueRPS ? MaxTorque : FMath::Abs(Model.MaxPowerkW / Crank.RPS);			//constant torque or power regime?
		const float TorqueDemand = Throttle * AvailableTorque;			//how much torque we want?
		const float EnergyFlowDemand = TorqueDemand * Crank.RPS * DeltaTime;		//how much energy does that require? Does 0 make sense?
		const float EnergyFlow = FMath::Clamp(EnergyFlowDemand, Electricity.StoredJoules, Electricity.StoredJoules - Electricity.CapacityJoules);		//how much energy or room for energy do we have?
		const float ActualTorque = EnergyFlow / (Crank.RPS * DeltaTime);			//friggin divide by zero again


		
		const float CrankRPS_Out = Crank.RPS + ActualTorque * DeltaTime / Crank.MOI;
		DomainRotation->SetNetworkAngVel(Model.Index_Rotation_Crankshaft, CrankRPS_Out);
	}
}

void USLMDeviceSubsystemMotor::PostSimulate(const float DeltaTime)
{
}

void USLMDeviceSubsystemMotor::RegisterDeviceComponent(USLMDeviceComponentMotor* DeviceComponent)
{
	const auto Index = AddDevice(DeviceComponent->DeviceSettings);
	DeviceComponent->DeviceIndex = Index;
	DeviceComponents.Insert(Index, DeviceComponent);
}

void USLMDeviceSubsystemMotor::DeRegisterDeviceComponent(const USLMDeviceComponentMotor* DeviceComponent)
{
	const auto Index = DeviceComponent->DeviceIndex;
	RemoveDevice(Index);
	DeviceComponents.RemoveAt(Index);
}

int32 USLMDeviceSubsystemMotor::AddDevice(FSLMDeviceMotor Device)
{
	Device.DeviceModel.Index_Rotation_Crankshaft = DomainRotation->AddPort(Device.Port_Rotation_Crankshaft);
	Device.DeviceModel.Index_Signal_Throttle = DomainSignal->AddPort(Device.Port_Signal_Throttle);
	return DeviceModels.Add(Device.DeviceModel);
}

void USLMDeviceSubsystemMotor::RemoveDevice(const int32 DeviceIndex)
{
	DomainRotation->RemovePort(DeviceModels[DeviceIndex].Index_Rotation_Crankshaft);
	DomainSignal->RemovePort(DeviceModels[DeviceIndex].Index_Signal_Throttle);
	DeviceModels.RemoveAt(DeviceIndex);
}

FSLMDeviceModelMotor USLMDeviceSubsystemMotor::GetDeviceState(const int32 DeviceIndex)
{
	return DeviceModels[DeviceIndex];
}
