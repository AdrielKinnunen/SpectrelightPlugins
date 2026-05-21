// Copyright Spectrelight Studios, LLC

#include "Devices/SLMDeviceProbe.h"
#include "Net/UnrealNetwork.h"

void USLMDeviceSubsystemProbe::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
}

void USLMDeviceSubsystemProbe::PostInitialize()
{
	Super::PostInitialize();
	DomainRotation = GetWorld()->GetSubsystem<USLMDomainRotation>();
	DomainSignal = GetWorld()->GetSubsystem<USLMDomainSignal>();
}

FString USLMDeviceSubsystemProbe::GetDebugString(const bool Verbose)
{
	return GetDebugString_Impl(Verbose);
}

uint32 USLMDeviceSubsystemProbe::GetDebugHash()
{
	return GetDebugHash_Impl();
}

void USLMDeviceSubsystemProbe::PreSimulate(const float DeltaTime)
{
}

void USLMDeviceSubsystemProbe::Simulate(const float DeltaTime, const float SubstepScalar)
{
	for (auto& Model : DeviceModels)
	{
		
		FSLMDataRotation& In = DomainRotation->GetParticleRef(Model.State.PortID_Rotation);
		//In.AddTorque(Model.State.ProbeValue * 100, DeltaTime);
		In.AngularVelocity = Model.State.ProbeValue;
		DomainSignal->WriteValue(Model.State.PortID_Signal, Model.State.ProbeValue);
		Model.State.bDirty = true;
	}
}

void USLMDeviceSubsystemProbe::PostSimulate(const float DeltaTime)
{
	PullRepState();
}

void USLMDeviceSubsystemProbe::Client_AddOrChangeDescriptor(const FSLMDeviceAddress& DeviceAddress, const FInstancedStruct& Payload)
{
	Client_AddOrChangeDescriptor_Impl(DeviceAddress, Payload);
}

void USLMDeviceSubsystemProbe::Client_RemoveDescriptor(const FSLMDeviceAddress& DeviceAddress)
{
	Client_RemoveDescriptor_Impl(DeviceAddress);
}

void USLMDeviceSubsystemProbe::Client_AddOrChangeState(const FSLMDeviceAddress& DeviceAddress, const FInstancedStruct& Payload)
{
	Client_AddOrChangeState_Impl(DeviceAddress, Payload);
}

void USLMDeviceSubsystemProbe::Client_RemoveState(const FSLMDeviceAddress& DeviceAddress)
{
	Client_RemoveState_Impl(DeviceAddress);
}

void USLMDeviceSubsystemProbe::RegisterPorts(const FSLMPortSettingsProbe& PortSettings, FSLMModelStateProbe& ModelState, const FSLMPortAddressesProbe& Addresses) const
{
	ModelState.PortID_Rotation = DomainRotation->AddPort(PortSettings.Port_Rotation, Addresses.Address_Rotation);
	ModelState.PortID_Signal = DomainSignal->AddPort(PortSettings.Port_Signal, Addresses.Address_Signal);
}

void USLMDeviceSubsystemProbe::RemovePorts(const FSLMPortAddressesProbe& Addresses) const
{
	DomainRotation->RemovePort(Addresses.Address_Rotation);
	DomainSignal->RemovePort(Addresses.Address_Signal);
}

void USLMDeviceSubsystemProbe::DeviceIDToPortAddresses(const int32 DeviceID, FSLMPortAddressesProbe& Addresses) const
{
	Addresses.Address_Rotation		= MakePortAddress(this, DomainRotation, DeviceID, 0);
	Addresses.Address_Signal		= MakePortAddress(this, DomainSignal, DeviceID, 0);
}

void USLMDeviceSubsystemProbe::ModelToCosmeticState(const FSLMModelProbe& Model, FSLMCosmeticStateProbe& CosmeticState)
{
	CosmeticState.ProbeValue = Model.State.ProbeValue;
}

void USLMDeviceSubsystemProbe::ModelToRepState(const FSLMModelProbe& Model, FSLMRepStateProbe& RepState)
{
	RepState.ProbeValue = Model.State.ProbeValue;
}

void USLMDeviceSubsystemProbe::RepStateToModel(const FSLMRepStateProbe& RepState, FSLMModelProbe& Model)
{
	Model.State.ProbeValue = RepState.ProbeValue;
}

void USLMDeviceSubsystemProbe::InputToModel(const FSLMInputProbe& Input, FSLMModelProbe& Model)
{
	Model.State.ProbeValue = Input.ProbeValue;
}




USLMDeviceComponentProbe::USLMDeviceComponentProbe()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void USLMDeviceComponentProbe::BeginPlay()
{
	Super::BeginPlay();
	Subsystem = GetWorld()->GetSubsystem<USLMDeviceSubsystemProbe>();
	if (GetOwner()->HasAuthority())
	{
		const AActor* Owner = GetOwner();
		DeviceDescriptor.PortSettings.Port_Rotation.PortMetaData.AssociatedActor = Owner;
		DeviceDescriptor.PortSettings.Port_Signal.PortMetaData.AssociatedActor = Owner;
		DeviceAddress = Subsystem->AddDevice(DeviceDescriptor);
	}
}

void USLMDeviceComponentProbe::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (GetOwner()->HasAuthority())
	{
		Subsystem->RemoveDevice(DeviceAddress);
	}
	Super::EndPlay(EndPlayReason);
}

void USLMDeviceComponentProbe::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(USLMDeviceComponentProbe, DeviceAddress);
}

FSLMModelSettingsProbe USLMDeviceComponentProbe::GetDeviceSettings() const
{
	return Subsystem->GetModelSettings(DeviceAddress);
}

void USLMDeviceComponentProbe::SetDeviceSettings(const FSLMModelSettingsProbe& Settings) const
{
	Subsystem->SetModelSettings(DeviceAddress, Settings);
}

void USLMDeviceComponentProbe::ApplyInput(const FSLMInputProbe& Input) const
{
	Subsystem->ApplyInput(DeviceAddress, Input);
}

FSLMCosmeticStateProbe USLMDeviceComponentProbe::GetCosmeticState() const
{
	return Subsystem->GetCosmeticState(DeviceAddress);
}

FSLMPortAddressesProbe USLMDeviceComponentProbe::GetPortAddresses() const
{
	return Subsystem->GetPortAddresses(DeviceAddress);
}
