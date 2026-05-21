// Copyright Spectrelight Studios, LLC

#include "Devices/SLMDeviceGearbox.h"
#include "Net/UnrealNetwork.h"

void USLMDeviceSubsystemGearbox::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
}

void USLMDeviceSubsystemGearbox::PostInitialize()
{
	Super::PostInitialize();
	DomainRotation = GetWorld()->GetSubsystem<USLMDomainRotation>();
	DomainSignal = GetWorld()->GetSubsystem<USLMDomainSignal>();
}

FString USLMDeviceSubsystemGearbox::GetDebugString(const bool Verbose)
{
	return GetDebugString_Impl(Verbose);
}

uint32 USLMDeviceSubsystemGearbox::GetDebugHash()
{
	return GetDebugHash_Impl();
}

void USLMDeviceSubsystemGearbox::PreSimulate(const float DeltaTime)
{
}

void USLMDeviceSubsystemGearbox::Simulate(const float DeltaTime, const float SubstepScalar)
{
	for (auto& Model : DeviceModels)
	{
		//const float ShiftSignal = DomainSignal->ReadValue(Model.State.PortID_Signal_Shift);
		
		if (GetWorld()->GetNetMode() != NM_Client)
		{
			const float ShiftSignal = DomainSignal->ReadValue(Model.State.PortID_Signal_Shift);
			if (Model.State.LastShiftSignal < 0.99 && ShiftSignal >= 0.99)
			{
				Model.State.CurrentGear = FMath::Clamp(Model.State.CurrentGear + 1, -1 * Model.Settings.NumReverseGears, Model.Settings.NumForwardGears);
			}
			else if (Model.State.LastShiftSignal > -0.99 && ShiftSignal <= -0.99)
			{
				Model.State.CurrentGear = FMath::Clamp(Model.State.CurrentGear - 1, -1 * Model.Settings.NumReverseGears, Model.Settings.NumForwardGears);
			}
			Model.State.LastShiftSignal = ShiftSignal;
		}
		
		if (Model.State.CurrentGear != Model.State.PreviousGear)
		{
			Model.State.GearRatio = FMath::Sign(Model.State.CurrentGear) * Model.Settings.FirstGearRatio * FMath::Pow(Model.Settings.RatioBetweenGears, Model.Settings.GearSpreadExponent * (1 - FMath::Abs(Model.State.CurrentGear)));
			Model.State.bDirty = true;
			UE_LOG(LogTemp, Warning, TEXT("Shifted from gear %i to gear %i"), Model.State.PreviousGear, Model.State.CurrentGear);
			Model.State.PreviousGear = Model.State.CurrentGear;
		}
		
		if (!FMath::IsNearlyZero(Model.State.GearRatio))
		{
			FSLMDataRotation& In = DomainRotation->GetParticleRef(Model.State.PortID_Rotation_Input);
			FSLMDataRotation& Out = DomainRotation->GetParticleRef(Model.State.PortID_Rotation_Output);
			
			const FSLMDataRotation InApparent = In.GetApparentStateThroughGearRatio(Model.State.GearRatio);

			const float OutAngVel = (InApparent.AngularVelocity * InApparent.MomentOfInertia + Out.AngularVelocity * Out.MomentOfInertia) / (InApparent.MomentOfInertia + Out.MomentOfInertia);
			const float InAngVel = OutAngVel * Model.State.GearRatio;
			
			In.AngularVelocity = InAngVel;
			Out.AngularVelocity = OutAngVel;
			
			//const float OutImpulse = Out.MomentOfInertia * (OutAngVel - Out.AngularVelocity);
			//const float InImpulse = In.MomentOfInertia * (InAngVel - In.AngularVelocity);
			
			//In.AddImpulse(InImpulse);
			//Out.AddImpulse(OutImpulse);
		}	
	}
}

void USLMDeviceSubsystemGearbox::PostSimulate(const float DeltaTime)
{
	PullRepState();
}

void USLMDeviceSubsystemGearbox::Client_AddOrChangeDescriptor(const FSLMDeviceAddress& DeviceAddress, const FInstancedStruct& Payload)
{
	Client_AddOrChangeDescriptor_Impl(DeviceAddress, Payload);
}

void USLMDeviceSubsystemGearbox::Client_RemoveDescriptor(const FSLMDeviceAddress& DeviceAddress)
{
	Client_RemoveDescriptor_Impl(DeviceAddress);
}

void USLMDeviceSubsystemGearbox::Client_AddOrChangeState(const FSLMDeviceAddress& DeviceAddress, const FInstancedStruct& Payload)
{
	Client_AddOrChangeState_Impl(DeviceAddress, Payload);
}

void USLMDeviceSubsystemGearbox::Client_RemoveState(const FSLMDeviceAddress& DeviceAddress)
{
	Client_RemoveState_Impl(DeviceAddress);
}

