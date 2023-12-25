// Copyright Spectrelight Studios, LLC

#pragma once

#include "CoreMinimal.h"
#include "SLMDomainBase.h"
#include "UObject/Object.h"
#include "SLMDomainElectricity.generated.h"


USTRUCT(BlueprintType)
struct FSLMDataElectricity
{
	GENERATED_BODY()

	FSLMDataElectricity()
	{
	}

	FSLMDataElectricity(const float StoredJoules, const float CapacityJoules): StoredJoules(StoredJoules), CapacityJoules(CapacityJoules)
	{
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics", meta=(Tooltip="Stored energy in Joules"))
	float StoredJoules = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics", meta=(Tooltip="Energy capacity in Joules"))
	float CapacityJoules = 1;
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

	UFUNCTION(BlueprintCallable, Category = "SLMechatronics")
	int32 AddPort(const FSLMPortElectricity& Port);
	UFUNCTION(BlueprintCallable, Category = "SLMechatronics")
	void RemovePort(const int32 PortIndex);

	UFUNCTION(BlueprintPure, Category = "SLMechatronics")
	FSLMDataElectricity GetByPortIndex(const int32 PortIndex);
	UFUNCTION(BlueprintCallable, Category = "SLMechatronics")
	void SetJoulesByPortIndex(int32 PortIndex, float NewJoules);

	virtual FString GetDebugString(const int32 PortIndex) override;

	
private:
	TSparseArray<FSLMDataElectricity> PortsData;
	TSparseArray<FSLMDataElectricity> Networks;

	void CreateNetworkForPort(const int32 Port);

	virtual void CreateNetworkForPorts(const TArray<int32> PortIndices) override;
	virtual void DissolveNetworkIntoPort(const int32 NetworkIndex, const int32 PortIndex) override;
	virtual void RemovePortAtIndex(const int32 PortIndex) override;
	virtual void RemoveNetworkAtIndex(const int32 NetworkIndex) override;
};
