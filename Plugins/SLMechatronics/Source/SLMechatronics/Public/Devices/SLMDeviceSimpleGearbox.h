// Copyright Spectrelight Studios, LLC

#pragma once

#include "CoreMinimal.h"
#include "SLMDeviceBase.h"
#include "Domains/SLMDomainRotation.h"
#include "Domains/SLMDomainSignal.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "SLMDeviceSimpleGearbox.generated.h"

class USLMDeviceSubsystemSimpleGearbox;
class ASLMDeviceReplicatorSimpleGearbox;

//Typed Handle for safety
USTRUCT(BlueprintType)
struct FSLMDeviceHandleSimpleGearbox
{
	GENERATED_BODY()
	UPROPERTY()
	int32 ID = INDEX_NONE;
};

//Hot loop model
USTRUCT(BlueprintType)
struct FSLMDeviceModelSimpleGearbox
{
    GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	int32 NumForwardGears = 5;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	int32 NumReverseGears = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	float FirstGearRatio = 5.0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	float RatioBetweenGears = 1.3;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	float GearSpreadExponent = 1.0;
	
	int32 CurrentGear = 0;
	int32 PreviousGear = 0;
	float GearRatio = 0.0;
	
	int32 PortID_Rotation_Input = INDEX_NONE;
	int32 PortID_Rotation_Output = INDEX_NONE;
	int32 PortID_Signal_Shift = INDEX_NONE;
	bool bDirty = false;
	
	FString GetDebugString() const
	{
    	FString Result;
		Result += FString::Printf(TEXT("%i,%i,%f,%f,%f,%i"), NumForwardGears, NumReverseGears, FirstGearRatio, RatioBetweenGears, GearSpreadExponent, CurrentGear);
    	return Result;
	}
};

FORCEINLINE uint32 GetTypeHash(const FSLMDeviceModelSimpleGearbox& Model)
{
	uint32 Hash = 0;
	Hash = HashCombine(Hash, GetTypeHash(Model.NumForwardGears));
	Hash = HashCombine(Hash, GetTypeHash(Model.NumReverseGears));
	Hash = HashCombine(Hash, GetTypeHash(FMath::RoundToInt(Model.FirstGearRatio * 1000.0f)));
	Hash = HashCombine(Hash, GetTypeHash(FMath::RoundToInt(Model.RatioBetweenGears * 1000.0f)));
	Hash = HashCombine(Hash, GetTypeHash(FMath::RoundToInt(Model.GearSpreadExponent * 1000.0f)));
	Hash = HashCombine(Hash, GetTypeHash(Model.CurrentGear));
	Hash = HashCombine(Hash, GetTypeHash(FMath::RoundToInt(Model.GearRatio * 1000.0f)));
	return Hash;
}

//Dynamic Replicated State
USTRUCT(BlueprintType)
struct FSLMDeviceRepStateSimpleGearbox
{
	GENERATED_BODY()
	UPROPERTY()
	int32 Gear = 0;
};

//Public cosmetic state
USTRUCT(BlueprintType)
struct FSLMDeviceCosmeticStateSimpleGearbox
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly, Category = "SLMechatronics")
	int32 CurrentGear = 0;
	UPROPERTY(BlueprintReadOnly, Category = "SLMechatronics")
	float CurrentGearRatio = 0.0;
	UPROPERTY(BlueprintReadOnly, Category = "SLMechatronics")
	float InputAngVelDegS = 0.0;
	UPROPERTY(BlueprintReadOnly, Category = "SLMechatronics")
	float OutputAngVelDegS = 0.0;
};

//Public Input Structure
USTRUCT(BlueprintType)
struct FSLMDeviceInputSimpleGearbox
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	int32 DesiredGear = 0;
};

//Model + Port Settings
USTRUCT(BlueprintType)
struct FSLMDeviceSettingsSimpleGearbox
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
    FSLMDeviceModelSimpleGearbox DeviceModel;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
    FSLMPortRotation Port_Rotation_Input;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	FSLMPortRotation Port_Rotation_Output;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	FSLMPortSignal Port_Signal_Shift;
};

//Port Addresses
USTRUCT(BlueprintType)
struct FSLMDevicePortAddressesSimpleGearbox
{
	GENERATED_BODY()
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	FSLMPortAddress Address_Rotation_Input;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	FSLMPortAddress Address_Rotation_Output;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	FSLMPortAddress Address_Signal_Shift;
};

//FastArray item for replication of settings
USTRUCT()
struct FSLMRepItemSettingsSimpleGearbox : public FFastArraySerializerItem
{
	GENERATED_BODY()
	UPROPERTY()
	FSLMDeviceHandleSimpleGearbox Handle;
	UPROPERTY()
	FSLMDeviceSettingsSimpleGearbox RepSettings;
};

