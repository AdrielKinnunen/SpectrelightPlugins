// Copyright Spectrelight Studios, LLC

#pragma once

#include "CoreMinimal.h"
#include "SLMDomainTemplate.h"
#include "SLMDomainRotation.generated.h"

UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_SLM_System_Domain_Rotation);

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
	
	static void Merge(const TArray<FSLMDataRotation>& Sources, FSLMDataRotation& Target)
    {
    	float SumMomentum = 0;
    	float SumMOI = 0;
    	for (const auto& Source : Sources)
    	{
    		SumMomentum += Source.AngularVelocity * Source.MomentOfInertia;
    		SumMOI += Source.MomentOfInertia;
    	}
    	Target.AngularVelocity = SumMomentum / SumMOI;
    	Target.MomentOfInertia = SumMOI;	    
    }
	static void Dissolve(const FSLMDataRotation& Source, FSLMDataRotation& Target)
    {
    	Target.AngularVelocity = Source.AngularVelocity;
    }
	
	FString GetDebugString() const;
	uint32 GetDebugHash(const bool bVerbose) const;
};

USTRUCT(BlueprintType)
struct FSLMPortRotation
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
    FSLMDataRotation PortData;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	FSLMSpatialContextAuthored SpatialContext;
};


class SLMECHATRONICSDEMO_API FSLMDomainSystemRotation : public TSLMDomainSystem<FSLMDomainSystemRotation, FSLMDataRotation, FSLMPortRotation>
{
public:
	FSLMDomainSystemRotation();
	static FGameplayTag GetSystemTagStatic();
	virtual void Initialize() override;
	
	virtual void RunTests() override;
	virtual void PreSimulate(const float DeltaTime) override;
	virtual void Simulate(const float DeltaTime, const float SubstepScalar) override;
	virtual void PostSimulate(const float DeltaTime) override;
};