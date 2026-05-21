// Copyright Spectrelight Studios, LLC
#if 0
#pragma once

#include "CoreMinimal.h"
#include "SLMDeviceBase.h"
#include "Domains/SLMDomainRotation.h"
#include "Domains/SLMDomainSignal.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "SLMDeviceSimpleEngine.generated.h"

class USLMDeviceSubsystemSimpleEngine;
class ASLMDeviceReplicatorSimpleEngine;

//Typed Handle for safety
USTRUCT(BlueprintType)
struct FSLMSimpleEngineHandle
{
	GENERATED_BODY()
	UPROPERTY()
	int32 ID = INDEX_NONE;
};

//Hot loop settings
USTRUCT(BlueprintType)
struct FSLMSimpleEngineModelSettings
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
    float MaxTorque = 1000;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	float MaxRPM = 7500;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	float IdleRPM = 1000;
	FString GetDebugString() const;
	uint32 GetDebugHash() const;
};

//Hot loop state
USTRUCT()
struct FSLMSimpleEngineModelState
{
	GENERATED_BODY()
	
	int32 PortID_Rotation_Crankshaft = INDEX_NONE;
	int32 PortID_Signal_Throttle = INDEX_NONE;
	
	bool bIgnition = false;
	
	bool bDirty = false;
	FString GetDebugString() const;
	uint32 GetDebugHash() const;
};

//Hot loop model
USTRUCT()
struct FSLMSimpleEngineModel
{
	GENERATED_BODY()
	FSLMSimpleEngineModelSettings Settings;
	FSLMSimpleEngineModelState State;
	FString GetDebugString() const;
	uint32 GetDebugHash() const;
};

//Port Defaults
USTRUCT(BlueprintType)
struct FSLMSimpleEnginePortSettings
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	FSLMPortRotation Port_Rotation_Crankshaft;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	FSLMPortSignal Port_Signal_Throttle;
};

//Device Defaults
USTRUCT(BlueprintType)
struct FSLMSimpleEngineDescriptor
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	FSLMSimpleEngineModelSettings ModelSettings;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	FSLMSimpleEnginePortSettings PortSettings;
};

//Device port addresses
USTRUCT(BlueprintType)
struct FSLMSimpleEngineAddresses
{
	GENERATED_BODY()
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	FSLMPortAddress Address_Rotation_Crankshaft;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	FSLMPortAddress Address_Signal_Throttle;
};

//Dynamic Replicated State
USTRUCT()
struct FSLMSimpleEngineRepState
{
	GENERATED_BODY()
	//UPROPERTY()
	//int32 Fake = 0;
};

//Public cosmetic state
USTRUCT(BlueprintType)
struct FSLMSimpleEngineCosmeticState
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly, Category = "SLMechatronics")
	float Throttle = 0.0;
	UPROPERTY(BlueprintReadOnly, Category = "SLMechatronics")
	float CrankshaftRPM = 0.0;
};

//Public Input Structure
USTRUCT(BlueprintType)
struct FSLMSimpleEngineInput
{
	GENERATED_BODY()
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	//int32 Fake = 0;
};



//FastArray item for replication of Descriptor
USTRUCT()
struct FSLMSimpleEngineRepItemDescriptor : public FFastArraySerializerItem
{
	GENERATED_BODY()
	UPROPERTY()
	FSLMSimpleEngineHandle Handle;
	UPROPERTY()
	FSLMSimpleEngineDescriptor Descriptor;
};

//FastArray container for replication of Descriptors
USTRUCT()
struct FSLMSimpleEngineRepArrayDescriptor : public FFastArraySerializer
{
	GENERATED_BODY()
	UPROPERTY()
	TArray<FSLMSimpleEngineRepItemDescriptor> Items;
	UPROPERTY()
	USLMDeviceSubsystemSimpleEngine* Subsystem = nullptr;
	void PostReplicatedAdd(const TArrayView<int32>& AddedIndices, int32 FinalSize) const;
	void PostReplicatedChange(const TArrayView<int32>& ChangedIndices, int32 FinalSize) const;
	void PreReplicatedRemove(const TArrayView<int32>& RemovedIndices, int32 FinalSize) const;
	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FastArrayDeltaSerialize<FSLMSimpleEngineRepItemDescriptor, FSLMSimpleEngineRepArrayDescriptor>(Items, DeltaParms, *this);
	}
};
template<>
struct TStructOpsTypeTraits<FSLMSimpleEngineRepArrayDescriptor> : public TStructOpsTypeTraitsBase2<FSLMSimpleEngineRepArrayDescriptor>
{
	enum 
	{
		WithNetDeltaSerializer = true,
   };
};

