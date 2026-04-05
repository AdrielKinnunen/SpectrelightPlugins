// Copyright Spectrelight Studios, LLC

#include "Devices/SLMDeviceSimpleGearbox.h"
#include "Net/UnrealNetwork.h"


void FSLMRepArraySimpleGearbox::PostReplicatedAdd(const TArrayView<int32>& AddedIndices, int32 FinalSize) const
{
	for (const auto Index : AddedIndices)
	{
		const FSLMRepItemSimpleGearbox& Item = Items[Index];
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

void FSLMRepArraySimpleGearbox::PostReplicatedChange(const TArrayView<int32>& ChangedIndices, int32 FinalSize) const
{
	for (const auto Index : ChangedIndices)
	{
		const FSLMRepItemSimpleGearbox& Item = Items[Index];
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

void FSLMRepArraySimpleGearbox::PreReplicatedRemove(const TArrayView<int32>& RemovedIndices, int32 FinalSize) const
{
	for (const auto Index : RemovedIndices)
	{
		const FSLMRepItemSimpleGearbox& Item = Items[Index];
		if (Subsystem->IsValidHandle(Item.Handle))
		{
			Subsystem->RemoveDevice(Item.Handle);
		}
	}
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
}

FSLMDeviceHandleSimpleGearbox USLMDeviceSubsystemSimpleGearbox::AddDevice(const FSLMDeviceSettingsSimpleGearbox& Settings, const FSLMDeviceHandleSimpleGearbox ExplicitHandle)
{
	FSLMDeviceHandleSimpleGearbox Handle;
	if (GetWorld()->GetNetMode() < NM_Client && Replicator)
	{
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
	const auto Addresses = GetPortAddresses(Handle);
	Model.ParticleID_Rotation_Input = DomainRotation->AddPort(Settings.Port_Rotation_Input, Addresses.Address_Rotation_Input);
	Model.ParticleID_Rotation_Output = DomainRotation->AddPort(Settings.Port_Rotation_Output, Addresses.Address_Rotation_Output);
	return Handle;
}


FSLMDeviceCosmeticStateSimpleGearbox USLMDeviceSubsystemSimpleGearbox::GetCosmeticState(const FSLMDeviceHandleSimpleGearbox Handle) const
{
	FSLMDeviceCosmeticStateSimpleGearbox Result;
	if (IsValidHandle(Handle))
	{
		const auto& Model = DeviceModels[Handle.ID]; 
		Result.CurrentGear = Model.CurrentGear;
		Result.CurrentGearRatio = Model.GearRatio;
	}
	return Result;
}


FSLMDeviceSettingsSimpleGearbox USLMDeviceSubsystemSimpleGearbox::GetDeviceSettings(const FSLMDeviceHandleSimpleGearbox Handle) const
{
	FSLMDeviceSettingsSimpleGearbox Result;
	if (IsValidHandle(Handle))
	{
		Result.DeviceModel = DeviceModels[Handle.ID];
	}
	return Result;
}

FSLMDevicePortAddressesSimpleGearbox USLMDeviceSubsystemSimpleGearbox::GetPortAddresses(const FSLMDeviceHandleSimpleGearbox Handle) const
{
	FSLMDevicePortAddressesSimpleGearbox Result;
	Result.Address_Rotation_Input.DeviceClass = this->StaticClass();
	Result.Address_Rotation_Output.DeviceClass = this->StaticClass();
	Result.Address_Rotation_Input.DeviceID = Handle.ID;
	Result.Address_Rotation_Output.DeviceID = Handle.ID;
	Result.Address_Rotation_Input.PortIndex = 0;
	Result.Address_Rotation_Output.PortIndex = 1;
	return Result;
}

void USLMDeviceSubsystemSimpleGearbox::EditDeviceSettings(const FSLMDeviceHandleSimpleGearbox Handle, const FSLMDeviceSettingsSimpleGearbox& Settings)
{
	if (!IsValidHandle(Handle))
	{
		return;
	}
	auto& Model = DeviceModels[Handle.ID];
	Model.NumForwardGears = Settings.DeviceModel.NumForwardGears;
	Model.NumReverseGears = Settings.DeviceModel.NumReverseGears;
	Model.FirstGearRatio = Settings.DeviceModel.FirstGearRatio;
	Model.RatioBetweenGears = Settings.DeviceModel.RatioBetweenGears;
	Model.GearSpreadExponent = Settings.DeviceModel.GearSpreadExponent;
	Model.CurrentGear = FMath::Clamp(Settings.DeviceModel.CurrentGear, -Model.NumReverseGears, Model.NumForwardGears);
	if (GetWorld()->GetNetMode() != NM_Client && Replicator)
	{
		UE_LOG(LogTemp, Error, TEXT("Server edited ID %i"), Handle.ID);
		Replicator->EditItem(Handle, Settings);
	}
}

void USLMDeviceSubsystemSimpleGearbox::ApplyInput(const FSLMDeviceHandleSimpleGearbox Handle, const FSLMDeviceInputSimpleGearbox& Input)
{
	if (!IsValidHandle(Handle))
	{
		return;
	}
	auto& Model = DeviceModels[Handle.ID];
	Model.CurrentGear = FMath::Clamp(Input.DesiredGear, -Model.NumReverseGears, Model.NumForwardGears);
}

void USLMDeviceSubsystemSimpleGearbox::RemoveDevice(const FSLMDeviceHandleSimpleGearbox Handle)
{
	if (IsValidHandle(Handle))
	{
		const FSLMPortAddress InputAddress = {this->StaticClass(), Handle.ID, 0};
		const FSLMPortAddress OutputAddress = {this->StaticClass(), Handle.ID, 1};
		DomainRotation->RemovePort(InputAddress);
		DomainRotation->RemovePort(OutputAddress);
		DeviceModels.RemoveAt(Handle.ID);		
	}
	if (GetWorld()->GetNetMode() != NM_Client && Replicator)
	{
		UE_LOG(LogTemp, Error, TEXT("Server removed ID %i"), Handle.ID);
		Replicator->RemoveItem(Handle);
	}
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
		Replicator->UpdateItems(DeviceModels);
	}
}

FString USLMDeviceSubsystemSimpleGearbox::GetDebugString()
{
	FString Result;
	Result += "SimpleGearbox";
	for (int32 i = 0; i < DeviceModels.GetMaxIndex(); i++)
	{
		if (DeviceModels.IsValidIndex(i))
		{
			Result += "\n";
			Result += FString::Printf(TEXT("Model %i has state: "), i);
			Result += DeviceModels[i].GetDebugString();
		}
	}
	Result += "\n";
	return Result;
}

uint32 USLMDeviceSubsystemSimpleGearbox::GetDebugHash()
{
	uint32 Result = 0;
	for (int32 i = 0; i < DeviceModels.GetMaxIndex(); i++)
	{
		if (DeviceModels.IsValidIndex(i))
		{
			const uint32 IDHash = GetTypeHash(i);
			const uint32 DataHash = GetTypeHash(DeviceModels[i]);
			const uint32 CombinedHash = HashCombine(IDHash, DataHash);
			Result += CombinedHash;
		}
	}
	return Result;
}

bool USLMDeviceSubsystemSimpleGearbox::IsValidHandle(const FSLMDeviceHandleSimpleGearbox Handle) const
{
	return DeviceModels.IsValidIndex(Handle.ID);
}

ASLMDeviceReplicatorSimpleGearbox::ASLMDeviceReplicatorSimpleGearbox()
{
	bReplicates = true;
	bAlwaysRelevant = true;
	SetReplicatingMovement(false);
}

void ASLMDeviceReplicatorSimpleGearbox::PostInitializeComponents()
{
	FastArray.Subsystem = GetWorld()->GetSubsystem<USLMDeviceSubsystemSimpleGearbox>();
	Super::PostInitializeComponents();
}

void ASLMDeviceReplicatorSimpleGearbox::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASLMDeviceReplicatorSimpleGearbox, FastArray);
}

void ASLMDeviceReplicatorSimpleGearbox::AddItem(const FSLMDeviceHandleSimpleGearbox Handle, const FSLMDeviceSettingsSimpleGearbox& Settings)
{
	FSLMRepItemSimpleGearbox& Item = FastArray.Items.AddDefaulted_GetRef();
	Item.Handle = Handle;
	Item.Settings = Settings;
	FastArray.MarkItemDirty(Item);
}

void ASLMDeviceReplicatorSimpleGearbox::RemoveItem(const FSLMDeviceHandleSimpleGearbox Handle)
{
	for (int32 i = 0; i < FastArray.Items.Num(); ++i)
	{
		if (FastArray.Items[i].Handle.ID == Handle.ID)
		{
			FastArray.Items.RemoveAt(i);
			FastArray.MarkArrayDirty();
			break;
		}
	}
}

void ASLMDeviceReplicatorSimpleGearbox::EditItem(const FSLMDeviceHandleSimpleGearbox Handle, const FSLMDeviceSettingsSimpleGearbox& Settings)
{
	for (auto& Item : FastArray.Items)
	{
		if (Item.Handle.ID == Handle.ID)
		{
			Item.Settings = Settings;
			FastArray.MarkItemDirty(Item);
			break;
		}
	}
}

void ASLMDeviceReplicatorSimpleGearbox::UpdateItems(const TSparseArray<FSLMDeviceModelSimpleGearbox>& DeviceModels)
{
	for (auto& Item : FastArray.Items)
	{
		const auto Handle = Item.Handle;
		if (DeviceModels.IsValidIndex(Handle.ID))
		{
			auto& Model = DeviceModels[Handle.ID];
			if (Item.Settings.DeviceModel.CurrentGear != Model.CurrentGear)
			{
				Item.Settings.DeviceModel.CurrentGear = Model.CurrentGear;
				FastArray.MarkItemDirty(Item);
			}
		}
	}
}

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