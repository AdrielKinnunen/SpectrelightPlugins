// Copyright Spectrelight Studios, LLC

#include "Devices/SLMDeviceGearbox.h"

FSLMDeviceModelGearbox USLMDeviceComponentGearbox::GetDeviceState() const
{
	return Subsystem->GetDeviceState(DeviceIndex);
}

void USLMDeviceComponentGearbox::BeginPlay()
{
	Super::BeginPlay();
	const AActor* OwningActor = GetOwner();
	DeviceSettings.Port_Rotation_Input.PortMetaData.AssociatedActor = OwningActor;
	DeviceSettings.Port_Rotation_Output.PortMetaData.AssociatedActor = OwningActor;
	DeviceSettings.Port_Signal_GearRatio.PortMetaData.AssociatedActor = OwningActor;
	
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

void USLMDeviceSubsystemGearbox::Simulate(const float DeltaTime)
{
	for (auto& Device : DeviceModels)
	{
		const FSLMDataRotation In = DomainRotation->GetData(Device.Index_Rotation_Input);
		const FSLMDataRotation Out = DomainRotation->GetData(Device.Index_Rotation_Output);

		Device.GearRatio = DomainSignal->ReadByPortIndex(Device.Index_Signal_GearRatio);

		if (!FMath::IsNearlyZero(Device.GearRatio))
		{
			const float MOIShaftInputEffective = Device.GearRatio * Device.GearRatio * In.MomentOfInertia;
			const float AngVelShaftInputEffective = In.AngularVelocity / Device.GearRatio;

			const float OutAngVel = (AngVelShaftInputEffective * MOIShaftInputEffective + Out.AngularVelocity * Out.MomentOfInertia) / (MOIShaftInputEffective + Out.MomentOfInertia);
			const float InAngVel = OutAngVel * Device.GearRatio;

			DomainRotation->SetAngularVelocity(Device.Index_Rotation_Input, InAngVel);
			DomainRotation->SetAngularVelocity(Device.Index_Rotation_Output, OutAngVel);
		}
	}
}

void USLMDeviceSubsystemGearbox::PostSimulate(const float DeltaTime)
{
}

int32 USLMDeviceSubsystemGearbox::AddDevice(FSLMDeviceGearbox Device)
{
	Device.DeviceModel.Index_Rotation_Input = DomainRotation->AddPort(Device.Port_Rotation_Input);
	Device.DeviceModel.Index_Rotation_Output = DomainRotation->AddPort(Device.Port_Rotation_Output);
	Device.DeviceModel.Index_Signal_GearRatio = DomainSignal->AddPort(Device.Port_Signal_GearRatio);
	return DeviceModels.Add(Device.DeviceModel);
}

void USLMDeviceSubsystemGearbox::RemoveDevice(const int32 DeviceIndex)
{
	DomainRotation->RemovePort(DeviceModels[DeviceIndex].Index_Rotation_Input);
	DomainRotation->RemovePort(DeviceModels[DeviceIndex].Index_Rotation_Output);
	DomainSignal->RemovePort(DeviceModels[DeviceIndex].Index_Signal_GearRatio);
	
	DeviceModels.RemoveAt(DeviceIndex);
}

FSLMDeviceModelGearbox USLMDeviceSubsystemGearbox::GetDeviceState(const int32 DeviceIndex)
{
	return DeviceModels[DeviceIndex];
}