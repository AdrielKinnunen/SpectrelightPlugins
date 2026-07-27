// Copyright Spectrelight Studios, LLC

#pragma once
#include "SLMDeviceBase.h"
#include "SLMManager.h"



template<typename SystemType, typename Traits>
class TSLMDeviceSystem : public FSLMDeviceSystemBase
{
	using FDescriptorType		= Traits::FDescriptorType;
	using FSettingsType			= Traits::FSettingsType;
	using FStateType			= Traits::FStateType;
	using FGameplayContextType	= Traits::FGameplayContextType;
	using FAddressesType		= Traits::FAddressesType;
	using FRepStateType			= Traits::FRepStateType;
	
public:	
	
	virtual TUniquePtr<FSLMDeviceSystemBase> CreateInstance() const override
	{
		return MakeUnique<SystemType>();
	}

	virtual FSLMDeviceAddress AddDeviceBySnapshot(const FSLMDeviceSnapshot& DeviceSnapshot, AActor* AssociatedActor) override
	{
		const FDescriptorType* DescriptorPtr = DeviceSnapshot.DeviceDescriptor.GetPtr<FDescriptorType>();
		check(DescriptorPtr);
		FDescriptorType Descriptor = *DescriptorPtr;
		Descriptor.AssociatedActor = AssociatedActor;
		const FSLMDeviceAddress DeviceAddress = DeviceSnapshot.DeviceAddress;
		return AddEditDevice(Descriptor, DeviceAddress);
	}

	FSLMDeviceAddress AddEditDevice(const FDescriptorType& Descriptor, const FSLMDeviceAddress& ExplicitAddress = FSLMDeviceAddress())
	{
		if (DevicePendingRemoval(ExplicitAddress))
		{
			DeviceAddressesToRemove.Remove(ExplicitAddress);
		}
		
		const bool bAlreadyExists = DeviceExists(ExplicitAddress);
		const bool bHasExplicitAddress = ExplicitAddress.DeviceID != INDEX_NONE;
		const bool bRemap = Manager->bRemappingContextOpen;
		
		const bool bServerFreshAdd = bIsServer && !bHasExplicitAddress && !bRemap && !bAlreadyExists;
		const bool bServerRemapAdd = bIsServer && bHasExplicitAddress && bRemap;// && !bAlreadyExists;
		const bool bServerEdit = bIsServer && bHasExplicitAddress && !bRemap && bAlreadyExists;
		const bool bClientEmplaceAdd = !bIsServer && bHasExplicitAddress && !bRemap && !bAlreadyExists;
		const bool bClientEdit = !bIsServer && bHasExplicitAddress && !bRemap && bAlreadyExists;
		
		FSLMDeviceAddress DeviceAddress;
		DeviceAddress.DeviceTag = SystemTag;
		
		if (bServerFreshAdd)
		{
			DeviceAddress.DeviceID = AddInstance(Descriptor);
			OnDeviceAdded.Broadcast(DeviceAddress);
			UE_LOG(LogTemp, Warning, TEXT("Server added device at ID %i"), DeviceAddress.DeviceID);
		}
		else if (bServerRemapAdd)
		{
			DeviceAddress.DeviceID = AddInstance(Descriptor);
			OnDeviceAdded.Broadcast(DeviceAddress);
			Manager->OldAddressToNewAddress.Add(ExplicitAddress, DeviceAddress);
			UE_LOG(LogTemp, Warning, TEXT("Server remapped device ID %i to ID %i"), ExplicitAddress.DeviceID, DeviceAddress.DeviceID);
		}
		else if (bClientEmplaceAdd)
		{
			DeviceAddress = ExplicitAddress;
			EmplaceInstance(Descriptor, DeviceAddress.DeviceID);
			OnDeviceAdded.Broadcast(DeviceAddress);
			UE_LOG(LogTemp, Warning, TEXT("Client emplaced device at ID %i"), DeviceAddress.DeviceID);
		}
		else if (bServerEdit)
		{
			DeviceAddress = ExplicitAddress;
			DescriptorArray[DeviceAddress.DeviceID] = Descriptor;
			SettingsArray[DeviceAddress.DeviceID] = Descriptor.Settings;
			UE_LOG(LogTemp, Warning, TEXT("Server edited device at ID %i"), DeviceAddress.DeviceID);
		}
		else if (bClientEdit)
		{
			DeviceAddress = ExplicitAddress;
			DescriptorArray[DeviceAddress.DeviceID] = Descriptor;
			SettingsArray[DeviceAddress.DeviceID] = Descriptor.Settings;
			UE_LOG(LogTemp, Warning, TEXT("Client edited device at ID %i"), DeviceAddress.DeviceID);
		}
		else
		{
			checkNoEntry();
		}		
		
		//Pass Descriptor to Replicator
		if (bIsServer)
		{
			RepSetDescriptor(DeviceAddress, Descriptor);
		}
		
		//Port Registration
		const FAddressesType PortAddresses = GetPortAddresses(DeviceAddress);
		GetSystem()->RegisterPorts_Impl(DeviceAddress.DeviceID, PortAddresses);
		
		//Apply pending rep state
		if (PendingRepStateArray.IsValidIndex(DeviceAddress.DeviceID))
		{
			GetSystem()->ApplyRepState_Impl(DeviceAddress.DeviceID, PendingRepStateArray[DeviceAddress.DeviceID]);
			PendingRepStateArray.RemoveAt(DeviceAddress.DeviceID);
		}
		
		return DeviceAddress;
	}

