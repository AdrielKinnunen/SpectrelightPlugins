// Copyright Spectrelight Studios, LLC

#pragma once

#include "CoreMinimal.h"
#include "SLMDeviceBase.h"
#include "Domains/SLMDomainRotation.h"
#include "Domains/SLMDomainSignal.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "SLMDeviceProbe.generated.h"

class USLMDeviceSubsystemProbe;
class ASLMDeviceReplicatorProbe;

//Typed Handle for safety
USTRUCT(BlueprintType)
struct FSLMDeviceHandleProbe
{
	GENERATED_BODY()
	UPROPERTY()
	int32 ID = INDEX_NONE;
};

//Hot loop model
USTRUCT(BlueprintType)
struct FSLMDeviceModelProbe
{
    GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	float ProbeValue = 0;
	
	int32 PortID_Rotation_Output = INDEX_NONE;
	int32 PortID_Signal_Output = INDEX_NONE;
	
	bool bDirty = false;

	
	FString GetDebugString() const
	{
    	FString Result;
		Result += FString::Printf(TEXT("%f"), ProbeValue);
    	return Result;
	}
};

FORCEINLINE uint32 GetTypeHash(const FSLMDeviceModelProbe& Model)
{
	return GetTypeHash(Model.ProbeValue);
}

//Dynamic Replicated State
USTRUCT(BlueprintType)
struct FSLMDeviceRepStateProbe
{
	GENERATED_BODY()
	UPROPERTY()
	float ProbeValue = 0;
};

//Public cosmetic state
USTRUCT(BlueprintType)
struct FSLMDeviceCosmeticStateProbe
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly, Category = "SLMechatronics")
	float ProbeValue = 0.0;
};

//Public Input Structure
USTRUCT(BlueprintType)
struct FSLMDeviceInputProbe
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	float ProbeValue = 0;
};

//Model + Port Settings
USTRUCT(BlueprintType)
struct FSLMDeviceSettingsProbe
{
    GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	FSLMDeviceModelProbe DeviceModel;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	FSLMPortRotation Port_Rotation_Output;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	FSLMPortSignal Port_Signal_Output;
};

//Port Addresses
USTRUCT(BlueprintType)
struct FSLMDevicePortAddressesProbe
{
	GENERATED_BODY()
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	FSLMPortAddress Address_Rotation_Output;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	FSLMPortAddress Address_Signal_Output;
};

//FastArray item for replication of settings
USTRUCT()
struct FSLMRepItemSettingsProbe : public FFastArraySerializerItem
{
	GENERATED_BODY()
	UPROPERTY()
	FSLMDeviceHandleProbe Handle;
	UPROPERTY()
	FSLMDeviceSettingsProbe Settings;
};

//FastArray container for replication of settings
USTRUCT()
struct FSLMRepArraySettingsProbe : public FFastArraySerializer
{
	GENERATED_BODY()
	UPROPERTY()
	TArray<FSLMRepItemSettingsProbe> Items;
	UPROPERTY()
	USLMDeviceSubsystemProbe* Subsystem = nullptr;
	void PostReplicatedAdd(const TArrayView<int32>& AddedIndices, int32 FinalSize) const;
	void PostReplicatedChange(const TArrayView<int32>& ChangedIndices, int32 FinalSize) const;
	void PreReplicatedRemove(const TArrayView<int32>& RemovedIndices, int32 FinalSize) const;
	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FastArrayDeltaSerialize<FSLMRepItemSettingsProbe, FSLMRepArraySettingsProbe>(Items, DeltaParms, *this);
	}
};
template<>
struct TStructOpsTypeTraits<FSLMRepArraySettingsProbe> : public TStructOpsTypeTraitsBase2<FSLMRepArraySettingsProbe>
{
	enum 
	{
		WithNetDeltaSerializer = true,
   };
};

//FastArray item for replication of state
USTRUCT()
struct FSLMRepItemStateProbe : public FFastArraySerializerItem
{
	GENERATED_BODY()
	UPROPERTY()
	FSLMDeviceHandleProbe Handle;
	UPROPERTY()
	FSLMDeviceRepStateProbe State;
};

//FastArray container for replication of state
USTRUCT()
struct FSLMRepArrayStateProbe : public FFastArraySerializer
{
	GENERATED_BODY()
	UPROPERTY()
	TArray<FSLMRepItemStateProbe> Items;
	UPROPERTY()
	USLMDeviceSubsystemProbe* Subsystem = nullptr;
	void PostReplicatedAdd(const TArrayView<int32>& AddedIndices, int32 FinalSize) const;
	void PostReplicatedChange(const TArrayView<int32>& ChangedIndices, int32 FinalSize) const;
	void PreReplicatedRemove(const TArrayView<int32>& RemovedIndices, int32 FinalSize) const;
	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FastArrayDeltaSerialize<FSLMRepItemStateProbe, FSLMRepArrayStateProbe>(Items, DeltaParms, *this);
	}
};
template<>
struct TStructOpsTypeTraits<FSLMRepArrayStateProbe> : public TStructOpsTypeTraitsBase2<FSLMRepArrayStateProbe>
{
	enum 
	{
		WithNetDeltaSerializer = true,
   };
};

//Core Subsystem
UCLASS()
class SLMECHATRONICS_API USLMDeviceSubsystemProbe : public USLMDeviceSubsystemBase
{
    GENERATED_BODY()
	
