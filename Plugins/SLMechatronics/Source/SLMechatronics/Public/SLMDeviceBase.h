// Copyright Spectrelight Studios, LLC

#pragma once

#include "CoreMinimal.h"
#include "SLMManager.h"
#include "SLMSubsystemBase.h"
#include "StructUtils/InstancedStruct.h"
#include "Subsystems/WorldSubsystem.h"
#include "SLMDeviceBase.generated.h"


class USLMDomainSubsystemBase;


UCLASS(Abstract)
class SLMECHATRONICS_API USLMDeviceComponentBase : public UActorComponent
{
    GENERATED_BODY()
public:
    USLMDeviceComponentBase();
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
    //FSLMHandle Handle;
};






UCLASS()
class SLMECHATRONICS_API USLMDeviceSubsystemBase : public USLMSubsystemBase
{
    GENERATED_BODY()
public:
	virtual void Client_AddOrChangeDescriptor(const FSLMDeviceAddress& DeviceAddress, const FInstancedStruct& Payload);
	virtual void Client_RemoveDescriptor(const FSLMDeviceAddress& DeviceAddress);
	virtual void Client_AddOrChangeState(const FSLMDeviceAddress& DeviceAddress, const FInstancedStruct& Payload);
	virtual void Client_RemoveState(const FSLMDeviceAddress& DeviceAddress);
};







template<typename SystemType, typename Traits>
class TSLMDeviceSystem
{
	using FModelSettingsType	= Traits::FModelSettingsType;
	using FModelStateType		= Traits::FModelStateType;
	using FModelType			= Traits::FModelType;
	using FDescriptorType		= Traits::FDescriptorType;
	using FAddressesType		= Traits::FAddressesType;
	using FRepStateType			= Traits::FRepStateType;
	using FCosmeticStateType	= Traits::FCosmeticStateType;
	using FInputType			= Traits::FInputType;
	
public:
	virtual ~TSLMDeviceSystem() = default;
	
	FSLMDeviceAddress AddDevice(const FDescriptorType& Descriptor, const FSLMDeviceAddress& ExplicitAddress = FSLMDeviceAddress())
	{
		const bool bIsServer = GetSystem()->GetWorld()->GetNetMode() != NM_Client;
		const bool bHasExplicitAddress = ExplicitAddress.DeviceID != INDEX_NONE;
		
		FSLMDeviceAddress DeviceAddress;
		DeviceAddress.DeviceClass = GetSystem()->GetClass();
		
		if (bHasExplicitAddress)
		{
			check(ExplicitAddress.DeviceClass == DeviceAddress.DeviceClass);
		}
		
		if (bIsServer)
		{
			if (bHasExplicitAddress)
			{
				USLMManager* Manager = GetSystem()->GetWorld()->template GetSubsystem<USLMManager>();
				check(Manager->bRemappingContextOpen)  //Remapping context better be open if we hit this
				if (DeviceModels.IsValidIndex(ExplicitAddress.DeviceID))
				{
					DeviceAddress.DeviceID = DeviceModels.Add(MakeDeviceModel(Descriptor.ModelSettings));
					Manager->RemappingContext.Add(ExplicitAddress, DeviceAddress);
					AddOrChangeDescriptor(DeviceAddress, Descriptor);					
				}
				else
				{
					DeviceAddress = ExplicitAddress;
					DeviceModels.EmplaceAt(DeviceAddress.DeviceID, MakeDeviceModel(Descriptor.ModelSettings));
					AddOrChangeDescriptor(DeviceAddress, Descriptor);					
				}
			}
			else
			{
				DeviceAddress.DeviceID = DeviceModels.Add(MakeDeviceModel(Descriptor.ModelSettings));
				AddOrChangeDescriptor(DeviceAddress, Descriptor);				
			}
		}
		if (!bIsServer)
		{
			check(ExplicitAddress.DeviceID != INDEX_NONE);
			DeviceAddress = ExplicitAddress;
			DeviceModels.EmplaceAt(DeviceAddress.DeviceID, MakeDeviceModel(Descriptor.ModelSettings));
		}
		
		FModelType& Model = DeviceModels[DeviceAddress.DeviceID];
		const FAddressesType PortAddresses = GetPortAddresses(DeviceAddress);
		GetSystem()->RegisterPorts(Descriptor.PortSettings, Model.State, PortAddresses);
		if (OrphanedRepStates.IsValidIndex(DeviceAddress.DeviceID))
		{
			GetSystem()->RepStateToModel(OrphanedRepStates[DeviceAddress.DeviceID], Model);
			OrphanedRepStates.RemoveAt(DeviceAddress.DeviceID);
		}
		return DeviceAddress;
	}

