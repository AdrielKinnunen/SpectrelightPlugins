// Copyright Spectrelight Studios, LLC

#include "Devices/SLMDeviceGearbox.h"


UE_DEFINE_GAMEPLAY_TAG(TAG_SLM_System_Device_Gearbox, "SLM.System.Device.Gearbox");


FString FSLMDeviceSettingsGearbox::GetDebugString() const
{
	FString Result;
	Result += FString::Format(TEXT("\n{0}, {1}, {2}, {3}, {4}"), {
								NumForwardGears, 
								NumReverseGears, 
								FirstGearRatio, 
								RatioBetweenGears, 
								GearSpreadExponent});
	return Result;
}

uint32 FSLMDeviceSettingsGearbox::GetDebugHash() const
{
	uint32 Hash = 0;
	Hash = HashCombine(Hash, GetTypeHash(NumForwardGears));
	Hash = HashCombine(Hash, GetTypeHash(NumReverseGears));
	Hash = HashCombine(Hash, GetTypeHash(FMath::RoundToInt(FirstGearRatio * 100.0f)));
	Hash = HashCombine(Hash, GetTypeHash(FMath::RoundToInt(RatioBetweenGears * 100.0f)));
	Hash = HashCombine(Hash, GetTypeHash(FMath::RoundToInt(GearSpreadExponent * 100.0f)));
	return Hash;
}

FString FSLMDeviceStateGearbox::GetDebugString() const
{
	FString Result;
	Result += FString::Format(TEXT("\n{0}, {1}, {2}, {3}, {4}"), {CurrentGear, GearRatio, PortID_Rotation_Input, PortID_Rotation_Output, PortID_Signal_Shift});
	return Result;
}

uint32 FSLMDeviceStateGearbox::GetDebugHash() const
{
	uint32 Hash = 0;
	Hash = HashCombine(Hash, GetTypeHash(CurrentGear));
	Hash = HashCombine(Hash, GetTypeHash(FMath::RoundToInt(GearRatio * 100.0f)));
	return Hash;
}

FSLMDeviceSystemGearbox::FSLMDeviceSystemGearbox()
{
	SystemTag = TAG_SLM_System_Device_Gearbox;
}

FGameplayTag FSLMDeviceSystemGearbox::GetSystemTagStatic()
{
	return TAG_SLM_System_Device_Gearbox;
}

void FSLMDeviceSystemGearbox::Initialize()
{
	DomainRotation = Manager->GetDomainSystem<FSLMDomainSystemRotation>();
	DomainSignal = Manager->GetDomainSystem<FSLMDomainSystemSignal>();
}

void FSLMDeviceSystemGearbox::PreSimulate_Impl(const int32 ID, const float DeltaTime)
{
	auto& State = StateArray[ID];
	const auto& Settings = SettingsArray[ID];
	auto& Context = GameplayContextArray[ID];
	
	if (bIsServer)
	{
		const float ShiftSignal = DomainSignal->GetParticleRef(State.PortID_Signal_Shift).Read();
		if (State.LastShiftSignal < 0.99 && ShiftSignal >= 0.99)
		{
			State.CurrentGear = FMath::Clamp(State.CurrentGear + 1, -1 * Settings.NumReverseGears, Settings.NumForwardGears);
		}
		else if (State.LastShiftSignal > -0.99 && ShiftSignal <= -0.99)
		{
			State.CurrentGear = FMath::Clamp(State.CurrentGear - 1, -1 * Settings.NumReverseGears, Settings.NumForwardGears);
		}
		State.LastShiftSignal = ShiftSignal;
	}
		
	if (State.CurrentGear != State.PreviousGear)
	{
		State.GearRatio = FMath::Sign(State.CurrentGear) * Settings.FirstGearRatio * FMath::Pow(Settings.RatioBetweenGears, Settings.GearSpreadExponent * (1 - FMath::Abs(State.CurrentGear)));
		if (bIsServer)
		{
			UE_LOG(LogTemp, Warning, TEXT("Server shifted from gear %i to gear %i"), State.PreviousGear, State.CurrentGear);			
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Client shifted from gear %i to gear %i"), State.PreviousGear, State.CurrentGear);
		}
		State.PreviousGear = State.CurrentGear;
		Context.OnGearChanged.ExecuteIfBound(State.CurrentGear);
		if (bIsServer)
		{
			MarkForReplication(ID);
		}
	}
}

void FSLMDeviceSystemGearbox::Simulate_Impl(const int32 ID, const float DeltaTime, const float SubstepScalar)
{
	//auto& Settings = SettingsArray[ID];
	const auto& State = StateArray[ID];
	//auto& Context = GameplayContextArray[ID];
		
	if (!FMath::IsNearlyZero(State.GearRatio))
	{
		FSLMDataRotation& In = DomainRotation->GetParticleRef(State.PortID_Rotation_Input);
		FSLMDataRotation& Out = DomainRotation->GetParticleRef(State.PortID_Rotation_Output);
			
		const FSLMDataRotation InApparent = In.GetApparentStateThroughGearRatio(State.GearRatio);

		const float OutAngVel = (InApparent.AngularVelocity * InApparent.MomentOfInertia + Out.AngularVelocity * Out.MomentOfInertia) / (InApparent.MomentOfInertia + Out.MomentOfInertia);
		const float InAngVel = OutAngVel * State.GearRatio;
			
		In.AngularVelocity = InAngVel;
		Out.AngularVelocity = OutAngVel;
			
		//const float OutImpulse = Out.MomentOfInertia * (OutAngVel - Out.AngularVelocity);
		//const float InImpulse = In.MomentOfInertia * (InAngVel - In.AngularVelocity);
			
		//In.AddImpulse(InImpulse);
		//Out.AddImpulse(OutImpulse);
	}	
}

