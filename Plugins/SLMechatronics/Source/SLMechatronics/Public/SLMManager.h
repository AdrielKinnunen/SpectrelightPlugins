// Copyright Spectrelight Studios, LLC

#pragma once

#include "CoreMinimal.h"
#include "SLMDeviceBase.h"
#include "SLMDomainBase.h"
#include "SLMTypes.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "StructUtils/InstancedStruct.h"
#include "Subsystems/WorldSubsystem.h"
#include "SLMManager.generated.h"

class USLMManager;
class ASLMManagerReplicator;

DECLARE_STATS_GROUP(TEXT("SLMechatronics"), STATGROUP_SLMechatronics, STATCAT_Advanced);


USTRUCT()
struct FSLMechatronicsSubsystemTickFunction : public FTickFunction
{
    GENERATED_BODY()
    UPROPERTY()
    USLMManager* Target = nullptr;
    SLMECHATRONICS_API virtual void ExecuteTick(float DeltaTime, ELevelTick TickType, ENamedThreads::Type CurrentThread, const FGraphEventRef& MyCompletionEventGraph) override;
    SLMECHATRONICS_API virtual FString DiagnosticMessage() override;
    SLMECHATRONICS_API virtual FName DiagnosticContext(bool bDetailed) override;
};
template <>
struct TStructOpsTypeTraits<FSLMechatronicsSubsystemTickFunction> : public TStructOpsTypeTraitsBase2<FSLMechatronicsSubsystemTickFunction>
{
    enum { WithCopy = false };
};






USTRUCT()
struct FSLMRepItemDeviceDescriptor : public FFastArraySerializerItem
{
	GENERATED_BODY()
	UPROPERTY()
	FSLMDeviceAddress DeviceAddress;
	UPROPERTY()
	FInstancedStruct DeviceDescriptor;
};

USTRUCT()
struct FSLMRepArrayDeviceDescriptors : public FFastArraySerializer
{
	GENERATED_BODY()
	UPROPERTY()
	TArray<FSLMRepItemDeviceDescriptor> Items;
	UPROPERTY()
	USLMManager* Manager = nullptr;
	void PostReplicatedAdd(const TArrayView<int32>& AddedIndices, int32 FinalSize) const;
	void PostReplicatedChange(const TArrayView<int32>& ChangedIndices, int32 FinalSize) const;
	void PreReplicatedRemove(const TArrayView<int32>& RemovedIndices, int32 FinalSize) const;
	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FastArrayDeltaSerialize<FSLMRepItemDeviceDescriptor, FSLMRepArrayDeviceDescriptors>(Items, DeltaParms, *this);
	}
};
template<>
struct TStructOpsTypeTraits<FSLMRepArrayDeviceDescriptors> : public TStructOpsTypeTraitsBase2<FSLMRepArrayDeviceDescriptors>
{
	enum 
	{
		WithNetDeltaSerializer = true,
   };
};










USTRUCT()
struct FSLMRepItemDeviceState : public FFastArraySerializerItem
{
	GENERATED_BODY()
	UPROPERTY()
	FSLMDeviceAddress DeviceAddress;
	UPROPERTY()
	FInstancedStruct DeviceState;
};

USTRUCT()
struct FSLMRepArrayDeviceState : public FFastArraySerializer
{
	GENERATED_BODY()
	UPROPERTY()
	TArray<FSLMRepItemDeviceState> Items;
	UPROPERTY()
	USLMManager* Manager = nullptr;
	void PostReplicatedAdd(const TArrayView<int32>& AddedIndices, int32 FinalSize) const;
	void PostReplicatedChange(const TArrayView<int32>& ChangedIndices, int32 FinalSize) const;
	void PreReplicatedRemove(const TArrayView<int32>& RemovedIndices, int32 FinalSize) const;
	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FastArrayDeltaSerialize<FSLMRepItemDeviceState, FSLMRepArrayDeviceState>(Items, DeltaParms, *this);
	}
};
template<>
struct TStructOpsTypeTraits<FSLMRepArrayDeviceState> : public TStructOpsTypeTraitsBase2<FSLMRepArrayDeviceState>
{
	enum 
	{
		WithNetDeltaSerializer = true,
   };
};






