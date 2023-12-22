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
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SLMechatronics")
	FSLMPortMetaData PortMetaData;
};


UCLASS(BlueprintType)
class SLMECHATRONICS_API USLMDomainRotation : public USLMDomainSubsystemBase
{
	GENERATED_BODY()
public:
	static constexpr float RADS_TO_RPM = 9.54929658551372;

	UFUNCTION(BlueprintCallable, Category = "SLMechatronics|Rotation")
	int32 AddPort(const FSLMPortRotation& Port);
	UFUNCTION(BlueprintCallable, Category = "SLMechatronics|Rotation")
	void RemovePort(const int32 PortIndex);

	UFUNCTION(BlueprintPure, Category = "SLMechatronics|Rotation")
	FSLMDataRotation GetByPortIndex(const int32 PortIndex);
	UFUNCTION(BlueprintCallable, Category = "SLMechatronics|Rotation")
	void SetNetworkAngVel(const int32 PortIndex, const float NewAngVel);
	UFUNCTION(BlueprintCallable, Category = "SLMechatronics|Rotation")
	void AddTorque(int32 PortIndex, float Torque, float DeltaTime);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static float ConvertRPMtoRads(const float RPM)
	{
		return RPM / RADS_TO_RPM;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static float ConvertRadsToRPM(const float RPM)
	{
		return RPM * RADS_TO_RPM;
	}

	
private:
	TSparseArray<FSLMDataRotation> PortsData;
	TSparseArray<FSLMDataRotation> Networks;

	void CreateNetworkForPort(const int32 Port);

	virtual void CreateNetworkForPorts(const TArray<int32> PortIndices) override;
	virtual void DissolveNetworkIntoPort(const int32 NetworkIndex, const int32 PortIndex) override;
	virtual void RemovePortAtIndex(const int32 PortIndex) override;
	virtual void RemoveNetworkAtIndex(const int32 NetworkIndex) override;
};
