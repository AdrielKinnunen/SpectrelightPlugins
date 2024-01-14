// Copyright Spectrelight Studios, LLC

#include "SLMSubsystem.h"

#include "SLMDeviceBase.h"
#include "SLMDomainBase.h"

void FSLMechatronicsSubsystemTickFunction::ExecuteTick(float DeltaTime, ELevelTick TickType, ENamedThreads::Type CurrentThread, const FGraphEventRef& MyCompletionEventGraph)
{
    if (Target && IsValid(Target) && TickType != LEVELTICK_ViewportsOnly)
    {
        Target->Tick(DeltaTime);
    }
}

FString FSLMechatronicsSubsystemTickFunction::DiagnosticMessage()
{
    return TEXT("FSLDSubsystemTickFunction");
}

FName FSLMechatronicsSubsystemTickFunction::DiagnosticContext(bool bDetailed)
{
    return FName(TEXT("FSLDSubsystem"));
}

void USLMechatronicsSubsystem::Initialize(FSubsystemCollectionBase& Collection)
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

void USLMechatronicsSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
    DeviceSubsystems = GetWorld()->GetSubsystemArray<USLMDeviceSubsystemBase>();
    DomainSubsystems = GetWorld()->GetSubsystemArray<USLMDomainSubsystemBase>();
    UE_LOG(LogTemp, Warning, TEXT("There are %i Device Subsystems and %i Domain Subsystems"), DeviceSubsystems.Num(), DomainSubsystems.Num());
    Super::OnWorldBeginPlay(InWorld);
}

void USLMechatronicsSubsystem::Tick(float DeltaTime)
{
    //Graph maintenance
    DECLARE_SCOPE_CYCLE_COUNTER(TEXT("Entire Tick"), STAT_EntireTick, STATGROUP_SLMechatronics)
    {
        DECLARE_SCOPE_CYCLE_COUNTER(TEXT("Cleanup"), STAT_Cleanup, STATGROUP_SLMechatronics)
        for (const auto DomainSubsystem : DomainSubsystems)
        {
            DomainSubsystem->CheckForCleanUp();
        }
    }

    //Debug
    {
        DECLARE_SCOPE_CYCLE_COUNTER(TEXT("Debug"), STAT_Debug, STATGROUP_SLMechatronics)
        for (const auto DomainSubsystem : DomainSubsystems)
        {
            if (DebugDrawPorts)
            {
                DomainSubsystem->DebugDrawPorts();
            }
            
            if (DebugDrawConnections)
            {
                DomainSubsystem->DebugDrawConnections();
            }
            if (DebugPrint)
            {
                DomainSubsystem->DebugPrint();
            }
            DomainSubsystem->PreSimulate(DeltaTime);
        }
    }

    //PreSimulate, runs once per frame, sets up state for calculations
    {
        DECLARE_SCOPE_CYCLE_COUNTER(TEXT("PreSimulate"), STAT_PreSimulate, STATGROUP_SLMechatronics)
        for (const auto DomainSubsystem : DomainSubsystems)
        {
            DomainSubsystem->PreSimulate(DeltaTime);
        }
        for (const auto& DeviceSubsystem : DeviceSubsystems)
        {
            DeviceSubsystem->PreSimulate(DeltaTime);
        }
    }

    //Simulate, substeps StepCount times per frame, this is where the bulk of simulation happens
    {
        DECLARE_SCOPE_CYCLE_COUNTER(TEXT("Simulate"), STAT_Simulate, STATGROUP_SLMechatronics)
        const float SubstepDeltaTime = DeltaTime / StepCount;
        const float SubstepScalar = 1.0 / StepCount;
        for (int32 i = 0; i < StepCount; i++)
        {
            for (const auto DomainSubsystem : DomainSubsystems)
            {
                DomainSubsystem->Simulate(SubstepDeltaTime, SubstepScalar);
            }
            for (const auto& DeviceSubsystem : DeviceSubsystems)
            {
                DeviceSubsystem->Simulate(SubstepDeltaTime, SubstepScalar);
            }
        }
    }

    //PostSimulate, this is where results are ready for usage outside of SLMechactronics
    {
        DECLARE_SCOPE_CYCLE_COUNTER(TEXT("PostSimulate"), STAT_PostSimulate, STATGROUP_SLMechatronics)
        for (const auto DomainSubsystem : DomainSubsystems)
        {
            DomainSubsystem->PostSimulate(DeltaTime);
        }
        for (const auto& DeviceSubsystem : DeviceSubsystems)
        {
            DeviceSubsystem->PostSimulate(DeltaTime);
        }
    }
}