void FSLMDeviceSystemGearbox::PostSimulate_Impl(const int32 ID, const float DeltaTime)
{
	//auto& Settings = SettingsArray[ID];
	const auto& State = StateArray[ID];
	auto& GameplayContext = GameplayContextArray[ID];

	GameplayContext.CurrentGear = State.CurrentGear;
	GameplayContext.CurrentGearRatio = State.GearRatio;
	GameplayContext.InputAngVelDegS = DomainRotation->GetParticleRef(State.PortID_Rotation_Input).AngularVelocity * SLMRadToDeg;
	GameplayContext.OutputAngVelDegS = DomainRotation->GetParticleRef(State.PortID_Rotation_Output).AngularVelocity * SLMRadToDeg;
}

void FSLMDeviceSystemGearbox::RegisterPorts_Impl(const int32 DeviceID, const FSLMDevicePortAddressesGearbox& Addresses)
{
	auto& State = StateArray[DeviceID];
	const auto& Descriptor = DescriptorArray[DeviceID];
	const auto& Ports = DescriptorArray[DeviceID].Ports;
	
	State.PortID_Rotation_Input		= DomainRotation->RegisterPort(Ports.Port_Rotation_Input, Addresses.Address_Rotation_Input, Descriptor.AssociatedActor.Get());
	State.PortID_Rotation_Output	= DomainRotation->RegisterPort(Ports.Port_Rotation_Output, Addresses.Address_Rotation_Output, Descriptor.AssociatedActor.Get());
	State.PortID_Signal_Shift		= DomainSignal->RegisterPort(Ports.Port_Signal_Shift, Addresses.Address_Signal_Shift, Descriptor.AssociatedActor.Get());
}

void FSLMDeviceSystemGearbox::RemovePorts_Impl(const FSLMDevicePortAddressesGearbox& Addresses) const
{
	DomainRotation	->RemovePort(Addresses.Address_Rotation_Input);
	DomainRotation	->RemovePort(Addresses.Address_Rotation_Output);
	DomainSignal	->RemovePort(Addresses.Address_Signal_Shift);
}

void FSLMDeviceSystemGearbox::GetPortAddresses_Impl(const int32 DeviceID, FSLMDevicePortAddressesGearbox& OutAddresses) const
{
	OutAddresses.Address_Rotation_Input		= MakePortAddress(this, DeviceID, DomainRotation, 0);
	OutAddresses.Address_Rotation_Output	= MakePortAddress(this, DeviceID, DomainRotation, 1);
	OutAddresses.Address_Signal_Shift		= MakePortAddress(this, DeviceID, DomainSignal, 0);
}

void FSLMDeviceSystemGearbox::GetRepState_Impl(const int32 DeviceID, FSLMDeviceRepStateGearbox& OutRepState) const
{
	OutRepState.CurrentGear = StateArray[DeviceID].CurrentGear;
}

void FSLMDeviceSystemGearbox::ApplyRepState_Impl(const int32 DeviceID, const FSLMDeviceRepStateGearbox& RepState)
{
	StateArray[DeviceID].CurrentGear = RepState.CurrentGear;
}

void USLMDeviceComponentGearbox::BeginPlay()
{
	Super::BeginPlay();
	if (GetOwner()->HasAuthority() && !Manager->bRemappingContextOpen)
	{
		DefaultDeviceDescriptor.AssociatedActor = GetOwner();
		DeviceAddress = DeviceSystem->AddEditDevice(DefaultDeviceDescriptor);
		OnRep_DeviceAddress(DeviceAddress);
	}
}

void USLMDeviceComponentGearbox::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (GetOwner()->HasAuthority() && bDeviceResolved)
	{
		DeviceSystem->RemoveDevice(DeviceAddress);
	}
	Super::EndPlay(EndPlayReason);
}

void USLMDeviceComponentGearbox::OnManagerReady()
{
	DeviceSystem = Manager->GetDeviceSystem<FSLMDeviceSystemGearbox>();
}

void USLMDeviceComponentGearbox::OnDeviceResolved()
{
	DeviceSystem->GetGameplayContextRef(DeviceAddress).OnGearChanged.BindUObject(this, &USLMDeviceComponentGearbox::HandleGearChanged);
}

FSLMDeviceDescriptorGearbox USLMDeviceComponentGearbox::GetDeviceDescriptor() const
{
	return DeviceSystem->GetDescriptor(DeviceAddress);
}

void USLMDeviceComponentGearbox::EditDevice(const FSLMDeviceDescriptorGearbox& Descriptor)
{
	DeviceSystem->AddEditDevice(Descriptor, DeviceAddress);
}

FSLMDevicePortAddressesGearbox USLMDeviceComponentGearbox::GetPortAddresses() const
{
	return DeviceSystem->GetPortAddresses(DeviceAddress);
}

FSLMDeviceGameplayContextGearbox USLMDeviceComponentGearbox::GetGameplayContext() const
{
	if (DeviceSystem->DeviceExists(DeviceAddress))
	{
		return DeviceSystem->GetGameplayContextRef(DeviceAddress);
	}
	return FSLMDeviceGameplayContextGearbox();
}

void USLMDeviceComponentGearbox::HandleGearChanged(const int32 NewGear) const
{
	OnGearChanged.Broadcast(NewGear);
}
