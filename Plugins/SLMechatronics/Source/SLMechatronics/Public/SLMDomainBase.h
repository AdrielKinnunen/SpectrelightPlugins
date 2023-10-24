// Copyright Spectrelight Studios, LLC

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "SLMDomainBase.generated.h"

class USLMechatronicsSubsystem;


USTRUCT(BlueprintType)
struct FSLMPortLocationData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SLMechatronics")
	FName ComponentName;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SLMechatronics")
	FName SocketName;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SLMechatronics")
	FVector Offset;
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	//USceneComponent* SceneComponent = nullptr;
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


UCLASS(Abstract)
class SLMECHATRONICS_API USLMDomainSubsystemBase : public UWorldSubsystem
{
	GENERATED_BODY()
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	USLMechatronicsSubsystem* Subsystem;

	UFUNCTION(Blueprintcallable, Category = "SLMechatronics")
	virtual void TestPrintAllData();

	UFUNCTION(Blueprintcallable, Category = "SLMechatronics")
	void ConnectPorts(int32 FirstPortIndex, int32 SecondPortIndex);
	UFUNCTION(Blueprintcallable, Category = "SLMechatronics")
	void DisconnectPorts(int32 FirstPortIndex, int32 SecondPortIndex);
	UFUNCTION(Blueprintcallable, Category = "SLMechatronics")
	bool ArePortsConnected(int32 FirstPortIndex, int32 SecondPortIndex);

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

	virtual void CreateNetworkForPorts(TArray<int32> PortIndices);
	virtual void DissolveNetworkIntoPort(int32 NetworkIndex, int32 PortIndex);
	virtual void RemovePortAtIndex(int32 PortIndex);
	virtual void RemoveNetworkAtIndex(int32 NetworkIndex);
private:
	void CleanUpGraph();
	TSet<int32> GetConnectedPorts(const TSet<int32>& Roots) const;
};
