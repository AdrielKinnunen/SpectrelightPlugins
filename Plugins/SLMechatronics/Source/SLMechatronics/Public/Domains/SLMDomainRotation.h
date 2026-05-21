// Copyright Spectrelight Studios, LLC

#pragma once

#include "CoreMinimal.h"
#include "SLMDomainBase.h"
#include "SLMDomainRotation.generated.h"

constexpr float SLMRadToRPM				= 9.54929658551;			//Convert rad/s to RPM
constexpr float SLMRadToDeg				= 57.2957795131;			//Convert rad/s to deg/s
constexpr float SLMTorqueUnrealToSI		= 0.0001;					//Convert Unreal torque to SI
constexpr float SLMTorqueSIToUnreal		= 10000;					//Convert SI torque to Unreal

USTRUCT(BlueprintType)
struct FSLMDataRotation
{
    GENERATED_BODY()

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
	
	FString GetDebugString() const;
	uint32 GetDebugHash() const;
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
	
	int32 AddPort(const FSLMPortRotation& Port, const FSLMPortAddress& PortAddress);
	
	virtual void RunTests() override;
	virtual void PreSimulate(const float DeltaTime) override;
	virtual void Simulate(const float DeltaTime, const float SubstepScalar) override;
	virtual void PostSimulate(const float DeltaTime) override;
	virtual uint32 GetDebugHash() override;
	virtual FString GetDebugString(const bool Verbose) override;
	virtual FString GetPortDebugString(const FSLMPortAddress& Address) override;
	
	FSLMDataRotation& GetParticleRef(const int32 PortID);

protected:
	virtual void CreateParticleForPorts(const TArray<int32> PortIDs) override;
	virtual void DissolveParticleIntoPort(const int32 ParticleID, const int32 PortID) override;
	virtual void RemovePortAtAddress(const FSLMPortAddress& PortAddress) override;
	virtual void RemoveParticleAtID(const int32 ParticleID) override;

private:	
	TSparseArray<FSLMDataRotation> PortDefaults;
	TSparseArray<FSLMDataRotation> Particles;
};
