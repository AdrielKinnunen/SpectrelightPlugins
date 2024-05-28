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


template <>
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
    int32 StepCount = 5;
    
    //Functions
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void OnWorldBeginPlay(UWorld& InWorld) override;
    void Tick(float DeltaTime);
private:
    //Properties
    FSLMechatronicsSubsystemTickFunction PrimarySystemTick;
    TArray<USLMDeviceSubsystemBase*> DeviceSubsystems;
    TArray<USLMDomainSubsystemBase*> DomainSubsystems;
};
