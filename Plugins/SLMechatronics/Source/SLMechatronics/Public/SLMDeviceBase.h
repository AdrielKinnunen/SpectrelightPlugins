// Copyright Spectrelight Studios, LLC

#pragma once

#include "CoreMinimal.h"
#include "SLMSystemBase.h"
#include "SLMTypes.h"
#include "StructUtils/InstancedStruct.h"
#include "SLMDeviceBase.generated.h"

UCLASS(Abstract)
class SLMECHATRONICS_API USLMDeviceComponentBase : public UActorComponent
{
    GENERATED_BODY()
public:
    USLMDeviceComponentBase();
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UPROPERTY(ReplicatedUsing = OnRep_DeviceAddress, BlueprintReadOnly, Category = "SLMechatronics")
	FSLMDeviceAddress DeviceAddress;
	bool bDeviceResolved = false;

protected:
	UFUNCTION()
	void OnRep_DeviceAddress(const FSLMDeviceAddress OldDeviceAddress);
	void TryResolveDevice(const FSLMDeviceAddress NewDeviceAddress);
	virtual void OnManagerReady();
	virtual void OnDeviceResolved();
	
	
	
protected:	
	
	
	
	UPROPERTY()
	USLMManager* Manager = nullptr;
};


class SLMECHATRONICS_API FSLMDeviceSystemBase : public FSLMSystemBase
{
public:
	
	virtual TUniquePtr<FSLMDeviceSystemBase> CreateInstance() const = 0;
	
	virtual void OnRepSetDescriptor(const FSLMDeviceAddress& DeviceAddress, const FInstancedStruct& Payload) = 0;
	virtual void OnRepRemoveDescriptor(const FSLMDeviceAddress& DeviceAddress) = 0;
	virtual void OnRepSetState(const FSLMDeviceAddress& DeviceAddress, const FInstancedStruct& Payload) = 0;
	virtual void OnRepRemoveState(const FSLMDeviceAddress& DeviceAddress) = 0;
	
	virtual FSLMDeviceSnapshot GetDeviceSnapshot(const FSLMDeviceAddress& DeviceAddress) const = 0;
	virtual FSLMDeviceAddress AddDeviceBySnapshot(const FSLMDeviceSnapshot& DeviceSnapshot, AActor* AssociatedActor) = 0;
	
	TMulticastDelegate<void(FSLMDeviceAddress)> OnDeviceAdded;
	bool DeviceExists(const FSLMDeviceAddress& DeviceAddress) const;
	
	TBitArray<> OccupiedDeviceIDs;
};