	FDescriptorType GetDescriptor(const FSLMDeviceAddress& DeviceAddress) const
	{
		FDescriptorType Result;
		if (DeviceExists(DeviceAddress))
		{
			Result = DescriptorArray[DeviceAddress.DeviceID];
		}
		return Result;
	}

	FSettingsType GetModelSettings(const FSLMDeviceAddress& DeviceAddress) const
	{
		FSettingsType Result;
		if (DeviceExists(DeviceAddress))
		{
			Result = SettingsArray[DeviceAddress.DeviceID];
		}
		return Result;
	}

	virtual FSLMDeviceSnapshot GetDeviceSnapshot(const FSLMDeviceAddress& DeviceAddress) const override
	{
		FSLMDeviceSnapshot Result;
		if (DeviceExists(DeviceAddress))
		{
			Result.DeviceAddress = DeviceAddress;
			Result.DeviceDescriptor.InitializeAs<FDescriptorType>(DescriptorArray[DeviceAddress.DeviceID]);
		}
		return Result;
	}


	void RemoveDevice(const FSLMDeviceAddress& DeviceAddress)
	{
		//const bool bIsServer = Manager->GetWorld()->GetNetMode() != NM_Client;
		const bool bDeviceExists = DeviceExists(DeviceAddress);
		const bool bPendingRemoval = DevicePendingRemoval(DeviceAddress);
		
		if (bPendingRemoval)
		{
			UE_LOG(LogTemp, Warning, TEXT("Tried to remove device ID %i which is already pending removal."), DeviceAddress.DeviceID);
			return;
		}
		
		if (!bDeviceExists)
		{
			UE_LOG(LogTemp, Warning, TEXT("Tried to remove device ID %i which doesn't exist."), DeviceAddress.DeviceID);
			return;
		}
		
		if (bIsServer)
		{
			UE_LOG(LogTemp, Warning, TEXT("Server removing device ID %i."), DeviceAddress.DeviceID);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Client removing device ID %i."), DeviceAddress.DeviceID);
		}
		
		
		
		DeviceAddressesToRemove.Add(DeviceAddress);
		bNeedsCleanUp = true;
		
		if (bIsServer)
		{
			RepRemoveDescriptor(DeviceAddress);
		}
	}

	
	
	
	virtual void PreSimulate(const float DeltaTime) override final
	{
		for (auto It = DescriptorArray.CreateIterator(); It; ++It)
		{
			const int32 ID = It.GetIndex();
			GetSystem()->PreSimulate_Impl(ID, DeltaTime);
		}
	}
	
