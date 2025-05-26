// Copyright Spectrelight Studios, LLC

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "SLBallisticsSubsystem.generated.h"

class USLBallisticsSubsystem;

USTRUCT(BlueprintType)
struct FSLBProjectile
{
	GENERATED_BODY()

	FSLBProjectile()
	{
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLBallistics")
	FVector Position = FVector(0,0,0);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLBallistics")
	FVector Velocity = FVector(0,0,0);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLBallistics")
	FVector Acceleration = FVector(0,0,-980);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLBallistics")
	float Lifetime = 10;
};

USTRUCT()
struct FSLBallisticsSubsystemTickFunction : public FTickFunction
{
    GENERATED_BODY()
    UPROPERTY()
    USLBallisticsSubsystem* Target = nullptr;
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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLBallistics")
	FVector Gravity = FVector(0,0,-980);
	
	UFUNCTION(BlueprintCallable, Category = "SLBallistics")
	int32 FireProjectile(FSLBProjectile ProjectileData);

    //Functions
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    void Tick(float DeltaTime);
private:
    //Properties
    FSLBallisticsSubsystemTickFunction PrimarySystemTick;
    TSparseArray<FSLBProjectile> Projectiles;
};
