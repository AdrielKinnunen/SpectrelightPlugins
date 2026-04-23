// Copyright Spectrelight Studios, LLC

#include "Devices/SLMDeviceProbe.h"
#include "Net/UnrealNetwork.h"


void FSLMRepArraySettingsProbe::PostReplicatedAdd(const TArrayView<int32>& AddedIndices, int32 FinalSize) const
{
	for (const auto Index : AddedIndices)
	{
		const FSLMRepItemSettingsProbe& Item = Items[Index];
		if (Subsystem->IsValidHandle(Item.Handle))
		{
			Subsystem->EditDeviceSettings(Item.Handle, Item.Settings);
		}
		else
		{
			Subsystem->AddDevice(Item.Settings, Item.Handle);
		}
	}
}

void FSLMRepArraySettingsProbe::PostReplicatedChange(const TArrayView<int32>& ChangedIndices, int32 FinalSize) const
{
	for (const auto Index : ChangedIndices)
	{
		const FSLMRepItemSettingsProbe& Item = Items[Index];
		if (Subsystem->IsValidHandle(Item.Handle))
		{
			Subsystem->EditDeviceSettings(Item.Handle, Item.Settings);
		}
		else
		{
			Subsystem->AddDevice(Item.Settings, Item.Handle);
		}
	}
}

void FSLMRepArraySettingsProbe::PreReplicatedRemove(const TArrayView<int32>& RemovedIndices, int32 FinalSize) const
{
	for (const auto Index : RemovedIndices)
	{
		const FSLMRepItemSettingsProbe& Item = Items[Index];
		if (Subsystem->IsValidHandle(Item.Handle))
		{
			Subsystem->RemoveDevice(Item.Handle);
		}
	}
}

void FSLMRepArrayStateProbe::PostReplicatedAdd(const TArrayView<int32>& AddedIndices, int32 FinalSize) const
{
	for (const auto Index : AddedIndices)
	{
		const FSLMRepItemStateProbe& Item = Items[Index];
		Subsystem->ApplyReplicatedState(Item.Handle, Item.State);
	}
}

void FSLMRepArrayStateProbe::PostReplicatedChange(const TArrayView<int32>& ChangedIndices, int32 FinalSize) const
{
	for (const auto Index : ChangedIndices)
	{
		const FSLMRepItemStateProbe& Item = Items[Index];
		Subsystem->ApplyReplicatedState(Item.Handle, Item.State);
	}
}

void FSLMRepArrayStateProbe::PreReplicatedRemove(const TArrayView<int32>& RemovedIndices, int32 FinalSize) const
{
}

void USLMDeviceSubsystemProbe::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
	if (GetWorld()->GetNetMode() != NM_Client)
	{
		Replicator = InWorld.SpawnActor<ASLMDeviceReplicatorProbe>();
	}
}

void USLMDeviceSubsystemProbe::PostInitialize()
{
	Super::PostInitialize();
	DomainRotation = GetWorld()->GetSubsystem<USLMDomainRotation>();
	DomainSignal = GetWorld()->GetSubsystem<USLMDomainSignal>();
}

FSLMDeviceHandleProbe USLMDeviceSubsystemProbe::AddDevice(const FSLMDeviceSettingsProbe& Settings, const FSLMDeviceHandleProbe ExplicitHandle)
{
	FSLMDeviceHandleProbe Handle;
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
	FSLMDeviceModelProbe& Model = DeviceModels[Handle.ID];
	const FSLMDevicePortAddressesProbe PortAddresses = GetPortAddresses(Handle);
	Model.PortID_Rotation_Output = DomainRotation->AddPort(Settings.Port_Rotation_Output, PortAddresses.Address_Rotation_Output);
	Model.PortID_Signal_Output = DomainSignal->AddPort(Settings.Port_Signal_Output, PortAddresses.Address_Signal_Output);
	if (OrphanedRepStates.IsValidIndex(Handle.ID))
	{
		ApplyReplicatedState(Handle, OrphanedRepStates[Handle.ID]);
	}
	return Handle;
}


FSLMDeviceCosmeticStateProbe USLMDeviceSubsystemProbe::GetCosmeticState(const FSLMDeviceHandleProbe Handle) const
{
	FSLMDeviceCosmeticStateProbe Result;
	if (IsValidHandle(Handle))
	{
		const auto& Model = DeviceModels[Handle.ID];
		Result.ProbeValue = Model.ProbeValue;
	}
	return Result;
}


FSLMDeviceSettingsProbe USLMDeviceSubsystemProbe::GetDeviceSettings(const FSLMDeviceHandleProbe Handle) const
{
	FSLMDeviceSettingsProbe Result = FSLMDeviceSettingsProbe();
	if (IsValidHandle(Handle))
	{
		Result.DeviceModel = DeviceModels[Handle.ID];
	}
	return Result;
}