	virtual void Simulate(const float DeltaTime, const float SubstepScalar) override final
	{
		for (auto It = DescriptorArray.CreateIterator(); It; ++It)
		{
			const int32 ID = It.GetIndex();
			GetSystem()->Simulate_Impl(ID, DeltaTime, SubstepScalar);
		}
	}
	
	virtual void PostSimulate(const float DeltaTime) override final
	{
		for (auto It = DescriptorArray.CreateIterator(); It; ++It)
		{
			const int32 ID = It.GetIndex();
			GetSystem()->PostSimulate_Impl(ID, DeltaTime);
		}
		if (bIsServer)
		{
			PullRepState();			
		}
	}
	
	
	
	
	
	void MarkForReplication(const int32 ID)
	{
		DirtyFlags[ID] = true;
	}
	
	
	
	
	
	
	FAddressesType GetPortAddresses(const FSLMDeviceAddress& DeviceAddress)
	{
		check(DeviceExists(DeviceAddress))
		FAddressesType Result;
		GetSystem()->GetPortAddresses_Impl(DeviceAddress.DeviceID, Result);
		return Result;
	}

	FGameplayContextType& GetGameplayContextRef(const FSLMDeviceAddress& DeviceAddress)
	{
		check(DeviceExists(DeviceAddress))
		return GameplayContextArray[DeviceAddress.DeviceID];
	}
/*
	bool IDExists(const int32 DeviceID) const
	{
		return OccupiedDeviceIDs.IsValidIndex(DeviceID) && OccupiedDeviceIDs[DeviceID];
	}
*/	
	bool DevicePendingRemoval(const FSLMDeviceAddress& DeviceAddress) const
	{
		return DeviceAddressesToRemove.Contains(DeviceAddress);
	}

	virtual uint32 GetDebugHash(const bool bVerbose) override
	{
		uint32 Result = 0;
		for (auto It = DescriptorArray.CreateIterator(); It; ++It)
		{
			const int32 ID = It.GetIndex();
			//Result =  Result ^ HashCombine(GetTypeHash(ID),DescriptorArray[ID].GetDebugHash(), SettingsArray[ID].GetDebugHash(), StateArray[ID].GetDebugHash(), ContextArray[ID].GetDebugHash());
			Result =  Result ^ HashCombine(GetTypeHash(ID),SettingsArray[ID].GetDebugHash(), StateArray[ID].GetDebugHash());
			
		}
		return Result;
	}

	virtual FString GetDebugString(const bool Verbose) override
	{
		///////////////////////////////TODO: IMPLEMENT THIS////////////////////////////////////
		FString Result;
		return Result;
	}

	
	
	
	
	
	
	
	
	
	virtual void OnRepSetDescriptor(const FSLMDeviceAddress& DeviceAddress, const FInstancedStruct& Payload) override
	{
		const FDescriptorType* DescriptorPtr = Payload.GetPtr<FDescriptorType>();
		check(DescriptorPtr);
		//FDescriptorType& Descriptor = *DescriptorPtr;
		FDescriptorType Descriptor = *DescriptorPtr;
		AddEditDevice(Descriptor, DeviceAddress);
	}

	virtual void OnRepRemoveDescriptor(const FSLMDeviceAddress& DeviceAddress) override
	{
		RemoveDevice(DeviceAddress);
	}

	virtual void OnRepSetState(const FSLMDeviceAddress& DeviceAddress, const FInstancedStruct& Payload) override
	{
		const FRepStateType* RepStatePtr = Payload.GetPtr<FRepStateType>();
		check(RepStatePtr);
		const FRepStateType& RepState = *RepStatePtr;
		
		if (DeviceExists(DeviceAddress))
		{
			GetSystem()->ApplyRepState_Impl(DeviceAddress.DeviceID, RepState);			
		}
		else
		{
			PendingRepStateArray.EmplaceAt(DeviceAddress.DeviceID, RepState);
		}
	}

