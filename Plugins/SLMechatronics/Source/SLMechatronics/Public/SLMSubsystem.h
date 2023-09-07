// Copyright Spectrelight Studios, LLC

#pragma once

#include "CoreMinimal.h"
#include "SLMTypes.h"
#include "Subsystems/WorldSubsystem.h"
#include "SLMSubsystem.generated.h"

class USLMDeviceSubsystemBase;
class USLMDeviceComponentBase;
class USLMechatronicsSubsystem;

DECLARE_STATS_GROUP(TEXT("SLMechatronics"), STATGROUP_SLMechatronics, STATCAT_Advanced);



USTRUCT()
struct FSLMechatronicsSubsystemTickFunction : public FTickFunction
{
	GENERATED_BODY()
	UPROPERTY()
	USLMechatronicsSubsystem* Target;
	SLMECHATRONICS_API virtual void ExecuteTick(float DeltaTime, ELevelTick TickType, ENamedThreads::Type CurrentThread, const FGraphEventRef& MyCompletionEventGraph) override;
	SLMECHATRONICS_API virtual FString DiagnosticMessage() override;
	SLMECHATRONICS_API virtual FName DiagnosticContext(bool bDetailed) override;
};
template<>
struct TStructOpsTypeTraits<FSLMechatronicsSubsystemTickFunction> : public TStructOpsTypeTraitsBase2<FSLMechatronicsSubsystemTickFunction>
{
	enum { WithCopy = false };
};


UCLASS()
class SLMECHATRONICS_API USLMechatronicsSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
public:
	//Properties
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	int32 StepCount = 1;
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	//bool bOnlyConnectMatchingDomains = true;
	
	//Functions
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	void Tick(float DeltaTime);
	
	int32 AddDevice(USLMDeviceComponentBase* DeviceComponent);
	void RemoveDevice(int32 DeviceIndex);
	int32 AddPort(FSLMPort Port);
	void RemovePort(int32 PortIndex);

	UFUNCTION(BlueprintPure, Category = "SLMechatronics")
	FSLMData GetNetworkData(int32 PortIndex);
	UFUNCTION(BlueprintPure, Category = "SLMechatronics")
	float GetNetworkValue(int32 PortIndex);
	UFUNCTION(BlueprintPure, Category = "SLMechatronics")
	float GetNetworkCapacity(int32 PortIndex);
	UFUNCTION(BlueprintCallable, Category = "SLMechatronics")
	void SetNetworkValue(int32 PortIndex, float NetworkValue);
		
	
	//BP Callable Stuff
	UFUNCTION(Blueprintcallable, Category = "SLMechatronics")
	void ConnectPorts(int32 FirstPortIndex, int32 SecondPortIndex);
	UFUNCTION(Blueprintcallable, Category = "SLMechatronics")
	void DisconnectPorts(int32 FirstPortIndex, int32 SecondPortIndex);
	UFUNCTION(Blueprintcallable, Category = "SLMechatronics")
	bool ArePortsConnected(int32 FirstPortIndex, int32 SecondPortIndex);

	//Testing
	UFUNCTION(Blueprintcallable, Category = "SLMechatronics")
	TArray<int32> TestGetAllConnectedPortsMulti(TArray<int32> Roots);
	UFUNCTION(Blueprintcallable, Category = "SLMechatronics")
	void TestPrintALlPortData();

private:
	//Properties
	FSLMechatronicsSubsystemTickFunction PrimarySystemTick;

	TArray<USLMDeviceSubsystemBase*> DeviceSubsystems;
	TSparseArray<USLMDeviceComponentBase*> DeviceComponents;
	TSparseArray<FSLMPort> Ports;
	TMultiMap<int32, int32> Adjacencies;
	TSparseArray<int32> PortIndexToNetworkIndex;
	TSparseArray<FSLMData> Networks;
	
	bool bNeedsCleanup = false;
	TArray<int32> PortsToAdd;
	TArray<int32> PortsToRemove;
	TArray<int32> PortsDirty;
	
	//Functions
	void CleanUpGraph();
	TArray<int32> GetConnectedPorts(const TArray<int32>& Roots) const;
	void CreateNetworkForPorts(TArray<int32> Ports);
	void CreateNetworkForPort(int32 Port);
};

//UFUNCTION(Blueprintcallable, Category = "SLMechatronics")
//void BulkAddConnections(TArray<FSLMConnection> Connections);
//UFUNCTION(Blueprintcallable, Category = "SLMechatronics")
//TArray<FSLMConnection> BPGetAdjacenciesAsConnections();
//TArray<FSLMConnection> GetAdjacenciesAsConnections();
//virtual void Initialize(FSubsystemCollectionBase& Collection) override;
//virtual void Deinitialize() override;
//virtual UWorld* GetTickableGameObjectWorld() const override { return GetWorld(); }
//virtual ETickableTickType GetTickableTickType() const override;
//virtual bool IsAllowedToTick() const override final;
//void DoThing(FPhysScene_Chaos* PhysScene, float DeltaTime);
//virtual void OnWorldBeginPlay(UWorld& InWorld) override;


