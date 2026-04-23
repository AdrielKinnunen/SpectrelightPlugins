// Copyright Spectrelight Studios, LLC

#pragma once

#include "CoreMinimal.h"
#include "SLMDomainBase.h"
#include "UObject/Object.h"
#include "SLMDomainSignal.generated.h"

USTRUCT(BlueprintType)
struct FSLMDataSignal
{
    GENERATED_BODY()

    FSLMDataSignal()
    {
    }

    FSLMDataSignal(const float Read, const float Write): Read(Read), Write(Write)
    {
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics", meta=(Tooltip="Angular Velocity in rad/s"))
    float Read = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics", meta=(Tooltip="Moment of Inertia in kg*m2"))
    float Write = 0;
    
	FString GetDebugString() const
    {
    	FString Result;
    	Result += FString::Printf(TEXT("%f,%f"), Read, Write);
    	return Result;
    }
};

FORCEINLINE uint32 GetTypeHash(const FSLMDataSignal& Data)
{
	uint32 Hash = 0;
	Hash = HashCombine(Hash, GetTypeHash(FMath::RoundToInt(Data.Read * 100.0f)));
	Hash = HashCombine(Hash, GetTypeHash(FMath::RoundToInt(Data.Write * 100.0f)));
	return Hash;
}


USTRUCT(BlueprintType)
struct FSLMPortSignal
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
    FSLMDataSignal PortData;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	FSLMPortMetaData PortMetaData;
};


UCLASS(BlueprintType)
class SLMECHATRONICS_API USLMDomainSignal : public USLMDomainSubsystemBase
{
    GENERATED_BODY()
	
public:
    USLMDomainSignal();
	int32 AddPort(const FSLMPortSignal& Port, const FSLMPortAddress& PortAddress);
	void RemovePort(const FSLMPortAddress& PortAddress);
	float ReadValue(const int32 PortID);
	void WriteValue(const int32 PortID, const float Value);
	
protected:
	virtual void RunTests() override;
	virtual void CreateParticleForPorts(const TArray<int32> PortIDs) override;
	virtual void DissolveParticleIntoPort(const int32 ParticleID, const int32 PortID) override;
	virtual void RemovePortAtAddress(const FSLMPortAddress& PortAddress) override;
	virtual void RemoveParticleAtID(const int32 ParticleID) override;

private:
	//virtual void PreSimulate(const float DeltaTime);
	//virtual void Simulate(const float DeltaTime, const float SubstepScalar);
	//virtual void PostSimulate(const float DeltaTime);
	virtual uint32 GetDebugHash() override;
	virtual FString GetDebugString(const bool Verbose) override;
	virtual FString GetPortDebugString(const FSLMPortAddress& Address) override;
	
	TSparseArray<FSLMDataSignal> PortDefaults;
	TSparseArray<FSLMDataSignal> Particles;
};