void USLMDeviceSubsystemGearbox::RegisterPorts(const FSLMPortSettingsGearbox& PortSettings, FSLMModelStateGearbox& ModelState, const FSLMPortAddressesGearbox& Addresses) const
{
	ModelState.PortID_Rotation_Input = DomainRotation->AddPort(PortSettings.Port_Rotation_Input, Addresses.Address_Rotation_Input);
	ModelState.PortID_Rotation_Output = DomainRotation->AddPort(PortSettings.Port_Rotation_Output, Addresses.Address_Rotation_Output);
	ModelState.PortID_Signal_Shift = DomainSignal->AddPort(PortSettings.Port_Signal_Shift, Addresses.Address_Signal_Shift);
}

void USLMDeviceSubsystemGearbox::RemovePorts(const FSLMPortAddressesGearbox& Addresses) const
{
	DomainRotation->RemovePort(Addresses.Address_Rotation_Input);
	DomainRotation->RemovePort(Addresses.Address_Rotation_Output);
	DomainSignal->RemovePort(Addresses.Address_Signal_Shift);
}

void USLMDeviceSubsystemGearbox::DeviceIDToPortAddresses(const int32 DeviceID, FSLMPortAddressesGearbox& Addresses) const
{
	Addresses.Address_Rotation_Input	= MakePortAddress(this, DomainRotation, DeviceID, 0);
	Addresses.Address_Rotation_Output	= MakePortAddress(this, DomainRotation, DeviceID, 1);
	Addresses.Address_Signal_Shift		= MakePortAddress(this, DomainSignal, DeviceID, 0);
}

void USLMDeviceSubsystemGearbox::ModelToCosmeticState(const FSLMModelGearbox& Model, FSLMCosmeticStateGearbox& CosmeticState) const
{
	CosmeticState.CurrentGear = Model.State.CurrentGear;
	CosmeticState.CurrentGearRatio = Model.State.GearRatio;
	CosmeticState.InputAngVelDegS = DomainRotation->GetParticleRef(Model.State.PortID_Rotation_Input).AngularVelocity * SLMRadToDeg;
	CosmeticState.OutputAngVelDegS = DomainRotation->GetParticleRef(Model.State.PortID_Rotation_Output).AngularVelocity * SLMRadToDeg;
}

void USLMDeviceSubsystemGearbox::ModelToRepState(const FSLMModelGearbox& Model, FSLMRepStateGearbox& RepState)
{
	RepState.Gear = Model.State.CurrentGear;
}

void USLMDeviceSubsystemGearbox::RepStateToModel(const FSLMRepStateGearbox& RepState, FSLMModelGearbox& Model)
{
	Model.State.CurrentGear = RepState.Gear;
}

void USLMDeviceSubsystemGearbox::InputToModel(const FSLMInputGearbox& Input, FSLMModelGearbox& Model)
{
	Model.State.CurrentGear = FMath::Clamp(Input.DesiredGear, -Model.Settings.NumReverseGears, Model.Settings.NumForwardGears);
}













USLMDeviceComponentGearbox::USLMDeviceComponentGearbox()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void USLMDeviceComponentGearbox::BeginPlay()
{
	Super::BeginPlay();
	Subsystem = GetWorld()->GetSubsystem<USLMDeviceSubsystemGearbox>();
	if (GetOwner()->HasAuthority())
	{
		const AActor* Owner = GetOwner();
		DeviceDescriptor.PortSettings.Port_Rotation_Input.PortMetaData.AssociatedActor = Owner;
		DeviceDescriptor.PortSettings.Port_Rotation_Output.PortMetaData.AssociatedActor = Owner;
		DeviceDescriptor.PortSettings.Port_Signal_Shift.PortMetaData.AssociatedActor = Owner;
		DeviceAddress = Subsystem->AddDevice(DeviceDescriptor);
		bInitialized = true;
	}
}

void USLMDeviceComponentGearbox::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (GetOwner()->HasAuthority())
	{
		check(bInitialized);
		Subsystem->RemoveDevice(DeviceAddress);
	}
	Super::EndPlay(EndPlayReason);
}

void USLMDeviceComponentGearbox::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(USLMDeviceComponentGearbox, DeviceAddress);
}

FSLMModelSettingsGearbox USLMDeviceComponentGearbox::GetDeviceSettings() const
{
	return Subsystem->GetModelSettings(DeviceAddress);
}

void USLMDeviceComponentGearbox::SetDeviceSettings(const FSLMModelSettingsGearbox& Settings) const
{
	Subsystem->SetModelSettings(DeviceAddress, Settings);
}

void USLMDeviceComponentGearbox::ApplyInput(const FSLMInputGearbox& Input) const
{
	Subsystem->ApplyInput(DeviceAddress, Input);
}

FSLMCosmeticStateGearbox USLMDeviceComponentGearbox::GetCosmeticState() const
{
	return Subsystem->GetCosmeticState(DeviceAddress);
}

FSLMPortAddressesGearbox USLMDeviceComponentGearbox::GetPortAddresses() const
{
	return Subsystem->GetPortAddresses(DeviceAddress);
}
