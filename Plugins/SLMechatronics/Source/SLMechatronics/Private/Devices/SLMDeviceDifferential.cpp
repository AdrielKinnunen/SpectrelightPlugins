// Copyright Spectrelight Studios, LLC

#include "Devices/SLMDeviceDifferential.h"

FSLMDeviceModelDifferential USLMDeviceComponentDifferential::GetDeviceState() const
{
	return Subsystem->GetDeviceState(DeviceIndex);
}

void USLMDeviceComponentDifferential::BeginPlay()
{
	Super::BeginPlay();
	const AActor* OwningActor = GetOwner();
	DeviceSettings.Port_Rotation_Input.PortMetaData.AssociatedActor = OwningActor;
	DeviceSettings.Port_Rotation_Left.PortMetaData.AssociatedActor = OwningActor;
	DeviceSettings.Port_Rotation_Right.PortMetaData.AssociatedActor = OwningActor;
	
	Subsystem = GetWorld()->GetSubsystem<USLMDeviceSubsystemDifferential>();
	DeviceIndex = Subsystem->AddDevice(DeviceSettings);
}

void USLMDeviceComponentDifferential::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Subsystem->RemoveDevice(DeviceIndex);
	Super::EndPlay(EndPlayReason);
}

void USLMDeviceSubsystemDifferential::OnWorldBeginPlay(UWorld& InWorld)
{
	DomainRotation = GetWorld()->GetSubsystem<USLMDomainRotation>();
	Super::OnWorldBeginPlay(InWorld);
}

void USLMDeviceSubsystemDifferential::PreSimulate(const float DeltaTime)
{
}

void USLMDeviceSubsystemDifferential::Simulate(const float DeltaTime)
{
	for (const auto Model : DeviceModels)
	{
		const auto [B,M] = DomainRotation->GetByPortIndex(Model.Index_Rotation_Input);
		const auto [C,N] = DomainRotation->GetByPortIndex(Model.Index_Rotation_Left);
		const auto [D,O] = DomainRotation->GetByPortIndex(Model.Index_Rotation_Right);
	
		const float Divisor = M*N + M*O + 4*N*O;

		const float LeftShaftVel_Out = (2*B*M*O + C*M*N + 4*C*N*O - D*M*O) / Divisor;
		const float RightShaftVel_Out = (2*B*M*N - C*M*N + D*M*O + 4*D*N*O) / Divisor;
		const float InputShaftVel_Out = 0.5 * (RightShaftVel_Out + LeftShaftVel_Out);

		DomainRotation->SetAngVelByPortIndex(Model.Index_Rotation_Input, InputShaftVel_Out);
		DomainRotation->SetAngVelByPortIndex(Model.Index_Rotation_Left, LeftShaftVel_Out);
		DomainRotation->SetAngVelByPortIndex(Model.Index_Rotation_Right, RightShaftVel_Out);
	}
}

void USLMDeviceSubsystemDifferential::PostSimulate(const float DeltaTime)
{
}

int32 USLMDeviceSubsystemDifferential::AddDevice(FSLMDeviceDifferential Device)
{
	Device.DeviceModel.Index_Rotation_Input = DomainRotation->AddPort(Device.Port_Rotation_Input);
	Device.DeviceModel.Index_Rotation_Left = DomainRotation->AddPort(Device.Port_Rotation_Left);
	Device.DeviceModel.Index_Rotation_Right = DomainRotation->AddPort(Device.Port_Rotation_Right);
	return DeviceModels.Add(Device.DeviceModel);
}

void USLMDeviceSubsystemDifferential::RemoveDevice(const int32 DeviceIndex)
{
	DomainRotation->RemovePort(DeviceModels[DeviceIndex].Index_Rotation_Input);
	DomainRotation->RemovePort(DeviceModels[DeviceIndex].Index_Rotation_Left);
	DomainRotation->RemovePort(DeviceModels[DeviceIndex].Index_Rotation_Right);
	DeviceModels.RemoveAt(DeviceIndex);
}

FSLMDeviceModelDifferential USLMDeviceSubsystemDifferential::GetDeviceState(const int32 DeviceIndex)
{
	return DeviceModels[DeviceIndex];
}