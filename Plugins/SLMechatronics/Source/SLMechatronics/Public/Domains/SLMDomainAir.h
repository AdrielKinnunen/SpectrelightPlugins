// Copyright Spectrelight Studios, LLC
#pragma once
#include "CoreMinimal.h"
#include "SLMDomainBase.h"
#include "SLMDomainAir.generated.h"


USTRUCT(BlueprintType)
struct FSLMDataAir
{
	GENERATED_BODY()
	FSLMDataAir()
	{
	}

	FSLMDataAir(const float Pressure_atm, const float Volume_l, const float Temp_K, const float Oxygen): Pressure_atm(Pressure_atm), Volume_l(Volume_l), Temp_K(Temp_K), OxygenRatio(Oxygen)
	{
	}

	static constexpr float GammaAir = 1.4;						//Specific heat ratio for air
	static constexpr float IdealGasConstant = 0.0821;			//Ideal gas constant for atm*L/(mol*K)
	static constexpr float MolarMassAir = 28.97;				//Molar mass of air in g/mol
		
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	float Pressure_atm = 1.0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	float Volume_l = 1.0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	float Temp_K = 288.15;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	float OxygenRatio = 0.21;
	
	float GetMoles() const
	{
		return (Pressure_atm * Volume_l) / (IdealGasConstant * Temp_K);											// PV = NrT		->		N = PV/rT
	}

	float GetMassGrams() const
	{
		const float Mass = GetMoles() * MolarMassAir;
		return Mass;
	}

	void AddHeatJoules(const float Joules)
	{
		Temp_K += Joules / (Volume_l * GammaAir);
	}

	void ChangeVolumeIsentropically(const float NewVolume)
	{
		const float NewTemp = Temp_K * FMath::Pow(Volume_l / NewVolume, GammaAir - 1.0);
		const float NewPressure = Pressure_atm * FMath::Pow(NewVolume / Volume_l, GammaAir * -1.0);

		Temp_K = NewTemp;
		Pressure_atm = NewPressure;
		Volume_l = NewVolume;
	}
	
	
	static FSLMDataAir Mix(const FSLMDataAir First, const FSLMDataAir Second)
	{
		const float FirstN = First.GetMoles();
		const float SecondN = Second.GetMoles();
		const float FinalN = FirstN + SecondN;

		const float FinalOxygen = (FirstN * First.OxygenRatio + SecondN * Second.OxygenRatio) / FinalN;
		const float FinalVolume = First.Volume_l + Second.Volume_l;
		const float FinalPressure = (First.Pressure_atm * First.Volume_l + Second.Pressure_atm * Second.Volume_l) / FinalVolume;
		const float FinalTemp = (FinalPressure * FinalVolume) / (FinalN * IdealGasConstant);					// PV = NrT		->		T = PV/Nr
		//const float FinalTemp = (FirstN * First.Temp_K + SecondN * Second.Temp_K) / FinalN;
		
		return FSLMDataAir(FinalPressure, FinalVolume, FinalTemp, FinalOxygen);
	}

	
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
	FSLMDataAir GetCopy(const int32 PortIndex);
	FSLMDataAir& GetRef(const int32 PortIndex);
	
	FSLMDataAir RemoveAir(const int32 PortIndex, const float VolumeLiters);
	void AddAir(const int32 PortIndex, const FSLMDataAir AirToAdd);
		
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
