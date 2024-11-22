// Copyright Spectrelight Studios, LLC

#pragma once

#include "CoreMinimal.h"
#include "SLMDomainBase.h"
#include "UObject/Object.h"
#include "SLMDomainRotation.generated.h"

UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_SPECTRELIGHTDYNAMICS_DOMAIN_ROTATION)

constexpr float SLMRadToRPM				= 9.54929658551;			//Convert rad/s to RPM

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
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics", meta=(Tooltip="Moment of Inertia in kg*m2"))
    float StaticFriction = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics", meta=(Tooltip="Moment of Inertia in kg*m2"))
    float DynamicFriction = 0;
    
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

    UFUNCTION(BlueprintCallable, Category = "SLMechatronics")
    int32 AddPort(const FSLMPortRotation& Port);
    UFUNCTION(BlueprintCallable, Category = "SLMechatronics")
    void RemovePort(const int32 PortIndex);

    UFUNCTION(BlueprintPure, Category = "SLMechatronics")
    FSLMDataRotation GetData(const int32 PortIndex);
    UFUNCTION(BlueprintCallable, Category = "SLMechatronics")
    void SetAngularVelocity(const int32 PortIndex, const float NewAngVel);
	UFUNCTION(BlueprintCallable, Category = "SLMechatronics")
	void AddAngularImpulse(const int32 PortIndex, const float Impulse);
	UFUNCTION(BlueprintCallable, Category = "SLMechatronics")
	void AddTorque(const int32 PortIndex, const float Torque, const float DeltaTime);

    virtual void Simulate(const float DeltaTime, const float SubstepScalar) override;
    virtual FString GetDebugString(const int32 PortIndex) override;
private:
    TSparseArray<FSLMDataRotation> Ports;
    TSparseArray<FSLMDataRotation> Networks;

    void CreateNetworkForPort(const int32 Port);

    virtual void CreateNetworkForPorts(const TArray<int32> PortIndices) override;
    virtual void DissolveNetworkIntoPort(const int32 NetworkIndex, const int32 PortIndex) override;
    virtual void RemovePortAtIndex(const int32 PortIndex) override;
    virtual void RemoveNetworkAtIndex(const int32 NetworkIndex) override;
};
