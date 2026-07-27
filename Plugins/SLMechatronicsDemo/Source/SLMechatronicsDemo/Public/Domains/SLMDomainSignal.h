// Copyright Spectrelight Studios, LLC

#pragma once

#include "CoreMinimal.h"
#include "SLMDomainTemplate.h"
#include "SLMDomainSignal.generated.h"

UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_SLM_System_Domain_Signal);


USTRUCT(BlueprintType)
struct FSLMDataSignal
{
    GENERATED_BODY()

private:
    float ReadValue = 0;
    float WriteValue = 0;
	
public:
	float Read() const
	{
		return ReadValue;
	}
	
	void Write(const float Value)
	{
		WriteValue += Value;
	}

	static void Merge(const TArray<FSLMDataSignal>& Sources, FSLMDataSignal& Target)
	{
		float SumRead = 0;
		float SumWrite = 0;
		for (const auto& Source : Sources)
		{
			SumRead += Source.ReadValue;
			SumWrite += Source.WriteValue;
		}
		Target.ReadValue = SumRead;
		Target.WriteValue = SumWrite;
	}
	
	static void Dissolve(const FSLMDataSignal& Source, FSLMDataSignal& Target)
	{
		Target.ReadValue = Source.ReadValue;
		Target.WriteValue = Source.WriteValue;
	}

	void Flush()
	{
		ReadValue = WriteValue;
		WriteValue = 0;
	}
	
	FString GetDebugString() const;
	uint32 GetDebugHash(const bool bVerbose) const;
};

USTRUCT(BlueprintType)
struct FSLMPortSignal
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
    FSLMDataSignal PortData;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	FSLMSpatialContextAuthored SpatialContext;
};


class SLMECHATRONICSDEMO_API FSLMDomainSystemSignal : public TSLMDomainSystem<FSLMDomainSystemSignal, FSLMDataSignal, FSLMPortSignal>
{
public:
	FSLMDomainSystemSignal();
	static FGameplayTag GetSystemTagStatic();
	virtual void Initialize() override;
	
	//Public Interface
	virtual void RunTests() override;
	virtual void PreSimulate(const float DeltaTime) override;
	virtual void Simulate(const float DeltaTime, const float SubstepScalar) override;
	virtual void PostSimulate(const float DeltaTime) override;

	//CRTP Hooks
	//static void Merge(const TArray<FSLMDataSignal>& Sources, FSLMDataSignal& Target);
	//static void Dissolve(const FSLMDataSignal& Source, FSLMDataSignal& Target);
};