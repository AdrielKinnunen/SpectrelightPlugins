// Copyright Spectrelight Studios, LLC
#if 0
#pragma once

#include "CoreMinimal.h"
#include "SLMDeviceBase.h"
#include "Domains/SLMDomainElectricity.h"
#include "Domains/SLMDomainRotation.h"
#include "Domains/SLMDomainSignal.h"
#include "SLMDeviceMotor.generated.h"

class USLMDeviceSubsystemMotorGenerator;


//Typed Handle for safety
USTRUCT(BlueprintType)
struct FSLMHandleMotorGenerator
{
	GENERATED_BODY()
	UPROPERTY()
	int32 ID = INDEX_NONE;
};

//Hot loop settings
USTRUCT(BlueprintType)
struct FSLMModelSettingsMotorGenerator
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	float MaxPowerWatts = 10000;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	float ConstantTorqueAngVel = 10;
	FString GetDebugString() const;
	uint32 GetDebugHash() const;
};

//Hot loop state
USTRUCT()
struct FSLMModelStateMotorGenerator
{
	GENERATED_BODY()
	float PowerFlow = 0.0;
	int32 PortID_Rotation = INDEX_NONE;
	int32 PortID_Electricity = INDEX_NONE;
	int32 PortID_Signal_Throttle = INDEX_NONE;
	bool bDirty = false;
	FString GetDebugString() const;
	uint32 GetDebugHash() const;
};

//Hot loop model
USTRUCT()
struct FSLMModelMotorGenerator
{
	GENERATED_BODY()
	FSLMModelSettingsMotorGenerator Settings;
	FSLMModelStateMotorGenerator State;
	FString GetDebugString() const;
	uint32 GetDebugHash() const;
};

//Port Defaults
USTRUCT(BlueprintType)
struct FSLMPortSettingsMotorGenerator
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	FSLMPortRotation Port_Rotation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	FSLMPortElectricity Port_Electricity;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	FSLMPortSignal Port_Signal_Throttle;
};

//Device Defaults
USTRUCT(BlueprintType)
struct FSLMDeviceDescriptorMotorGenerator
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	FSLMModelSettingsMotorGenerator ModelSettings;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	FSLMPortSettingsMotorGenerator PortSettings;
};

//Device port addresses
USTRUCT(BlueprintType)
struct FSLMPortAddressesMotorGenerator
{
	GENERATED_BODY()
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	FSLMPortAddress Address_Rotation;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	FSLMPortAddress Address_Electricity;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	FSLMPortAddress Address_Signal_Throttle;
};

//Dynamic Replicated State
USTRUCT()
struct FSLMRepStateMotorGenerator
{
	GENERATED_BODY()	
};

//Public cosmetic state
USTRUCT(BlueprintType)
struct FSLMCosmeticStateMotorGenerator
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly, Category = "SLMechatronics")
	float PowerFlowWatts = 0.0;
	UPROPERTY(BlueprintReadOnly, Category = "SLMechatronics")
	float AngVelDegS = 0.0;
};

//Public Input Structure
USTRUCT(BlueprintType)
struct FSLMInputMotorGenerator
{
	GENERATED_BODY()
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	//int32 DesiredGear = 0;
};



struct FSLMMotorGeneratorTraits
{
	using FHandleType			= FSLMHandleMotorGenerator;
	using FModelSettingsType	= FSLMModelSettingsMotorGenerator;
	using FModelStateType		= FSLMModelStateMotorGenerator;
	using FModelType			= FSLMModelMotorGenerator;
	using FDescriptorType		= FSLMDeviceDescriptorMotorGenerator;
	using FAddressesType		= FSLMPortAddressesMotorGenerator;
	using FRepStateType			= FSLMRepStateMotorGenerator;
	using FCosmeticStateType	= FSLMCosmeticStateMotorGenerator;
	using FInputType			= FSLMInputMotorGenerator;
};

//Core Subsystem
UCLASS()
class SLMECHATRONICS_API USLMDeviceSubsystemMotorGenerator : public USLMDeviceSubsystemBase, public TSLMDeviceSystem<USLMDeviceSubsystemMotorGenerator, FSLMMotorGeneratorTraits>
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
	virtual void Client_AddOrChangeDescriptor(const int32 HandleID, const FInstancedStruct& Payload) override;
	virtual void Client_RemoveDescriptor(const int32 HandleID) override;
	virtual void Client_AddOrChangeState(const int32 HandleID, const FInstancedStruct& Payload) override;
	virtual void Client_RemoveState(const int32 HandleID) override;

private:
	//CRTP Hooks, must implement
	void RegisterPorts(const FSLMPortSettingsMotorGenerator& PortSettings, FSLMModelStateMotorGenerator& ModelState, const FSLMPortAddressesMotorGenerator& Addresses) const;
	void RemovePorts(const FSLMPortAddressesMotorGenerator& Addresses) const;
	void HandleToAddresses(const FSLMHandleMotorGenerator Handle, FSLMPortAddressesMotorGenerator& Addresses) const;
	void ModelToCosmeticState(const FSLMModelMotorGenerator& Model, FSLMCosmeticStateMotorGenerator& CosmeticState) const;
	static void ModelToRepState(const FSLMModelMotorGenerator& Model, FSLMRepStateMotorGenerator& RepState);
	static void RepStateToModel(const FSLMRepStateMotorGenerator& RepState, FSLMModelMotorGenerator& Model);
	static void InputToModel(const FSLMInputMotorGenerator& Input, FSLMModelMotorGenerator& Model);
	
	UPROPERTY()
    USLMDomainRotation* DomainRotation;
	UPROPERTY()
	USLMDomainElectricity* DomainElectricity;
	UPROPERTY()
	USLMDomainSignal* DomainSignal;
};


//Component for BP API
UCLASS(ClassGroup=("SLMechatronics"), meta=(BlueprintSpawnableComponent))
class SLMECHATRONICS_API USLMDeviceComponentMotorGenerator : public USLMDeviceComponentBase
{
	GENERATED_BODY()
	
public:
	USLMDeviceComponentMotorGenerator();
	
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "SLMechatronics")
	FSLMModelSettingsMotorGenerator GetDeviceSettings() const;
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "SLMechatronics")
	void SetDeviceSettings(const FSLMModelSettingsMotorGenerator& Settings) const;
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, BlueprintPure=false, Category = "SLMechatronics")
	void ApplyInput(const FSLMInputMotorGenerator& Input) const;
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "SLMechatronics")
	FSLMCosmeticStateMotorGenerator GetCosmeticState() const;
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "SLMechatronics")
	FSLMPortAddressesMotorGenerator GetPortAddresses() const;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	FSLMDeviceDescriptorMotorGenerator DeviceDescriptor;

private:
	UPROPERTY(Replicated)
	FSLMHandleMotorGenerator Handle;
	UPROPERTY()
	USLMDeviceSubsystemMotorGenerator* Subsystem;
};
#endif