//FastArray item for replication of state
USTRUCT()
struct FSLMSimpleEngineRepItemState : public FFastArraySerializerItem
{
	GENERATED_BODY()
	UPROPERTY()
	FSLMSimpleEngineHandle Handle;
	UPROPERTY()
	FSLMSimpleEngineRepState RepState;
};

//FastArray container for replication of state
USTRUCT()
struct FSLMSimpleEngineRepArrayState : public FFastArraySerializer
{
	GENERATED_BODY()
	UPROPERTY()
	TArray<FSLMSimpleEngineRepItemState> Items;
	UPROPERTY()
	USLMDeviceSubsystemSimpleEngine* Subsystem = nullptr;
	void PostReplicatedAdd(const TArrayView<int32>& AddedIndices, int32 FinalSize) const;
	void PostReplicatedChange(const TArrayView<int32>& ChangedIndices, int32 FinalSize) const;
	static void PreReplicatedRemove(const TArrayView<int32>& RemovedIndices, int32 FinalSize);
	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FastArrayDeltaSerialize<FSLMSimpleEngineRepItemState, FSLMSimpleEngineRepArrayState>(Items, DeltaParms, *this);
	}
};
template<>
struct TStructOpsTypeTraits<FSLMSimpleEngineRepArrayState> : public TStructOpsTypeTraitsBase2<FSLMSimpleEngineRepArrayState>
{
	enum 
	{
		WithNetDeltaSerializer = true,
	};
};

struct FSLMSimpleEngineTraits
{
	using FHandleType			= FSLMSimpleEngineHandle;
	using FModelSettingsType	= FSLMSimpleEngineModelSettings;
	using FModelStateType		= FSLMSimpleEngineModelState;
	using FModelType			= FSLMSimpleEngineModel;
	using FDescriptorType		= FSLMSimpleEngineDescriptor;
	using FAddressesType		= FSLMSimpleEngineAddresses;
	using FRepStateType			= FSLMSimpleEngineRepState;
	using FCosmeticStateType	= FSLMSimpleEngineCosmeticState;
	using FInputType			= FSLMSimpleEngineInput;
	using FRepItemDescType		= FSLMSimpleEngineRepItemDescriptor;
	using FRepItemStateType		= FSLMSimpleEngineRepItemState;
};

//Core Subsystem
UCLASS()
class SLMECHATRONICS_API USLMDeviceSubsystemSimpleEngine : public USLMDeviceSubsystemBase, public TSLMDeviceSystem<USLMDeviceSubsystemSimpleEngine, FSLMSimpleEngineTraits>
{
    GENERATED_BODY()
	
	friend class TSLMDeviceSystem;
	
public:
    virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	virtual void PostInitialize() override;

	virtual FString GetDebugString(const bool Verbose) override;
	virtual uint32 GetDebugHash() override;
	
	virtual void PreSimulate(const float DeltaTime) override;
	virtual void Simulate(const float DeltaTime, const float SubstepScalar) override;
	virtual void PostSimulate(const float DeltaTime) override;

private:
	//CRTP Hooks, must implement
	void RegisterPorts(const FSLMSimpleEnginePortSettings& PortSettings, FSLMSimpleEngineModelState& ModelState, const FSLMSimpleEngineAddresses& Addresses) const;
	void RemovePorts(const FSLMSimpleEngineAddresses& Addresses) const;
	void HandleToAddresses(const FSLMSimpleEngineHandle Handle, FSLMSimpleEngineAddresses& Addresses) const;
	void ModelToCosmeticState(const FSLMSimpleEngineModel& Model, FSLMSimpleEngineCosmeticState& CosmeticState) const;
	static void ModelToRepState(const FSLMSimpleEngineModel& Model, FSLMSimpleEngineRepState& RepState);
	static void RepStateToModel(const FSLMSimpleEngineRepState& RepState, FSLMSimpleEngineModel& Model);
	static void InputToModel(const FSLMSimpleEngineInput& Input, FSLMSimpleEngineModel& Model);
	
	UPROPERTY()
    USLMDomainRotation* DomainRotation;
	UPROPERTY()
	USLMDomainSignal* DomainSignal;
	UPROPERTY()
	ASLMDeviceReplicatorSimpleEngine* Replicator;
};

//Replicator Actor
UCLASS()
class ASLMDeviceReplicatorSimpleEngine : public AInfo
{
	GENERATED_BODY()
	
