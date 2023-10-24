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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	float Read = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	float Write = 0;
};


USTRUCT(BlueprintType)
struct FSLMPortSignal
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SLMechatronics")
	FName PortName;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SLMechatronics")
	FSLMPortLocationData PortLocationData;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	FSLMDataSignal DefaultData;
};


UCLASS()
class SLMECHATRONICS_API USLMDomainSignal : public USLMDomainSubsystemBase
{
	GENERATED_BODY()

public:
	int32 AddPort(const FSLMPortSignal& Port);
	void RemovePort(const int32 PortIndex);
	float ReadData(const int32 PortIndex);
	void WriteData(const int32 PortIndex, const float Data);

	virtual void PostSimulate(const float DeltaTime) override;

private:
	TSparseArray<FSLMPortSignal> Ports;
	TSparseArray<FSLMDataSignal> Networks;

	void CreateNetworkForPort(const int32 Port);

	virtual void CreateNetworkForPorts(const TArray<int32> PortIndices) override;
	virtual void DissolveNetworkIntoPort(const int32 NetworkIndex, int32 PortIndex) override;
	virtual void RemovePortAtIndex(const int32 PortIndex) override;
	virtual void RemoveNetworkAtIndex(const int32 NetworkIndex) override;
};