USTRUCT()
struct FSLMRepItemConnection : public FFastArraySerializerItem
{
	GENERATED_BODY()
	UPROPERTY()
	FSLMConnection Connection;
};

USTRUCT()
struct FSLMRepArrayConnections : public FFastArraySerializer
{
	GENERATED_BODY()
	UPROPERTY()
	TArray<FSLMRepItemConnection> Items;
	UPROPERTY()
	USLMManager* Manager = nullptr;
	void PostReplicatedAdd(const TArrayView<int32>& AddedIndices, int32 FinalSize) const;
	void PostReplicatedChange(const TArrayView<int32>& ChangedIndices, int32 FinalSize) const;
	void PreReplicatedRemove(const TArrayView<int32>& RemovedIndices, int32 FinalSize) const;
	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FastArrayDeltaSerialize<FSLMRepItemConnection, FSLMRepArrayConnections>(Items, DeltaParms, *this);
	}
};
template<>
struct TStructOpsTypeTraits<FSLMRepArrayConnections> : public TStructOpsTypeTraitsBase2<FSLMRepArrayConnections>
{
	enum 
	{
		WithNetDeltaSerializer = true,
   };
};







USTRUCT()
struct FSLMRemappingContext
{
	GENERATED_BODY()

	UPROPERTY()
	FSLMDeviceAddress Address;
	UPROPERTY()
	FInstancedStruct Descriptor;
};









//Manager
UCLASS()
class SLMECHATRONICS_API USLMManager : public UWorldSubsystem
{
    GENERATED_BODY()
	
	template <typename SubsystemType, typename TraitsType>
	friend class TSLMDeviceSystem;
	friend class ASLMManagerReplicator;
	friend struct FSLMRepArrayDeviceDescriptors;
	friend struct FSLMRepArrayConnections;
	friend struct FSLMRepArrayDeviceState;
	
public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	
	//Tick
    void Tick(const float DeltaTime);
	FSLMechatronicsSubsystemTickFunction PrimarySystemTick;
	UPROPERTY(BlueprintReadWrite, Category="SLMechatronics")
	int32 StepCount = 10;
	
	//Remapping Context
	void OpenRemappingContext();
	void CloseRemappingContext();
	bool bRemappingContextOpen = false;
	TMap<FSLMDeviceAddress, FSLMDeviceAddress> OldAddressToNewAddress;
	TMap<FName, FName> OldActorNameToNewActorName;
	
	//Device Snapshots
	FSLMDeviceSnapshot GetDeviceSnapshot(const FSLMDeviceAddress& DeviceAddress);
	FSLMDeviceAddress AddDeviceBySnapshot(const FSLMDeviceSnapshot& Snapshot, AActor* AssociatedActor);
	TArray<FSLMDeviceSnapshot> GetDeviceSnapshotsByActor(AActor* Actor);
	void ApplyDeviceSnapshotsByActor(const TArray<FSLMDeviceSnapshot>& Snapshots, AActor* Actor);
	
	
	//Ports and Connections
	void AddConnection(const FSLMConnection& Connection);
	void RemoveConnection(const FSLMConnection& Connection);
	bool WorldLocationToPortAddress(FGameplayTag DomainTag, const FSLMSpatialContextRuntime& Filter, const FVector& WorldLocation, FSLMPortAddress& OutAddress);
	bool PortAddressToWorldLocation(const FSLMPortAddress& PortAddress, FVector& OutWorldLocation);
	bool DoesConnectionExist(const FSLMConnection& Connection);
	TArray<FSLMPortAddress> GetPortAddressesForDevices(const TArray<FSLMDeviceAddress>& DeviceAddresses) const;
	TArray<FSLMConnection> GetConnectionsWithinPortAddresses(const TArray<FSLMPortAddress>& PortAddresses) const;
	TArray<FSLMConnection> GetConnectionsWithinDevices(const TArray<FSLMDeviceAddress>& DeviceAddresses) const;
	
