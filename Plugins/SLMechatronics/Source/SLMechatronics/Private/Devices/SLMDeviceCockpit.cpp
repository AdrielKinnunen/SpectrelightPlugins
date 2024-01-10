// Copyright Spectrelight Studios, LLC

#include "Devices/SLMDeviceCockpit.h"

FSLMDeviceModelCockpit USLMDeviceComponentCockpit::GetDeviceState()
{
    return Subsystem->GetDeviceState(DeviceIndex);
}

void USLMDeviceComponentCockpit::SetCockpitValues(FSLMCockpitValues Values)
{
    Subsystem->SetCockpitValues(DeviceIndex, Values);
}

void USLMDeviceSubsystemCockpit::SetCockpitValues(const int32 DeviceIndex, FSLMCockpitValues Values)
{
    DeviceModels[DeviceIndex].Values = Values;
}

void USLMDeviceComponentCockpit::BeginPlay()
{
    Super::BeginPlay();
    const AActor* OwningActor = GetOwner();
    DeviceSettings.Port_Signal_Throttle.PortMetaData.AssociatedActor = OwningActor;
    DeviceSettings.Port_Signal_Brake.PortMetaData.AssociatedActor = OwningActor;
    DeviceSettings.Port_Signal_Steer.PortMetaData.AssociatedActor = OwningActor;
    DeviceSettings.Port_Signal_Shift.PortMetaData.AssociatedActor = OwningActor;
    DeviceSettings.Port_Signal_Fire.PortMetaData.AssociatedActor = OwningActor;

    Subsystem = GetWorld()->GetSubsystem<USLMDeviceSubsystemCockpit>();
    DeviceIndex = Subsystem->AddDevice(DeviceSettings);
}

void USLMDeviceComponentCockpit::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Subsystem->RemoveDevice(DeviceIndex);
    Super::EndPlay(EndPlayReason);
}

void USLMDeviceSubsystemCockpit::OnWorldBeginPlay(UWorld& InWorld)
{
    DomainSignal = GetWorld()->GetSubsystem<USLMDomainSignal>();
    Super::OnWorldBeginPlay(InWorld);
}

void USLMDeviceSubsystemCockpit::PreSimulate(const float DeltaTime)
{
}

void USLMDeviceSubsystemCockpit::Simulate(const float DeltaTime, const int32 StepCount)
{
    for (const auto& Device : DeviceModels)
    {
        DomainSignal->WriteByPortIndex(Device.Index_Signal_Throttle, Device.Values.Throttle);
        DomainSignal->WriteByPortIndex(Device.Index_Signal_Brake, Device.Values.Brake);
        DomainSignal->WriteByPortIndex(Device.Index_Signal_Steer, Device.Values.Steer);
        DomainSignal->WriteByPortIndex(Device.Index_Signal_Shift, Device.Values.Shift);
        DomainSignal->WriteByPortIndex(Device.Index_Signal_Fire, Device.Values.Fire);
    }
}

void USLMDeviceSubsystemCockpit::PostSimulate(const float DeltaTime)
{
}

int32 USLMDeviceSubsystemCockpit::AddDevice(FSLMDeviceCockpit Device)
{
    Device.DeviceModel.Index_Signal_Throttle = DomainSignal->AddPort(Device.Port_Signal_Throttle);
    Device.DeviceModel.Index_Signal_Brake = DomainSignal->AddPort(Device.Port_Signal_Brake);
    Device.DeviceModel.Index_Signal_Steer = DomainSignal->AddPort(Device.Port_Signal_Steer);
    Device.DeviceModel.Index_Signal_Shift = DomainSignal->AddPort(Device.Port_Signal_Shift);
    Device.DeviceModel.Index_Signal_Fire = DomainSignal->AddPort(Device.Port_Signal_Fire);

    return DeviceModels.Add(Device.DeviceModel);
}

void USLMDeviceSubsystemCockpit::RemoveDevice(const int32 DeviceIndex)
{
    DomainSignal->RemovePort(DeviceModels[DeviceIndex].Index_Signal_Throttle);
    DomainSignal->RemovePort(DeviceModels[DeviceIndex].Index_Signal_Brake);
    DomainSignal->RemovePort(DeviceModels[DeviceIndex].Index_Signal_Steer);
    DomainSignal->RemovePort(DeviceModels[DeviceIndex].Index_Signal_Shift);
    DomainSignal->RemovePort(DeviceModels[DeviceIndex].Index_Signal_Fire);

    DeviceModels.RemoveAt(DeviceIndex);
}

FSLMDeviceModelCockpit USLMDeviceSubsystemCockpit::GetDeviceState(const int32 DeviceIndex)
{
    return DeviceModels[DeviceIndex];
}
