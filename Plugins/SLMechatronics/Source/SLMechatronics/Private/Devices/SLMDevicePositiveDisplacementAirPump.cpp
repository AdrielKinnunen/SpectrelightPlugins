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
        const FSLMDataRotation Crank = DomainRotation->GetData(Model.Index_Rotation_Crankshaft);
        const bool bIsNormalDirection = Crank.AngularVelocity >= 0.0;
        const int32 FromPort = bIsNormalDirection ? Model.Index_Air_Intake : Model.Index_Air_Exhaust;
        const int32 ToPort = bIsNormalDirection ? Model.Index_Air_Exhaust : Model.Index_Air_Intake;
    	FSLMDataAir Intake = DomainAir->GetData(FromPort);
    	FSLMDataAir Exhaust = DomainAir->GetData(ToPort);

    	const float LitersMoved = Model.DisplacementPerRev * FMath::Abs(Crank.AngularVelocity) * DeltaTime * SLMRadToRev;

    	const float IntakeOriginalVolume = Intake.Volume_l;
    	//const float IntakeEnergyBeforeExpansion = Intake.GetInternalEnergy();
    	Intake.CompressOrExpandToVolume(Intake.Volume_l + LitersMoved);
    	//const float IntakeWork = Intake.GetInternalEnergy() - IntakeEnergyBeforeExpansion;
    	FSLMDataAir Charge = Intake;
    	Intake.Volume_l = IntakeOriginalVolume;
    	Charge.Volume_l = LitersMoved;					//Intake and Charge should be in good states at this point

    	const float ExhaustOriginalVolume = Exhaust.Volume_l;
    	//const float ExhaustEnergyBeforeCompression = Exhaust.GetInternalEnergy();
    	Charge.CompressOrExpandToPressure(Exhaust.Pressure_bar);
    	Exhaust.MixWith(Charge);
    	Exhaust.CompressOrExpandToVolume(ExhaustOriginalVolume);
    	//float ExhaustWork = Exhaust.GetInternalEnergy() - ExhaustEnergyBeforeCompression;

    	DomainAir->SetData(FromPort, Intake);
    	DomainAir->SetData(ToPort, Exhaust);
    	const float PumpingTorque = (Intake.Pressure_bar - Exhaust.Pressure_bar) * Model.DisplacementPerRev * SLMRadToRev;
    	DomainRotation->AddTorque(Model.Index_Rotation_Crankshaft, PumpingTorque, DeltaTime);
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