//FastArray container for replication of settings
USTRUCT()
struct FSLMRepArraySettingsSimpleGearbox : public FFastArraySerializer
{
	GENERATED_BODY()
	UPROPERTY()
	TArray<FSLMRepItemSettingsSimpleGearbox> Items;
	UPROPERTY()
	USLMDeviceSubsystemSimpleGearbox* Subsystem = nullptr;
	void PostReplicatedAdd(const TArrayView<int32>& AddedIndices, int32 FinalSize) const;
	void PostReplicatedChange(const TArrayView<int32>& ChangedIndices, int32 FinalSize) const;
	void PreReplicatedRemove(const TArrayView<int32>& RemovedIndices, int32 FinalSize) const;
	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FastArrayDeltaSerialize<FSLMRepItemSettingsSimpleGearbox, FSLMRepArraySettingsSimpleGearbox>(Items, DeltaParms, *this);
	}
};
template<>
struct TStructOpsTypeTraits<FSLMRepArraySettingsSimpleGearbox> : public TStructOpsTypeTraitsBase2<FSLMRepArraySettingsSimpleGearbox>
{
	enum 
	{
		WithNetDeltaSerializer = true,
   };
};

//FastArray item for replication of state
USTRUCT()
struct FSLMRepItemStateSimpleGearbox : public FFastArraySerializerItem
{
	GENERATED_BODY()
	UPROPERTY()
	FSLMDeviceHandleSimpleGearbox Handle;
	UPROPERTY()
	FSLMDeviceRepStateSimpleGearbox State;
};

//FastArray container for replication of state
USTRUCT()
struct FSLMRepArrayStateSimpleGearbox : public FFastArraySerializer
{
	GENERATED_BODY()
	UPROPERTY()
	TArray<FSLMRepItemStateSimpleGearbox> Items;
	UPROPERTY()
	USLMDeviceSubsystemSimpleGearbox* Subsystem = nullptr;
	void PostReplicatedAdd(const TArrayView<int32>& AddedIndices, int32 FinalSize) const;
	void PostReplicatedChange(const TArrayView<int32>& ChangedIndices, int32 FinalSize) const;
	void PreReplicatedRemove(const TArrayView<int32>& RemovedIndices, int32 FinalSize) const;
	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FastArrayDeltaSerialize<FSLMRepItemStateSimpleGearbox, FSLMRepArrayStateSimpleGearbox>(Items, DeltaParms, *this);
	}
};
template<>
struct TStructOpsTypeTraits<FSLMRepArrayStateSimpleGearbox> : public TStructOpsTypeTraitsBase2<FSLMRepArrayStateSimpleGearbox>
{
	enum 
	{
		WithNetDeltaSerializer = true,
   };
};

//Core Subsystem
UCLASS()
class SLMECHATRONICS_API USLMDeviceSubsystemSimpleGearbox : public USLMDeviceSubsystemBase
{
    GENERATED_BODY()
	
	friend struct FSLMRepArraySettingsSimpleGearbox;
	friend struct FSLMRepArrayStateSimpleGearbox;
	
public:
    virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	virtual void PostInitialize() override;
	
	FSLMDeviceHandleSimpleGearbox AddDevice(const FSLMDeviceSettingsSimpleGearbox& Settings, const FSLMDeviceHandleSimpleGearbox ExplicitHandle = FSLMDeviceHandleSimpleGearbox());
	FSLMDeviceCosmeticStateSimpleGearbox GetCosmeticState(const FSLMDeviceHandleSimpleGearbox Handle) const;
	FSLMDeviceSettingsSimpleGearbox GetDeviceSettings(const FSLMDeviceHandleSimpleGearbox Handle) const;
	FSLMDevicePortAddressesSimpleGearbox GetPortAddresses(const FSLMDeviceHandleSimpleGearbox Handle) const;
	void EditDeviceSettings(const FSLMDeviceHandleSimpleGearbox Handle, const FSLMDeviceSettingsSimpleGearbox& Settings);
	void ApplyReplicatedState(const FSLMDeviceHandleSimpleGearbox Handle, const FSLMDeviceRepStateSimpleGearbox& State);
	void ApplyInput(const FSLMDeviceHandleSimpleGearbox Handle, const FSLMDeviceInputSimpleGearbox& Input);
	void RemoveDevice(const FSLMDeviceHandleSimpleGearbox Handle);

private:
	virtual void PreSimulate(const float DeltaTime) override;
	virtual void Simulate(const float DeltaTime, const float SubstepScalar) override;
	virtual void PostSimulate(const float DeltaTime) override;
	virtual FString GetDebugString(const bool Verbose) override;
	virtual uint32 GetDebugHash() override;
	
