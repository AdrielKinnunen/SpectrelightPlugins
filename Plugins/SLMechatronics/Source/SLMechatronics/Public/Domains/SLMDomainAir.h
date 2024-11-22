// Copyright Spectrelight Studios, LLC

#pragma once

#include "CoreMinimal.h"
#include "SLMDomainBase.h"
#include "SLMDomainAir.generated.h"

UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_SPECTRELIGHTDYNAMICS_DOMAIN_AIR)

constexpr float SLMGammaAir                 = 1.4;                          //Specific heat ratio for air
constexpr float SLMIdealGasConstant         = 0.0831446261815324;           //Ideal gas constant for atm*L/(mol*K)
constexpr float SLMMolarMassAir             = 28.97;                        //Molar mass of air in g/mol
constexpr float SLMCvAir                    = 250 * SLMIdealGasConstant;    //Molar heat capacity at constant volume
constexpr float SLMFuelPerAirGrams          = 0.323939;                     //Grams of fuel per gram of air for stoichiometric combustion
constexpr float SLMFuelJoulesPerGram        = 45000;                        //Combustion Energy per gram of fuel
constexpr float SLMRadToRev					= 0.159155;				        //Convert rad/s to rev/s

USTRUCT(BlueprintType)
struct FSLMDataAir
{
    GENERATED_BODY()
    FSLMDataAir()
    {
    }
	
    FSLMDataAir(const float Volume_l, const float Pressure_bar, const float Temp_K, const float OxygenRatio, const bool bConnectedToAtmosphere):
		Volume_l(Volume_l),
		Pressure_bar(Pressure_bar),
		Temp_K(Temp_K),
		OxygenRatio(OxygenRatio),
		bConnectedToAtmosphere(bConnectedToAtmosphere)
    {
    }
	
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics", meta=(Tooltip="Volume in liters"))
    float Volume_l = 1.0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics", meta=(Tooltip="Absolute pressure in bar"))
	float Pressure_bar = 1.0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics", meta=(Tooltip="Temperature in Kelvin"))
	float Temp_K = 300;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics", meta=(Tooltip="N in Moles"))
	float N_Moles = Pressure_bar * Volume_l / (Temp_K * SLMIdealGasConstant);	//n = PV/RT
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics", meta=(Tooltip="Oxygen ratio"))
    float OxygenRatio = 0.21;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics", meta=(Tooltip="Indicates if this volume is connected to atmosphere"))
	bool bConnectedToAtmosphere = true;

	void UpdateVolume()
	{
		Volume_l = N_Moles * SLMIdealGasConstant * Temp_K / Pressure_bar;		//V = nRT/P
	}
	
	void UpdatePressure()
	{
		Pressure_bar = N_Moles * SLMIdealGasConstant * Temp_K / Volume_l;		//P = nRT/V
	}

	void UpdateTemperature()
	{
		Temp_K = Pressure_bar * Volume_l / (N_Moles * SLMIdealGasConstant);		//T = PV/nR
	}

	void UpdateMoles()
	{
		N_Moles = Pressure_bar * Volume_l / (Temp_K * SLMIdealGasConstant);		//n = PV/RT
	}
	
	void SetSaneLimits()
	{
		Pressure_bar = FMath::Clamp(Pressure_bar, 0.01, 1000);
		Temp_K = FMath::Clamp(Temp_K, 1, 10000);
	}

    float GetMassGrams() const
    {
        return N_Moles * SLMMolarMassAir;
    }

	float GetInternalEnergy() const
    {
	    return Pressure_bar * Volume_l * 250;
    }

    void AddHeatJoules(const float Joules)
    {
		Temp_K += Joules / (N_Moles * SLMCvAir);
		UpdatePressure();
    }

    void CompressOrExpandToVolume(const float NewVolume)
    {
        check(NewVolume > 0.0);
    	Pressure_bar = Pressure_bar * FMath::Pow(Volume_l / NewVolume, SLMGammaAir);
    	Volume_l = NewVolume;
        UpdateTemperature();
		SetSaneLimits();
    }

	void CompressOrExpandToPressure(const float NewPressure)
    {
    	check(NewPressure > 0.0);
    	Volume_l = Volume_l * FMath::Pow(Pressure_bar / NewPressure, 1/SLMGammaAir);
		Pressure_bar = NewPressure;
		UpdateTemperature();
		SetSaneLimits();
    }