	//Debug
	FString GetGlobalDebugString(const bool bVerbose);
	int32 GetGlobalDebugHash(const bool bVerbose);
	FString GetPortDebugString(const FSLMPortAddress& Address);
	FString GetDeviceDebugString(const FSLMPortAddress& Address);
	UPROPERTY(BlueprintReadWrite, Category="SLMechatronics")
	bool bDebugDraw = true;


	//Network Stuff
	void Local_AddConnection(const FSLMConnection& Connection);
	void Local_RemoveConnection(const FSLMConnection& Connection);
	void OnRepSetDescriptor(const FSLMDeviceAddress& DeviceAddress, const FInstancedStruct& Payload);
	void OnRepRemoveDescriptor(const FSLMDeviceAddress& DeviceAddress) const;
	void OnRepSetState(const FSLMDeviceAddress& DeviceAddress, const FInstancedStruct& Payload);
	void OnRepRemoveState(const FSLMDeviceAddress& DeviceAddress) const;
	bool bIsServer = false;
	//UPROPERTY()
	//ASLMManagerReplicator* Replicator;
	TWeakObjectPtr<ASLMManagerReplicator> Replicator;

	//Systems
	static void RegisterDomainSystem(TUniquePtr<FSLMDomainSystemBase> Prototype);
	static void RegisterDeviceSystem(TUniquePtr<FSLMDeviceSystemBase> Prototype);
	FSLMDeviceSystemBase* GetDeviceSystemByTag(const FGameplayTag& Tag);
	static TArray<TUniquePtr<FSLMDomainSystemBase>> DomainSystemPrototypes;
	static TArray<TUniquePtr<FSLMDeviceSystemBase>> DeviceSystemPrototypes;
	TArray<TUniquePtr<FSLMDomainSystemBase>> DomainSystems;
	TArray<TUniquePtr<FSLMDeviceSystemBase>> DeviceSystems;
	
	template<typename DomainSystemType>
	DomainSystemType* GetDomainSystem()
	{
		static_assert(std::is_base_of_v<FSLMDomainSystemBase, DomainSystemType>, "DomainSystemType must derive from FSLMDomainSystemBase");
		for (const auto& DomainSystem : DomainSystems)
		{
			if (DomainSystem->SystemTag == DomainSystemType::GetSystemTagStatic())
			{
				return static_cast<DomainSystemType*>(DomainSystem.Get());
			}
		}
		checkNoEntry();
		return nullptr;
	}

	template<typename DeviceSystemType>
	DeviceSystemType* GetDeviceSystem()
	{
		static_assert(std::is_base_of_v<FSLMDeviceSystemBase, DeviceSystemType>, "DeviceSystemType must derive from FSLMDeviceSystemBase");
		for (const auto& DeviceSystem : DeviceSystems)
		{
			if (DeviceSystem->SystemTag == DeviceSystemType::GetSystemTagStatic())
			{
				return static_cast<DeviceSystemType*>(DeviceSystem.Get());
			}
		}
		checkNoEntry();
		return nullptr;
	}
};





//Replicator Actor
UCLASS()
class ASLMManagerReplicator : public AInfo
{
	GENERATED_BODY()
public:
	ASLMManagerReplicator();
	virtual void PostInitializeComponents() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	SLMECHATRONICS_API void AddConnection(const FSLMConnection& Connection);
	SLMECHATRONICS_API void RemoveConnection(const FSLMConnection& Connection);
	SLMECHATRONICS_API void SetDescriptor(const FSLMDeviceAddress& Address, const FInstancedStruct& Payload);
	SLMECHATRONICS_API void RemoveDescriptor(const FSLMDeviceAddress& Address);
	SLMECHATRONICS_API void SetState(const FSLMDeviceAddress& Address, const FInstancedStruct& Payload);
	SLMECHATRONICS_API void RemoveState(const FSLMDeviceAddress& Address);
	
private:
	UPROPERTY(Replicated)
	FSLMRepArrayDeviceDescriptors RepArrayDescriptors;
	UPROPERTY(Replicated)
	FSLMRepArrayDeviceState RepArrayState;
	UPROPERTY(Replicated)
	FSLMRepArrayConnections RepArrayConnections;
	
