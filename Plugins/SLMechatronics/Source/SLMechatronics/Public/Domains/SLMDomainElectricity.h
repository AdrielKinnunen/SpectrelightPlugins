// Copyright Spectrelight Studios, LLC

#pragma once

#include "CoreMinimal.h"
#include "SLMDomainBase.h"
#include "SLMDomainElectricity.generated.h"

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
  
	FString GetDebugString() const;
	uint32 GetDebugHash() const;
};

USTRUCT(BlueprintType)
struct FSLMPortElectricity
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
    FSLMDataElectricity PortData;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	FSLMPortMetaData PortMetaData;
};

UCLASS(BlueprintType)
class SLMECHATRONICS_API USLMDomainElectricity : public USLMDomainSubsystemBase
{
    GENERATED_BODY()
	
public:
    USLMDomainElectricity();
	
	int32 AddPort(const FSLMPortElectricity& Port, const FSLMPortAddress& PortAddress);
	
	virtual void RunTests() override;
	virtual void PreSimulate(const float DeltaTime) override;
	virtual void Simulate(const float DeltaTime, const float SubstepScalar) override;
	virtual void PostSimulate(const float DeltaTime) override;
	virtual uint32 GetDebugHash() override;
	virtual FString GetDebugString(const bool Verbose) override;
	virtual FString GetPortDebugString(const FSLMPortAddress& Address) override;
	
	FSLMDataElectricity& GetParticleRef(const int32 PortID);

protected:
	virtual void CreateParticleForPorts(const TArray<int32> PortIDs) override;
	virtual void DissolveParticleIntoPort(const int32 ParticleID, const int32 PortID) override;
	virtual void RemovePortAtAddress(const FSLMPortAddress& PortAddress) override;
	virtual void RemoveParticleAtID(const int32 ParticleID) override;

private:	
	TSparseArray<FSLMDataElectricity> PortDefaults;
	TSparseArray<FSLMDataElectricity> Particles;
};