	void MixWith(const FSLMDataAir Other)
	{
    	const float FinalMoles = N_Moles + Other.N_Moles;
    	const float FinalOxygen = (N_Moles * OxygenRatio + Other.N_Moles * Other.OxygenRatio) / FinalMoles;
    	const float FinalVolume = Volume_l + Other.Volume_l;
    	const float FinalPressure = (Pressure_bar * Volume_l + Other.Pressure_bar * Other.Volume_l) / FinalVolume;
		UpdateTemperature();
    	const float FinalTemp = (FinalPressure * FinalVolume) / (FinalMoles * SLMIdealGasConstant);
    	Volume_l = FinalVolume;
    	Pressure_bar = FinalPressure;
		Temp_K = FinalTemp;
    	OxygenRatio = FinalOxygen;
		SetSaneLimits();
    }

	bool NearlyEqualWith( const FSLMDataAir Other) const
	{
		const bool V = FMath::IsNearlyEqual(Volume_l, Other.Volume_l);
		const bool P = FMath::IsNearlyEqual(Pressure_bar, Other.Pressure_bar);
		const bool T = FMath::IsNearlyEqual(Temp_K, Other.Temp_K);
		const bool N = FMath::IsNearlyEqual(OxygenRatio, Other.OxygenRatio);
		const bool A = bConnectedToAtmosphere == Other.bConnectedToAtmosphere;
		return V && P && T && N && A;
	}
	
	/*
	void Inject(const FSLMDataAir Other)
    {
    	const float OriginalVolume = Volume_l;
	    MixWith(Other);
    	CompressOrExpandToVolume(OriginalVolume);
    }

	FSLMDataAir Extract(const float VolumeToExtract)
    {
    	const float OriginalVolume = Volume_l;
	    CompressOrExpandToVolume(OriginalVolume + VolumeToExtract);
    	Volume_l = OriginalVolume;
    	return FSLMDataAir(VolumeToExtract, Pressure_bar, Temp_K, OxygenRatio, false);
    }
    */

	FString GetDebugString()
	{
		FString Result;
		Result += FString::Printf(TEXT("Volume(l) = %f\n"), Volume_l);
		Result += FString::Printf(TEXT("Pressure(bar) = %f\n"), Pressure_bar);
		Result += FString::Printf(TEXT("Temperature(K) = %f\n"), Temp_K);
		Result += FString::Printf(TEXT("Oxygen(percent) = %f\n"), 100 * OxygenRatio);
		Result += FString::Printf(TEXT("Connected To Atmosphere = %i\n"), bConnectedToAtmosphere);
		return Result;
	}
};


USTRUCT(BlueprintType)
struct FSLMPortAir
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
    FSLMDataAir PortData;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
    FSLMPortMetaData PortMetaData;
};


UCLASS(BlueprintType)
class SLMECHATRONICS_API USLMDomainAir : public USLMDomainSubsystemBase
{
    GENERATED_BODY()
public:
    USLMDomainAir();

    UFUNCTION(BlueprintCallable, Category = "SLMechatronics")
    int32 AddPort(const FSLMPortAir& Port);
    UFUNCTION(BlueprintCallable, Category = "SLMechatronics")
    void RemovePort(const int32 PortIndex);

    UFUNCTION(BlueprintPure, Category = "SLMechatronics")
    FSLMDataAir GetData(const int32 PortIndex);
	UFUNCTION(BlueprintCallable, Category = "SLMechatronics")
	void SetData(const int32 PortIndex, const FSLMDataAir Data);
	//UFUNCTION(BlueprintCallable, Category = "SLMechatronics")
	//void MixAndCompressIntoByIndex (const int32 PortIndex, const FSLMDataAir AirToAdd);
    //UFUNCTION(BlueprintCallable, Category = "SLMechatronics")
    //FSLMDataAir RemoveAir(const int32 PortIndex, const float VolumeLiters);

	virtual void RunTests() override;

    virtual void Simulate(const float DeltaTime, const float SubstepScalar) override;
    virtual FString GetDebugString(const int32 PortIndex) override;
private:
    TSparseArray<FSLMDataAir> Ports;
    TSparseArray<FSLMDataAir> Networks;

    void CreateNetworkForPort(const int32 Port);

    virtual void CreateNetworkForPorts(const TArray<int32> PortIndices) override;
    virtual void DissolveNetworkIntoPort(const int32 NetworkIndex, int32 PortIndex) override;
    virtual void RemovePortAtIndex(const int32 PortIndex) override;
    virtual void RemoveNetworkAtIndex(const int32 NetworkIndex) override;
};
