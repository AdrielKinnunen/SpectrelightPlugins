// Copyright Spectrelight Studios, LLC

#include "Devices/SLMDeviceSimpleGearbox.h"
#include "Net/UnrealNetwork.h"



void FSLMRepArraySettingsSimpleGearbox::PostReplicatedAdd(const TArrayView<int32>& AddedIndices, int32 FinalSize) const
{
	for (const auto Index : AddedIndices)
	{
		const FSLMRepItemSettingsSimpleGearbox& Item = Items[Index];
		if (Subsystem->IsValidHandle_Impl(Item.Handle))
		{
			Subsystem->SetDeviceSettings_Impl(Item.Handle, Item.RepSettings);
		}
		else
		{
			Subsystem->AddDevice(Item.RepSettings, Item.Handle);
		}
	}
}

void FSLMRepArraySettingsSimpleGearbox::PostReplicatedChange(const TArrayView<int32>& ChangedIndices, int32 FinalSize) const
{
	for (const auto Index : ChangedIndices)
	{
		const FSLMRepItemSettingsSimpleGearbox& Item = Items[Index];
		if (Subsystem->IsValidHandle_Impl(Item.Handle))
		{
			Subsystem->SetDeviceSettings_Impl(Item.Handle, Item.RepSettings);
		}
		else
		{
			Subsystem->AddDevice(Item.RepSettings, Item.Handle);
		}
	}
}

void FSLMRepArraySettingsSimpleGearbox::PreReplicatedRemove(const TArrayView<int32>& RemovedIndices, int32 FinalSize) const
{
	for (const auto Index : RemovedIndices)
	{
		const FSLMRepItemSettingsSimpleGearbox& Item = Items[Index];
		if (Subsystem->IsValidHandle_Impl(Item.Handle))
		{
			Subsystem->RemoveDevice_Impl(Item.Handle);
		}
	}
}

void FSLMRepArrayStateSimpleGearbox::PostReplicatedAdd(const TArrayView<int32>& AddedIndices, int32 FinalSize) const
{
	for (const auto Index : AddedIndices)
	{
		const FSLMRepItemStateSimpleGearbox& Item = Items[Index];
		Subsystem->ApplyReplicatedState(Item.Handle, Item.State);
	}
}

void FSLMRepArrayStateSimpleGearbox::PostReplicatedChange(const TArrayView<int32>& ChangedIndices, int32 FinalSize) const
{
	for (const auto Index : ChangedIndices)
	{
		const FSLMRepItemStateSimpleGearbox& Item = Items[Index];
		Subsystem->ApplyReplicatedState(Item.Handle, Item.State);
	}
}

void FSLMRepArrayStateSimpleGearbox::PreReplicatedRemove(const TArrayView<int32>& RemovedIndices, int32 FinalSize)
{
}

void USLMDeviceSubsystemSimpleGearbox::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
	if (GetWorld()->GetNetMode() != NM_Client)
	{
		Replicator = InWorld.SpawnActor<ASLMDeviceReplicatorSimpleGearbox>();
	}
}

void USLMDeviceSubsystemSimpleGearbox::PostInitialize()
{
	Super::PostInitialize();
	DomainRotation = GetWorld()->GetSubsystem<USLMDomainRotation>();
	DomainSignal = GetWorld()->GetSubsystem<USLMDomainSignal>();
}

FSLMDeviceHandleSimpleGearbox USLMDeviceSubsystemSimpleGearbox::AddDevice(const FSLMDeviceSettingsSimpleGearbox& Settings, const FSLMDeviceHandleSimpleGearbox ExplicitHandle)
{
	FSLMDeviceHandleSimpleGearbox Handle;
	if (GetWorld()->GetNetMode() != NM_Client)
	{
		check(Replicator);
		Handle = {DeviceModels.Add(Settings.DeviceModel)};
		Replicator->AddItem({Handle}, Settings);
	}
	else
	{
		check(ExplicitHandle.ID != INDEX_NONE);
		Handle = ExplicitHandle;
		DeviceModels.EmplaceAt(Handle.ID, Settings.DeviceModel);
	}
	FSLMDeviceModelSimpleGearbox& Model = DeviceModels[Handle.ID];
	const FSLMDevicePortAddressesSimpleGearbox PortAddresses = GetPortAddresses_Impl(Handle);
	Model.PortID_Rotation_Input = DomainRotation->AddPort(Settings.Port_Rotation_Input, PortAddresses.Address_Rotation_Input);
	Model.PortID_Rotation_Output = DomainRotation->AddPort(Settings.Port_Rotation_Output, PortAddresses.Address_Rotation_Output);
	Model.PortID_Signal_Shift = DomainSignal->AddPort(Settings.Port_Signal_Shift, PortAddresses.Address_Signal_Shift);
	if (OrphanedRepStates.IsValidIndex(Handle.ID))
	{
		ApplyReplicatedState(Handle, OrphanedRepStates[Handle.ID]);
	}
	return Handle;
}

