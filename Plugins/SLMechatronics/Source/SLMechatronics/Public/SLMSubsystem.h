// Copyright Spectrelight Studios, LLC

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "SLMSubsystem.generated.h"

class USLMechatronicsSubsystem;
class USLMDeviceComponentBase;
class USLMDeviceSubsystemBase;
class USLMDomainSubsystemBase;

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
	
	//Functions
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	void Tick(float DeltaTime);	

private:
	//Properties
	FSLMechatronicsSubsystemTickFunction PrimarySystemTick;
	TSparseArray<USLMDeviceComponentBase*> DeviceComponents;
	TArray<USLMDeviceSubsystemBase*> DeviceSubsystems;
	TArray<USLMDomainSubsystemBase*> DomainSubsystems;
};



/*
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
*/






	
/*
int32 AddDeviceComponent(USLMDeviceComponentBase* DeviceComponent);
void RemoveDevice(int32 DeviceIndex);
int32 AddPort(FSLMPort Port);
void RemovePort(int32 PortIndex);
*/
	
/*
//BP Callable Stuff
UFUNCTION(Blueprintcallable, Category = "SLMechatronics")
void ConnectPorts(int32 FirstPortIndex, int32 SecondPortIndex);
UFUNCTION(Blueprintcallable, Category = "SLMechatronics")
void DisconnectPorts(int32 FirstPortIndex, int32 SecondPortIndex);
UFUNCTION(Blueprintcallable, Category = "SLMechatronics")
bool ArePortsConnected(int32 FirstPortIndex, int32 SecondPortIndex);
*/

/*
//Testing
UFUNCTION(Blueprintcallable, Category = "SLMechatronics")
TArray<int32> TestGetAllConnectedPortsMulti(TArray<int32> Roots);
UFUNCTION(Blueprintcallable, Category = "SLMechatronics")
void TestPrintALlPortData();
*/
	










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


