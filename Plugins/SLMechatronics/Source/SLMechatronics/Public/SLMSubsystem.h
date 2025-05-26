// Copyright Spectrelight Studios, LLC

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "SLMSubsystem.generated.h"

class ASLMReplicationHelper;
struct FSLMConnectionByMetaData;
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
    USLMechatronicsSubsystem* Target = nullptr;
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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	bool bDebugDraw = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	bool bDebugPrint = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	ASLMReplicationHelper* ReplicationHelper;
    
    //Functions
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void OnWorldBeginPlay(UWorld& InWorld) override;
    void Tick(float DeltaTime);
	UFUNCTION(BlueprintCallable, Category = "SLMechatronics")
	void PropagateSettings();
	UFUNCTION(BlueprintCallable, Category = "SLMechatronics")
	void MakeConnectionByMetadata(FSLMConnectionByMetaData Connection);
	UFUNCTION(BlueprintCallable, Category = "SLMechatronics")
	TArray<FSLMConnectionByMetaData> GetAllConnectionsByMetadata();
private:
    //Properties
    FSLMechatronicsSubsystemTickFunction PrimarySystemTick;
	UPROPERTY()
    TArray<USLMDeviceSubsystemBase*> DeviceSubsystems;
	UPROPERTY()
    TArray<USLMDomainSubsystemBase*> DomainSubsystems;
	ENetMode WorldNetMode;
};










/*
USTRUCT(BlueprintType)
struct FInputData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	int32 Iterations = 1;
};

USTRUCT(BlueprintType)
struct FOutputData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	int32 Result = 0;
};

class FTickWorker : public FRunnable
{
public:
    FTickWorker():bStopThread(false), bWorkReady(false)
    {
        WorkEvent = FPlatformProcess::GetSynchEventFromPool(false); // Manual reset
    }

    virtual ~FTickWorker()
    {
        FPlatformProcess::ReturnSynchEventToPool(WorkEvent);
        WorkEvent = nullptr;
    }

    virtual bool Init() override
    {
	    return true;
    }

    virtual uint32 Run() override
    {
        while (!bStopThread)
        {
            // Wait for owner to signal work
            WorkEvent->Wait();

            if (bStopThread) break;

            {
                FScopeLock Lock(&DataMutex);
                // Copy input for thread safety
                LocalInput = SharedInput;
            }

            // Do work (simulate processing)
            LocalOutput = Process(LocalInput);

            {
                FScopeLock Lock(&DataMutex);
                SharedOutput = LocalOutput;
                bWorkReady = false;
            }
        }
        return 0;
    }

    virtual void Stop() override { bStopThread = true; WorkEvent->Trigger(); }

    virtual void Exit() override {}

    // Called from main thread on Tick
    void SubmitWork(const FInputData& InData)
    {
        FScopeLock Lock(&DataMutex);
        SharedInput = InData;
        bWorkReady = true;
        WorkEvent->Trigger(); // Wake the thread
    }

    // Called from main thread on Tick after submission
    bool GetResult(FOutputData& OutData)
    {
        FScopeLock Lock(&DataMutex);
        if (!bWorkReady)
        {
            OutData = SharedOutput;
            return true;
        }
        return false; // Work not done yet
    }

private:
    std::atomic<bool> bStopThread;
    bool bWorkReady;

    FInputData SharedInput;
    FOutputData SharedOutput;

    FInputData LocalInput;
    FOutputData LocalOutput;

    FCriticalSection DataMutex;
    FEvent* WorkEvent;
    
    FOutputData Process(const FInputData& In) { 
        // Your background logic here
        return FOutputData{}; 
    }
};

*/