FSLMDevicePortAddressesProbe USLMDeviceSubsystemProbe::GetPortAddresses(const FSLMDeviceHandleProbe Handle) const
{
	FSLMDevicePortAddressesProbe Result;
	Result.Address_Rotation_Output	= MakePortAddress(this, DomainRotation, Handle.ID, 0);
	Result.Address_Signal_Output	= MakePortAddress(this, DomainSignal, Handle.ID, 0);
	return Result;
}

void USLMDeviceSubsystemProbe::EditDeviceSettings(const FSLMDeviceHandleProbe Handle, const FSLMDeviceSettingsProbe& Settings)
{
	if (!IsValidHandle(Handle))
	{
		return;
	}
	auto& Model = DeviceModels[Handle.ID];
	Model.ProbeValue = Settings.DeviceModel.ProbeValue;
	if (GetWorld()->GetNetMode() != NM_Client)
	{
		check(Replicator);
		Replicator->EditItem(Handle, Settings);
	}
}

void USLMDeviceSubsystemProbe::ApplyReplicatedState(const FSLMDeviceHandleProbe Handle, const FSLMDeviceRepStateProbe& State)
{
	if (!IsValidHandle(Handle))
	{
		OrphanedRepStates.EmplaceAt(Handle.ID, State);
		return;
	}
	auto& Model = DeviceModels[Handle.ID];
	Model.ProbeValue = State.ProbeValue;
}

void USLMDeviceSubsystemProbe::ApplyInput(const FSLMDeviceHandleProbe Handle, const FSLMDeviceInputProbe& Input)
{
	if (!IsValidHandle(Handle))
	{
		return;
	}
	auto& Model = DeviceModels[Handle.ID];
	Model.ProbeValue = Input.ProbeValue;
}

void USLMDeviceSubsystemProbe::RemoveDevice(const FSLMDeviceHandleProbe Handle)
{
	if (IsValidHandle(Handle))
	{
		const FSLMDevicePortAddressesProbe PortAddresses = GetPortAddresses(Handle);
		DomainRotation->RemovePort(PortAddresses.Address_Rotation_Output);
		DomainSignal->RemovePort(PortAddresses.Address_Signal_Output);
		DeviceModels.RemoveAt(Handle.ID);		
	}
	if (GetWorld()->GetNetMode() != NM_Client && Replicator)
	{
		Replicator->RemoveItem(Handle);
	}
}

void USLMDeviceSubsystemProbe::PreSimulate(const float DeltaTime)
{
}

void USLMDeviceSubsystemProbe::Simulate(const float DeltaTime, const float SubstepScalar)
{
	for (auto& Model : DeviceModels)
	{
		DomainSignal->WriteValue(Model.PortID_Signal_Output, Model.ProbeValue);
		DomainRotation->AddTorque(Model.PortID_Rotation_Output, Model.ProbeValue * 100, DeltaTime);
		Model.bDirty = true;
	}
}

void USLMDeviceSubsystemProbe::PostSimulate(const float DeltaTime)
{
	if (GetWorld()->GetNetMode() != NM_Client && Replicator)
	{
		Replicator->PullDynamicState(DeviceModels);
	}
}

FString USLMDeviceSubsystemProbe::GetDebugString(const bool Verbose)
{
	FString Result;
	Result += "\n------------------Probe------------------";
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

uint32 USLMDeviceSubsystemProbe::GetDebugHash()
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

bool USLMDeviceSubsystemProbe::IsValidHandle(const FSLMDeviceHandleProbe Handle) const
{
	return DeviceModels.IsValidIndex(Handle.ID);
}


ASLMDeviceReplicatorProbe::ASLMDeviceReplicatorProbe()
{
	bReplicates = true;
	bAlwaysRelevant = true;
	SetReplicatingMovement(false);
}

void ASLMDeviceReplicatorProbe::PostInitializeComponents()
{
	RepArraySettings.Subsystem = GetWorld()->GetSubsystem<USLMDeviceSubsystemProbe>();
	RepArrayState.Subsystem = GetWorld()->GetSubsystem<USLMDeviceSubsystemProbe>();
	Super::PostInitializeComponents();
}

void ASLMDeviceReplicatorProbe::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASLMDeviceReplicatorProbe, RepArraySettings);
	DOREPLIFETIME(ASLMDeviceReplicatorProbe, RepArrayState);
}

