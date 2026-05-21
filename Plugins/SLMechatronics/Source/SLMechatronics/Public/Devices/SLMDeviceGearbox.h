// Copyright Spectrelight Studios, LLC

#pragma once

#include "CoreMinimal.h"
#include "SLMDeviceBase.h"
#include "Domains/SLMDomainRotation.h"
#include "Domains/SLMDomainSignal.h"
#include "SLMDeviceGearbox.generated.h"

class USLMDeviceSubsystemGearbox;

//Hot loop settings
USTRUCT(BlueprintType)
struct FSLMModelSettingsGearbox
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
	FString GetDebugString() const;
	uint32 GetDebugHash() const;
};

//Hot loop state
USTRUCT()
struct FSLMModelStateGearbox
{
	GENERATED_BODY()
	int32 CurrentGear = 0;
	int32 PreviousGear = 0;
	float GearRatio = 0.0;
	float LastShiftSignal = 0.0;
	int32 PortID_Rotation_Input = INDEX_NONE;
	int32 PortID_Rotation_Output = INDEX_NONE;
	int32 PortID_Signal_Shift = INDEX_NONE;
	bool bDirty = false;
	FString GetDebugString() const;
	uint32 GetDebugHash() const;
};

//Hot loop model
USTRUCT()
struct FSLMModelGearbox
{
	GENERATED_BODY()
	FSLMModelSettingsGearbox Settings;
	FSLMModelStateGearbox State;
	FString GetDebugString() const;
	uint32 GetDebugHash() const;
};

//Port Defaults
USTRUCT(BlueprintType)
struct FSLMPortSettingsGearbox
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	FSLMPortRotation Port_Rotation_Input;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	FSLMPortRotation Port_Rotation_Output;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	FSLMPortSignal Port_Signal_Shift;
};

//Device Defaults
USTRUCT(BlueprintType)
struct FSLMDeviceDescriptorGearbox
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	FSLMModelSettingsGearbox ModelSettings;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	FSLMPortSettingsGearbox PortSettings;
};

//Device port addresses
USTRUCT(BlueprintType)
struct FSLMPortAddressesGearbox
{
	GENERATED_BODY()
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	FSLMPortAddress Address_Rotation_Input;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	FSLMPortAddress Address_Rotation_Output;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	FSLMPortAddress Address_Signal_Shift;
};

//Dynamic Replicated State
USTRUCT()
struct FSLMRepStateGearbox
{
	GENERATED_BODY()
	UPROPERTY()
	int32 Gear = 0;
};

//Public cosmetic state
USTRUCT(BlueprintType)
struct FSLMCosmeticStateGearbox
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
struct FSLMInputGearbox
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	int32 DesiredGear = 0;
};



struct FSLMDeviceGearboxTraits
{
	//using FHandleType			= FSLMHandleGearbox;
	using FModelSettingsType	= FSLMModelSettingsGearbox;
	using FModelStateType		= FSLMModelStateGearbox;
	using FModelType			= FSLMModelGearbox;
	using FDescriptorType		= FSLMDeviceDescriptorGearbox;
	using FAddressesType		= FSLMPortAddressesGearbox;
	using FRepStateType			= FSLMRepStateGearbox;
	using FCosmeticStateType	= FSLMCosmeticStateGearbox;
	using FInputType			= FSLMInputGearbox;
};

//Core Subsystem
UCLASS()
class SLMECHATRONICS_API USLMDeviceSubsystemGearbox : public USLMDeviceSubsystemBase, public TSLMDeviceSystem<USLMDeviceSubsystemGearbox, FSLMDeviceGearboxTraits>
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
	void RegisterPorts(const FSLMPortSettingsGearbox& PortSettings, FSLMModelStateGearbox& ModelState, const FSLMPortAddressesGearbox& Addresses) const;
	void RemovePorts(const FSLMPortAddressesGearbox& Addresses) const;
	void DeviceIDToPortAddresses(const int32 DeviceID, FSLMPortAddressesGearbox& Addresses) const;
	void ModelToCosmeticState(const FSLMModelGearbox& Model, FSLMCosmeticStateGearbox& CosmeticState) const;
	static void ModelToRepState(const FSLMModelGearbox& Model, FSLMRepStateGearbox& RepState);
	static void RepStateToModel(const FSLMRepStateGearbox& RepState, FSLMModelGearbox& Model);
	static void InputToModel(const FSLMInputGearbox& Input, FSLMModelGearbox& Model);
	
	UPROPERTY()
    USLMDomainRotation* DomainRotation;
	UPROPERTY()
	USLMDomainSignal* DomainSignal;
};

//Component for BP API
UCLASS(ClassGroup=("SLMechatronics"), meta=(BlueprintSpawnableComponent))
class SLMECHATRONICS_API USLMDeviceComponentGearbox : public USLMDeviceComponentBase
{
	GENERATED_BODY()
	
public:
	USLMDeviceComponentGearbox();
	
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "SLMechatronics")
	FSLMModelSettingsGearbox GetDeviceSettings() const;
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "SLMechatronics")
	void SetDeviceSettings(const FSLMModelSettingsGearbox& Settings) const;
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, BlueprintPure=false, Category = "SLMechatronics")
	void ApplyInput(const FSLMInputGearbox& Input) const;
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "SLMechatronics")
	FSLMCosmeticStateGearbox GetCosmeticState() const;
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "SLMechatronics")
	FSLMPortAddressesGearbox GetPortAddresses() const;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	FSLMDeviceDescriptorGearbox DeviceDescriptor;

private:
	//UPROPERTY(Replicated)
	//FSLMHandleGearbox Handle;
	UPROPERTY(Replicated)
	FSLMDeviceAddress DeviceAddress;
	UPROPERTY()
	USLMDeviceSubsystemGearbox* Subsystem;
	bool bInitialized = false;
};