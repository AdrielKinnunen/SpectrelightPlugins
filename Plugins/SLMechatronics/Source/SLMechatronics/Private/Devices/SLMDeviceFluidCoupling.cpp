// Copyright Spectrelight Studios, LLC

#include "Devices/SLMDeviceFluidCoupling.h"

FSLMDeviceModelFluidCoupling USLMDeviceComponentFluidCoupling::GetDeviceState()
{
    return Subsystem->GetDeviceState(DeviceIndex);
}

void USLMDeviceComponentFluidCoupling::BeginPlay()
{
    Super::BeginPlay();
    const AActor* OwningActor = GetOwner();
    DeviceSettings.Port_Rotation_Input.PortMetaData.AssociatedActor = OwningActor;
    DeviceSettings.Port_Rotation_Output.PortMetaData.AssociatedActor = OwningActor;

    Subsystem = GetWorld()->GetSubsystem<USLMDeviceSubsystemFluidCoupling>();
    DeviceIndex = Subsystem->AddDevice(DeviceSettings);
}

void USLMDeviceComponentFluidCoupling::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Subsystem->RemoveDevice(DeviceIndex);
    Super::EndPlay(EndPlayReason);
}

void USLMDeviceSubsystemFluidCoupling::OnWorldBeginPlay(UWorld& InWorld)
{
    DomainRotation = GetWorld()->GetSubsystem<USLMDomainRotation>();
    Super::OnWorldBeginPlay(InWorld);
}

void USLMDeviceSubsystemFluidCoupling::PreSimulate(const float DeltaTime)
{
}

void USLMDeviceSubsystemFluidCoupling::Simulate(const float DeltaTime, const float SubstepScalar)
{
    for (auto& Device : DeviceModels)
    {
        const FSLMDataRotation In = DomainRotation->GetData(Device.Index_Rotation_Input);
        const FSLMDataRotation Out = DomainRotation->GetData(Device.Index_Rotation_Output);

    	const float AngularDifference = FMath::Abs(In.AngularVelocity - Out.AngularVelocity);
        const float TorqueLimit = Device.TorquePerAngularDifference * AngularDifference;
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

void USLMDeviceSubsystemFluidCoupling::PostSimulate(const float DeltaTime)
{
}

int32 USLMDeviceSubsystemFluidCoupling::AddDevice(FSLMDeviceFluidCoupling Device)
{
    Device.DeviceModel.Index_Rotation_Input = DomainRotation->AddPort(Device.Port_Rotation_Input);
    Device.DeviceModel.Index_Rotation_Output = DomainRotation->AddPort(Device.Port_Rotation_Output);

    return DeviceModels.Add(Device.DeviceModel);
}

void USLMDeviceSubsystemFluidCoupling::RemoveDevice(const int32 DeviceIndex)
{
    DomainRotation->RemovePort(DeviceModels[DeviceIndex].Index_Rotation_Input);
    DomainRotation->RemovePort(DeviceModels[DeviceIndex].Index_Rotation_Output);

    DeviceModels.RemoveAt(DeviceIndex);
}

FSLMDeviceModelFluidCoupling USLMDeviceSubsystemFluidCoupling::GetDeviceState(const int32 DeviceIndex)
{
    return DeviceModels[DeviceIndex];
}