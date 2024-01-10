// Copyright Spectrelight Studios, LLC

#pragma once

#include "CoreMinimal.h"
//#include "UObject/Object.h"
#include "Subsystems/WorldSubsystem.h"
#include "SLBallisticsSubsystem.generated.h"

class USLBallisticsSubsystem;


USTRUCT()
struct FSLBallisticsSubsystemTickFunction : public FTickFunction
{
    GENERATED_BODY()
    UPROPERTY()
    USLBallisticsSubsystem* Target;
    SLBALLISTICS_API virtual void ExecuteTick(float DeltaTime, ELevelTick TickType, ENamedThreads::Type CurrentThread, const FGraphEventRef& MyCompletionEventGraph) override;
    SLBALLISTICS_API virtual FString DiagnosticMessage() override;
    SLBALLISTICS_API virtual FName DiagnosticContext(bool bDetailed) override;
};


template <>
struct TStructOpsTypeTraits<FSLBallisticsSubsystemTickFunction> : public TStructOpsTypeTraitsBase2<FSLBallisticsSubsystemTickFunction>
{
    enum { WithCopy = false };
};


UCLASS()
class SLBALLISTICS_API USLBallisticsSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()
public:
    //Properties
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLBallistics")
    int32 StepCount = 1;

    //Functions
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    void Tick(float DeltaTime);
private:
    //Properties
    FSLBallisticsSubsystemTickFunction PrimarySystemTick;
    TSparseArray<int32> Projectiles;
};
