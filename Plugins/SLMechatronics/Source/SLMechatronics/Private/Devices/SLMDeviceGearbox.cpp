// Copyright Spectrelight Studios, LLC

#include "Devices/SLMDeviceGearbox.h"

FSLMDeviceModelGearbox USLMDeviceComponentGearbox::GetDeviceState()
{
    return Subsystem->GetDeviceState(DeviceIndex);
}

void USLMDeviceComponentGearbox::BindToOnChangedGear(const FSLMEvent& Delegate)
{
    Subsystem->BindToOnChangedGear(DeviceIndex, Delegate);
}

void USLMDeviceComponentGearbox::BeginPlay()
{
    Super::BeginPlay();
    const AActor* OwningActor = GetOwner();
    DeviceSettings.Port_Rotation_Input.PortMetaData.AssociatedActor = OwningActor;
    DeviceSettings.Port_Rotation_Output.PortMetaData.AssociatedActor = OwningActor;
    DeviceSettings.Port_Signal_Shift.PortMetaData.AssociatedActor = OwningActor;

    Subsystem = GetWorld()->GetSubsystem<USLMDeviceSubsystemGearbox>();
    DeviceIndex = Subsystem->AddDevice(DeviceSettings);
}

void USLMDeviceComponentGearbox::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Subsystem->RemoveDevice(DeviceIndex);
    Super::EndPlay(EndPlayReason);
}

void USLMDeviceSubsystemGearbox::OnWorldBeginPlay(UWorld& InWorld)
{
    DomainRotation = GetWorld()->GetSubsystem<USLMDomainRotation>();
    DomainSignal = GetWorld()->GetSubsystem<USLMDomainSignal>();
    Super::OnWorldBeginPlay(InWorld);
}

void USLMDeviceSubsystemGearbox::PreSimulate(const float DeltaTime)
{
}

void USLMDeviceSubsystemGearbox::Simulate(const float DeltaTime, const int32 StepCount)
{
    for (auto& Device : DeviceModels)
    {
        const FSLMDataRotation In = DomainRotation->GetData(Device.Index_Rotation_Input);
        const FSLMDataRotation Out = DomainRotation->GetData(Device.Index_Rotation_Output);
        const float ShiftSignal = DomainSignal->ReadByPortIndex(Device.Index_Signal_Shift);
        bool bChangedGear = false;
        if (Device.LastSignalValue < 0.99 && ShiftSignal >= 0.99)
        {
            Device.CurrentGear = FMath::Clamp(Device.CurrentGear + 1, -1 * Device.NumReverseGears, Device.NumForwardGears);
            bChangedGear = true;
        }
        else if (Device.LastSignalValue > -0.99 && ShiftSignal <= -0.99)
        {
            Device.CurrentGear = FMath::Clamp(Device.CurrentGear - 1, -1 * Device.NumReverseGears, Device.NumForwardGears);
            bChangedGear = true;
        }
        Device.LastSignalValue = ShiftSignal;
        if (bChangedGear)
        {
            Device.GearRatio = FMath::Sign(Device.CurrentGear) * Device.FirstGearRatio * FMath::Pow(Device.RatioBetweenGears, Device.GearSpreadExponent * (1 - FMath::Abs(Device.CurrentGear)));
            Device.bChangedGear = true;
        }

        const FSLMDataRotation InApparent = In.GetApparentStateThroughGearRatio(Device.GearRatio);

        if (!FMath::IsNearlyZero(Device.GearRatio))
        {
            const float OutAngVel = (InApparent.AngularVelocity * InApparent.MomentOfInertia + Out.AngularVelocity * Out.MomentOfInertia) / (InApparent.MomentOfInertia + Out.MomentOfInertia);
            const float InAngVel = OutAngVel * Device.GearRatio;

            DomainRotation->SetAngularVelocity(Device.Index_Rotation_Input, InAngVel);
            DomainRotation->SetAngularVelocity(Device.Index_Rotation_Output, OutAngVel);
        }
    }
}

void USLMDeviceSubsystemGearbox::PostSimulate(const float DeltaTime)
{
    for (auto& Device : DeviceModels)
    {
        if (Device.bChangedGear)
        {
            Device.OnChangedGear.ExecuteIfBound();
        }
        Device.bChangedGear = false;
    }
}

int32 USLMDeviceSubsystemGearbox::AddDevice(FSLMDeviceGearbox Device)
{
    Device.DeviceModel.Index_Rotation_Input = DomainRotation->AddPort(Device.Port_Rotation_Input);
    Device.DeviceModel.Index_Rotation_Output = DomainRotation->AddPort(Device.Port_Rotation_Output);
    Device.DeviceModel.Index_Signal_Shift = DomainSignal->AddPort(Device.Port_Signal_Shift);

    return DeviceModels.Add(Device.DeviceModel);
}

void USLMDeviceSubsystemGearbox::RemoveDevice(const int32 DeviceIndex)
{
    DomainRotation->RemovePort(DeviceModels[DeviceIndex].Index_Rotation_Input);
    DomainRotation->RemovePort(DeviceModels[DeviceIndex].Index_Rotation_Output);
    DomainSignal->RemovePort(DeviceModels[DeviceIndex].Index_Signal_Shift);

    DeviceModels.RemoveAt(DeviceIndex);
}

FSLMDeviceModelGearbox USLMDeviceSubsystemGearbox::GetDeviceState(const int32 DeviceIndex)
{
    return DeviceModels[DeviceIndex];
}

void USLMDeviceSubsystemGearbox::BindToOnChangedGear(const int32 DeviceIndex, const FSLMEvent& Delegate)
{
    DeviceModels[DeviceIndex].OnChangedGear = Delegate;
}