	TMap<FSLMDeviceAddress, int32> DeviceAddressToDescriptorIndex;
	TMap<FSLMDeviceAddress, int32> DeviceAddressToStateIndex;
	TMap<FSLMConnection, int32> ConnectionToIndex;
};









UCLASS()
class SLMECHATRONICS_API USLMBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
	UFUNCTION(BlueprintPure, Category="SLMechatronics")
	static bool IsValidAddress(const FSLMPortAddress& Address);
	UFUNCTION(BlueprintPure, Category="SLMechatronics")
	static bool IsValidConnection(const FSLMConnection& Connection);
	UFUNCTION(BlueprintCallable, Category="SLMechatronics", meta=(WorldContext="WorldContextObject"))
	static bool DoesConnectionExist(const UObject* WorldContextObject, const FSLMConnection Connection);
	UFUNCTION(BlueprintCallable, Category="SLMechatronics", meta=(WorldContext="WorldContextObject"))
	static bool WorldLocationToPortAddress(const UObject* WorldContextObject, const FGameplayTag DomainTag, const FSLMSpatialContextRuntime& Filter, const FVector& WorldLocation, FSLMPortAddress& OutAddress);
	UFUNCTION(BlueprintCallable, Category="SLMechatronics", meta=(WorldContext="WorldContextObject"))
	static bool PortAddressToWorldLocation(const UObject* WorldContextObject, const FSLMPortAddress& PortAddress, FVector& OutWorldLocation);
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="SLMechatronics", meta=(WorldContext="WorldContextObject"))
	static void AddConnection(const UObject* WorldContextObject, const FSLMConnection Connection);
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="SLMechatronics", meta=(WorldContext="WorldContextObject"))
	static void RemoveConnection(const UObject* WorldContextObject, const FSLMConnection Connection);
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="SLMechatronics", meta=(WorldContext="WorldContextObject"))
	static TArray<FSLMConnection> GetConnectionsWithinDevices(const UObject* WorldContextObject, const TArray<FSLMDeviceAddress>& DeviceAddresses);

	
	
	
	
	UFUNCTION(BlueprintCallable, Category="SLMechatronics", meta=(WorldContext="WorldContextObject"))
	static int32 GetGlobalDebugHash(const UObject* WorldContextObject, const bool bVerbose);
	UFUNCTION(BlueprintCallable, Category="SLMechatronics", meta=(WorldContext="WorldContextObject"))
	static FString GetGlobalDebugString(const UObject* WorldContextObject, const bool bVerbose);
	UFUNCTION(BlueprintCallable, Category="SLMechatronics", meta=(WorldContext="WorldContextObject"))
	static FString GetPortDebugString(const UObject* WorldContextObject, const FSLMPortAddress& Address);
	UFUNCTION(BlueprintCallable, Category="SLMechatronics", meta=(WorldContext="WorldContextObject"))
	static FString GetDeviceDebugString(const UObject* WorldContextObject, const FSLMPortAddress& Address);
	UFUNCTION(BlueprintCallable, Category="SLMechatronics")
	static FString DiffDebugStrings(const FString Server, const FString Client);
	
	
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="SLMechatronics", meta=(WorldContext="WorldContextObject"))
	static void OpenRemappingContext(const UObject* WorldContextObject);
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="SLMechatronics", meta=(WorldContext="WorldContextObject"))
	static void CloseRemappingContext(const UObject* WorldContextObject);
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="SLMechatronics", meta=(WorldContext="WorldContextObject"))
	static TArray<FSLMDeviceSnapshot> GetDeviceSnapshotsByActor(const UObject* WorldContextObject, AActor* Actor);
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="SLMechatronics", meta=(WorldContext="WorldContextObject"))
	static void ApplyDeviceSnapshotsByActor(const UObject* WorldContextObject, AActor* Actor, const TArray<FSLMDeviceSnapshot>& Snapshots);
};