	void RemoveDevice(const FSLMDeviceAddress& DeviceAddress)
	{
		check(IsValidDeviceAddress(DeviceAddress))
		const bool bIsServer = GetSystem()->GetWorld()->GetNetMode() != NM_Client;
		
		const FAddressesType PortAddresses = GetPortAddresses(DeviceAddress);
		GetSystem()->RemovePorts(PortAddresses);
		DeviceModels.RemoveAt(DeviceAddress.DeviceID);
		if (bIsServer)
		{
			RemoveDescriptor(DeviceAddress);
		}
	}
	
	FModelSettingsType GetModelSettings(const FSLMDeviceAddress& DeviceAddress)
	{
		FModelSettingsType Result;
		if (IsValidDeviceAddress(DeviceAddress))
		{
			Result = DeviceModels[DeviceAddress.DeviceID].Settings;
		}
		return Result;
	}

	void SetModelSettings(const FSLMDeviceAddress& DeviceAddress, const FModelSettingsType& ModelSettings)
	{
		if (IsValidDeviceAddress(DeviceAddress))
		{
			DeviceModels[DeviceAddress.DeviceID].Settings = ModelSettings;
		}
		if (GetSystem()->GetWorld()->GetNetMode() != NM_Client)
		{
			//RepEditDescriptor(Handle, FDescriptorType(ModelSettings));
		}
	}

	FAddressesType GetPortAddresses(const FSLMDeviceAddress& DeviceAddress)
	{
		check(IsValidDeviceAddress(DeviceAddress))
		FAddressesType Result;
		GetSystem()->DeviceIDToPortAddresses(DeviceAddress.DeviceID, Result);
		return Result;
	}

	void ApplyInput(const FSLMDeviceAddress& DeviceAddress, const FInputType& Input)
	{
		if (IsValidDeviceAddress(DeviceAddress))
		{
			GetSystem()->InputToModel(Input, DeviceModels[DeviceAddress.DeviceID]);
		}
	}

	FCosmeticStateType GetCosmeticState(const FSLMDeviceAddress& DeviceAddress) const
	{
		FCosmeticStateType Result;
		if (IsValidDeviceAddress(DeviceAddress))
		{
			GetSystem()->ModelToCosmeticState(DeviceModels[DeviceAddress.DeviceID], Result);
		}
		return Result;
	}

	void PullRepState()
	{
		if (GetSystem()->GetWorld()->GetNetMode() != NM_Client)
		{
			RepPullState();
		}
	}
	
	FString GetDebugString_Impl(const bool Verbose)
	{
		FString Result;
		/*
		Result += "\n------------------SimpleGearbox------------------";
		Result += FString::Format(TEXT("\nHas {0} Device Models"), {DeviceModels.Num()});
		if (Verbose)
		{
			for (int32 i = 0; i < DeviceModels.GetMaxIndex(); i++)
			{
				if (DeviceModels.IsValidIndex(i))
				{
					//Result += FString::Format(TEXT("\nModel {0} has state: {1}"), { i, DeviceModels[i].GetDebugString()});
				}
			}
		}
		*/
		return Result;
	}
	
	uint32 GetDebugHash_Impl()
	{
		uint32 Result = 0;
		for (int32 i = 0; i < DeviceModels.GetMaxIndex(); i++)
		{
			if (DeviceModels.IsValidIndex(i))
			{
				//Result =  Result ^ HashCombine(GetTypeHash(i), DeviceModels[i].GetDebugHash());
			}
		}
		return Result;
	}
	
	bool IsValidDeviceAddress(const FSLMDeviceAddress& DeviceAddress) const
	{
		const bool bDeviceTypeMatches = DeviceAddress.DeviceClass == GetSystem()->GetClass();
		const bool bDeviceIDIsValid = DeviceModels.IsValidIndex(DeviceAddress.DeviceID);
		return bDeviceTypeMatches && bDeviceIDIsValid;
	}
	
