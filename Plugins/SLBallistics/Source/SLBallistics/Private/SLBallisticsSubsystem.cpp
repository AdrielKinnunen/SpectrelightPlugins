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

void USLBallisticsSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    PrimarySystemTick.Target = this;
    PrimarySystemTick.bCanEverTick = true;
    PrimarySystemTick.bHighPriority = true;
    PrimarySystemTick.bRunOnAnyThread = false;
    PrimarySystemTick.bTickEvenWhenPaused = false;
    PrimarySystemTick.TickGroup = TG_PrePhysics;
    PrimarySystemTick.TickInterval = 0.0;
    PrimarySystemTick.RegisterTickFunction(GetWorld()->PersistentLevel);

    Super::Initialize(Collection);
}

void USLBallisticsSubsystem::Tick(float DeltaTime)
{
    for (const auto Projectile : Projectiles)
    {
        //Do the stuff
    }
}
