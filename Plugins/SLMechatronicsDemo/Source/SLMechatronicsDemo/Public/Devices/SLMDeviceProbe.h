// Copyright Spectrelight Studios, LLC

#pragma once

#include "CoreMinimal.h"
#include "SLMDeviceTemplate.h"
#include "Domains/SLMDomainRotation.h"
#include "Domains/SLMDomainSignal.h"
#include "SLMDeviceProbe.generated.h"


UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_SLM_System_Device_Probe);

//Sim Settings
USTRUCT(BlueprintType)
struct FSLMDeviceSettingsProbe
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	float Smoothing = 0;
	
	FString GetDebugString() const;
	uint32 GetDebugHash() const;
};

//Sim State
USTRUCT()
struct FSLMDeviceStateProbe
{
	GENERATED_BODY()
	
	float ProbeValue = 0.0;
	int32 PortID_Rotation = INDEX_NONE;
	int32 PortID_Signal = INDEX_NONE;
	
	FString GetDebugString() const;
	uint32 GetDebugHash() const;
};

//Gameplay Context
USTRUCT(BlueprintType)
struct FSLMDeviceGameplayContextProbe
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly, Category = "SLMechatronics")
	float DesiredProbeValue = 0.0;
};

//Port Defaults
USTRUCT(BlueprintType)
struct FSLMDevicePortsProbe
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
	FSLMDeviceSettingsProbe Settings;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	FSLMDevicePortsProbe Ports;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	FSLMSpatialContextAuthored SpatialContext;
	UPROPERTY(VisibleAnywhere, Transient, BlueprintReadOnly, Category = "SLMechatronics")
	TWeakObjectPtr<AActor> AssociatedActor;
};

//Device port addresses
USTRUCT(BlueprintType)
struct FSLMDevicePortAddressesProbe
{
	GENERATED_BODY()
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	FSLMPortAddress Address_Rotation;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	FSLMPortAddress Address_Signal;
};

//Dynamic Replicated State
USTRUCT()
struct FSLMDeviceRepStateProbe
{
	GENERATED_BODY()
	UPROPERTY()
	float ProbeValue = 0.0;
	UPROPERTY()
	float ProbeAngVel = 0.0;
};



struct FSLMDeviceProbeTraits
{
	using FDescriptorType		= FSLMDeviceDescriptorProbe;
	using FSettingsType			= FSLMDeviceSettingsProbe;
	using FStateType			= FSLMDeviceStateProbe;
	using FGameplayContextType	= FSLMDeviceGameplayContextProbe;
	using FAddressesType		= FSLMDevicePortAddressesProbe;
	using FRepStateType			= FSLMDeviceRepStateProbe;
};

//Core System
class SLMECHATRONICSDEMO_API FSLMDeviceSystemProbe : public TSLMDeviceSystem<FSLMDeviceSystemProbe, FSLMDeviceProbeTraits>
{
	friend class TSLMDeviceSystem<FSLMDeviceSystemProbe, FSLMDeviceProbeTraits>;
	
public:
	FSLMDeviceSystemProbe();
	static FGameplayTag GetSystemTagStatic();
	virtual void Initialize() override;
	
private:
	void PreSimulate_Impl(const int32 ID, const float DeltaTime);
	void Simulate_Impl(const int32 ID, const float DeltaTime, const float SubstepScalar);
	void PostSimulate_Impl(const int32 ID, const float DeltaTime);
	void RegisterPorts_Impl(const int32 DeviceID, const FSLMDevicePortAddressesProbe& Addresses);
	void RemovePorts_Impl(const FSLMDevicePortAddressesProbe& Addresses) const;
	void GetPortAddresses_Impl(const int32 DeviceID, FSLMDevicePortAddressesProbe& OutAddresses) const;
	void GetRepState_Impl(const int32 DeviceID, FSLMDeviceRepStateProbe& OutRepState) const;
	void ApplyRepState_Impl(const int32 DeviceID, const FSLMDeviceRepStateProbe& RepState);
	
    FSLMDomainSystemRotation* DomainRotation = nullptr;
	FSLMDomainSystemSignal* DomainSignal = nullptr;
};







//Component for BP API
UCLASS(ClassGroup=("SLMechatronics"), meta=(BlueprintSpawnableComponent))
class SLMECHATRONICSDEMO_API USLMDeviceComponentProbe : public USLMDeviceComponentBase
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
	FSLMDeviceDescriptorProbe GetDeviceDescriptor() const;
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "SLMechatronics")
	void EditDevice(const FSLMDeviceDescriptorProbe& Descriptor);
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "SLMechatronics")
	FSLMDevicePortAddressesProbe GetPortAddresses() const;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	FSLMDeviceDescriptorProbe DefaultDeviceDescriptor;
	FSLMDeviceSystemProbe* DeviceSystem = nullptr;
	
	
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "SLMechatronics")
	void SetDesiredProbeValue(const float Value);
};