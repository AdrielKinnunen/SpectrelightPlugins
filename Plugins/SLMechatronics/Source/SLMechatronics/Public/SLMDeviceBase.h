// Copyright Spectrelight Studios, LLC

#pragma once

#include "CoreMinimal.h"
#include "SLMSubsystemBase.h"
#include "Subsystems/WorldSubsystem.h"
#include "SLMDeviceBase.generated.h"


class USLMDomainSubsystemBase;


UCLASS(Abstract)
class SLMECHATRONICS_API USLMDeviceComponentBase : public UActorComponent
{
    GENERATED_BODY()
public:
    USLMDeviceComponentBase();
protected:
    //FSLMHandle Handle;

    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};


UCLASS()
class SLMECHATRONICS_API USLMDeviceSubsystemBase : public USLMSubsystemBase
{
    GENERATED_BODY()
	friend class USLMManager;
};







template<	typename DerivedType,
			typename HandleType,
			typename ModelType,
			typename SettingsType,
			typename CosmeticType,
			typename InputType,
			typename RepStateType,
			typename AddressesType>//,
			//typename ReplicatorType,
			//typename RepArraySettingsType,
			//typename RepArrayStateType>
class TSLMDeviceSystem
{
public:
	virtual ~TSLMDeviceSystem() = default;
	
	
	
	/*
	HandleType AddDevice_Impl(const SettingsType& Settings, const HandleType ExplicitHandle = HandleType())
	{
		//auto* System = static_cast<DerivedType*>(this);
		auto* Replicator = static_cast<ReplicatorType*>(System->Replicator);
		auto& RepArraySettings = Replicator->RepArraySettings;
		auto& RepArrayState = Replicator->RepArrayState;
		
		HandleType Handle;
		
		if (System->GetWorld()->GetNetMode() != NM_Client)
		{
			Handle.ID = DeviceModels.Add(Settings.DeviceModel);
			
			auto& SettingsItem = RepArraySettings.Items.AddDefaulted_GetRef();
			SettingsItem.RepHandle = Handle;
			SettingsItem.RepSettings = Settings;
			RepArraySettings.MarkItemDirty(SettingsItem);
			
			auto& StateItem = System->RepArrayState.Items.AddDefaulted_GetRef();
			StateItem.RepHandle = Handle;
			StateItem.RepState = Settings.DeviceModel.MakeRepState();
			RepArrayState.MarkItemDirty(StateItem);
		}
		else
		{
			check(ExplicitHandle.ID != INDEX_NONE);
			Handle = ExplicitHandle;
			DeviceModels.EmplaceAt(Handle.ID, Settings.DeviceModel);
		}
		ModelType& Model = DeviceModels[Handle.ID];
		const AddressesType PortAddresses = GetPortAddresses_Impl(Handle);
		//System->RegisterPorts(Settings, Model, PortAddresses);
		if (OrphanedRepStates.IsValidIndex(Handle.ID))
		{
			ApplyReplicatedState(Handle, OrphanedRepStates[Handle.ID]);
		}
		return Handle;
	}
	*/
	
	
	void RemoveDevice_Impl(const HandleType Handle)
	{
		if (IsValidHandle_Impl(Handle))
		{
			const AddressesType PortAddresses = GetPortAddresses_Impl(Handle);
			GetDerived()->RemovePorts(PortAddresses);
			DeviceModels.RemoveAt(Handle.ID);		
		}
		if (GetDerived()->GetWorld()->GetNetMode() != NM_Client)
		{
			GetDerived()->Replicator->RemoveItem(Handle);
		}
	}
	
	AddressesType GetPortAddresses_Impl(HandleType Handle)
	{
		check (IsValidHandle_Impl(Handle));
		AddressesType Result;
		GetDerived()->WritePortAddresses(Handle, Result);
		return Result;
	}
	
	CosmeticType GetCosmeticState_Impl(const HandleType Handle) const
	{
		CosmeticType Result;
		if (IsValidHandle_Impl(Handle))
		{
			GetDerived()->WriteModelToCosmeticState(DeviceModels[Handle.ID], Result);
		}
		return Result;
	}
	
