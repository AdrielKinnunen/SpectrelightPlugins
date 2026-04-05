// Copyright Spectrelight Studios, LLC

#pragma once

#include "CoreMinimal.h"
#include "SLMDeviceBase.h"
#include "Domains/SLMDomainRotation.h"
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
	
	int32 ParticleID_Rotation_Input = INDEX_NONE;
	int32 ParticleID_Rotation_Output = INDEX_NONE;
	
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

//Input Structure
USTRUCT(BlueprintType)
struct FSLMDeviceInputSimpleGearbox
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	int32 DesiredGear = 0;
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
};

USTRUCT(BlueprintType)
struct FSLMDevicePortAddressesSimpleGearbox
{
	GENERATED_BODY()
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	FSLMPortAddress Address_Rotation_Input;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	FSLMPortAddress Address_Rotation_Output;
};

//FastArray item for replication of state
USTRUCT()
struct FSLMRepItemSimpleGearbox : public FFastArraySerializerItem
{
	GENERATED_BODY()
	UPROPERTY()
	FSLMDeviceHandleSimpleGearbox Handle;
	UPROPERTY()
	FSLMDeviceSettingsSimpleGearbox Settings;
};

//FastArray container for replication of state
USTRUCT()
struct FSLMRepArraySimpleGearbox : public FFastArraySerializer
{
	GENERATED_BODY()
	UPROPERTY()
	TArray<FSLMRepItemSimpleGearbox> Items;
	UPROPERTY()
	USLMDeviceSubsystemSimpleGearbox* Subsystem = nullptr;
	void PostReplicatedAdd(const TArrayView<int32>& AddedIndices, int32 FinalSize) const;
	void PostReplicatedChange(const TArrayView<int32>& ChangedIndices, int32 FinalSize) const;
	void PreReplicatedRemove(const TArrayView<int32>& RemovedIndices, int32 FinalSize) const;
	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FastArrayDeltaSerialize<FSLMRepItemSimpleGearbox, FSLMRepArraySimpleGearbox>(Items, DeltaParms, *this);
	}
};
template<>
struct TStructOpsTypeTraits<FSLMRepArraySimpleGearbox> : public TStructOpsTypeTraitsBase2<FSLMRepArraySimpleGearbox>
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
	
	friend struct FSLMRepArraySimpleGearbox;
	
public:
    virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	virtual void PostInitialize() override;
	
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "SLMechatronics", meta=(ReturnDisplayName = "Handle", HidePin="ExplicitHandle"))
	FSLMDeviceHandleSimpleGearbox AddDevice(const FSLMDeviceSettingsSimpleGearbox& Settings, const FSLMDeviceHandleSimpleGearbox ExplicitHandle = FSLMDeviceHandleSimpleGearbox());
	UFUNCTION(BlueprintCallable, Category = "SLMechatronics", BlueprintPure=false)
	FSLMDeviceCosmeticStateSimpleGearbox GetCosmeticState(const FSLMDeviceHandleSimpleGearbox Handle) const;
	UFUNCTION(BlueprintCallable, Category = "SLMechatronics", BlueprintPure=false)
	FSLMDeviceSettingsSimpleGearbox GetDeviceSettings(const FSLMDeviceHandleSimpleGearbox Handle) const;
	UFUNCTION(BlueprintCallable, Category = "SLMechatronics", BlueprintPure=false)
	FSLMDevicePortAddressesSimpleGearbox GetPortAddresses(const FSLMDeviceHandleSimpleGearbox Handle) const;
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "SLMechatronics")
	void EditDeviceSettings(const FSLMDeviceHandleSimpleGearbox Handle, const FSLMDeviceSettingsSimpleGearbox& Settings);
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "SLMechatronics")
	void ApplyInput(const FSLMDeviceHandleSimpleGearbox Handle, const FSLMDeviceInputSimpleGearbox& Input);
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "SLMechatronics")
	void RemoveDevice(const FSLMDeviceHandleSimpleGearbox Handle);

private:
	virtual void PreSimulate(const float DeltaTime) override;
	virtual void Simulate(const float DeltaTime, const float SubstepScalar) override;
	virtual void PostSimulate(const float DeltaTime) override;
	virtual FString GetDebugString() override;
	virtual uint32 GetDebugHash() override;
	
	bool IsValidHandle(const FSLMDeviceHandleSimpleGearbox Handle) const;
	
	UPROPERTY()
    USLMDomainRotation* DomainRotation;
	UPROPERTY()
	ASLMDeviceReplicatorSimpleGearbox* Replicator;
	TSparseArray<FSLMDeviceModelSimpleGearbox> DeviceModels;
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
	void UpdateItems(const TSparseArray<FSLMDeviceModelSimpleGearbox>& DeviceModels);
private:
	UPROPERTY(Replicated)
	FSLMRepArraySimpleGearbox FastArray;
};

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