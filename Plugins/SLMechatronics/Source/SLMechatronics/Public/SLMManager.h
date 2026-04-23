// Copyright Spectrelight Studios, LLC

#pragma once

#include "CoreMinimal.h"
#include "SLMDomainBase.h"
#include "Net/Serialization/FastArraySerializer.h"
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
};


UCLASS()
class SLMECHATRONICS_API USLMManager : public UWorldSubsystem
{
    GENERATED_BODY()
public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void OnWorldBeginPlay(UWorld& InWorld) override;
    void Tick(const float DeltaTime);
	
	bool WorldLocationToPortAddress(const TSubclassOf<USLMDomainSubsystemBase> Domain, const FSLMPortMetaData& Filter, const FVector& WorldLocation, FSLMPortAddress& OutAddress);
	bool PortAddressToWorldLocation(const FSLMPortAddress& PortAddress, FVector& OutWorldLocation);
	bool DoesConnectionExist(const FSLMConnection& Connection);
	void AddConnection(const FSLMConnection& Connection);
	void RemoveConnection(const FSLMConnection& Connection);	
	
	FString GetGlobalDebugString(const bool Verbose);
	int32 GetGlobalDebugHash();
	FString GetPortDebugString(const FSLMPortAddress& Address);
	FString GetDeviceDebugString(const FSLMPortAddress& Address);
	
	UPROPERTY(BlueprintReadWrite, Category="SLMechatronics")
	int32 StepCount = 5;
	UPROPERTY(BlueprintReadWrite, Category="SLMechatronics")
	bool bDebugDraw = true;

private:
    FSLMechatronicsSubsystemTickFunction PrimarySystemTick;
	UPROPERTY()
	TArray<USLMDeviceSubsystemBase*> DeviceSubsystems;
	UPROPERTY()
    TArray<USLMDomainSubsystemBase*> DomainSubsystems;
	UPROPERTY()
	ASLMManagerReplicator* Replicator;
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
	
private:
	UPROPERTY(Replicated)
	FSLMRepArrayConnections FastArray;
};