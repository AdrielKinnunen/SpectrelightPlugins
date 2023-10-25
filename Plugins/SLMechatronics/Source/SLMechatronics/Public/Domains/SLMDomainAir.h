// Copyright Spectrelight Studios, LLC
#pragma once
#include "CoreMinimal.h"
#include "SLMDomainBase.h"
#include "UObject/Object.h"
#include "SLMDomainAir.generated.h"


USTRUCT(BlueprintType)
struct FSLMDataAir
{
	GENERATED_BODY()
	FSLMDataAir()
	{
	}

	FSLMDataAir(const float Pressure_atm, const float Volume_l, const float Temp_K, const float Oxygen): Pressure_atm(Pressure_atm), Volume_l(Volume_l), Temp_K(Temp_K), Oxygen(Oxygen)
	{
	}

	//const float kgPerLiterAtSSL = 0.00129;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	float Pressure_atm = 1.0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	float Volume_l = 1.0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	float Temp_K = 288.15;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	float Oxygen = 0.21;
};


USTRUCT(BlueprintType)
struct FSLMPortAir
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SLMechatronics")
	FName PortName;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SLMechatronics")
	FSLMPortLocationData PortLocationData;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	FSLMDataAir DefaultData;
};


UCLASS()
class SLMECHATRONICS_API USLMDomainAir : public USLMDomainSubsystemBase
{
	GENERATED_BODY()
public:
	int32 AddPort(const FSLMPortAir& Port);
	void RemovePort(const int32 PortIndex);
	FSLMDataAir GetNetworkData(const int32 PortIndex);
	//void WriteData(const int32 PortIndex, const float Data);
	//virtual void PostSimulate(const float DeltaTime) override;
private:
	TSparseArray<FSLMPortAir> Ports;
	TSparseArray<FSLMDataAir> Networks;
	void CreateNetworkForPort(const int32 Port);
	virtual void CreateNetworkForPorts(const TArray<int32> PortIndices) override;
	virtual void DissolveNetworkIntoPort(const int32 NetworkIndex, int32 PortIndex) override;
	virtual void RemovePortAtIndex(const int32 PortIndex) override;
	virtual void RemoveNetworkAtIndex(const int32 NetworkIndex) override;
};