void USLMDeviceSubsystemSimpleGearbox::ApplyReplicatedState(const FSLMDeviceHandleSimpleGearbox Handle, const FSLMDeviceRepStateSimpleGearbox& State)
{
	if (!IsValidHandle_Impl(Handle))
	{
		OrphanedRepStates.EmplaceAt(Handle.ID, State);
		return;
	}
	auto& Model = DeviceModels[Handle.ID];
	Model.CurrentGear = State.Gear;
}

void USLMDeviceSubsystemSimpleGearbox::RemovePorts(const FSLMDevicePortAddressesSimpleGearbox& Addresses) const
{
	DomainRotation->RemovePort(Addresses.Address_Rotation_Input);
	DomainRotation->RemovePort(Addresses.Address_Rotation_Output);
	DomainSignal->RemovePort(Addresses.Address_Signal_Shift);
}

void USLMDeviceSubsystemSimpleGearbox::WritePortAddresses(const FSLMDeviceHandleSimpleGearbox Handle, FSLMDevicePortAddressesSimpleGearbox& Addresses) const
{
	Addresses.Address_Rotation_Input	= MakePortAddress(this, DomainRotation, Handle.ID, 0);
	Addresses.Address_Rotation_Output	= MakePortAddress(this, DomainRotation, Handle.ID, 1);
	Addresses.Address_Signal_Shift		= MakePortAddress(this, DomainSignal, Handle.ID, 0);
}

void USLMDeviceSubsystemSimpleGearbox::WriteModelToCosmeticState(const FSLMDeviceModelSimpleGearbox& Model, FSLMDeviceCosmeticStateSimpleGearbox& CosmeticStateSimpleGearbox) const
{
	CosmeticStateSimpleGearbox.CurrentGear = Model.CurrentGear;
	CosmeticStateSimpleGearbox.CurrentGearRatio = Model.GearRatio;
	CosmeticStateSimpleGearbox.InputAngVelDegS = DomainRotation->GetData(Model.PortID_Rotation_Input).AngularVelocity * SLMRadToDeg;
	CosmeticStateSimpleGearbox.OutputAngVelDegS = DomainRotation->GetData(Model.PortID_Rotation_Output).AngularVelocity * SLMRadToDeg;
}

void USLMDeviceSubsystemSimpleGearbox::WriteModelToSettings(const FSLMDeviceModelSimpleGearbox& Model, FSLMDeviceSettingsSimpleGearbox& Settings)
{
	Settings.DeviceModel.NumForwardGears = Model.NumForwardGears;
	Settings.DeviceModel.NumReverseGears = Model.NumReverseGears;
	Settings.DeviceModel.FirstGearRatio = Model.FirstGearRatio;
	Settings.DeviceModel.RatioBetweenGears = Model.RatioBetweenGears;
	Settings.DeviceModel.GearSpreadExponent = Model.GearSpreadExponent;
}

void USLMDeviceSubsystemSimpleGearbox::WriteSettingsToModel(FSLMDeviceModelSimpleGearbox& Model, const FSLMDeviceSettingsSimpleGearbox& Settings)
{
	Model.NumForwardGears = Settings.DeviceModel.NumForwardGears;
	Model.NumReverseGears = Settings.DeviceModel.NumReverseGears;
	Model.FirstGearRatio = Settings.DeviceModel.FirstGearRatio;
	Model.RatioBetweenGears = Settings.DeviceModel.RatioBetweenGears;
	Model.GearSpreadExponent = Settings.DeviceModel.GearSpreadExponent;
	Model.CurrentGear = FMath::Clamp(Settings.DeviceModel.CurrentGear, -Model.NumReverseGears, Model.NumForwardGears);
}

void USLMDeviceSubsystemSimpleGearbox::WriteInputToModel(FSLMDeviceModelSimpleGearbox& Model, const FSLMDeviceInputSimpleGearbox& Input)
{
	Model.CurrentGear = FMath::Clamp(Input.DesiredGear, -Model.NumReverseGears, Model.NumForwardGears);
}