	SettingsType GetDeviceSettings_Impl(const HandleType Handle) const
	{
		SettingsType Result;
		if (IsValidHandle_Impl(Handle))
		{
			GetDerived()->WriteModelToSettings(DeviceModels[Handle.ID], Result);
		}
		return Result;
	}
	
	void SetDeviceSettings_Impl(const HandleType Handle, const SettingsType& Settings)
	{
		if (IsValidHandle_Impl(Handle))
		{
			GetDerived()->WriteSettingsToModel(DeviceModels[Handle.ID], Settings);
		}
	}
	/*
	void ApplyReplicatedState_Impl(const HandleType Handle, const RepStateType& State)
	{
		if (!IsValidHandle(Handle))
		{
			OrphanedRepStates.EmplaceAt(Handle.ID, State);
			return;
		}
		DeviceModels[Handle.ID].ApplyRepState(State);
	}
	*/
	void ApplyInput_Impl(const HandleType Handle, const InputType& Input)
	{
		if (IsValidHandle_Impl(Handle))
		{
			GetDerived()->WriteInputToModel(DeviceModels[Handle.ID], Input);
		}
	}
	/*
	void RemoveDevice_Impl(const HandleType Handle)
	{
		if (IsValidHandle(Handle))
		{
			//static_cast<DerivedType*>(this)->RemovePorts(Handle);
			DeviceModels.RemoveAt(Handle.ID);
		}
	}
    */
	
	DerivedType* GetDerived()
	{
		return static_cast<DerivedType*>(this);
	}
	
	const DerivedType* GetDerived() const
	{
		return static_cast<const DerivedType*>(this);
	}
	
	bool IsValidHandle_Impl(const HandleType Handle) const
	{
		return DeviceModels.IsValidIndex(Handle.ID);
	}
	/*
	
	*/
protected:
	TSparseArray<ModelType> DeviceModels;
	TSparseArray<RepStateType> OrphanedRepStates;
};



/*

#define SLM_IMPLEMENT_REP_ARRAY_SETTINGS(ArrayType, ItemType) \
void ArrayType::PostReplicatedAdd(const TArrayView<int32>& AddedIndices, int32 FinalSize) const \
{ \
    for (const auto Index : AddedIndices) \
    { \
        const ItemType& Item = Items[Index]; \
        if (System->IsValidHandle(Item.RepHandle)) \
            System->EditDeviceSettings(Item.RepHandle, Item.RepSettings); \
        else \
            System->AddDevice(Item.RepSettings, Item.RepHandle); \
    } \
} \
void ArrayType::PostReplicatedChange(const TArrayView<int32>& ChangedIndices, int32 FinalSize) const \
{ \
    for (const auto Index : ChangedIndices) \
    { \
        const ItemType& Item = Items[Index]; \
        if (System->IsValidHandle(Item.RepHandle)) \
            System->EditDeviceSettings(Item.RepHandle, Item.RepSettings); \
        else \
            System->AddDevice(Item.RepSettings, Item.RepHandle); \
    } \
} \
void ArrayType::PreReplicatedRemove(const TArrayView<int32>& RemovedIndices, int32 FinalSize) const \
{ \
    for (const auto Index : RemovedIndices) \
    { \
        const ItemType& Item = Items[Index]; \
        if (System->IsValidHandle(Item.RepHandle)) \
            System->RemoveDevice(Item.RepHandle); \
    } \
}






#define SLM_IMPLEMENT_REP_ARRAY_STATE(ArrayType, ItemType) \
void ArrayType::PostReplicatedAdd(const TArrayView<int32>& AddedIndices, int32 FinalSize) const \
{ \
    for (const auto Index : AddedIndices) \
    { \
        const ItemType& Item = Items[Index]; \
        System->ApplyReplicatedState(Item.RepHandle, Item.RepState); \
    } \
} \
void ArrayType::PostReplicatedChange(const TArrayView<int32>& ChangedIndices, int32 FinalSize) const \
{ \
    for (const auto Index : ChangedIndices) \
    { \
        const ItemType& Item = Items[Index]; \
        System->ApplyReplicatedState(Item.RepHandle, Item.RepState); \
    } \
} \
void ArrayType::PreReplicatedRemove(const TArrayView<int32>& RemovedIndices, int32 FinalSize) const \
{ \
}



*/