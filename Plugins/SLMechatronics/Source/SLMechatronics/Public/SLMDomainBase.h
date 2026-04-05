// Copyright Spectrelight Studios, LLC

#pragma once

#include "CoreMinimal.h"
#include "SLMDeviceBase.h"
#include "SLMSubsystemBase.h"
//#include "Subsystems/WorldSubsystem.h"
#include "SLMDomainBase.generated.h"


class USLMDeviceSubsystemBase;
class USLMDomainSubsystemBase;
class USLMDeviceComponentBase;



USTRUCT(BlueprintType)
struct FSLMPortAddress
{
	GENERATED_BODY()
	TSubclassOf<USLMDeviceSubsystemBase> DeviceClass;
	int32 DeviceID = INDEX_NONE;
	int32 PortIndex = INDEX_NONE;
	
	FString GetDebugString() const
	{
		FString Result;
		Result += FString::Printf(TEXT("   %i,%i   "), DeviceID, PortIndex);
		return Result;
	}
};

FORCEINLINE uint32 GetTypeHash(const FSLMPortAddress& Address)
{
	uint32 Hash = 0;
	Hash = HashCombine(Hash, GetTypeHash(Address.DeviceClass));
	Hash = HashCombine(Hash, GetTypeHash(Address.DeviceID));
	Hash = HashCombine(Hash, GetTypeHash(Address.PortIndex));	
	return Hash;
}

FORCEINLINE bool operator==(const FSLMPortAddress& A, const FSLMPortAddress& B)
{
	return A.DeviceClass == B.DeviceClass && A.DeviceID == B.DeviceID && A.PortIndex == B.PortIndex;
}


USTRUCT(BlueprintType)
struct FSLMConnection
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	TSubclassOf<USLMDomainSubsystemBase> DomainClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	FSLMPortAddress First;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	FSLMPortAddress Second;
};



USTRUCT(BlueprintType)
struct FSLMPortMetaData
{
    GENERATED_BODY()

	FSLMPortMetaData()
    {
    	DeviceComponent = nullptr;
	    AssociatedActor = nullptr;
    	AssociatedSceneComponent = nullptr;
    	PortName = NAME_None;
    	DeviceComponentName = NAME_None;
    	SceneComponentName = NAME_None;
    	SocketName = NAME_None;
    	OffsetLocal = FVector::ZeroVector;
    }
	
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "SLMechatronics")
	const USLMDeviceComponentBase* DeviceComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
    const AActor* AssociatedActor;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "SLMechatronics")
	const USceneComponent* AssociatedSceneComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	FName PortName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	FName DeviceComponentName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	FName SceneComponentName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
    FName SocketName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
    FVector OffsetLocal;
};


USTRUCT(BlueprintType)
struct FSLMConnectionByMetaData
{
    GENERATED_BODY()
	
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
    FSLMPortMetaData FirstMetaData;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	FSLMPortMetaData SecondMetaData;
};



UCLASS(Abstract, BlueprintType)
class SLMECHATRONICS_API USLMDomainSubsystemBase : public USLMSubsystemBase
{
    GENERATED_BODY()
	friend class USLMManager;
	
	//virtual void RunTests();
	
public:
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	//FGameplayTag DomainTag = TAG_SLMECHATRONICS_DOMAIN;
    //void ConnectPorts(const int32 FirstPortIndex, const int32 SecondPortIndex);
    //void DisconnectPorts(const int32 FirstPortIndex, const int32 SecondPortIndex);
    //bool ArePortsConnected(const int32 FirstPortIndex, const int32 SecondPortIndex);
    //FVector PortIndexToWorldLocation(const int32 PortIndex);
	//int32 GetPortIndex(const FSLMPortMetaData Filter, const FVector WorldLocation);
    //FTransform PortMetaDataToWorldTransform(const FSLMPortMetaData MetaData);
	//TArray<FSLMConnectionByMetaData> GetAllConnections();
    //virtual void CheckForCleanUp();
	
protected:
	
	virtual void ConnectPortsByAddress(const FSLMPortAddress First, const FSLMPortAddress Second);
	
	
	
    //TMultiMap<int32, int32> Adjacencies;
    //TSparseArray<int32> PortIndexToParticleIndex;

    //bool bNeedsCleanup = false;
    //TSet<int32> PortsToRemove;
    //TArray<FSLMConnection> ConnectionsToAdd;
    //TArray<FSLMConnection> ConnectionsToRemove;

    //TMultiMap<const AActor*, int32> ActorToPorts;
    //TSparseArray<FSLMPortMetaData> PortsMetaData;
	//TSparseArray<FSLMPortAddress> PortAddresses;
	TMap<FSLMPortAddress, int32> PortAddressToID;
	
	//TArray<FSLMPortAddress> PortsToRemove;
	

    //void AddPortMetaData(FSLMPortMetaData MetaData);

    //virtual void CreateParticleForPorts(TArray<int32> PortIndices);
    //virtual void DissolveParticleIntoPort(int32 ParticleIndex, int32 PortIndex);
    //virtual void RemovePortAtIndex(int32 PortIndex);
    //virtual void RemoveParticleAtIndex(int32 ParticleIndex);

private:
    //void CleanUpGraph();
    //TSet<int32> GetConnectedPorts(const TSet<int32>& Roots) const;
};