void USLMDeviceSubsystemSimpleGearbox::PreSimulate(const float DeltaTime)
{
}

void USLMDeviceSubsystemSimpleGearbox::Simulate(const float DeltaTime, const float SubstepScalar)
{
	for (auto& Model : DeviceModels)
	{
		if (Model.CurrentGear != Model.PreviousGear)
		{
			Model.GearRatio = FMath::Sign(Model.CurrentGear) * Model.FirstGearRatio * FMath::Pow(Model.RatioBetweenGears, Model.GearSpreadExponent * (1 - FMath::Abs(Model.CurrentGear)));
		}
		//Do math based on GearRatio
	}
}

void USLMDeviceSubsystemSimpleGearbox::PostSimulate(const float DeltaTime)
{
	if (GetWorld()->GetNetMode() != NM_Client && Replicator)
	{
		Replicator->PullDynamicState(DeviceModels);
	}
}

FString USLMDeviceSubsystemSimpleGearbox::GetDebugString(const bool Verbose)
{
	FString Result;
	Result += "\n------------------SimpleGearbox------------------";
	Result += FString::Format(TEXT("\nHas {0} Device Models"), {DeviceModels.Num()});
	if (Verbose)
	{
		for (int32 i = 0; i < DeviceModels.GetMaxIndex(); i++)
		{
			if (DeviceModels.IsValidIndex(i))
			{
				Result += FString::Format(TEXT("\nModel {0} has state: {1}"), { i, DeviceModels[i].GetDebugString()});
			}
		}
	}
	return Result;
}

uint32 USLMDeviceSubsystemSimpleGearbox::GetDebugHash()
{
	uint32 Result = 0;
	for (int32 i = 0; i < DeviceModels.GetMaxIndex(); i++)
	{
		if (DeviceModels.IsValidIndex(i))
		{
			Result =  Result ^ HashCombine(GetTypeHash(i), GetTypeHash(DeviceModels[i]));
		}
	}
	return Result;
}









ASLMDeviceReplicatorSimpleGearbox::ASLMDeviceReplicatorSimpleGearbox()
{
	bReplicates = true;
	bAlwaysRelevant = true;
	SetReplicatingMovement(false);
}

void ASLMDeviceReplicatorSimpleGearbox::PostInitializeComponents()
{
	RepArraySettings.Subsystem = GetWorld()->GetSubsystem<USLMDeviceSubsystemSimpleGearbox>();
	RepArrayState.Subsystem = GetWorld()->GetSubsystem<USLMDeviceSubsystemSimpleGearbox>();
	Super::PostInitializeComponents();
}

void ASLMDeviceReplicatorSimpleGearbox::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASLMDeviceReplicatorSimpleGearbox, RepArraySettings);
	DOREPLIFETIME(ASLMDeviceReplicatorSimpleGearbox, RepArrayState);
}

void ASLMDeviceReplicatorSimpleGearbox::AddItem(const FSLMDeviceHandleSimpleGearbox Handle, const FSLMDeviceSettingsSimpleGearbox& Settings)
{
	FSLMRepItemSettingsSimpleGearbox& SettingsItem = RepArraySettings.Items.AddDefaulted_GetRef();
	SettingsItem.Handle = Handle;
	SettingsItem.RepSettings = Settings;
	RepArraySettings.MarkItemDirty(SettingsItem);
	FSLMRepItemStateSimpleGearbox& StateItem = RepArrayState.Items.AddDefaulted_GetRef();
	StateItem.Handle = Handle;
	StateItem.State.Gear = Settings.DeviceModel.CurrentGear;
	RepArrayState.MarkItemDirty(StateItem);
}

void ASLMDeviceReplicatorSimpleGearbox::RemoveItem(const FSLMDeviceHandleSimpleGearbox Handle)
{
	for (int32 i = 0; i < RepArraySettings.Items.Num(); ++i)
	{
		if (RepArraySettings.Items[i].Handle.ID == Handle.ID)
		{
			RepArraySettings.Items.RemoveAt(i);
			RepArraySettings.MarkArrayDirty();
			break;
		}
	}
	for (int32 i = 0; i < RepArrayState.Items.Num(); ++i)
	{
		if (RepArrayState.Items[i].Handle.ID == Handle.ID)
		{
			RepArrayState.Items.RemoveAt(i);
			RepArrayState.MarkArrayDirty();
			break;
		}
	}
}