	virtual void OnRepRemoveState(const FSLMDeviceAddress& DeviceAddress) override
	{
	}

	
	
	
	
	
	
	
	
	
private:
	SystemType* GetSystem()
	{
		return static_cast<SystemType*>(this);
	}

	const SystemType* GetSystem() const
	{
		return static_cast<const SystemType*>(this);
	}

	virtual void CleanUp() override
	{
		for (const auto& DeviceAddress : DeviceAddressesToRemove)
		{
			if (DeviceExists(DeviceAddress))
			{
				const FAddressesType PortAddresses = GetPortAddresses(DeviceAddress);
				GetSystem()->RemovePorts_Impl(PortAddresses);
				RemoveInstance(DeviceAddress.DeviceID);
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Tried to clean up an invalid device address"));
			}
		}
		DeviceAddressesToRemove.Reset(16);
	}

	int32 AddInstance(const FDescriptorType& Descriptor)
	{
		const int32 ID = DescriptorArray.Add(Descriptor);
		SettingsArray.EmplaceAt(ID, Descriptor.Settings);
		StateArray.EmplaceAt(ID, FStateType());
		GameplayContextArray.EmplaceAt(ID, FGameplayContextType());
		DirtyFlags.PadToNum(ID + 1, false);
		DirtyFlags[ID] = false;
		OccupiedDeviceIDs.PadToNum(ID + 1, false);
		OccupiedDeviceIDs[ID] = true;
		return ID;
	}
	
	void EmplaceInstance(const FDescriptorType& Descriptor, const int32 ID)
	{
		DescriptorArray.EmplaceAt(ID, Descriptor);
		SettingsArray.EmplaceAt(ID, Descriptor.Settings);
		StateArray.EmplaceAt(ID, FStateType());
		GameplayContextArray.EmplaceAt(ID, FGameplayContextType());
		DirtyFlags.PadToNum(ID + 1, false);
		DirtyFlags[ID] = false;
		OccupiedDeviceIDs.PadToNum(ID + 1, false);
		OccupiedDeviceIDs[ID] = true;
	}
	
	void RemoveInstance(const int32 ID)
	{
		DescriptorArray.RemoveAt(ID);
		SettingsArray.RemoveAt(ID);
		StateArray.RemoveAt(ID);
		GameplayContextArray.RemoveAt(ID);
		DirtyFlags[ID] = false;
		OccupiedDeviceIDs[ID] = false;
	}
	
	void PullRepState()
	{
		for (TConstSetBitIterator<> It(DirtyFlags); It; ++It)
		{
			const int32 ID = It.GetIndex();
			
			FRepStateType RepState;
			GetSystem()->GetRepState_Impl(ID, RepState);
			FInstancedStruct Payload;
			Payload.InitializeAs<FRepStateType>(RepState);
			FSLMDeviceAddress DeviceAddress = {SystemTag, ID};
			Manager->Replicator->SetState(DeviceAddress, Payload);
		}
		DirtyFlags.Init(false, DirtyFlags.Num());
	}
	
	void RepSetDescriptor(const FSLMDeviceAddress& DeviceAddress, const FDescriptorType& Descriptor) const
	{
		//check(Manager->Replicator);
		FInstancedStruct Payload;
		Payload.InitializeAs<FDescriptorType>(Descriptor);
		Manager->Replicator->SetDescriptor(DeviceAddress, Payload);
	}
	
	void RepRemoveDescriptor(const FSLMDeviceAddress& DeviceAddress) const
	{
		//check(Manager->Replicator);
		Manager->Replicator->RemoveDescriptor(DeviceAddress);
	}

protected:
	TSparseArray<FDescriptorType> DescriptorArray;
	TSparseArray<FSettingsType> SettingsArray;
	TSparseArray<FStateType> StateArray;
	TSparseArray<FGameplayContextType> GameplayContextArray;

private:
	TArray<FSLMDeviceAddress> DeviceAddressesToRemove;
	TSparseArray<FRepStateType> PendingRepStateArray;
	TBitArray<> DirtyFlags;
};
