// Copyright Spectrelight Studios, LLC

#pragma once

#include "CoreMinimal.h"
#include "SLMTypes.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "StructUtils/InstancedStruct.h"
#include "Subsystems/WorldSubsystem.h"
#include "SLMManager.generated.h"

class USLMManager;
class ASLMManagerReplicator;
class USLMDeviceSubsystemBase;
class USLMDomainSubsystemBase;

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
    void Tick(const float DeltaTime);
	
	void OpenRemappingContext();
	void CloseRemappingContext();
	
	void AddConnection(const FSLMConnection& Connection);
	void RemoveConnection(const FSLMConnection& Connection);
	
	bool WorldLocationToPortAddress(const TSubclassOf<USLMDomainSubsystemBase> DomainClass, const FSLMPortMetaData& Filter, const FVector& WorldLocation, FSLMPortAddress& OutAddress);
	bool PortAddressToWorldLocation(const FSLMPortAddress& PortAddress, FVector& OutWorldLocation);
	bool DoesConnectionExist(const FSLMConnection& Connection);
	
	FString GetGlobalDebugString(const bool Verbose);
	int32 GetGlobalDebugHash();
	FString GetPortDebugString(const FSLMPortAddress& Address);
	FString GetDeviceDebugString(const FSLMPortAddress& Address);
	
	UPROPERTY(BlueprintReadWrite, Category="SLMechatronics")
	int32 StepCount = 10;
	UPROPERTY(BlueprintReadWrite, Category="SLMechatronics")
	bool bDebugDraw = true;

private:
	void Local_AddConnection(const FSLMConnection& Connection);
	void Local_RemoveConnection(const FSLMConnection& Connection);
	
	void Client_AddOrChangeDescriptor(const FSLMDeviceAddress& DeviceAddress, const FInstancedStruct& Payload);
	void Client_RemoveDescriptor(const FSLMDeviceAddress& DeviceAddress) const;
	
	void Client_AddOrChangeState(const FSLMDeviceAddress& DeviceAddress, const FInstancedStruct& Payload);
	void Client_RemoveState(const FSLMDeviceAddress& DeviceAddress) const;

	

	FSLMechatronicsSubsystemTickFunction PrimarySystemTick;
	UPROPERTY()
	TArray<USLMDeviceSubsystemBase*> DeviceSubsystems;
	UPROPERTY()
    TArray<USLMDomainSubsystemBase*> DomainSubsystems;
	UPROPERTY()
	ASLMManagerReplicator* Replicator;
	
	bool bRemappingContextOpen = false;
	TMap<FSLMDeviceAddress, FSLMDeviceAddress> RemappingContext;
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
	void AddConnection(const FSLMConnection& Connection);
	void RemoveConnection(const FSLMConnection& Connection);
	void AddOrChangeDescriptor(const FSLMDeviceAddress& Address, const FInstancedStruct& Payload);
	void RemoveDescriptor(const FSLMDeviceAddress& Address);
	void AddOrChangeState(const FSLMDeviceAddress& Address, const FInstancedStruct& Payload);
	void RemoveState(const FSLMDeviceAddress& Address);
	
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
	static bool WorldLocationToPortAddress(const UObject* WorldContextObject, const TSubclassOf<USLMDomainSubsystemBase> Domain, const FSLMPortMetaData& Filter, const FVector& WorldLocation, FSLMPortAddress& OutAddress);
	UFUNCTION(BlueprintCallable, Category="SLMechatronics", meta=(WorldContext="WorldContextObject"))
	static bool PortAddressToWorldLocation(const UObject* WorldContextObject, const FSLMPortAddress& PortAddress, FVector& OutWorldLocation);
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="SLMechatronics", meta=(WorldContext="WorldContextObject"))
	static void AddConnection(const UObject* WorldContextObject, const FSLMConnection Connection);
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="SLMechatronics", meta=(WorldContext="WorldContextObject"))
	static void RemoveConnection(const UObject* WorldContextObject, const FSLMConnection Connection);
	UFUNCTION(BlueprintCallable, Category="SLMechatronics", meta=(WorldContext="WorldContextObject"))
	static int32 GetGlobalDebugHash(const UObject* WorldContextObject);
	UFUNCTION(BlueprintCallable, Category="SLMechatronics", meta=(WorldContext="WorldContextObject"))
	static FString GetGlobalDebugString(const UObject* WorldContextObject, const bool Verbose);
	UFUNCTION(BlueprintCallable, Category="SLMechatronics", meta=(WorldContext="WorldContextObject"))
	static FString GetPortDebugString(const UObject* WorldContextObject, const FSLMPortAddress& Address);
	UFUNCTION(BlueprintCallable, Category="SLMechatronics", meta=(WorldContext="WorldContextObject"))
	static FString GetDeviceDebugString(const UObject* WorldContextObject, const FSLMPortAddress& Address);
	UFUNCTION(BlueprintCallable, Category="SLMechatronics")
	static FString DiffDebugStrings(const FString Server, const FString Client);
	
	
	UFUNCTION(BlueprintCallable, Category="SLMechatronics", meta=(WorldContext="WorldContextObject"))
	static void OpenRemappingContext(const UObject* WorldContextObject);
};