	friend struct FSLMRepArraySettingsProbe;
	friend struct FSLMRepArrayStateProbe;
	
public:
    virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	virtual void PostInitialize() override;
	
	FSLMDeviceHandleProbe AddDevice(const FSLMDeviceSettingsProbe& Settings, const FSLMDeviceHandleProbe ExplicitHandle = FSLMDeviceHandleProbe());
	FSLMDeviceCosmeticStateProbe GetCosmeticState(const FSLMDeviceHandleProbe Handle) const;
	FSLMDeviceSettingsProbe GetDeviceSettings(const FSLMDeviceHandleProbe Handle) const;
	FSLMDevicePortAddressesProbe GetPortAddresses(const FSLMDeviceHandleProbe Handle) const;
	void EditDeviceSettings(const FSLMDeviceHandleProbe Handle, const FSLMDeviceSettingsProbe& Settings);
	void ApplyReplicatedState(const FSLMDeviceHandleProbe Handle, const FSLMDeviceRepStateProbe& State);
	void ApplyInput(const FSLMDeviceHandleProbe Handle, const FSLMDeviceInputProbe& Input);
	void RemoveDevice(const FSLMDeviceHandleProbe Handle);

private:
	virtual void PreSimulate(const float DeltaTime) override;
	virtual void Simulate(const float DeltaTime, const float SubstepScalar) override;
	virtual void PostSimulate(const float DeltaTime) override;
	virtual FString GetDebugString(const bool Verbose) override;
	virtual uint32 GetDebugHash() override;
	
	bool IsValidHandle(const FSLMDeviceHandleProbe Handle) const;
	
	UPROPERTY()
    USLMDomainRotation* DomainRotation;
	UPROPERTY()
	USLMDomainSignal* DomainSignal;
	UPROPERTY()
	ASLMDeviceReplicatorProbe* Replicator;
	TSparseArray<FSLMDeviceModelProbe> DeviceModels;
	TSparseArray<FSLMDeviceRepStateProbe> OrphanedRepStates;
};

//Replicator Actor
UCLASS()
class ASLMDeviceReplicatorProbe : public AInfo
{
	GENERATED_BODY()
public:
	ASLMDeviceReplicatorProbe();
	virtual void PostInitializeComponents() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void AddItem(const FSLMDeviceHandleProbe Handle, const FSLMDeviceSettingsProbe& Settings);
	void RemoveItem(const FSLMDeviceHandleProbe Handle);
	void EditItem(const FSLMDeviceHandleProbe Handle, const FSLMDeviceSettingsProbe& Settings);
	void PullDynamicState(TSparseArray<FSLMDeviceModelProbe>& DeviceModels);
private:
	UPROPERTY(Replicated)
	FSLMRepArraySettingsProbe RepArraySettings;
	UPROPERTY(Replicated)
	FSLMRepArrayStateProbe RepArrayState;
};

//BP Function Library for direct access
UCLASS()
class SLMECHATRONICS_API USLMBPFLProbe : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="SLMechatronics", meta=(WorldContext="WorldContextObject", ReturnDisplayName = "Handle"))
	static FSLMDeviceHandleProbe AddDeviceProbe(const UObject* WorldContextObject, const FSLMDeviceSettingsProbe& Settings);
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="SLMechatronics", meta=(WorldContext="WorldContextObject"))
	static void RemoveDevice(const UObject* WorldContextObject, const FSLMDeviceHandleProbe Handle);
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="SLMechatronics", meta=(WorldContext="WorldContextObject"))
	static void EditDeviceSettings(const UObject* WorldContextObject, const FSLMDeviceHandleProbe Handle, const FSLMDeviceSettingsProbe& Settings);
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="SLMechatronics", meta=(WorldContext="WorldContextObject"))
	static void ApplyInput(const UObject* WorldContextObject, const FSLMDeviceHandleProbe Handle, const FSLMDeviceInputProbe& Input);
	UFUNCTION(BlueprintCallable, Category="SLMechatronics", meta=(WorldContext="WorldContextObject"))
	static FSLMDeviceCosmeticStateProbe GetCosmeticState(const UObject* WorldContextObject, const FSLMDeviceHandleProbe Handle);
	UFUNCTION(BlueprintCallable, Category="SLMechatronics", meta=(WorldContext="WorldContextObject"))
	static FSLMDeviceSettingsProbe GetDeviceSettings(const UObject* WorldContextObject, const FSLMDeviceHandleProbe Handle);
	UFUNCTION(BlueprintCallable, Category="SLMechatronics", meta=(WorldContext="WorldContextObject"))
	static FSLMDevicePortAddressesProbe GetPortAddresses(const UObject* WorldContextObject, const FSLMDeviceHandleProbe Handle);
};


/*
//Component for convenience
UCLASS(ClassGroup=("SLMechatronics"), meta=(BlueprintSpawnableComponent))
class SLMECHATRONICS_API USLMDeviceComponentProbe : public USLMDeviceComponentBase
{
	GENERATED_BODY()
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	UFUNCTION(BlueprintCallable, Category = "SLMechatronics")
	FSLMDeviceCosmeticStateProbe GetDeviceCosmeticState();
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	FSLMDeviceSettingsProbe DeviceSettings;
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
private:
	UPROPERTY()
	USLMDeviceSubsystemProbe* Subsystem;
	UPROPERTY(Replicated)
	FSLMDeviceHandleProbe Handle;
};
*/