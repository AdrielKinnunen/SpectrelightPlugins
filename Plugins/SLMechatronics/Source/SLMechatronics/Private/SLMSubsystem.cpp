// Copyright Spectrelight Studios, LLC

#include "SLMSubsystem.h"

#include "SLMDeviceBase.h"
#include "SLMDomainBase.h"
#include "SLMReplicationHelper.h"

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
    PrimarySystemTick.TickGroup = TG_PostPhysics;
    PrimarySystemTick.TickInterval = 0.0;
    PrimarySystemTick.RegisterTickFunction(GetWorld()->PersistentLevel);

    Super::Initialize(Collection);
}

void USLMechatronicsSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	//DeviceSubsystems = GetWorld()->GetSubsystemArray<USLMDeviceSubsystemBase>();
	DeviceSubsystems = GetWorld()->GetSubsystemArrayCopy<USLMDeviceSubsystemBase>();
    DomainSubsystems = GetWorld()->GetSubsystemArrayCopy<USLMDomainSubsystemBase>();
    UE_LOG(LogTemp, Warning, TEXT("There are %i Device Subsystems and %i Domain Subsystems"), DeviceSubsystems.Num(), DomainSubsystems.Num());

	
	WorldNetMode = InWorld.GetNetMode();
	auto asdf = TEXT("None");
	switch (WorldNetMode)
	{
	case NM_Client:
		asdf = TEXT("Client");
		break;
	case NM_ListenServer:
		asdf = TEXT("Listen");
		break;
	case NM_DedicatedServer:
		asdf = TEXT("Dedicated");
		break;
	case NM_Standalone:
		asdf = TEXT("Standalone");
		break;
	case NM_MAX:
		asdf = TEXT("MAX");
		break;
	default: break;
	}
	UE_LOG(LogTemp, Warning, TEXT("World Net Mode is %s"), asdf);
	if (WorldNetMode != NM_Client)
	{
		ReplicationHelper = InWorld.SpawnActor<ASLMReplicationHelper>();
	}
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

void USLMechatronicsSubsystem::MakeConnectionByMetadata(FSLMConnectionByMetaData Connection)
{
	if (WorldNetMode != NM_Client)
	{
		ReplicationHelper->ReplicatedConnections.Add(Connection);
	}
	
	USLMDomainSubsystemBase* TargetSubsystem = nullptr;
	for (const auto DomainSubsystem : DomainSubsystems)
	{
		if (DomainSubsystem->DomainTag == Connection.DomainTag)
		{
			TargetSubsystem = DomainSubsystem;
		}
	}
	if (TargetSubsystem)
	{
		const int32 FirstPortIndex = TargetSubsystem->GetPortIndex(Connection.FirstMetaData, FVector::ZeroVector);
		const int32 SecondPortIndex = TargetSubsystem->GetPortIndex(Connection.SecondMetaData, FVector::ZeroVector);
		TargetSubsystem->ConnectPorts(FirstPortIndex, SecondPortIndex);
	}
}

TArray<FSLMConnectionByMetaData> USLMechatronicsSubsystem::GetAllConnectionsByMetadata()
{
	TArray<FSLMConnectionByMetaData> Out;
	for (const auto& Domain : DomainSubsystems)
	{
		Out.Append(Domain->GetAllConnections());
	}
	return Out;
}
