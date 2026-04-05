// Copyright Spectrelight Studios, LLC

#pragma once

#include "CoreMinimal.h"
#include "SLMDomainBase.h"
#include "UObject/Object.h"
#include "SLMDomainRotation.generated.h"

//UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_SLMECHATRONICS_DOMAIN_ROTATION)

constexpr float SLMRadToRPM				= 9.54929658551;			//Convert rad/s to RPM
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
    	//Result += FString::Printf(TEXT("%f,%f,%f,%f"), AngularVelocity, MomentOfInertia, StaticFriction, DynamicFriction);
    	Result += FString::Printf(TEXT("%f,%f,%f,%f"), AngularVelocity, MomentOfInertia);
    	return Result;
    }
};

FORCEINLINE uint32 GetTypeHash(const FSLMDataRotation& Data)
{
	uint32 Hash = 0;
	Hash = HashCombine(Hash, GetTypeHash(FMath::RoundToInt(Data.AngularVelocity * 1.0f)));
	Hash = HashCombine(Hash, GetTypeHash(FMath::RoundToInt(Data.MomentOfInertia * 1.0f)));
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
	
	//virtual void Simulate(const float DeltaTime, const float SubstepScalar) override;

public:
    USLMDomainRotation();

	int32 AddPort(const FSLMPortRotation& Port, const FSLMPortAddress& Address);
	void RemovePort(const FSLMPortAddress& Address);
    //void EnqueueRemovePort(const FSLMPortAddress& Address);

    //FSLMDataRotation GetData(const int32 PortIndex);
    //void SetAngularVelocity(const int32 PortIndex, const float NewAngVel);
	//void AddAngularImpulse(const int32 PortIndex, const float Impulse);
	//void AddTorque(const int32 PortIndex, const float Torque, const float DeltaTime);

private:
    TSparseArray<FSLMDataRotation> PortDefaults;
    TSparseArray<FSLMDataRotation> Particles;
	TSparseArray<int32> PortIDToParticleID;
	
	virtual uint32 GetDebugHash() override;
	virtual FString GetDebugString() override;


	virtual void ConnectPortsByAddress(const FSLMPortAddress First, const FSLMPortAddress Second) override;
	
    //void CreateParticleForPort(const int32 Port);

    //virtual void CreateParticleForPorts(const TArray<int32> PortIndices) override;
    //virtual void DissolveParticleIntoPort(const int32 ParticleIndex, const int32 PortIndex) override;
    //virtual void RemovePortAtIndex(const int32 PortIndex) override;
    //virtual void RemoveParticleAtIndex(const int32 ParticleIndex) override;
};
