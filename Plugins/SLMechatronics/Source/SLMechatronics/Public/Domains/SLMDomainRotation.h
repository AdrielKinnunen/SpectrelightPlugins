// Copyright Spectrelight Studios, LLC

#pragma once

#include "CoreMinimal.h"
#include "SLMDomainBase.h"
#include "UObject/Object.h"
#include "SLMDomainRotation.generated.h"

constexpr float SLMRadToRPM				= 9.54929658551;			//Convert rad/s to RPM
constexpr float SLMRadToDeg				= 57.2957795131;			//Convert rad/s to deg/s
constexpr float SLMTorqueUnrealToSI		= 0.0001;					//Convert Unreal torque to SI
constexpr float SLMTorqueSIToUnreal		= 10000;					//Convert SI torque to Unreal

USTRUCT(BlueprintType)
struct FSLMDataRotation
{
    GENERATED_BODY()

    FSLMDataRotation()
    {
    }

    FSLMDataRotation(const float AngularVelocity, const float MomentOfInertia): AngularVelocity(AngularVelocity), MomentOfInertia(MomentOfInertia)
    {
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics", meta=(Tooltip="Angular Velocity in rad/s"))
    float AngularVelocity = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics", meta=(Tooltip="Moment of Inertia in kg*m2"))
    float MomentOfInertia = 1;
    //UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics", meta=(Tooltip="Moment of Inertia in kg*m2"))
    //float StaticFriction = 0;
    //UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics", meta=(Tooltip="Moment of Inertia in kg*m2"))
    //float DynamicFriction = 0;
    
    FSLMDataRotation GetApparentStateThroughGearRatio(const float GearRatio) const
    {
        const float ApparentAngularVelocity = AngularVelocity / GearRatio;
        const float ApparentMomentOfInertia = GearRatio * GearRatio * MomentOfInertia;
        return FSLMDataRotation(ApparentAngularVelocity, ApparentMomentOfInertia);
    }

	float GetRPM() const
    {
	    return AngularVelocity * SLMRadToRPM;
    }

	void AddImpulse(const float Impulse)
    {
    	AngularVelocity += Impulse / MomentOfInertia;
    }

	void AddTorque(const float Torque, const float DeltaTime)
    {
    	AngularVelocity += Torque * DeltaTime / MomentOfInertia;
    }
	
	FString GetDebugString() const
    {
    	FString Result;
    	Result += FString::Printf(TEXT("%f,%f"), AngularVelocity, MomentOfInertia);
    	return Result;
    }
};

FORCEINLINE uint32 GetTypeHash(const FSLMDataRotation& Data)
{
	uint32 Hash = 0;
	Hash = HashCombine(Hash, GetTypeHash(FMath::RoundToInt(Data.AngularVelocity * 100.0f)));
	Hash = HashCombine(Hash, GetTypeHash(FMath::RoundToInt(Data.MomentOfInertia * 100.0f)));
	//Hash = HashCombine(Hash, GetTypeHash(FMath::RoundToInt(Data.StaticFriction * 1.0f)));
	//Hash = HashCombine(Hash, GetTypeHash(FMath::RoundToInt(Data.DynamicFriction * 1.0f)));
	return Hash;
}


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
	int32 AddPort(const FSLMPortRotation& Port, const FSLMPortAddress& PortAddress);
	void RemovePort(const FSLMPortAddress& PortAddress);
	FSLMDataRotation GetData(const int32 PortID);
	//void SetAngularVelocity(const int32 PortIndex, const float NewAngVel);
	//void AddAngularImpulse(const int32 PortIndex, const float Impulse);
	void AddTorque(const int32 PortID, const float Torque, const float DeltaTime);
	
protected:
	virtual void RunTests() override;
	virtual void CreateParticleForPorts(const TArray<int32> PortIDs) override;
	virtual void DissolveParticleIntoPort(const int32 ParticleID, const int32 PortID) override;
	virtual void RemovePortAtAddress(const FSLMPortAddress& PortAddress) override;
	virtual void RemoveParticleAtID(const int32 ParticleID) override;

private:
	//virtual void PreSimulate(const float DeltaTime);
	//virtual void Simulate(const float DeltaTime, const float SubstepScalar);
	//virtual void PostSimulate(const float DeltaTime);
	virtual uint32 GetDebugHash() override;
	virtual FString GetDebugString(const bool Verbose) override;
	virtual FString GetPortDebugString(const FSLMPortAddress& Address) override;
	
	TSparseArray<FSLMDataRotation> PortDefaults;
	TSparseArray<FSLMDataRotation> Particles;
};
