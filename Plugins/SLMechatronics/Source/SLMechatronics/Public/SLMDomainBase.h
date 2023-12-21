// Copyright Spectrelight Studios, LLC

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "SLMDomainBase.generated.h"


USTRUCT(BlueprintType)
struct FSLMPortMetaData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	FName PortName;
	UPROPERTY(BlueprintReadWrite, Category = "SLMechatronics")
	const AActor* AssociatedActor;
	UPROPERTY(BlueprintReadWrite, Category = "SLMechatronics")
	const USceneComponent* AssociatedSceneComponent;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SLMechatronics")
	FName SceneComponentName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	FName SocketName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	FVector OffsetLocal;
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
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	//USLMechatronicsSubsystem* Subsystem;
	
	UFUNCTION(Blueprintcallable, Category = "SLMechatronics")
	void ConnectPorts(int32 FirstPortIndex, int32 SecondPortIndex);
	UFUNCTION(Blueprintcallable, Category = "SLMechatronics")
	void DisconnectPorts(int32 FirstPortIndex, int32 SecondPortIndex);
	UFUNCTION(Blueprintcallable, Category = "SLMechatronics")
	bool ArePortsConnected(int32 FirstPortIndex, int32 SecondPortIndex);
	UFUNCTION(Blueprintcallable, Category = "SLMechatronics")
	virtual void DebugPrint();
	UFUNCTION(Blueprintcallable, Category = "SLMechatronics")
	virtual void DebugDraw();

	UFUNCTION(Blueprintcallable, Category = "SLMechatronics")
	FVector PortIndexToWorldLocation(int32 PortIndex);
	UFUNCTION(Blueprintcallable, Category = "SLMechatronics")
	int32 GetClosestPortIndexGlobal(const FVector WorldLocation);
	UFUNCTION(Blueprintcallable, Category = "SLMechatronics")
	int32 GetClosestPortIndexActor(const FVector WorldLocation, const AActor* Actor);
	UFUNCTION(Blueprintcallable, Category = "SLMechatronics")
	FTransform PortMetaDataToWorldTransform(const FSLMPortMetaData MetaData);

	
	virtual void CheckForCleanUp();
	virtual void PreSimulate(const float DeltaTime);
	virtual void Simulate(const float DeltaTime);
	virtual void PostSimulate(const float DeltaTime);
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
	
	UFUNCTION(Blueprintcallable, Category = "SLMechatronics")
	virtual void AddPortMetaData(FSLMPortMetaData MetaData);

	virtual void CreateNetworkForPorts(TArray<int32> PortIndices);
	virtual void DissolveNetworkIntoPort(int32 NetworkIndex, int32 PortIndex);
	virtual void RemovePortAtIndex(int32 PortIndex);
	virtual void RemoveNetworkAtIndex(int32 NetworkIndex);
	
private:
	void CleanUpGraph();
	TSet<int32> GetConnectedPorts(const TSet<int32>& Roots) const;
};

/*
{
	double DistanceSquared = UE_BIG_NUMBER;
	int32 PortIndex = -1;

	if (Cast<USLMDomainRotation>(Domain))
	{
		//DeviceSettings.Port_Rotation_Input.PortLocationData.
	}

	

	for (const auto& Port : Ports)
	{
		const FVector PortLocation = PortToWorldLocation(Port);
		const double ThisPortDistanceSquared = FVector::DistSquared(WorldLocation, PortLocation);
		if (ThisPortDistanceSquared < DistanceSquared)
		{
			DistanceSquared = ThisPortDistanceSquared;
			OutPort = Port;
			Success = true;
		}
	}
	
	return PortIndex;
}
*/
