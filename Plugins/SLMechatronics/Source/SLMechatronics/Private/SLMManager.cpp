// Copyright Spectrelight Studios, LLC

#include "SLMManager.h"

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

void USLMManager::Initialize(FSubsystemCollectionBase& Collection)
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

void USLMManager::OnWorldBeginPlay(UWorld& InWorld)
{
	DeviceSubsystems = GetWorld()->GetSubsystemArrayCopy<USLMDeviceSubsystemBase>();
    DomainSubsystems = GetWorld()->GetSubsystemArrayCopy<USLMDomainSubsystemBase>();
	UE_LOG(LogTemp, Warning, TEXT("There are %i Device Subsystems and %i Domain Subsystems"), DeviceSubsystems.Num(), DomainSubsystems.Num());
    Super::OnWorldBeginPlay(InWorld);
}

void USLMManager::Tick(const float DeltaTime)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(SLMSubsystem::Tick);

	//Graph maintenance
    {
		TRACE_CPUPROFILER_EVENT_SCOPE(SLMSubsystem::Tick::Cleanup); 
        for (const auto DomainSubsystem : DomainSubsystems)
        {
            //DomainSubsystem->CheckForCleanUp();
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

    //PostSimulate, this is where results are ready for usage outside of SLMechatronics
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
		if (bDebugDraw)
		{
			for (const auto DomainSubsystem : DomainSubsystems)
			{
				//DomainSubsystem->DebugDraw();
			}
			for (const auto& DeviceSubsystem : DeviceSubsystems)
			{
				//DeviceSubsystem->DebugDraw();
			}			
		}
	}
}

void USLMManager::AddConnection(FSLMConnection Connection)
{
	
}

void USLMManager::ConnectPortsByAddress(const FSLMPortAddress First, const FSLMPortAddress Second)
{
	for (auto* DomainSubsystem : DomainSubsystems)
	{
		if (DomainSubsystem->PortAddressToID.Contains(First) && DomainSubsystem->PortAddressToID.Contains(Second))
		{
			
		}
	}
}

void USLMManager::DisconnectPortsByAddress(const FSLMPortAddress First, const FSLMPortAddress Second)
{
}


FString USLMManager::GetDebugString()
{
	FString Result;
	for (auto* DomainSubsystem : DomainSubsystems)
	{
		Result.Append(DomainSubsystem->GetDebugString());
	}
	for (auto* DeviceSubsystem : DeviceSubsystems)
	{
		Result.Append(DeviceSubsystem->GetDebugString());
	}
	return Result;
}

int32 USLMManager::GetDebugHash()
{
	int32 FinalSum = 0;
	for (auto* DomainSubsystem : DomainSubsystems)
	{
		FinalSum += DomainSubsystem->GetDebugHash();
	}
	for (auto* DeviceSubsystem : DeviceSubsystems)
	{
		FinalSum += DeviceSubsystem->GetDebugHash();
	}
	return FinalSum;
}

FString USLMManager::DiffDebugStrings(FString Server, FString Client)
{
	TArray<FString> ServerLines;
	TArray<FString> ClientLines;
	Server.ParseIntoArrayLines(ServerLines);
	Client.ParseIntoArrayLines(ClientLines);
	
	FString Result;
	Result += TEXT("\nIn Server but not Client: \n");
	for (const FString& Line : ServerLines)
	{
		if (!ClientLines.Contains(Line))
		{
			Result += Line + TEXT("\n");
		}
	}
	Result += TEXT("In Client but not Server: \n");
	for (const FString& Line : ClientLines)
	{
		if (!ServerLines.Contains(Line))
		{
			Result += Line + TEXT("\n");
		}
	}
	return Result;
}
