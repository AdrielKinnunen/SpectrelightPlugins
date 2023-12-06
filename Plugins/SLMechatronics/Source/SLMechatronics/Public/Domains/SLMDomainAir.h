// Copyright Spectrelight Studios, LLC
#pragma once
#include "CoreMinimal.h"
#include "SLMDomainBase.h"
#include "SLMDomainAir.generated.h"

constexpr float GammaAir			= 1.4;							//Specific heat ratio for air
constexpr float IdealGasConstant	= 0.0831446;					//Ideal gas constant for atm*L/(mol*K)
constexpr float MolarMassAir		= 28.97;						//Molar mass of air in g/mol
constexpr float CvAir				= 250 * IdealGasConstant;		//Molar heat capacity at constant volume
constexpr float FuelPerAirGrams		= 0.323939;						//Grams of fuel per gram of air for stochiometric combustion
constexpr float FuelJoulesPerGram	= 45000;						//Combustion Energy per gram of fuel
constexpr float OneOverTwoPi		= 0.159155;						//Used in pressure to torque calculation for a pump

USTRUCT(BlueprintType)
struct FSLMDataAir
{
	GENERATED_BODY()
	FSLMDataAir()
	{
	}

	FSLMDataAir(const float Pressure_atm, const float Volume_l, const float Temp_K, const float Oxygen): Pressure_bar(Pressure_atm), Volume_l(Volume_l), Temp_K(Temp_K), OxygenRatio(Oxygen)
	{
	}

	//static constexpr float GammaAir = 1.4;							//Specific heat ratio for air
	//static constexpr float IdealGasConstant = 0.0813144;				//Ideal gas constant for atm*L/(mol*K)
	//static constexpr float MolarMassAir = 28.97;						//Molar mass of air in g/mol
	//static constexpr float CvAir = 250 * IdealGasConstant;			//Molar heat capacity at constant volume
		
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	float Pressure_bar = 1.0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	float Volume_l = 1.0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	float Temp_K = 288.15;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	float OxygenRatio = 0.21;
	
	float GetMoles() const
	{
		// PV = NrT		->		N = PV/rT
		return (Pressure_bar * Volume_l) / (IdealGasConstant * Temp_K);
	}

	float GetMassGrams() const
	{
		const float Mass = GetMoles() * MolarMassAir;
		return Mass;
	}

	void AddHeatJoules(const float Joules)
	{
		Temp_K += Joules / (GetMassGrams() * CvAir);
	}

	void ChangeVolumeIsentropically(const float NewVolume)
	{
		const float NewTemp = Temp_K * FMath::Pow(Volume_l / NewVolume, GammaAir - 1.0);
		const float NewPressure = Pressure_bar * FMath::Pow(NewVolume / Volume_l, GammaAir * -1.0);

		Temp_K = NewTemp;
		Pressure_bar = NewPressure;
		Volume_l = NewVolume;
	}
	
	
	static FSLMDataAir Mix(const FSLMDataAir First, const FSLMDataAir Second)
	{
		const float FirstN = First.GetMoles();
		const float SecondN = Second.GetMoles();
		const float FinalN = FirstN + SecondN;

		const float FinalOxygen = (FirstN * First.OxygenRatio + SecondN * Second.OxygenRatio) / FinalN;
		const float FinalVolume = First.Volume_l + Second.Volume_l;
		const float FinalPressure = (First.Pressure_bar * First.Volume_l + Second.Pressure_bar * Second.Volume_l) / FinalVolume;
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
	
	FSLMDataAir GetByPortIndex(const int32 PortIndex);
	
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
