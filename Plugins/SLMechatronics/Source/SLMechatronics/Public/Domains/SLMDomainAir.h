// Copyright Spectrelight Studios, LLC

#pragma once

#include "CoreMinimal.h"
#include "SLMDomainBase.h"
#include "SLMDomainAir.generated.h"

constexpr float SLMGammaAir                 = 1.4;                          //Specific heat ratio for air
constexpr float SLMIdealGasConstant         = 8.31446261815324;				//Ideal gas constant for Pa*m3/(mol*K)
constexpr float SLMMolarMassAir             = 28.97;				        //Molar mass of air in g/mol
constexpr float SLMCvAir                    = 2.5 * SLMIdealGasConstant;    //Molar heat capacity at constant volume
constexpr float SLMFuelPerAirGrams          = 0.323939;                     //Grams of fuel per gram of air for stoichiometric combustion
constexpr float SLMFuelJoulesPerGram        = 45000;                        //Combustion Energy per gram of fuel
constexpr float SLMRadToRev					= 0.15915494309;		        //Convert rad/s to rev/s
constexpr float SLMSTPMoles					= 44.61;						//Moles of air in 1 m3 at STP
constexpr float SLMSTPEnergy				= 253313;						//Energy of air in 1 m3 at STP
constexpr float SLMSTPOxygen				= 0.21;

USTRUCT(BlueprintType)
struct FSLMDataAir
{
    GENERATED_BODY()
	
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics", meta=(Tooltip="Volume in Cubic Meters"))
    float Volume = 1.0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics", meta=(Tooltip="Internal Energy in Joules"))
	float Energy = Volume * SLMSTPEnergy;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics", meta=(Tooltip="N in Moles"))
	float Moles = Volume * SLMSTPMoles;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics", meta=(Tooltip="Oxygen Ratio"))
    float OxygenRatio = SLMSTPOxygen;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics", meta=(Tooltip="Indicates if this volume is connected to atmosphere"))
	bool bConnectedToAtmosphere = true;

	void SetToSTP()
	{
		Energy = Volume * SLMSTPEnergy;
		Moles = Volume * SLMSTPMoles;
		OxygenRatio = SLMSTPOxygen;
	}
	
	void UpdateVolume()
	{
		Volume = 0.4 * Energy * Moles * SLMIdealGasConstant;
	}
	
	void UpdateEnergy()
	{
		Energy = 2.5 * Volume / (Moles * SLMIdealGasConstant);
	}

	void UpdateMoles()
	{
		Moles = 2.5 * Volume / (Energy * SLMIdealGasConstant);
	}

	float GetPressure() const
	{
		return 0.4 * Energy / Volume;
	}

	float GetTemperature() const
	{
		return Energy / (Moles * SLMCvAir);
	}

    float GetMassGrams() const
    {
        return Moles * SLMMolarMassAir;
    }

	void CompressOrExpandToVolume(const float NewVolume)
	{
		check(NewVolume > 0.0);
		const float Ratio = Volume / NewVolume;
		Energy = Energy * FMath::Pow(Ratio, SLMGammaAir - 1);
		Volume = NewVolume;
	}

	void CompressOrExpandToPressure(const float NewPressure)
	{
		check(NewPressure > 0.0);
		const float PressureRatio = NewPressure / GetPressure();
		Volume = Volume * FMath::Pow(1.0f / PressureRatio, 1.0f / SLMGammaAir);
		Energy = Energy * FMath::Pow(PressureRatio, (SLMGammaAir - 1.0f) / SLMGammaAir);
	}
	
	void MixWith(const FSLMDataAir Other)
	{
    	OxygenRatio = (Moles * OxygenRatio + Other.Moles * Other.OxygenRatio) / (Moles + Other.Moles);
		Moles = Moles + Other.Moles;
    	Volume = Volume + Other.Volume;
		Energy = Energy + Other.Energy;
    }

	void Scale(const float Scalar)
	{
		Volume *= Scalar;
		Moles *= Scalar;
		Energy *= Scalar;
	}
	
	bool NearlyEqualWith( const FSLMDataAir Other) const
	{
		const bool V = FMath::IsNearlyEqual(Volume, Other.Volume);
		const bool E = FMath::IsNearlyEqual(Energy, Other.Energy);
		const bool N = FMath::IsNearlyEqual(OxygenRatio, Other.OxygenRatio);
		const bool A = bConnectedToAtmosphere == Other.bConnectedToAtmosphere;
		return V && E && N && A;
	}
	
	FString GetDebugString() const;
	uint32 GetDebugHash() const;
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

	int32 AddPort(const FSLMPortAir& Port, const FSLMPortAddress& PortAddress);
	
	virtual void RunTests() override;
	virtual void PreSimulate(const float DeltaTime) override;
	virtual void Simulate(const float DeltaTime, const float SubstepScalar) override;
	virtual void PostSimulate(const float DeltaTime) override;
	virtual uint32 GetDebugHash() override;
	virtual FString GetDebugString(const bool Verbose) override;
	virtual FString GetPortDebugString(const FSLMPortAddress& Address) override;
	
	FSLMDataAir& GetParticleRef(const int32 PortID);

protected:
	virtual void CreateParticleForPorts(const TArray<int32> PortIDs) override;
	virtual void DissolveParticleIntoPort(const int32 ParticleID, const int32 PortID) override;
	virtual void RemovePortAtAddress(const FSLMPortAddress& PortAddress) override;
	virtual void RemoveParticleAtID(const int32 ParticleID) override;

private:	
	TSparseArray<FSLMDataAir> PortDefaults;
	TSparseArray<FSLMDataAir> Particles;
};
