// Copyright Spectrelight Studios, LLC

#pragma once

#include "CoreMinimal.h"
#include "SLMDomainBase.h"
#include "Subsystems/WorldSubsystem.h"
#include "SLMManager.generated.h"

class USLMManager;
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


UCLASS()
class SLMECHATRONICS_API USLMManager : public UWorldSubsystem
{
    GENERATED_BODY()
public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void OnWorldBeginPlay(UWorld& InWorld) override;
    void Tick(const float DeltaTime);
	
	
	UFUNCTION(BlueprintCallable, Category="SLMechatronics")
	void AddConnection(FSLMConnection Connection);
	UFUNCTION(BlueprintCallable, Category="SLMechatronics")
	void ConnectPortsByAddress(const FSLMPortAddress First, const FSLMPortAddress Second);
	UFUNCTION(BlueprintCallable, Category="SLMechatronics")
	void DisconnectPortsByAddress(const FSLMPortAddress First, const FSLMPortAddress Second);
	
	
	
	UFUNCTION(BlueprintCallable, Category="SLMechatronics")
	FString GetDebugString();
	UFUNCTION(BlueprintCallable, Category="SLMechatronics")
	int32 GetDebugHash();
	UFUNCTION(BlueprintCallable, Category="SLMechatronics")
	FString DiffDebugStrings(FString Server, FString Client);
	UPROPERTY(BlueprintReadWrite, Category="SLMechatronics")
	int32 StepCount = 5;
	UPROPERTY(BlueprintReadWrite, Category="SLMechatronics")
	bool bDebugDraw = false;

private:
	
    FSLMechatronicsSubsystemTickFunction PrimarySystemTick;
	
	UPROPERTY()
	TArray<USLMDeviceSubsystemBase*> DeviceSubsystems;
	UPROPERTY()
    TArray<USLMDomainSubsystemBase*> DomainSubsystems;
};