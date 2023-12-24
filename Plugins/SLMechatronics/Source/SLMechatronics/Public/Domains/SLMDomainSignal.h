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
	float Read = 0.0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	float Write = 0.0;
};


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
	
	UFUNCTION(BlueprintCallable, Category = "SLMechatronics")
	int32 AddPort(const FSLMPortSignal& Port);
	UFUNCTION(BlueprintCallable, Category = "SLMechatronics")
	void RemovePort(const int32 PortIndex);

	UFUNCTION(BlueprintPure, Category = "SLMechatronics")
	float ReadByPortIndex(const int32 PortIndex);
	UFUNCTION(BlueprintCallable, Category = "SLMechatronics")
	void WriteByPortIndex(const int32 PortIndex, const float Data);
	
	virtual void PostSimulate(const float DeltaTime) override;
	
private:
	TSparseArray<FSLMDataSignal> PortsData;
	TSparseArray<FSLMDataSignal> Networks;
	
	void CreateNetworkForPort(const int32 Port);

	virtual void CreateNetworkForPorts(const TArray<int32> PortIndices) override;
	virtual void DissolveNetworkIntoPort(const int32 NetworkIndex, int32 PortIndex) override;
	virtual void RemovePortAtIndex(const int32 PortIndex) override;
	virtual void RemoveNetworkAtIndex(const int32 NetworkIndex) override;
};
