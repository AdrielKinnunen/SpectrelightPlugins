// Copyright Spectrelight Studios, LLC

#include "Devices/SLMDevicePositiveDisplacementAirPump.h"

FSLMDeviceModelPositiveDisplacementAirPump USLMDeviceComponentPositiveDisplacementAirPump::GetDeviceState()
{
    return Subsystem->GetDeviceState(DeviceIndex);
}

void USLMDeviceComponentPositiveDisplacementAirPump::BeginPlay()
{
    Super::BeginPlay();
    const AActor* OwningActor = GetOwner();
    DeviceSettings.Port_Rotation_Crankshaft.PortMetaData.AssociatedActor = OwningActor;
    DeviceSettings.Port_Air_Intake.PortMetaData.AssociatedActor = OwningActor;
    DeviceSettings.Port_Air_Exhaust.PortMetaData.AssociatedActor = OwningActor;

    Subsystem = GetWorld()->GetSubsystem<USLMDeviceSubsystemPositiveDisplacementAirPump>();
    DeviceIndex = Subsystem->AddDevice(DeviceSettings);
}

void USLMDeviceComponentPositiveDisplacementAirPump::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Subsystem->RemoveDevice(DeviceIndex);
    Super::EndPlay(EndPlayReason);
}

void USLMDeviceSubsystemPositiveDisplacementAirPump::OnWorldBeginPlay(UWorld& InWorld)
{
    DomainRotation = GetWorld()->GetSubsystem<USLMDomainRotation>();
    DomainAir = GetWorld()->GetSubsystem<USLMDomainAir>();
    Super::OnWorldBeginPlay(InWorld);
}

void USLMDeviceSubsystemPositiveDisplacementAirPump::PreSimulate(const float DeltaTime)
{
}

void USLMDeviceSubsystemPositiveDisplacementAirPump::Simulate(const float DeltaTime, const float SubstepScalar)
{
    for (const auto& Model : DeviceModels)
    {
    	//Setup
    	const FSLMDataRotation Crank = DomainRotation->GetData(Model.Index_Rotation_Crankshaft);
    	const bool bIsNormalDirection = Crank.AngularVelocity >= 0.0;
    	const int32 FromPort = bIsNormalDirection ? Model.Index_Air_Intake : Model.Index_Air_Exhaust;
    	const int32 ToPort = bIsNormalDirection ? Model.Index_Air_Exhaust : Model.Index_Air_Intake;
    	const float DirectionScalar = bIsNormalDirection ? 1.0 : -1.0;
    	FSLMDataAir Intake = DomainAir->GetData(FromPort);
    	FSLMDataAir Exhaust = DomainAir->GetData(ToPort);
    	//Air Transfer
    	const float VolumeMoved = Model.DisplacementPerRev * FMath::Abs(Crank.AngularVelocity) * DeltaTime * SLMRadToRev;
    	const float ExpansionRatio = (Intake.Volume + VolumeMoved) / Intake.Volume;
    	const float TransferRatio = VolumeMoved / (Intake.Volume + VolumeMoved);
    	const float MolesMoved = Intake.Moles * TransferRatio;
    	const float EnergyMoved = Intake.Energy * TransferRatio;
    	//Torque and Work, work is always work DONE on the shaft
    	const float PressureDifference = Intake.GetPressure() - Exhaust.GetPressure();
    	const float ShaftTorque = PressureDifference * Model.DisplacementPerRev * SLMRadToRev;
    	const float ShaftWork = VolumeMoved * PressureDifference;
    	const float IntakeWork = Intake.Energy * (1 - FMath::Pow(ExpansionRatio, 1 - SLMGammaAir));
    	const float HeatIntoExhaust = IntakeWork - ShaftWork;
    	//Update State
    	DomainAir->AddEnergyAndMoles(FromPort, -EnergyMoved - IntakeWork, -MolesMoved);
    	DomainAir->AddEnergyAndMoles(ToPort, EnergyMoved + HeatIntoExhaust, MolesMoved);
    	DomainRotation->AddTorque(Model.Index_Rotation_Crankshaft, ShaftTorque * DirectionScalar, DeltaTime);
    }
}

void USLMDeviceSubsystemPositiveDisplacementAirPump::PostSimulate(const float DeltaTime)
{
}

int32 USLMDeviceSubsystemPositiveDisplacementAirPump::AddDevice(FSLMDevicePositiveDisplacementAirPump Device)
{
    Device.DeviceModel.Index_Rotation_Crankshaft = DomainRotation->AddPort(Device.Port_Rotation_Crankshaft);
    Device.DeviceModel.Index_Air_Intake = DomainAir->AddPort(Device.Port_Air_Intake);
    Device.DeviceModel.Index_Air_Exhaust = DomainAir->AddPort(Device.Port_Air_Exhaust);
    return DeviceModels.Add(Device.DeviceModel);
}

void USLMDeviceSubsystemPositiveDisplacementAirPump::RemoveDevice(const int32 DeviceIndex)
{
    DomainRotation->RemovePort(DeviceModels[DeviceIndex].Index_Rotation_Crankshaft);
    DomainAir->RemovePort(DeviceModels[DeviceIndex].Index_Air_Intake);
    DomainAir->RemovePort(DeviceModels[DeviceIndex].Index_Air_Exhaust);
    DeviceModels.RemoveAt(DeviceIndex);
}

FSLMDeviceModelPositiveDisplacementAirPump USLMDeviceSubsystemPositiveDisplacementAirPump::GetDeviceState(const int32 DeviceIndex)
{
    return DeviceModels[DeviceIndex];
}