	bool IsValidHandle(const FSLMDeviceHandleSimpleGearbox Handle) const;
	
	UPROPERTY()
    USLMDomainRotation* DomainRotation;
	UPROPERTY()
	USLMDomainSignal* DomainSignal;
	UPROPERTY()
	ASLMDeviceReplicatorSimpleGearbox* Replicator;
	TSparseArray<FSLMDeviceModelSimpleGearbox> DeviceModels;
	TSparseArray<FSLMDeviceRepStateSimpleGearbox> OrphanedRepStates;
};

//Replicator Actor
UCLASS()
class ASLMDeviceReplicatorSimpleGearbox : public AInfo
{
	GENERATED_BODY()
public:
	ASLMDeviceReplicatorSimpleGearbox();
	virtual void PostInitializeComponents() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void AddItem(const FSLMDeviceHandleSimpleGearbox Handle, const FSLMDeviceSettingsSimpleGearbox& Settings);
	void RemoveItem(const FSLMDeviceHandleSimpleGearbox Handle);
	void EditItem(const FSLMDeviceHandleSimpleGearbox Handle, const FSLMDeviceSettingsSimpleGearbox& Settings);
	void PullDynamicState(TSparseArray<FSLMDeviceModelSimpleGearbox>& DeviceModels);
private:
	UPROPERTY(Replicated)
	FSLMRepArraySettingsSimpleGearbox RepArraySettings;
	UPROPERTY(Replicated)
	FSLMRepArrayStateSimpleGearbox RepArrayState;
};

//BP Function Library for direct access
UCLASS()
class SLMECHATRONICS_API USLMBPFLSimpleGearbox : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="SLMechatronics", meta=(WorldContext="WorldContextObject", ReturnDisplayName = "Handle"))
	static FSLMDeviceHandleSimpleGearbox AddDeviceSimpleGearbox(const UObject* WorldContextObject, const FSLMDeviceSettingsSimpleGearbox& Settings);
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="SLMechatronics", meta=(WorldContext="WorldContextObject"))
	static void RemoveDevice(const UObject* WorldContextObject, const FSLMDeviceHandleSimpleGearbox Handle);
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="SLMechatronics", meta=(WorldContext="WorldContextObject"))
	static void EditDeviceSettings(const UObject* WorldContextObject, const FSLMDeviceHandleSimpleGearbox Handle, const FSLMDeviceSettingsSimpleGearbox& Settings);
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="SLMechatronics", meta=(WorldContext="WorldContextObject"))
	static void ApplyInput(const UObject* WorldContextObject, const FSLMDeviceHandleSimpleGearbox Handle, const FSLMDeviceInputSimpleGearbox& Input);
	UFUNCTION(BlueprintCallable, Category="SLMechatronics", meta=(WorldContext="WorldContextObject"))
	static FSLMDeviceCosmeticStateSimpleGearbox GetCosmeticState(const UObject* WorldContextObject, const FSLMDeviceHandleSimpleGearbox Handle);
	UFUNCTION(BlueprintCallable, Category="SLMechatronics", meta=(WorldContext="WorldContextObject"))
	static FSLMDeviceSettingsSimpleGearbox GetDeviceSettings(const UObject* WorldContextObject, const FSLMDeviceHandleSimpleGearbox Handle);
	UFUNCTION(BlueprintCallable, Category="SLMechatronics", meta=(WorldContext="WorldContextObject"))
	static FSLMDevicePortAddressesSimpleGearbox GetPortAddresses(const UObject* WorldContextObject, const FSLMDeviceHandleSimpleGearbox Handle);
};


/*
//Component for convenience
UCLASS(ClassGroup=("SLMechatronics"), meta=(BlueprintSpawnableComponent))
class SLMECHATRONICS_API USLMDeviceComponentSimpleGearbox : public USLMDeviceComponentBase
{
	GENERATED_BODY()
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	UFUNCTION(BlueprintCallable, Category = "SLMechatronics")
	FSLMDeviceCosmeticStateSimpleGearbox GetDeviceCosmeticState();
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	FSLMDeviceSettingsSimpleGearbox DeviceSettings;
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
private:
	UPROPERTY()
	USLMDeviceSubsystemSimpleGearbox* Subsystem;
	UPROPERTY(Replicated)
	FSLMDeviceHandleSimpleGearbox Handle;
};
*/