	void Client_AddOrChangeDescriptor_Impl(const FSLMDeviceAddress& DeviceAddress, const FInstancedStruct& Payload)
	{
		const FDescriptorType* DescriptorPtr = Payload.GetPtr<FDescriptorType>();
		check(DescriptorPtr);
		const FDescriptorType& Descriptor = *DescriptorPtr;
		
		if (IsValidDeviceAddress(DeviceAddress))
		{
			SetModelSettings(DeviceAddress, Descriptor.ModelSettings);
		}
		else
		{
			AddDevice(Descriptor, DeviceAddress);
		}
	}
	
	void Client_RemoveDescriptor_Impl(const FSLMDeviceAddress& DeviceAddress)
	{
		RemoveDevice(DeviceAddress);
	}
	
	void Client_AddOrChangeState_Impl(const FSLMDeviceAddress& DeviceAddress, const FInstancedStruct& Payload)
	{
		const FRepStateType* RepStatePtr = Payload.GetPtr<FRepStateType>();
		check(RepStatePtr);
		const FRepStateType& RepState = *RepStatePtr;
		
		if (IsValidDeviceAddress(DeviceAddress))
		{
			GetSystem()->RepStateToModel(RepState, DeviceModels[DeviceAddress.DeviceID]);
		}
		else
		{
			OrphanedRepStates.EmplaceAt(DeviceAddress.DeviceID, RepState);
		}
	}
	
	void Client_RemoveState_Impl(const FSLMDeviceAddress& DeviceAddress)
	{
	}

private:

	void AddOrChangeDescriptor(const FSLMDeviceAddress& DeviceAddress, const FDescriptorType& Descriptor)
	{
		const USLMManager* Manager = GetSystem()->GetWorld()->template GetSubsystem<USLMManager>();
		FInstancedStruct Payload;
		Payload.InitializeAs<FDescriptorType>(Descriptor);
		check(Manager->Replicator);
		Manager->Replicator->AddOrChangeDescriptor(DeviceAddress, Payload);
	}
	
	void RemoveDescriptor(const FSLMDeviceAddress& DeviceAddress)
	{
		const USLMManager* Manager = GetSystem()->GetWorld()->template GetSubsystem<USLMManager>();
		check(Manager->Replicator);
		Manager->Replicator->RemoveDescriptor(DeviceAddress);
	}

	void RepPullState()
	{
		for (int32 i = 0; i < DeviceModels.GetMaxIndex(); i++)
		{
			if (DeviceModels.IsValidIndex(i))
			{
				const FModelType Model = DeviceModels[i];
				if (Model.State.bDirty)
				{
					FRepStateType RepState;
					GetSystem()->ModelToRepState(Model, RepState);
					FInstancedStruct Payload;
					Payload.InitializeAs<FRepStateType>(RepState);
					//GetManager()->Server_AddOrChangeState(GetSystem()->GetClass(), i, Payload);
					FSLMDeviceAddress DeviceAddress = {GetSystem()->GetClass(), i};
					check(GetManager()->Replicator);
					GetManager()->Replicator->AddOrChangeState(DeviceAddress, Payload);
				}
			}
		}
		//TODO: Switch to this:
		/*
		for (TConstSetBitIterator<> It(DirtyFlags); It; ++It)
		{
			const int32 Index = It.GetIndex();
			
			FRepStateType RepState;
			GetSystem()->ModelToRepState(DeviceModels[Index], RepState);
			
			FInstancedStruct Payload;
			Payload.InitializeAs<FRepStateType>(RepState);
			GetManager()->Server_AddOrChangeState(GetSystem()->GetClass(), Index, Payload);
			
			DirtyFlags[Index] = false;
		}
		*/
	}


	SystemType* GetSystem()
	{
		return static_cast<SystemType*>(this);
	}

	const SystemType* GetSystem() const
	{
		return static_cast<const SystemType*>(this);
	}
	
	const USLMManager* GetManager() const
	{
		return GetSystem()->GetWorld()->template GetSubsystem<USLMManager>();
	}

	static FModelType MakeDeviceModel(const FModelSettingsType& Settings)
	{
		FModelType Model;
		Model.Settings = Settings;
		return Model;
	}

protected:
	TSparseArray<FModelType> DeviceModels;
	TSparseArray<FRepStateType> OrphanedRepStates;
	TBitArray<> DirtyFlags;
};