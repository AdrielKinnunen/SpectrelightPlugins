// Copyright Spectrelight Studios, LLC
#pragma once
#include "CoreMinimal.h"
#include "SLMDomainBase.h"
#include "SLMDomainAir.generated.h"

constexpr float SLMGammaAir                 = 1.4;                          //Specific heat ratio for air
constexpr float SLMIdealGasConstant         = 0.0831446;                    //Ideal gas constant for atm*L/(mol*K)
constexpr float SLMMolarMassAir             = 28.97;                        //Molar mass of air in g/mol
constexpr float SLMCvAir                    = 250 * SLMIdealGasConstant;    //Molar heat capacity at constant volume
constexpr float SLMFuelPerAirGrams          = 0.323939;                     //Grams of fuel per gram of air for stochiometric combustion
constexpr float SLMFuelJoulesPerGram        = 45000;                        //Combustion Energy per gram of fuel
constexpr float SLMOneOverTwoPi             = 0.159155;                     //Used in pressure to torque calculation for a pump

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

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics", meta=(Tooltip="Absolute pressure in bar"))
    float Pressure_bar = 1.0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics", meta=(Tooltip="Volume in liters"))
    float Volume_l = 1.0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics", meta=(Tooltip="Temperature in Kelvin"))
    float Temp_K = 288.15;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics", meta=(Tooltip="Oxygen ratio"))
    float OxygenRatio = 0.21;

    float GetMoles() const
    {
        // PV = NrT		->		N = PV/rT
        return (Pressure_bar * Volume_l) / (SLMIdealGasConstant * Temp_K);
    }

    float GetMassGrams() const
    {
        return GetMoles() * SLMMolarMassAir;
    }

    void AddHeatJoules(const float Joules)
    {
        Temp_K += Joules / (GetMassGrams() * SLMCvAir);
    }

    void ChangeVolumeIsentropically(const float NewVolume)
    {
        check(NewVolume > 0.0);
        const float NewTemp = Temp_K * FMath::Pow(Volume_l / NewVolume, SLMGammaAir - 1.0);
        const float NewPressure = Pressure_bar * FMath::Pow(NewVolume / Volume_l, SLMGammaAir * -1.0);
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
        const float FinalTemp = (FinalPressure * FinalVolume) / (FinalN * SLMIdealGasConstant); // PV = NrT		->		T = PV/Nr
        //const float FinalTemp = (FirstN * First.Temp_K + SecondN * Second.Temp_K) / FinalN;

        return FSLMDataAir(FinalPressure, FinalVolume, FinalTemp, FinalOxygen);
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
    void AddAir(const int32 PortIndex, const FSLMDataAir AirToAdd);
    UFUNCTION(BlueprintCallable, Category = "SLMechatronics")
    FSLMDataAir RemoveAir(const int32 PortIndex, const float VolumeLiters);

    virtual void Simulate(const float DeltaTime, const int32 StepCount) override;
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
