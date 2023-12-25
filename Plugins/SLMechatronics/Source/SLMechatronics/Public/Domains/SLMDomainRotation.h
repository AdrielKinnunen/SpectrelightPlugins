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

	FSLMDataRotation(const float RPS, const float MOI): RPS(RPS), MOI(MOI)
	{
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	float RPS = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	float MOI = 1;
};


USTRUCT(BlueprintType)
struct FSLMPortRotation
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	FSLMDataRotation PortData;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	FSLMPortMetaData PortMetaData;
};


UCLASS(BlueprintType)
class SLMECHATRONICS_API USLMDomainRotation : public USLMDomainSubsystemBase
{
	GENERATED_BODY()
public:
	USLMDomainRotation();
	
	UFUNCTION(BlueprintCallable, Category = "SLMechatronics")
	int32 AddPort(const FSLMPortRotation& Port);
	UFUNCTION(BlueprintCallable, Category = "SLMechatronics")
	void RemovePort(const int32 PortIndex);

	UFUNCTION(BlueprintPure, Category = "SLMechatronics")
	FSLMDataRotation GetByPortIndex(const int32 PortIndex);
	UFUNCTION(BlueprintCallable, Category = "SLMechatronics")
	void SetAngVelByPortIndex(const int32 PortIndex, const float NewAngVel);

	virtual void Simulate(const float DeltaTime) override;
	virtual FString GetDebugString(const int32 PortIndex) override;

private:
	TSparseArray<FSLMDataRotation> PortsData;
	TSparseArray<FSLMDataRotation> Networks;

	void CreateNetworkForPort(const int32 Port);

	virtual void CreateNetworkForPorts(const TArray<int32> PortIndices) override;
	virtual void DissolveNetworkIntoPort(const int32 NetworkIndex, const int32 PortIndex) override;
	virtual void RemovePortAtIndex(const int32 PortIndex) override;
	virtual void RemoveNetworkAtIndex(const int32 NetworkIndex) override;
};
