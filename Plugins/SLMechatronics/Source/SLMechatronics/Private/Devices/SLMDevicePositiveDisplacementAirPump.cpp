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
        const FSLMDataAir Intake = DomainAir->GetData(Model.Index_Air_Intake);
        const FSLMDataAir Exhaust = DomainAir->GetData(Model.Index_Air_Exhaust);

        const float PressureDifference = Intake.Pressure_bar - Exhaust.Pressure_bar;
        const float PumpingTorque = PressureDifference * Model.DisplacementPerRev * SLMOneOverTwoPi;

        const float LitersMoved = Model.DisplacementPerRev * FMath::Abs(Crank.AngularVelocity) * DeltaTime;
        const bool bIsNormalDirection = Crank.AngularVelocity >= 0.0;
        const int32 FromPort = bIsNormalDirection ? Model.Index_Air_Intake : Model.Index_Air_Exhaust;
        const int32 ToPort = bIsNormalDirection ? Model.Index_Air_Exhaust : Model.Index_Air_Intake;
        const FSLMDataAir Charge = DomainAir->RemoveAir(FromPort, LitersMoved);
        DomainAir->AddAir(ToPort, Charge);

        const float CrankRPS_Out = Crank.AngularVelocity + (PumpingTorque * DeltaTime) / Crank.MomentOfInertia;
        DomainRotation->SetAngularVelocity(Model.Index_Rotation_Crankshaft, CrankRPS_Out);
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
