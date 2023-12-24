// Copyright Spectrelight Studios, LLC

#include "Devices/SLMDeviceMotor.h"

FSLMDeviceModelMotor USLMDeviceComponentMotor::GetDeviceState() const
{
	return Subsystem->GetDeviceState(DeviceIndex);
}

void USLMDeviceComponentMotor::BeginPlay()
{
	Super::BeginPlay();
	const AActor* OwningActor = GetOwner();
	DeviceSettings.Port_Electricity.PortMetaData.AssociatedActor = OwningActor;
	DeviceSettings.Port_Rotation_Crankshaft.PortMetaData.AssociatedActor = OwningActor;
	DeviceSettings.Port_Signal_Throttle.PortMetaData.AssociatedActor = OwningActor;

	Subsystem = GetWorld()->GetSubsystem<USLMDeviceSubsystemMotor>();
	DeviceIndex = Subsystem->AddDevice(DeviceSettings);
}

void USLMDeviceComponentMotor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Subsystem->RemoveDevice(DeviceIndex);
	Super::EndPlay(EndPlayReason);
}

void USLMDeviceSubsystemMotor::OnWorldBeginPlay(UWorld& InWorld)
{
	DomainRotation = GetWorld()->GetSubsystem<USLMDomainRotation>();
	DomainSignal = GetWorld()->GetSubsystem<USLMDomainSignal>();
	DomainElectricity = GetWorld()->GetSubsystem<USLMDomainElectricity>();
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
		const float MaxTorque = Model.MaxPowerkW / Model.ConstantTorqueRPS;
		const float TorqueDemand = Throttle * MaxTorque;
		const float InitialJoules = Electricity.StoredJoules;
		
		const float EnergyFlowDemand = -1 * TorqueDemand * DeltaTime * ((Crank.RPS > 0.0) ? 1.0 : -1.0);			//TODO: Better system than this, this sucks.
		const float NewJoules = FMath::Clamp(InitialJoules + EnergyFlowDemand, 0.0, Electricity.CapacityJoules);
		const float EnergyFlow = NewJoules - InitialJoules;
		
		float TorqueMultiplier = 0.0;
		if (!FMath::IsNearlyZero(EnergyFlowDemand))
		{
			TorqueMultiplier = EnergyFlow / EnergyFlowDemand;
		}
		const float ActualTorque = TorqueDemand * TorqueMultiplier;
		const float CrankRPS_Out = Crank.RPS + ActualTorque * DeltaTime / Crank.MOI;
		
		DomainRotation->SetAngVelByPortIndex(Model.Index_Rotation_Crankshaft, CrankRPS_Out);
		DomainElectricity->SetJoulesByPortIndex(Model.Index_Electricity, NewJoules);
	}
}

void USLMDeviceSubsystemMotor::PostSimulate(const float DeltaTime)
{
}

int32 USLMDeviceSubsystemMotor::AddDevice(FSLMDeviceMotor Device)
{
	Device.DeviceModel.Index_Rotation_Crankshaft = DomainRotation->AddPort(Device.Port_Rotation_Crankshaft);
	Device.DeviceModel.Index_Signal_Throttle = DomainSignal->AddPort(Device.Port_Signal_Throttle);
	Device.DeviceModel.Index_Electricity = DomainElectricity->AddPort(Device.Port_Electricity);
	return DeviceModels.Add(Device.DeviceModel);
}

void USLMDeviceSubsystemMotor::RemoveDevice(const int32 DeviceIndex)
{
	DomainRotation->RemovePort(DeviceModels[DeviceIndex].Index_Rotation_Crankshaft);
	DomainSignal->RemovePort(DeviceModels[DeviceIndex].Index_Signal_Throttle);
	DomainElectricity->RemovePort(DeviceModels[DeviceIndex].Index_Electricity);
	
	DeviceModels.RemoveAt(DeviceIndex);
}

FSLMDeviceModelMotor USLMDeviceSubsystemMotor::GetDeviceState(const int32 DeviceIndex)
{
	return DeviceModels[DeviceIndex];
}