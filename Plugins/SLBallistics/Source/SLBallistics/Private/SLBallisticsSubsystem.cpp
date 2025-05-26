// Fill out your copyright notice in the Description page of Project Settings.

#include "SLBallisticsSubsystem.h"



void FSLBallisticsSubsystemTickFunction::ExecuteTick(float DeltaTime, ELevelTick TickType, ENamedThreads::Type CurrentThread, const FGraphEventRef& MyCompletionEventGraph)
{
    if (Target && IsValid(Target) && TickType != LEVELTICK_ViewportsOnly)
    {
        Target->Tick(DeltaTime);
    }
}

FString FSLBallisticsSubsystemTickFunction::DiagnosticMessage()
{
    return TEXT("FSLDSubsystemTickFunction");
}

FName FSLBallisticsSubsystemTickFunction::DiagnosticContext(bool bDetailed)
{
    return FName(TEXT("FSLDSubsystem"));
}

int32 USLBallisticsSubsystem::FireProjectile(FSLBProjectile ProjectileData)
{
	const int32 Index = Projectiles.Add(ProjectileData);
	return Index;
}

void USLBallisticsSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    PrimarySystemTick.Target = this;
    PrimarySystemTick.bCanEverTick = true;
    PrimarySystemTick.bHighPriority = true;
    PrimarySystemTick.bRunOnAnyThread = false;
    PrimarySystemTick.bTickEvenWhenPaused = false;
    PrimarySystemTick.TickGroup = TG_PostPhysics;
    PrimarySystemTick.TickInterval = 0.0;
    PrimarySystemTick.RegisterTickFunction(GetWorld()->PersistentLevel);

    Super::Initialize(Collection);
}

void USLBallisticsSubsystem::Tick(float DeltaTime)
{
	//TArray<FSLBProjectile> DeadProjectiles;
	//DeadProjectiles.Reserve(32);

	//Physics
	for (auto It = Projectiles.CreateIterator(); It; ++It)
	{
		FVector Position = It->Position;
		FVector NextPosition = It->Position + It->Velocity * DeltaTime + 0.5 * It->Acceleration * DeltaTime * DeltaTime;
		FVector NextVelocity = It->Velocity + It->Acceleration * DeltaTime;

		FCollisionQueryParams TraceParams = FCollisionQueryParams();
		FHitResult Hit;
		GetWorld()->LineTraceSingleByChannel(Hit, Position, NextPosition, ECC_Visibility, TraceParams);
		DrawDebugLine(GetWorld(), Position, NextPosition, FColor::Red, false, 0.1, 0, 5);
		if (Hit.bBlockingHit)
		{
			DrawDebugPoint(GetWorld(), Hit.ImpactPoint, 10, FColor::Green, false, 3, 0);
			It->Lifetime = -1.0;
			NextPosition = Hit.ImpactPoint;
		}
		It->Position = NextPosition;
		It->Velocity = NextVelocity;
		It->Lifetime -= DeltaTime;
		if (It->Lifetime < 0.0)
		{
			It.RemoveCurrent();
		}
	}	
}