	friend class TSLMDeviceSystem<USLMDeviceSubsystemSimpleEngine, FSLMSimpleEngineTraits>;

public:
	ASLMDeviceReplicatorSimpleEngine();
	virtual void PostInitializeComponents() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
private:
	UPROPERTY(Replicated)
	FSLMSimpleEngineRepArrayDescriptor RepArrayDescriptor;
	UPROPERTY(Replicated)
	FSLMSimpleEngineRepArrayState RepArrayState;
};

//BP Function Library for direct access
UCLASS()
class SLMECHATRONICS_API USLMBPFLSimpleEngine : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="SLMechatronics", meta=(WorldContext="WorldContextObject", ReturnDisplayName = "Handle"))
	static FSLMSimpleEngineHandle AddDeviceSimpleEngine(const UObject* WorldContextObject, const FSLMSimpleEngineDescriptor& Settings);
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="SLMechatronics", meta=(WorldContext="WorldContextObject"))
	static void RemoveDevice(const UObject* WorldContextObject, const FSLMSimpleEngineHandle Handle);
	UFUNCTION(BlueprintCallable, Category="SLMechatronics", meta=(WorldContext="WorldContextObject"))
	static FSLMSimpleEngineModelSettings GetDeviceSettings(const UObject* WorldContextObject, const FSLMSimpleEngineHandle Handle);
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="SLMechatronics", meta=(WorldContext="WorldContextObject"))
	static void SetDeviceSettings(const UObject* WorldContextObject, const FSLMSimpleEngineHandle Handle, const FSLMSimpleEngineModelSettings& Settings);
	UFUNCTION(BlueprintCallable, Category="SLMechatronics", meta=(WorldContext="WorldContextObject"))
	static FSLMSimpleEngineAddresses GetPortAddresses(const UObject* WorldContextObject, const FSLMSimpleEngineHandle Handle);
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="SLMechatronics", meta=(WorldContext="WorldContextObject"))
	static void ApplyInput(const UObject* WorldContextObject, const FSLMSimpleEngineHandle Handle, const FSLMSimpleEngineInput& Input);
	UFUNCTION(BlueprintCallable, Category="SLMechatronics", meta=(WorldContext="WorldContextObject"))
	static FSLMSimpleEngineCosmeticState GetCosmeticState(const UObject* WorldContextObject, const FSLMSimpleEngineHandle Handle);
};










/*

USTRUCT(BlueprintType)
struct FSLMDeviceModelSimpleEngine
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
    float MaxTorque = 1000;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	float MaxRPM = 7500;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	float IdleRPM = 1000;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	bool bIgnition = true;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
    int32 Index_Rotation_Crankshaft = -1;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
    int32 Index_Signal_Throttle = -1;
};


USTRUCT(BlueprintType)
struct FSLMDeviceSimpleEngine
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
    FSLMDeviceModelSimpleEngine DeviceModel;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
    FSLMPortRotation Port_Rotation_Crankshaft;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
    FSLMPortSignal Port_Signal_Throttle;
};


UCLASS(ClassGroup=("SLMechatronics"), meta=(BlueprintSpawnableComponent))
class SLMECHATRONICS_API USLMDeviceComponentSimpleEngine : public USLMDeviceComponentBase
{
    GENERATED_BODY()
public:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
    USLMDeviceSubsystemSimpleEngine* Subsystem;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
    FSLMDeviceSimpleEngine DeviceSettings;

    UFUNCTION(BlueprintCallable, Category = "SLMechatronics")
    FSLMDeviceModelSimpleEngine GetDeviceState();
protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};


UCLASS()
class SLMECHATRONICS_API USLMDeviceSubsystemSimpleEngine : public USLMDeviceSubsystemBase
{
    GENERATED_BODY()
public:
    virtual void OnWorldBeginPlay(UWorld& InWorld) override;
    virtual void PreSimulate(const float DeltaTime) override;
    virtual void Simulate(const float DeltaTime,float SubstepScalar) override;
    virtual void PostSimulate(const float DeltaTime) override;

    UFUNCTION(BlueprintCallable, Category = "SLMechatronics")
    int32 AddDevice(FSLMDeviceSimpleEngine Device);
    UFUNCTION(BlueprintCallable, Category = "SLMechatronics")
    void RemoveDevice(const int32 DeviceIndex);
    UFUNCTION(BlueprintCallable, Category = "SLMechatronics")
    FSLMDeviceModelSimpleEngine GetDeviceState(const int32 DeviceIndex);
private:
    TWeakObjectPtr<USLMDomainRotation> DomainRotation;
    TWeakObjectPtr<USLMDomainSignal> DomainSignal;
    TSparseArray<FSLMDeviceModelSimpleEngine> DeviceModels;
};


*/

#endif