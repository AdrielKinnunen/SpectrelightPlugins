// Copyright Spectrelight Studios, LLC

#pragma once

#include "CoreMinimal.h"
#include "SLMDomainBase.h"
#include "UObject/Object.h"
#include "SLMDomainMech.generated.h"


USTRUCT(BlueprintType)
struct FSLMDataMech
{
	GENERATED_BODY()

	FSLMDataMech()
	{
	}

	FSLMDataMech(const float AngVel, const float MOI): AngVel(AngVel), MOI(MOI)
	{
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	float AngVel = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	float MOI = 1;
};


USTRUCT(BlueprintType)
struct FSLMPortMech
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SLMechatronics")
	FName PortName;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SLMechatronics")
	FSLMPortLocationData PortLocationData;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	FSLMDataMech DefaultData;
};


UCLASS()
class SLMECHATRONICS_API USLMDomainMech : public USLMDomainSubsystemBase
{
	GENERATED_BODY()

public:
	int32 AddPort(const FSLMPortMech& Port);
	void RemovePort(const int32 PortIndex);

	UFUNCTION(BlueprintCallable, Category = "SLMechatronics")
	FSLMDataMech GetNetworkData(const int32 PortIndex);
	UFUNCTION(BlueprintCallable, Category = "SLMechatronics")
	void SetNetworkAngVel(int32 PortIndex, float NewAngVel);

private:
	TSparseArray<FSLMPortMech> Ports;
	TSparseArray<FSLMDataMech> Networks;

	void CreateNetworkForPort(const int32 Port);

	virtual void CreateNetworkForPorts(const TArray<int32> PortIndices) override;
	virtual void DissolveNetworkIntoPort(const int32 NetworkIndex, const int32 PortIndex) override;
	virtual void RemovePortAtIndex(const int32 PortIndex) override;
	virtual void RemoveNetworkAtIndex(const int32 NetworkIndex) override;
};
