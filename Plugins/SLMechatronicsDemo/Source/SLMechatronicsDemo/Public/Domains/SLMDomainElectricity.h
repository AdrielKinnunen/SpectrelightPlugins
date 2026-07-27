// Copyright Spectrelight Studios, LLC

#pragma once

#include "CoreMinimal.h"
#include "SLMDomainTemplate.h"
#include "SLMDomainElectricity.generated.h"

UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_SLM_System_Domain_Electricity);


USTRUCT(BlueprintType)
struct FSLMDataElectricity
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics", meta=(Tooltip="Energy stored in Joules"))
    float StoredJoules = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics", meta=(Tooltip="Energy Capacity in Joules"))
    float CapacityJoules = 1;
	
	float TransferEnergyClamped(const float EnergyToAdd)
	{
		const float EnergyTransferred = FMath::Clamp(EnergyToAdd, -StoredJoules, CapacityJoules - StoredJoules);
		StoredJoules += EnergyTransferred;
		return EnergyTransferred;
	}
  
	static void Merge(const TArray<FSLMDataElectricity>& Sources, FSLMDataElectricity& Target)
	{
		float SumStored = 0;
		float SumCapacity = 0;
		for (const auto& Source : Sources)
		{
			SumStored += Source.StoredJoules;
			SumCapacity += Source.CapacityJoules;
		}
		Target.StoredJoules = SumStored;
		Target.CapacityJoules = SumCapacity;
	}
	
	static void Dissolve(const FSLMDataElectricity& Source, FSLMDataElectricity& Target)
	{
		const float Alpha = Source.StoredJoules / Source.CapacityJoules;
		Target.StoredJoules = Source.StoredJoules * Alpha;
	}
	
	FString GetDebugString() const;
	uint32 GetDebugHash(const bool bVerbose) const;
};

USTRUCT(BlueprintType)
struct FSLMPortElectricity
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
    FSLMDataElectricity PortData;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	FSLMSpatialContextAuthored SpatialContext;
};

class SLMECHATRONICSDEMO_API FSLMDomainSystemElectricity : public TSLMDomainSystem<FSLMDomainSystemElectricity, FSLMDataElectricity, FSLMPortElectricity>
{
public:
	FSLMDomainSystemElectricity();
	static FGameplayTag GetSystemTagStatic();
	virtual void Initialize() override;
	
	virtual void RunTests() override;
	virtual void PreSimulate(const float DeltaTime) override;
	virtual void Simulate(const float DeltaTime, const float SubstepScalar) override;
	virtual void PostSimulate(const float DeltaTime) override;
};