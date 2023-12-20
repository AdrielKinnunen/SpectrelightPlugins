// Copyright Spectrelight Studios, LLC
#include "Devices/SLMDeviceGearbox.h"

USLMDeviceComponentGearbox::USLMDeviceComponentGearbox()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void USLMDeviceComponentGearbox::BeginPlay()
{
	Super::BeginPlay();
	const TWeakObjectPtr<AActor> OwningActor = GetOwner();
	DeviceSettings.Port_Rotation_Input.PortMetaData.AssociatedActor = OwningActor;
	DeviceSettings.Port_Rotation_Output.PortMetaData.AssociatedActor = OwningActor;
	GetWorld()->GetSubsystem<USLMDeviceSubsystemGearbox>()->RegisterDeviceComponent(this);
}

void USLMDeviceComponentGearbox::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorld()->GetSubsystem<USLMDeviceSubsystemGearbox>()->DeRegisterDeviceComponent(this);
	Super::EndPlay(EndPlayReason);
}

void USLMDeviceSubsystemGearbox::OnWorldBeginPlay(UWorld& InWorld)
{
	DomainRotation = GetWorld()->GetSubsystem<USLMDomainRotation>();
	Super::OnWorldBeginPlay(InWorld);
}

void USLMDeviceSubsystemGearbox::PreSimulate(const float DeltaTime)
{
}

void USLMDeviceSubsystemGearbox::Simulate(const float DeltaTime)
{
	for (auto& [GearRatio, Index_Rotation_Input, Index_Rotation_Output] : DeviceModels)
	{
		const FSLMDataRotation In = DomainRotation->GetByPortIndex(Index_Rotation_Input);
		const FSLMDataRotation Out = DomainRotation->GetByPortIndex(Index_Rotation_Output);

		const float MOIShaftInputEffective = GearRatio * GearRatio * In.MOI;
		const float AngVelShaftInputEffective = In.RPS / GearRatio;

		const float OutAngVel = (AngVelShaftInputEffective * MOIShaftInputEffective + Out.RPS * Out.MOI) / (MOIShaftInputEffective + Out.MOI);
		const float InAngVel = OutAngVel * GearRatio;

		DomainRotation->SetNetworkAngVel(Index_Rotation_Input, InAngVel);
		DomainRotation->SetNetworkAngVel(Index_Rotation_Output, OutAngVel);
	}
}

void USLMDeviceSubsystemGearbox::PostSimulate(const float DeltaTime)
{
}

void USLMDeviceSubsystemGearbox::RegisterDeviceComponent(USLMDeviceComponentGearbox* DeviceComponent)
{
	const auto Index = AddDevice(DeviceComponent->DeviceSettings);
	DeviceComponent->DeviceIndex = Index;
	//DeviceComponents.Insert(Index, DeviceComponent);
}

void USLMDeviceSubsystemGearbox::DeRegisterDeviceComponent(const USLMDeviceComponentGearbox* DeviceComponent)
{
	const auto Index = DeviceComponent->DeviceIndex;
	RemoveDevice(Index);
	//DeviceComponents.RemoveAt(Index);
}

int32 USLMDeviceSubsystemGearbox::AddDevice(FSLMDeviceGearbox Device)
{
	Device.DeviceModel.Index_Rotation_Input = DomainRotation->AddPort(Device.Port_Rotation_Input);
	Device.DeviceModel.Index_Rotation_Output = DomainRotation->AddPort(Device.Port_Rotation_Output);
	return DeviceModels.Add(Device.DeviceModel);
}

void USLMDeviceSubsystemGearbox::RemoveDevice(const int32 DeviceIndex)
{
	DomainRotation->RemovePort(DeviceModels[DeviceIndex].Index_Rotation_Input);
	DomainRotation->RemovePort(DeviceModels[DeviceIndex].Index_Rotation_Output);
	DeviceModels.RemoveAt(DeviceIndex);
}

FSLMDeviceModelGearbox USLMDeviceSubsystemGearbox::GetDeviceState(const int32 DeviceIndex)
{
	return DeviceModels[DeviceIndex];
}