void ASLMDeviceReplicatorProbe::AddItem(const FSLMDeviceHandleProbe Handle, const FSLMDeviceSettingsProbe& Settings)
{
	FSLMRepItemSettingsProbe& Item = RepArraySettings.Items.AddDefaulted_GetRef();
	Item.Handle = Handle;
	Item.Settings = Settings;
	RepArraySettings.MarkItemDirty(Item);
	FSLMRepItemStateProbe& StateItem = RepArrayState.Items.AddDefaulted_GetRef();
	StateItem.Handle = Handle;
	StateItem.State.ProbeValue = Settings.DeviceModel.ProbeValue;
	RepArrayState.MarkItemDirty(StateItem);
}

void ASLMDeviceReplicatorProbe::RemoveItem(const FSLMDeviceHandleProbe Handle)
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

void ASLMDeviceReplicatorProbe::EditItem(const FSLMDeviceHandleProbe Handle, const FSLMDeviceSettingsProbe& Settings)
{
	for (auto& Item : RepArraySettings.Items)
	{
		if (Item.Handle.ID == Handle.ID)
		{
			Item.Settings = Settings;
			RepArraySettings.MarkItemDirty(Item);
			break;
		}
	}
}

void ASLMDeviceReplicatorProbe::PullDynamicState(TSparseArray<FSLMDeviceModelProbe>& DeviceModels)
{
	for (auto& Item : RepArrayState.Items)
	{
		const auto Handle = Item.Handle;
		if (DeviceModels.IsValidIndex(Handle.ID))
		{
			auto& Model = DeviceModels[Handle.ID];
			if (Model.bDirty)
			{
				Item.State.ProbeValue = Model.ProbeValue;
				RepArrayState.MarkItemDirty(Item);
				Model.bDirty = false;
			}
		}
	}
}

FSLMDeviceHandleProbe USLMBPFLProbe::AddDeviceProbe(const UObject* WorldContextObject, const FSLMDeviceSettingsProbe& Settings)
{
	check(WorldContextObject);
	return WorldContextObject->GetWorld()->GetSubsystem<USLMDeviceSubsystemProbe>()->AddDevice(Settings);
}

void USLMBPFLProbe::RemoveDevice(const UObject* WorldContextObject, const FSLMDeviceHandleProbe Handle)
{
	check(WorldContextObject);
	WorldContextObject->GetWorld()->GetSubsystem<USLMDeviceSubsystemProbe>()->RemoveDevice(Handle);
}

void USLMBPFLProbe::EditDeviceSettings(const UObject* WorldContextObject, const FSLMDeviceHandleProbe Handle, const FSLMDeviceSettingsProbe& Settings)
{
	check(WorldContextObject);
	WorldContextObject->GetWorld()->GetSubsystem<USLMDeviceSubsystemProbe>()->EditDeviceSettings(Handle, Settings);
}

void USLMBPFLProbe::ApplyInput(const UObject* WorldContextObject, const FSLMDeviceHandleProbe Handle, const FSLMDeviceInputProbe& Input)
{
	check(WorldContextObject);
	WorldContextObject->GetWorld()->GetSubsystem<USLMDeviceSubsystemProbe>()->ApplyInput(Handle, Input);
}

FSLMDeviceCosmeticStateProbe USLMBPFLProbe::GetCosmeticState(const UObject* WorldContextObject, const FSLMDeviceHandleProbe Handle)
{
	check(WorldContextObject);
	return WorldContextObject->GetWorld()->GetSubsystem<USLMDeviceSubsystemProbe>()->GetCosmeticState(Handle);
}

FSLMDeviceSettingsProbe USLMBPFLProbe::GetDeviceSettings(const UObject* WorldContextObject, const FSLMDeviceHandleProbe Handle)
{
	check(WorldContextObject);
	return WorldContextObject->GetWorld()->GetSubsystem<USLMDeviceSubsystemProbe>()->GetDeviceSettings(Handle);
}

FSLMDevicePortAddressesProbe USLMBPFLProbe::GetPortAddresses(const UObject* WorldContextObject, const FSLMDeviceHandleProbe Handle)
{
	check(WorldContextObject);
	return WorldContextObject->GetWorld()->GetSubsystem<USLMDeviceSubsystemProbe>()->GetPortAddresses(Handle);
}


/*
void USLMDeviceComponentProbe::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(USLMDeviceComponentProbe, Handle);
}

FSLMDeviceCosmeticStateProbe USLMDeviceComponentProbe::GetDeviceCosmeticState()
{
	return Subsystem->GetCosmeticState({Handle});
}

void USLMDeviceComponentProbe::BeginPlay()
{
	Super::BeginPlay();
	Subsystem = GetWorld()->GetSubsystem<USLMDeviceSubsystemProbe>();
	if (GetOwner()->HasAuthority())
	{
		Handle = Subsystem->AddDevice(DeviceSettings);
	}
}

void USLMDeviceComponentProbe::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (GetOwner()->HasAuthority())
	{
		Subsystem->RemoveDevice({Handle});
	}
	Super::EndPlay(EndPlayReason);
}
*/
