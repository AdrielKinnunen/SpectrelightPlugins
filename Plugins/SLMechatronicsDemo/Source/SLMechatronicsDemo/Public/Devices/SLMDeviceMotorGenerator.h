// Copyright Spectrelight Studios, LLC

#pragma once

#include "CoreMinimal.h"
#include "SLMDeviceTemplate.h"
#include "Domains/SLMDomainElectricity.h"
#include "Domains/SLMDomainRotation.h"
#include "Domains/SLMDomainSignal.h"
#include "SLMDeviceMotorGenerator.generated.h"


UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_SLM_System_Device_MotorGenerator);

//Sim Settings
USTRUCT(BlueprintType)
struct FSLMDeviceSettingsMotorGenerator
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	float MaxPowerWatts = 10000;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	float ConstantTorqueAngVel = 10;
	
	FString GetDebugString() const;
	uint32 GetDebugHash() const;
};

//Sim State
USTRUCT()
struct FSLMDeviceStateMotorGenerator
{
	GENERATED_BODY()
	
	//float PowerFlowWatts = 0.0;
	
	int32 PortID_Rotation = INDEX_NONE;
	int32 PortID_Electricity = INDEX_NONE;
	int32 PortID_Signal_Throttle = INDEX_NONE;

	FString GetDebugString() const;
	uint32 GetDebugHash() const;
};

//Gameplay Context
USTRUCT(BlueprintType)
struct FSLMDeviceGameplayContextMotorGenerator
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly, Category = "SLMechatronics")
	float CurrentThrottle = 0.0;
	UPROPERTY(BlueprintReadOnly, Category = "SLMechatronics")
	float CurrentRPM = 0.0;
};

//Port Defaults
USTRUCT(BlueprintType)
struct FSLMDevicePortsMotorGenerator
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	FSLMPortRotation Port_Rotation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	FSLMPortElectricity Port_Electricity;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	FSLMPortSignal Port_Signal_Throttle;
};

//Device Descriptor
USTRUCT(BlueprintType)
struct FSLMDeviceDescriptorMotorGenerator
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	FSLMDeviceSettingsMotorGenerator Settings;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	FSLMDevicePortsMotorGenerator Ports;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	FSLMSpatialContextAuthored SpatialContext;
	UPROPERTY(VisibleAnywhere, Transient, BlueprintReadOnly, Category = "SLMechatronics")
	TWeakObjectPtr<AActor> AssociatedActor;
};

//Device port addresses
USTRUCT(BlueprintType)
struct FSLMDevicePortAddressesMotorGenerator
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
struct FSLMDeviceRepStateMotorGenerator
{
	GENERATED_BODY()
};


struct FSLMDeviceMotorGeneratorTraits
{
	using FDescriptorType		= FSLMDeviceDescriptorMotorGenerator;
	using FSettingsType			= FSLMDeviceSettingsMotorGenerator;
	using FStateType			= FSLMDeviceStateMotorGenerator;
	using FGameplayContextType	= FSLMDeviceGameplayContextMotorGenerator;
	using FAddressesType		= FSLMDevicePortAddressesMotorGenerator;
	using FRepStateType			= FSLMDeviceRepStateMotorGenerator;
};

//Core System
class SLMECHATRONICSDEMO_API FSLMDeviceSystemMotorGenerator : public TSLMDeviceSystem<FSLMDeviceSystemMotorGenerator, FSLMDeviceMotorGeneratorTraits>
{
	friend class TSLMDeviceSystem<FSLMDeviceSystemMotorGenerator, FSLMDeviceMotorGeneratorTraits>;
	
public:
	FSLMDeviceSystemMotorGenerator();
	static FGameplayTag GetSystemTagStatic();
	virtual void Initialize() override;
	
private:
	void PreSimulate_Impl(const int32 ID, const float DeltaTime);
	void Simulate_Impl(const int32 ID, const float DeltaTime, const float SubstepScalar);
	void PostSimulate_Impl(const int32 ID, const float DeltaTime);
	void RegisterPorts_Impl(const int32 DeviceID, const FSLMDevicePortAddressesMotorGenerator& Addresses);
	void RemovePorts_Impl(const FSLMDevicePortAddressesMotorGenerator& Addresses) const;
	void GetPortAddresses_Impl(const int32 DeviceID, FSLMDevicePortAddressesMotorGenerator& OutAddresses) const;
	void GetRepState_Impl(const int32 DeviceID, FSLMDeviceRepStateMotorGenerator& OutRepState) const;
	void ApplyRepState_Impl(const int32 DeviceID, const FSLMDeviceRepStateMotorGenerator& RepState);
	
    FSLMDomainSystemRotation* DomainRotation = nullptr;
	FSLMDomainSystemSignal* DomainSignal = nullptr;
	FSLMDomainSystemElectricity* DomainElectricity = nullptr;
};







//Component for BP API
UCLASS(ClassGroup=("SLMechatronics"), meta=(BlueprintSpawnableComponent))
class SLMECHATRONICSDEMO_API USLMDeviceComponentMotorGenerator : public USLMDeviceComponentBase
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
protected:
	virtual void OnManagerReady() override;
	virtual void OnDeviceResolved() override;
	
public:
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "SLMechatronics")
	FSLMDeviceDescriptorMotorGenerator GetDeviceDescriptor() const;
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "SLMechatronics")
	void EditDevice(const FSLMDeviceDescriptorMotorGenerator& Descriptor);
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "SLMechatronics")
	FSLMDevicePortAddressesMotorGenerator GetPortAddresses() const;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	FSLMDeviceDescriptorMotorGenerator DefaultDeviceDescriptor;
	FSLMDeviceSystemMotorGenerator* DeviceSystem = nullptr;
	
	
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "SLMechatronics")
	FSLMDeviceGameplayContextMotorGenerator GetGameplayContext() const;
};