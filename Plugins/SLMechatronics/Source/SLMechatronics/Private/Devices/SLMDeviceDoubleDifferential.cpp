// Copyright Spectrelight Studios, LLC

#include "Devices/SLMDeviceDoubleDifferential.h"

FSLMDeviceModelDoubleDifferential USLMDeviceComponentDoubleDifferential::GetDeviceState() const
{
	return Subsystem->GetDeviceState(DeviceIndex);
}

void USLMDeviceComponentDoubleDifferential::BeginPlay()
{
	Super::BeginPlay();
	const AActor* OwningActor = GetOwner();
	DeviceSettings.Port_Rotation_Drive.PortMetaData.AssociatedActor = OwningActor;
	DeviceSettings.Port_Rotation_Steer.PortMetaData.AssociatedActor = OwningActor;
	DeviceSettings.Port_Rotation_Left.PortMetaData.AssociatedActor = OwningActor;
	DeviceSettings.Port_Rotation_Right.PortMetaData.AssociatedActor = OwningActor;
	
	Subsystem = GetWorld()->GetSubsystem<USLMDeviceSubsystemDoubleDifferential>();
	DeviceIndex = Subsystem->AddDevice(DeviceSettings);
}

void USLMDeviceComponentDoubleDifferential::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Subsystem->RemoveDevice(DeviceIndex);
	Super::EndPlay(EndPlayReason);
}

void USLMDeviceSubsystemDoubleDifferential::OnWorldBeginPlay(UWorld& InWorld)
{
	DomainRotation = GetWorld()->GetSubsystem<USLMDomainRotation>();
	Super::OnWorldBeginPlay(InWorld);
}

void USLMDeviceSubsystemDoubleDifferential::PreSimulate(const float DeltaTime)
{
}

void USLMDeviceSubsystemDoubleDifferential::Simulate(const float DeltaTime)
{
	for (const auto Model : DeviceModels)
	{
		const auto [A,L] = DomainRotation->GetData(Model.Index_Rotation_Drive);
		const auto [B,M] = DomainRotation->GetData(Model.Index_Rotation_Steer);
		const auto [C,N] = DomainRotation->GetData(Model.Index_Rotation_Left);
		const auto [D,O] = DomainRotation->GetData(Model.Index_Rotation_Right);
		
		const float Divisor = L*(M+N+O) + M*(N+O) + 4*N*O;
		
		const float W = (A*L*(M+N+O) + B*M*(N-O) - C*M*N - 2*C*N*O + D*M*O + 2*D*N*O) / Divisor;
		const float X = (A*L*(N-O) + B*M*(L+N+O) + C*L*N + 2*C*N*O + D*L*O + 2*D*N*O) / Divisor;
		const float Y = X - W;
		const float Z = X + W;
		
		DomainRotation->SetAngularVelocity(Model.Index_Rotation_Drive, W);
		DomainRotation->SetAngularVelocity(Model.Index_Rotation_Steer, X);
		DomainRotation->SetAngularVelocity(Model.Index_Rotation_Left, Y);
		DomainRotation->SetAngularVelocity(Model.Index_Rotation_Right, Z);
	}
}

void USLMDeviceSubsystemDoubleDifferential::PostSimulate(const float DeltaTime)
{
}

int32 USLMDeviceSubsystemDoubleDifferential::AddDevice(FSLMDeviceDoubleDifferential Device)
{
	Device.DeviceModel.Index_Rotation_Drive = DomainRotation->AddPort(Device.Port_Rotation_Drive);
	Device.DeviceModel.Index_Rotation_Left = DomainRotation->AddPort(Device.Port_Rotation_Left);
	Device.DeviceModel.Index_Rotation_Right = DomainRotation->AddPort(Device.Port_Rotation_Right);
	return DeviceModels.Add(Device.DeviceModel);
}

void USLMDeviceSubsystemDoubleDifferential::RemoveDevice(const int32 DeviceIndex)
{
	DomainRotation->RemovePort(DeviceModels[DeviceIndex].Index_Rotation_Drive);
	DomainRotation->RemovePort(DeviceModels[DeviceIndex].Index_Rotation_Left);
	DomainRotation->RemovePort(DeviceModels[DeviceIndex].Index_Rotation_Right);
	DeviceModels.RemoveAt(DeviceIndex);
}

FSLMDeviceModelDoubleDifferential USLMDeviceSubsystemDoubleDifferential::GetDeviceState(const int32 DeviceIndex)
{
	return DeviceModels[DeviceIndex];
}