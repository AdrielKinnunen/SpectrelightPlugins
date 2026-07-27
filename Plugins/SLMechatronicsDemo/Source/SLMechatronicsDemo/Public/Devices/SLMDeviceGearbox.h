// Copyright Spectrelight Studios, LLC

#pragma once

#include "CoreMinimal.h"
#include "SLMDeviceTemplate.h"
#include "Domains/SLMDomainRotation.h"
#include "Domains/SLMDomainSignal.h"
#include "SLMDeviceGearbox.generated.h"


UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_SLM_System_Device_Gearbox);

//Sim Settings
USTRUCT(BlueprintType)
struct FSLMDeviceSettingsGearbox
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

//Sim State
USTRUCT()
struct FSLMDeviceStateGearbox
{
	GENERATED_BODY()
	
	int32 CurrentGear = 0;
	int32 PreviousGear = 0;
	float GearRatio = 0.0;
	float LastShiftSignal = 0.0;
	int32 PortID_Rotation_Input = INDEX_NONE;
	int32 PortID_Rotation_Output = INDEX_NONE;
	int32 PortID_Signal_Shift = INDEX_NONE;

	FString GetDebugString() const;
	uint32 GetDebugHash() const;
};

//Gameplay Context
USTRUCT(BlueprintType)
struct FSLMDeviceGameplayContextGearbox
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
	
	TDelegate<void(int32)> OnGearChanged;
};

//Port Defaults
USTRUCT(BlueprintType)
struct FSLMDevicePortsGearbox
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	FSLMPortRotation Port_Rotation_Input;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	FSLMPortRotation Port_Rotation_Output;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	FSLMPortSignal Port_Signal_Shift;
};

//Device Descriptor
USTRUCT(BlueprintType)
struct FSLMDeviceDescriptorGearbox
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	FSLMDeviceSettingsGearbox Settings;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	FSLMDevicePortsGearbox Ports;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	FSLMSpatialContextAuthored SpatialContext;
	UPROPERTY(VisibleAnywhere, Transient, BlueprintReadOnly, Category = "SLMechatronics")
	TWeakObjectPtr<AActor> AssociatedActor;
};

//Device port addresses
USTRUCT(BlueprintType)
struct FSLMDevicePortAddressesGearbox
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
struct FSLMDeviceRepStateGearbox
{
	GENERATED_BODY()
	UPROPERTY()
	int32 CurrentGear = 0;
};


struct FSLMDeviceGearboxTraits
{
	using FDescriptorType		= FSLMDeviceDescriptorGearbox;
	using FSettingsType			= FSLMDeviceSettingsGearbox;
	using FStateType			= FSLMDeviceStateGearbox;
	using FGameplayContextType	= FSLMDeviceGameplayContextGearbox;
	using FAddressesType		= FSLMDevicePortAddressesGearbox;
	using FRepStateType			= FSLMDeviceRepStateGearbox;
};

//Core System
class SLMECHATRONICSDEMO_API FSLMDeviceSystemGearbox : public TSLMDeviceSystem<FSLMDeviceSystemGearbox, FSLMDeviceGearboxTraits>
{
	friend class TSLMDeviceSystem<FSLMDeviceSystemGearbox, FSLMDeviceGearboxTraits>;
	
public:
	FSLMDeviceSystemGearbox();
	static FGameplayTag GetSystemTagStatic();
	virtual void Initialize() override;
	
private:
	void PreSimulate_Impl(const int32 ID, const float DeltaTime);
	void Simulate_Impl(const int32 ID, const float DeltaTime, const float SubstepScalar);
	void PostSimulate_Impl(const int32 ID, const float DeltaTime);
	void RegisterPorts_Impl(const int32 DeviceID, const FSLMDevicePortAddressesGearbox& Addresses);
	void RemovePorts_Impl(const FSLMDevicePortAddressesGearbox& Addresses) const;
	void GetPortAddresses_Impl(const int32 DeviceID, FSLMDevicePortAddressesGearbox& OutAddresses) const;
	void GetRepState_Impl(const int32 DeviceID, FSLMDeviceRepStateGearbox& OutRepState) const;
	void ApplyRepState_Impl(const int32 DeviceID, const FSLMDeviceRepStateGearbox& RepState);
	
    FSLMDomainSystemRotation* DomainRotation = nullptr;
	FSLMDomainSystemSignal* DomainSignal = nullptr;
};







//Component for BP API
UCLASS(ClassGroup=("SLMechatronics"), meta=(BlueprintSpawnableComponent))
class SLMECHATRONICSDEMO_API USLMDeviceComponentGearbox : public USLMDeviceComponentBase
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
	FSLMDeviceDescriptorGearbox GetDeviceDescriptor() const;
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "SLMechatronics")
	void EditDevice(const FSLMDeviceDescriptorGearbox& Descriptor);
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "SLMechatronics")
	FSLMDevicePortAddressesGearbox GetPortAddresses() const;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	FSLMDeviceDescriptorGearbox DefaultDeviceDescriptor;
	FSLMDeviceSystemGearbox* DeviceSystem = nullptr;
	
	
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "SLMechatronics")
	FSLMDeviceGameplayContextGearbox GetGameplayContext() const;
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGearChangedSignature, int32, NewGear);
	UPROPERTY(BlueprintAssignable)
	FOnGearChangedSignature OnGearChanged;
	void HandleGearChanged(const int32 NewGear) const;
};