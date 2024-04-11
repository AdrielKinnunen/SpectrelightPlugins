// Copyright Spectrelight Studios, LLC

#include "Devices/SLMDeviceClutch.h"

FSLMDeviceModelClutch USLMDeviceComponentClutch::GetDeviceState()
{
    return Subsystem->GetDeviceState(DeviceIndex);
}

void USLMDeviceComponentClutch::BeginPlay()
{
    Super::BeginPlay();
    const AActor* OwningActor = GetOwner();
    DeviceSettings.Port_Rotation_Input.PortMetaData.AssociatedActor = OwningActor;
    DeviceSettings.Port_Rotation_Output.PortMetaData.AssociatedActor = OwningActor;
    DeviceSettings.Port_Signal_Engage.PortMetaData.AssociatedActor = OwningActor;

    Subsystem = GetWorld()->GetSubsystem<USLMDeviceSubsystemClutch>();
    DeviceIndex = Subsystem->AddDevice(DeviceSettings);
}

void USLMDeviceComponentClutch::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Subsystem->RemoveDevice(DeviceIndex);
    Super::EndPlay(EndPlayReason);
}

void USLMDeviceSubsystemClutch::OnWorldBeginPlay(UWorld& InWorld)
{
    DomainRotation = GetWorld()->GetSubsystem<USLMDomainRotation>();
    DomainSignal = GetWorld()->GetSubsystem<USLMDomainSignal>();
    Super::OnWorldBeginPlay(InWorld);
}

void USLMDeviceSubsystemClutch::PreSimulate(const float DeltaTime)
{
}

void USLMDeviceSubsystemClutch::Simulate(const float DeltaTime, const float SubstepScalar)
{
    for (auto& Device : DeviceModels)
    {
        const FSLMDataRotation In = DomainRotation->GetData(Device.Index_Rotation_Input);
        const FSLMDataRotation Out = DomainRotation->GetData(Device.Index_Rotation_Output);
        const float EngageSignal = DomainSignal->ReadByPortIndex(Device.Index_Signal_Engage);
        
        const float TorqueLimit = Device.MaxTorque * FMath::Clamp(EngageSignal, 0, 1);
        const float MaxImpulseThisIteration = TorqueLimit * DeltaTime;
        const float DesiredAngVel = (In.AngularVelocity * In.MomentOfInertia + Out.AngularVelocity * Out.MomentOfInertia) / (In.MomentOfInertia + Out.MomentOfInertia);
        const float InDesiredImpulse = In.MomentOfInertia * (DesiredAngVel - In.AngularVelocity);
        const float OutDesiredImpulse = Out.MomentOfInertia * (DesiredAngVel - Out.AngularVelocity);
        const float InClampedImpulse = FMath::Clamp(InDesiredImpulse, -MaxImpulseThisIteration, MaxImpulseThisIteration);
        const float OutClampedImpulse = FMath::Clamp(OutDesiredImpulse, -MaxImpulseThisIteration, MaxImpulseThisIteration);
        const float InAngVel = In.AngularVelocity + InClampedImpulse / In.MomentOfInertia;
        const float OutAngVel = Out.AngularVelocity + OutClampedImpulse / Out.MomentOfInertia; 
        
        DomainRotation->SetAngularVelocity(Device.Index_Rotation_Input, InAngVel);
        DomainRotation->SetAngularVelocity(Device.Index_Rotation_Output, OutAngVel);
    }
}

void USLMDeviceSubsystemClutch::PostSimulate(const float DeltaTime)
{
}

int32 USLMDeviceSubsystemClutch::AddDevice(FSLMDeviceClutch Device)
{
    Device.DeviceModel.Index_Rotation_Input = DomainRotation->AddPort(Device.Port_Rotation_Input);
    Device.DeviceModel.Index_Rotation_Output = DomainRotation->AddPort(Device.Port_Rotation_Output);
    Device.DeviceModel.Index_Signal_Engage = DomainSignal->AddPort(Device.Port_Signal_Engage);

    return DeviceModels.Add(Device.DeviceModel);
}

void USLMDeviceSubsystemClutch::RemoveDevice(const int32 DeviceIndex)
{
    DomainRotation->RemovePort(DeviceModels[DeviceIndex].Index_Rotation_Input);
    DomainRotation->RemovePort(DeviceModels[DeviceIndex].Index_Rotation_Output);
    DomainSignal->RemovePort(DeviceModels[DeviceIndex].Index_Signal_Engage);

    DeviceModels.RemoveAt(DeviceIndex);
}

FSLMDeviceModelClutch USLMDeviceSubsystemClutch::GetDeviceState(const int32 DeviceIndex)
{
    return DeviceModels[DeviceIndex];
}