void ASLMDeviceReplicatorSimpleGearbox::EditItem(const FSLMDeviceHandleSimpleGearbox Handle, const FSLMDeviceSettingsSimpleGearbox& Settings)
{
	for (auto& Item : RepArraySettings.Items)
	{
		if (Item.Handle.ID == Handle.ID)
		{
			Item.RepSettings = Settings;
			RepArraySettings.MarkItemDirty(Item);
			break;
		}
	}
}

void ASLMDeviceReplicatorSimpleGearbox::PullDynamicState(TSparseArray<FSLMDeviceModelSimpleGearbox>& DeviceModels)
{
	for (auto& Item : RepArrayState.Items)
	{
		const auto Handle = Item.Handle;
		if (DeviceModels.IsValidIndex(Handle.ID))
		{
			auto& Model = DeviceModels[Handle.ID];
			if (Model.bDirty)
			{
				Item.State.Gear = Model.CurrentGear;
				RepArrayState.MarkItemDirty(Item);
				Model.bDirty = false;
			}
		}
	}
}

FSLMDeviceHandleSimpleGearbox USLMBPFLSimpleGearbox::AddDeviceSimpleGearbox(const UObject* WorldContextObject, const FSLMDeviceSettingsSimpleGearbox& Settings)
{
	return WorldContextObject->GetWorld()->GetSubsystem<USLMDeviceSubsystemSimpleGearbox>()->AddDevice(Settings);
}

void USLMBPFLSimpleGearbox::RemoveDevice(const UObject* WorldContextObject, const FSLMDeviceHandleSimpleGearbox Handle)
{
	WorldContextObject->GetWorld()->GetSubsystem<USLMDeviceSubsystemSimpleGearbox>()->RemoveDevice_Impl(Handle);
}

void USLMBPFLSimpleGearbox::EditDeviceSettings(const UObject* WorldContextObject, const FSLMDeviceHandleSimpleGearbox Handle, const FSLMDeviceSettingsSimpleGearbox& Settings)
{
	WorldContextObject->GetWorld()->GetSubsystem<USLMDeviceSubsystemSimpleGearbox>()->SetDeviceSettings_Impl(Handle, Settings);
}

void USLMBPFLSimpleGearbox::ApplyInput(const UObject* WorldContextObject, const FSLMDeviceHandleSimpleGearbox Handle, const FSLMDeviceInputSimpleGearbox& Input)
{
	WorldContextObject->GetWorld()->GetSubsystem<USLMDeviceSubsystemSimpleGearbox>()->ApplyInput_Impl(Handle, Input);
}

FSLMDeviceCosmeticStateSimpleGearbox USLMBPFLSimpleGearbox::GetCosmeticState(const UObject* WorldContextObject, const FSLMDeviceHandleSimpleGearbox Handle)
{
	return WorldContextObject->GetWorld()->GetSubsystem<USLMDeviceSubsystemSimpleGearbox>()->GetCosmeticState_Impl(Handle);
}

FSLMDeviceSettingsSimpleGearbox USLMBPFLSimpleGearbox::GetDeviceSettings(const UObject* WorldContextObject, const FSLMDeviceHandleSimpleGearbox Handle)
{
	return WorldContextObject->GetWorld()->GetSubsystem<USLMDeviceSubsystemSimpleGearbox>()->GetDeviceSettings_Impl(Handle);
}

FSLMDevicePortAddressesSimpleGearbox USLMBPFLSimpleGearbox::GetPortAddresses(const UObject* WorldContextObject, const FSLMDeviceHandleSimpleGearbox Handle)
{
	return WorldContextObject->GetWorld()->GetSubsystem<USLMDeviceSubsystemSimpleGearbox>()->GetPortAddresses_Impl(Handle);
}


/*
void USLMDeviceComponentSimpleGearbox::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(USLMDeviceComponentSimpleGearbox, Handle);
}

FSLMDeviceCosmeticStateSimpleGearbox USLMDeviceComponentSimpleGearbox::GetDeviceCosmeticState()
{
	return Subsystem->GetCosmeticState({Handle});
}

void USLMDeviceComponentSimpleGearbox::BeginPlay()
{
	Super::BeginPlay();
	Subsystem = GetWorld()->GetSubsystem<USLMDeviceSubsystemSimpleGearbox>();
	if (GetOwner()->HasAuthority())
	{
		Handle = Subsystem->AddDevice(DeviceSettings);
	}
}

void USLMDeviceComponentSimpleGearbox::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (GetOwner()->HasAuthority())
	{
		Subsystem->RemoveDevice({Handle});
	}
	Super::EndPlay(EndPlayReason);
}
*/
