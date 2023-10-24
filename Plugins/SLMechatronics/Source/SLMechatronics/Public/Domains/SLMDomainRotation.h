// Copyright Spectrelight Studios, LLC

#pragma once

#include "CoreMinimal.h"
#include "SLMDomainBase.h"
#include "UObject/Object.h"
#include "SLMDomainRotation.generated.h"


USTRUCT(BlueprintType)
struct FSLMDataRotation
{
	GENERATED_BODY()

	FSLMDataRotation()
	{
	}

	FSLMDataRotation(const float AngVel, const float MOI): AngVel(AngVel), MOI(MOI)
	{
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	float AngVel = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	float MOI = 1;
};


USTRUCT(BlueprintType)
struct FSLMPortRotation
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SLMechatronics")
	FName PortName;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SLMechatronics")
	FSLMPortLocationData PortLocationData;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	FSLMDataRotation DefaultData;
};


UCLASS()
class SLMECHATRONICS_API USLMDomainRotation : public USLMDomainSubsystemBase
{
	GENERATED_BODY()
public:
	int32 AddPort(const FSLMPortRotation& Port);
	void RemovePort(const int32 PortIndex);

	UFUNCTION(BlueprintCallable, Category = "SLMechatronics")
	FSLMDataRotation GetNetworkData(const int32 PortIndex);
	UFUNCTION(BlueprintCallable, Category = "SLMechatronics")
	void SetNetworkAngVel(int32 PortIndex, float NewAngVel);
private:
	TSparseArray<FSLMPortRotation> Ports;
	TSparseArray<FSLMDataRotation> Networks;

	void CreateNetworkForPort(const int32 Port);

	virtual void CreateNetworkForPorts(const TArray<int32> PortIndices) override;
	virtual void DissolveNetworkIntoPort(const int32 NetworkIndex, const int32 PortIndex) override;
	virtual void RemovePortAtIndex(const int32 PortIndex) override;
	virtual void RemoveNetworkAtIndex(const int32 NetworkIndex) override;
};
