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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	float StoredJoules = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	float CapacityJoules = 1;
};


USTRUCT(BlueprintType)
struct FSLMPortElectricity
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	FSLMDataElectricity PortData;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SLMechatronics")
	FSLMPortMetaData PortMetaData;
};


UCLASS(BlueprintType)
class SLMECHATRONICS_API USLMDomainElectricity : public USLMDomainSubsystemBase
{
	GENERATED_BODY()
public:
	int32 AddPort(const FSLMPortElectricity& Port);
	void RemovePort(const int32 PortIndex);

	UFUNCTION(BlueprintCallable, Category = "SLMechatronics")
	FSLMDataElectricity GetByPortIndex(const int32 PortIndex);
	UFUNCTION(BlueprintCallable, Category = "SLMechatronics")
	void SetJoulesByPortIndex(int32 PortIndex, float NewJoules);
	
private:
	TSparseArray<FSLMDataElectricity> PortsData;
	TSparseArray<FSLMDataElectricity> Networks;

	void CreateNetworkForPort(const int32 Port);

	virtual void CreateNetworkForPorts(const TArray<int32> PortIndices) override;
	virtual void DissolveNetworkIntoPort(const int32 NetworkIndex, const int32 PortIndex) override;
	virtual void RemovePortAtIndex(const int32 PortIndex) override;
	virtual void RemoveNetworkAtIndex(const int32 NetworkIndex) override;
};
