// Copyright Spectrelight Studios, LLC

#include "Devices/SLMDeviceMotor.h"

FSLMDeviceModelMotor USLMDeviceComponentMotor::GetDeviceState()
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

void USLMDeviceSubsystemMotor::Simulate(const float DeltaTime, const float SubstepScalar)
{
    for (const auto& Model : DeviceModels)
    {
        const FSLMDataRotation Crank = DomainRotation->GetData(Model.Index_Rotation_Crankshaft);
        const FSLMDataElectricity Electricity = DomainElectricity->GetByPortIndex(Model.Index_Electricity);
        const float Throttle = FMath::Clamp(DomainSignal->ReadByPortIndex(Model.Index_Signal_Throttle), -1.0, 1.0);

        const float AngVelForTorqueCalculation = FMath::Max(FMath::Abs(Crank.AngularVelocity), Model.ConstantTorqueAngVel);
        const float MaxTorque = Model.MaxPowerWatts / AngVelForTorqueCalculation;
        const float TorqueDemand = Throttle * MaxTorque;

        //Motoring = Negative energy transfer
        //Generating = positive energy transfer
        const float MinEnergyTransfer = -Electricity.StoredJoules;
        const float MaxEnergyTransfer = Electricity.CapacityJoules - Electricity.StoredJoules;

        //TODO: Incorporate efficiency
        const float EnergyDemandMech = -1 * TorqueDemand * DeltaTime * Crank.AngularVelocity;
        const float EnergyDemandClamped = FMath::Clamp(EnergyDemandMech, MinEnergyTransfer, MaxEnergyTransfer);

        float TorqueMultiplier = 1.0;
        if (!FMath::IsNearlyZero(EnergyDemandMech))
        {
            TorqueMultiplier = EnergyDemandClamped / EnergyDemandMech;
        }
        const float ActualTorque = TorqueDemand * TorqueMultiplier;
        const float CrankRPS_Out = Crank.AngularVelocity + ActualTorque * DeltaTime / Crank.MomentOfInertia;

        DomainRotation->SetAngularVelocity(Model.Index_Rotation_Crankshaft, CrankRPS_Out);
        DomainElectricity->SetJoulesByPortIndex(Model.Index_Electricity, Electricity.StoredJoules + EnergyDemandClamped);
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
