// Copyright Spectrelight Studios, LLC

#pragma once

#include "CoreMinimal.h"
#include "SLMDeviceBase.h"
#include "Domains/SLMDomainRotation.h"
#include "Domains/SLMDomainSignal.h"
#include "SLMDeviceProbe.generated.h"

class USLMDeviceSubsystemProbe;

//Hot loop settings
USTRUCT(BlueprintType)
struct FSLMModelSettingsProbe
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	float Smoothing = 0.0;
	FString GetDebugString() const;
	uint32 GetDebugHash() const;
};

//Hot loop state
USTRUCT()
struct FSLMModelStateProbe
{
	GENERATED_BODY()
	float ProbeValue = 0.0;
	int32 PortID_Rotation = INDEX_NONE;
	int32 PortID_Signal = INDEX_NONE;
	bool bDirty = false;
	FString GetDebugString() const;
	uint32 GetDebugHash() const;
};

//Hot loop model
USTRUCT()
struct FSLMModelProbe
{
	GENERATED_BODY()
	FSLMModelSettingsProbe Settings;
	FSLMModelStateProbe State;
	FString GetDebugString() const;
	uint32 GetDebugHash() const;
};

//Port Defaults
USTRUCT(BlueprintType)
struct FSLMPortSettingsProbe
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	FSLMPortRotation Port_Rotation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	FSLMPortSignal Port_Signal;
};

//Device Defaults
USTRUCT(BlueprintType)
struct FSLMDeviceDescriptorProbe
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	FSLMModelSettingsProbe ModelSettings;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	FSLMPortSettingsProbe PortSettings;
};

//Device port addresses
USTRUCT(BlueprintType)
struct FSLMPortAddressesProbe
{
	GENERATED_BODY()
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	FSLMPortAddress Address_Rotation;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	FSLMPortAddress Address_Signal;
};

//Dynamic Replicated State
USTRUCT()
struct FSLMRepStateProbe
{
	GENERATED_BODY()
	UPROPERTY()
	float ProbeValue = 0.0;
};

//Public cosmetic state
USTRUCT(BlueprintType)
struct FSLMCosmeticStateProbe
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly, Category = "SLMechatronics")
	float ProbeValue = 0.0;
};

//Public Input Structure
USTRUCT(BlueprintType)
struct FSLMInputProbe
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	float ProbeValue = 0.0;
};



struct FSLMProbeTraits
{
	//using FHandleType			= FSLMHandleProbe;
	using FModelSettingsType	= FSLMModelSettingsProbe;
	using FModelStateType		= FSLMModelStateProbe;
	using FModelType			= FSLMModelProbe;
	using FDescriptorType		= FSLMDeviceDescriptorProbe;
	using FAddressesType		= FSLMPortAddressesProbe;
	using FRepStateType			= FSLMRepStateProbe;
	using FCosmeticStateType	= FSLMCosmeticStateProbe;
	using FInputType			= FSLMInputProbe;
};

//Core Subsystem
UCLASS()
class SLMECHATRONICS_API USLMDeviceSubsystemProbe : public USLMDeviceSubsystemBase, public TSLMDeviceSystem<USLMDeviceSubsystemProbe, FSLMProbeTraits>
{
    GENERATED_BODY()
	
	template<typename SystemType, typename Traits>
	friend class TSLMDeviceSystem;
	
public:
    virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	virtual void PostInitialize() override;

	virtual FString GetDebugString(const bool Verbose) override;
	virtual uint32 GetDebugHash() override;
	
	virtual void PreSimulate(const float DeltaTime) override;
	virtual void Simulate(const float DeltaTime, const float SubstepScalar) override;
	virtual void PostSimulate(const float DeltaTime) override;
	
	//Clientside OnReps
	virtual void Client_AddOrChangeDescriptor(const FSLMDeviceAddress& DeviceAddress, const FInstancedStruct& Payload) override;
	virtual void Client_RemoveDescriptor(const FSLMDeviceAddress& DeviceAddress) override;
	virtual void Client_AddOrChangeState(const FSLMDeviceAddress& DeviceAddress, const FInstancedStruct& Payload) override;
	virtual void Client_RemoveState(const FSLMDeviceAddress& DeviceAddress) override;

private:
	//CRTP Hooks, must implement
	void RegisterPorts(const FSLMPortSettingsProbe& PortSettings, FSLMModelStateProbe& ModelState, const FSLMPortAddressesProbe& Addresses) const;
	void RemovePorts(const FSLMPortAddressesProbe& Addresses) const;
	void DeviceIDToPortAddresses(const int32 DeviceID, FSLMPortAddressesProbe& Addresses) const;
	static void ModelToCosmeticState(const FSLMModelProbe& Model, FSLMCosmeticStateProbe& CosmeticState);
	static void ModelToRepState(const FSLMModelProbe& Model, FSLMRepStateProbe& RepState);
	static void RepStateToModel(const FSLMRepStateProbe& RepState, FSLMModelProbe& Model);
	static void InputToModel(const FSLMInputProbe& Input, FSLMModelProbe& Model);
	
	UPROPERTY()
    USLMDomainRotation* DomainRotation;
	UPROPERTY()
	USLMDomainSignal* DomainSignal;
};

//Component for BP API
UCLASS(ClassGroup=("SLMechatronics"), meta=(BlueprintSpawnableComponent))
class SLMECHATRONICS_API USLMDeviceComponentProbe : public USLMDeviceComponentBase
{
	GENERATED_BODY()
	
public:
	USLMDeviceComponentProbe();
	
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "SLMechatronics")
	FSLMModelSettingsProbe GetDeviceSettings() const;
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "SLMechatronics")
	void SetDeviceSettings(const FSLMModelSettingsProbe& Settings) const;
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, BlueprintPure=false, Category = "SLMechatronics")
	void ApplyInput(const FSLMInputProbe& Input) const;
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "SLMechatronics")
	FSLMCosmeticStateProbe GetCosmeticState() const;
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "SLMechatronics")
	FSLMPortAddressesProbe GetPortAddresses() const;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	FSLMDeviceDescriptorProbe DeviceDescriptor;

private:
	//UPROPERTY(Replicated)
	//FSLMHandleProbe Handle;
	UPROPERTY(Replicated)
	FSLMDeviceAddress DeviceAddress;
	UPROPERTY()
	USLMDeviceSubsystemProbe* Subsystem;
};
