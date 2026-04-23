// Copyright Spectrelight Studios, LLC

#pragma once

#include "CoreMinimal.h"
#include "SLMSubsystemBase.h"
#include "SLMDomainBase.generated.h"

class USLMDeviceSubsystemBase;
class USLMDomainSubsystemBase;

//Address
USTRUCT(BlueprintType)
struct FSLMPortAddress
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	TSubclassOf<USLMDeviceSubsystemBase> DeviceClass;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	TSubclassOf<USLMDomainSubsystemBase> DomainClass;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	int32 DeviceID = INDEX_NONE;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	int32 PortID = INDEX_NONE;
	
	FString GetDebugString() const;
	bool IsValid() const;
};
FORCEINLINE uint32 GetTypeHash(const FSLMPortAddress& Address)
{
	uint32 Hash = 0;
	Hash = HashCombine(Hash, GetTypeHash(Address.DeviceClass));
	Hash = HashCombine(Hash, GetTypeHash(Address.DomainClass));
	Hash = HashCombine(Hash, GetTypeHash(Address.DeviceID));
	Hash = HashCombine(Hash, GetTypeHash(Address.PortID));
	return Hash;
}
bool operator==(const FSLMPortAddress& A, const FSLMPortAddress& B);

FSLMPortAddress MakePortAddress(const USLMDeviceSubsystemBase* Device, const USLMDomainSubsystemBase* Domain, int32 DeviceID, int32 PortID);

//Connection
USTRUCT(BlueprintType)
struct FSLMConnection
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	FSLMPortAddress First;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	FSLMPortAddress Second;
	
	FString GetDebugString() const;
	bool IsValid() const;
};
FORCEINLINE uint32 GetTypeHash(const FSLMConnection& Connection)
{
	return GetTypeHash(Connection.First) ^ GetTypeHash(Connection.Second);
}
bool operator==(const FSLMConnection& A, const FSLMConnection& B);



USTRUCT(BlueprintType)
struct FSLMPortMetaData
{
    GENERATED_BODY()

	FSLMPortMetaData()
    {
		DeviceName = NAME_None;
		PortName = NAME_None;
		SceneComponentName = NAME_None;
		SocketName = NAME_None;
		OffsetLocal = FVector::ZeroVector;
		AssociatedActor = nullptr;
		AssociatedSceneComponent = nullptr;
	}


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	FName DeviceName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	FName PortName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	FName SceneComponentName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
    FName SocketName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	FVector OffsetLocal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	const AActor* AssociatedActor;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "SLMechatronics")
	const USceneComponent* AssociatedSceneComponent;
};


UCLASS(Abstract, BlueprintType)
class SLMECHATRONICS_API USLMDomainSubsystemBase : public USLMSubsystemBase
{
    GENERATED_BODY()
	
	friend class USLMManager;
	
protected:
	virtual void RunTests();
	virtual void CreateParticleForPorts(const TArray<int32> PortIDs);
	virtual void DissolveParticleIntoPort(const int32 ParticleID, const int32 PortID);
	virtual void RemovePortAtAddress(const FSLMPortAddress& PortAddress);
	virtual void RemoveParticleAtID(const int32 ParticleID);
	void AddPortMetaData(FSLMPortMetaData MetaData);

	TMap<FSLMPortAddress, int32> PortAddressToPortID;
	TSparseArray<FSLMPortAddress> PortIDToPortAddress;
	TSparseArray<int32> PortIDToParticleID;
	TSparseArray<FSLMPortMetaData> PortMetaData;
	TSet<FSLMPortAddress> PortsToRemove;
	bool bNeedsCleanup = false;
	

private:
	virtual void DebugDraw() override;
	virtual FString GetPortDebugString(const FSLMPortAddress& Address);
	bool DoesConnectionExist(const FSLMConnection& Connection);
	void AddConnection(const FSLMConnection& Connection);
	void RemoveConnection(const FSLMConnection& Connection);
	void CheckForCleanUp();

	void CleanUpGraph();
	void CleanContainers();
	void ProcessPortsToRemove();
	void ProcessConnectionsPending();
	void ProcessConnectionsToAdd();
	void ProcessConnectionsToRemove();
	void UpdateAdjacencyList();
	void GatherDirtyPortIDs();
	void DissolveAndRebuildParticles();
	void DestroyPorts();

	TSet<int32> GetConnectedPortIDs(const TSet<int32>& Roots) const;
	bool ArePortsConnected(const int32 FirstPortID, const int32 SecondPortID);

	FVector PortIDToWorldLocation(const int32 PortID);
	int32 WorldLocationToPortID(const FSLMPortMetaData& Filter, const FVector& WorldLocation);
	FTransform PortMetaDataToWorldTransform(const FSLMPortMetaData& MetaData);
	TMultiMap<const AActor*, int32> ActorToPortIDs;
	
	TSet<FSLMConnection> ConnectionsToAdd;
	TSet<FSLMConnection> ConnectionsPending;
	TSet<FSLMConnection> ConnectionsToRemove;

	TArray<TTuple<int32, int32>> ConnectionsToAddByID;
	TArray<TTuple<int32, int32>> ConnectionsToRemoveByID;

	TMultiMap<int32, int32> AdjacencyList;
	TSet<int32> DirtyPortIDs;
};
