// Copyright Spectrelight Studios, LLC

#include "Devices/SLMDeviceProbe.h"


UE_DEFINE_GAMEPLAY_TAG(TAG_SLM_System_Device_Probe, "SLM.System.Device.Probe");


FString FSLMDeviceSettingsProbe::GetDebugString() const
{
	FString Result;
	Result += FString::Format(TEXT("\n{0}"), {Smoothing});
	return Result;
}

uint32 FSLMDeviceSettingsProbe::GetDebugHash() const
{
	uint32 Hash = 0;
	Hash = HashCombine(Hash, GetTypeHash(FMath::RoundToInt(Smoothing * 100.0f)));
	return Hash;
}

FString FSLMDeviceStateProbe::GetDebugString() const
{
	FString Result;
	Result += FString::Format(TEXT("\n{0}"), {ProbeValue});
	return Result;
}

uint32 FSLMDeviceStateProbe::GetDebugHash() const
{
	uint32 Hash = 0;
	Hash = HashCombine(Hash, GetTypeHash(FMath::RoundToInt(ProbeValue * 100.0f)));
	return Hash;
}

FSLMDeviceSystemProbe::FSLMDeviceSystemProbe()
{
	SystemTag = TAG_SLM_System_Device_Probe;
}

FGameplayTag FSLMDeviceSystemProbe::GetSystemTagStatic()
{
	return TAG_SLM_System_Device_Probe;
}

void FSLMDeviceSystemProbe::Initialize()
{
	DomainRotation = Manager->GetDomainSystem<FSLMDomainSystemRotation>();
	DomainSignal = Manager->GetDomainSystem<FSLMDomainSystemSignal>();
}

void FSLMDeviceSystemProbe::PreSimulate_Impl(const int32 ID, const float DeltaTime)
{
	auto& State = StateArray[ID];
	//auto& Settings = SettingsArray[ID];
	const auto& Context = GameplayContextArray[ID];
	if (bIsServer)
	{
		State.ProbeValue = Context.DesiredProbeValue;
	}
}

void FSLMDeviceSystemProbe::Simulate_Impl(const int32 ID, const float DeltaTime, const float SubstepScalar)
{
	const auto& State = StateArray[ID];
	//auto& Settings = SettingsArray[ID];
	//auto& Context = GameplayContextArray[ID];
	
	FSLMDataRotation& RotationParticle = DomainRotation->GetParticleRef(State.PortID_Rotation);
	RotationParticle.AngularVelocity += State.ProbeValue * DeltaTime;
	FSLMDataSignal& SignalParticle = DomainSignal->GetParticleRef(State.PortID_Signal);
	SignalParticle.Write(State.ProbeValue);	
}

void FSLMDeviceSystemProbe::PostSimulate_Impl(const int32 ID, const float DeltaTime)
{
	MarkForReplication(ID);
}

void FSLMDeviceSystemProbe::RegisterPorts_Impl(const int32 DeviceID, const FSLMDevicePortAddressesProbe& Addresses)
{
	auto& State = StateArray[DeviceID];
	const auto& Descriptor = DescriptorArray[DeviceID];
	const auto& Ports = DescriptorArray[DeviceID].Ports;
	
	State.PortID_Rotation	= DomainRotation->RegisterPort(Ports.Port_Rotation, Addresses.Address_Rotation, Descriptor.AssociatedActor.Get());
	State.PortID_Signal		= DomainSignal->RegisterPort(Ports.Port_Signal, Addresses.Address_Signal, Descriptor.AssociatedActor.Get());
}

void FSLMDeviceSystemProbe::RemovePorts_Impl(const FSLMDevicePortAddressesProbe& Addresses) const
{
	DomainRotation	->RemovePort(Addresses.Address_Rotation);
	DomainSignal	->RemovePort(Addresses.Address_Signal);
}

void FSLMDeviceSystemProbe::GetPortAddresses_Impl(const int32 DeviceID, FSLMDevicePortAddressesProbe& OutAddresses) const
{
	OutAddresses.Address_Rotation	= MakePortAddress(this, DeviceID, DomainRotation, 0);
	OutAddresses.Address_Signal		= MakePortAddress(this, DeviceID, DomainSignal, 0);
}

void FSLMDeviceSystemProbe::GetRepState_Impl(const int32 DeviceID, FSLMDeviceRepStateProbe& OutRepState) const
{
	OutRepState.ProbeValue = StateArray[DeviceID].ProbeValue;
	OutRepState.ProbeAngVel = DomainRotation->GetParticleRef(StateArray[DeviceID].PortID_Rotation).AngularVelocity;
}

void FSLMDeviceSystemProbe::ApplyRepState_Impl(const int32 DeviceID, const FSLMDeviceRepStateProbe& RepState)
{
	StateArray[DeviceID].ProbeValue = RepState.ProbeValue;
	DomainRotation->GetParticleRef(StateArray[DeviceID].PortID_Rotation).AngularVelocity = RepState.ProbeAngVel;
}

void USLMDeviceComponentProbe::BeginPlay()
{
	Super::BeginPlay();
	if (GetOwner()->HasAuthority() && !Manager->bRemappingContextOpen)
	{
		DefaultDeviceDescriptor.AssociatedActor = GetOwner();
		DeviceAddress = DeviceSystem->AddEditDevice(DefaultDeviceDescriptor);
		OnRep_DeviceAddress(DeviceAddress);
	}
}

void USLMDeviceComponentProbe::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (GetOwner()->HasAuthority() && bDeviceResolved)
	{
		DeviceSystem->RemoveDevice(DeviceAddress);
	}
	Super::EndPlay(EndPlayReason);
}

void USLMDeviceComponentProbe::OnManagerReady()
{
	DeviceSystem = Manager->GetDeviceSystem<FSLMDeviceSystemProbe>();
}

void USLMDeviceComponentProbe::OnDeviceResolved()
{
	Super::OnDeviceResolved();
}

FSLMDeviceDescriptorProbe USLMDeviceComponentProbe::GetDeviceDescriptor() const
{
	return DeviceSystem->GetDescriptor(DeviceAddress);
}

void USLMDeviceComponentProbe::EditDevice(const FSLMDeviceDescriptorProbe& Descriptor)
{
	DeviceSystem->AddEditDevice(Descriptor, DeviceAddress);
}

FSLMDevicePortAddressesProbe USLMDeviceComponentProbe::GetPortAddresses() const
{
	return DeviceSystem->GetPortAddresses(DeviceAddress);
}

void USLMDeviceComponentProbe::SetDesiredProbeValue(const float Value)
{
	if (DeviceSystem->DeviceExists(DeviceAddress))
	{
		DeviceSystem->GetGameplayContextRef(DeviceAddress).DesiredProbeValue = Value;
	}
}
