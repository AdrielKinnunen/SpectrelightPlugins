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
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	USceneComponent* SceneComponent = nullptr;
};

USTRUCT(BlueprintType)
struct FSLMPortBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SLMechatronics")
	FName PortName;	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SLMechatronics")
	FSLMPortLocationData PortLocationData;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	int32 PortIndex = -1;
};


UCLASS(Abstract)
class SLMECHATRONICS_API USLMDomainSubsystemBase : public UWorldSubsystem
{
	GENERATED_BODY()
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	USLMechatronicsSubsystem* Subsystem;
	
	virtual void CheckForCleanUp();

	UFUNCTION(Blueprintcallable, Category = "SLMechatronics")
	void ConnectPorts(int32 FirstPortIndex, int32 SecondPortIndex);
	UFUNCTION(Blueprintcallable, Category = "SLMechatronics")
	void DisconnectPorts(int32 FirstPortIndex, int32 SecondPortIndex);
	UFUNCTION(Blueprintcallable, Category = "SLMechatronics")
	bool ArePortsConnected(int32 FirstPortIndex, int32 SecondPortIndex);

protected:

	TMultiMap<int32, int32> Adjacencies;
	TSparseArray<int32> PortIndexToNetworkIndex;
	
	bool bNeedsCleanup = false;
	TArray<int32> PortsToAdd;
	TArray<int32> PortsToRemove;
	TArray<int32> PortsDirty;

	virtual void CreateNetworkForPorts(TArray<int32> PortIndices);
	virtual void DissolveNetworkIntoPort(int32 NetworkIndex, int32 PortIndex);
	virtual void RemovePortAtIndex(int32 PortIndex);
	virtual void RemoveNetworkAtIndex(int32 NetworkIndex);

	
private:
	void CleanUpGraph();
	TArray<int32> GetConnectedPorts(const TArray<int32>& Roots) const;

};
