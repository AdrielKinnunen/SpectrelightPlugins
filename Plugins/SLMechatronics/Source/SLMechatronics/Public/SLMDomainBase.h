// Copyright Spectrelight Studios, LLC

#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"
#include "Subsystems/WorldSubsystem.h"
#include "SLMDomainBase.generated.h"

UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_SPECTRELIGHTDYNAMICS_DOMAIN)


class USLMDomainSubsystemBase;
class USLMDeviceComponentBase;


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
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	//TSubclassOf<USLMDomainSubsystemBase> Domain;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics", meta = (Categories = "SlectrelightDynamics.Domain"))
	FGameplayTag DomainTag;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
    FSLMPortMetaData FirstMetaData;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	FSLMPortMetaData SecondMetaData;
};

USTRUCT(BlueprintType)
struct FSLMConnection
{
	FSLMConnection()
	{
	}

	FSLMConnection(const int32 FirstIndex, const int32 SecondIndex): FirstIndex(FirstIndex), SecondIndex(SecondIndex)
	{
	}

	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	int32 FirstIndex = -1;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	int32 SecondIndex = -1;
};




UCLASS(Abstract, BlueprintType)
class SLMECHATRONICS_API USLMDomainSubsystemBase : public UWorldSubsystem
{
    GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	FGameplayTag DomainTag = TAG_SPECTRELIGHTDYNAMICS_DOMAIN;	
    UFUNCTION(Blueprintcallable, Category = "SLMechatronics|Connections")
    void ConnectPorts(const int32 FirstPortIndex, const int32 SecondPortIndex);
    UFUNCTION(Blueprintcallable, Category = "SLMechatronics|Connections")
    void DisconnectPorts(const int32 FirstPortIndex, const int32 SecondPortIndex);
    UFUNCTION(BlueprintPure, Category = "SLMechatronics|Connections")
    bool ArePortsConnected(const int32 FirstPortIndex, const int32 SecondPortIndex);

    UFUNCTION(Blueprintcallable, Category = "SLMechatronics")
    FVector PortIndexToWorldLocation(const int32 PortIndex);
	UFUNCTION(Blueprintcallable, Category = "SLMechatronics")
	int32 GetPortIndex(const FSLMPortMetaData Filter, const FVector WorldLocation);
    UFUNCTION(Blueprintcallable, Category = "SLMechatronics")
    FTransform PortMetaDataToWorldTransform(const FSLMPortMetaData MetaData);

    virtual void CheckForCleanUp();
    virtual void PreSimulate(const float DeltaTime);
    virtual void Simulate(const float DeltaTime, const float SubstepScalar);
    virtual void PostSimulate(const float DeltaTime);

	//Debug
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	bool bDebugDraw = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	bool bDebugPrint = false;
	UFUNCTION(Blueprintcallable, Category = "SLMechatronics")
	virtual FString GetDebugString(int32 PortIndex);
	virtual void DebugDraw();
	virtual void DebugPrint();
	FColor DebugColor = FColor::Black;
	
protected:
    TMultiMap<int32, int32> Adjacencies;
    TSparseArray<int32> PortIndexToNetworkIndex;

    bool bNeedsCleanup = false;
    TSet<int32> PortsRecentlyAdded;
    TSet<int32> PortsToRemove;
    TArray<FSLMConnection> ConnectionsToAdd;
    TArray<FSLMConnection> ConnectionsToRemove;

    TMultiMap<const AActor*, int32> ActorToPorts;
    TSparseArray<FSLMPortMetaData> PortsMetaData;

    void AddPortMetaData(FSLMPortMetaData MetaData);

    virtual void CreateNetworkForPorts(TArray<int32> PortIndices);
    virtual void DissolveNetworkIntoPort(int32 NetworkIndex, int32 PortIndex);
    virtual void RemovePortAtIndex(int32 PortIndex);
    virtual void RemoveNetworkAtIndex(int32 NetworkIndex);

private:
    void CleanUpGraph();
    TSet<int32> GetConnectedPorts(const TSet<int32>& Roots) const;
};
