// Copyright Spectrelight Studios, LLC

#include "SLMSubsystem.h"

#include <ThirdParty/ShaderConductor/ShaderConductor/External/DirectXShaderCompiler/include/dxc/DXIL/DxilConstants.h>

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
	TRACE_CPUPROFILER_EVENT_SCOPE(SLMSubsystem::Tick);

	//Graph maintenance
    {
		TRACE_CPUPROFILER_EVENT_SCOPE(SLMSubsystem::Tick::Cleanup); 
        for (const auto DomainSubsystem : DomainSubsystems)
        {
            DomainSubsystem->CheckForCleanUp();
        }
    }

    //PreSimulate, runs once per frame, sets up state for calculations
    {
		TRACE_CPUPROFILER_EVENT_SCOPE(SLMSubsystem::Tick::PreSimulate);
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
		TRACE_CPUPROFILER_EVENT_SCOPE(SLMSubsystem::Tick::Simulate);
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
		TRACE_CPUPROFILER_EVENT_SCOPE(SLMSubsystem::Tick::PostSimulate);
        for (const auto DomainSubsystem : DomainSubsystems)
        {
            DomainSubsystem->PostSimulate(DeltaTime);
        }
        for (const auto& DeviceSubsystem : DeviceSubsystems)
        {
            DeviceSubsystem->PostSimulate(DeltaTime);
        }
    }

	//Debug
	{
		TRACE_CPUPROFILER_EVENT_SCOPE(SLMSubsystem::Tick::Debug);
		for (const auto DomainSubsystem : DomainSubsystems)
		{
			if (DomainSubsystem->bDebugDraw)
			{
				DomainSubsystem->DebugDraw();	
			}
			if (DomainSubsystem->bDebugPrint)
			{
				DomainSubsystem->DebugPrint();
			}
		}
		for (const auto& DeviceSubsystem : DeviceSubsystems)
		{
			if (DeviceSubsystem->bDebugDraw)
			{
				DeviceSubsystem->DebugDraw();
			}
			if (DeviceSubsystem->bDebugPrint)
			{
				DeviceSubsystem->DebugPrint();
			}
		}
	}
}

void USLMechatronicsSubsystem::PropagateSettings()
{
	for (const auto DomainSubsystem : DomainSubsystems)
	{
		DomainSubsystem->bDebugDraw = bDebugDraw;
		DomainSubsystem->bDebugPrint = bDebugPrint;
	}
}
