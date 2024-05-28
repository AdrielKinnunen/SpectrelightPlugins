// Copyright Spectrelight Studios, LLC

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "SLMDeviceBase.generated.h"

class USLMDomainSubsystemBase;

DECLARE_DYNAMIC_DELEGATE(FSLMEvent);


UCLASS(Abstract)
class SLMECHATRONICS_API USLMDeviceComponentBase : public UActorComponent
{
    GENERATED_BODY()
public:
    USLMDeviceComponentBase();
protected:
    int32 DeviceIndex = -1;

    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};


UCLASS()
class SLMECHATRONICS_API USLMDeviceSubsystemBase : public UWorldSubsystem
{
    GENERATED_BODY()
public:
	virtual void PreSimulate(const float DeltaTime);
    virtual void Simulate(const float DeltaTime, const float SubstepScalar);
    virtual void PostSimulate(const float DeltaTime);

	//Debug
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	bool bDebugDraw = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	bool bDebugPrint = false;
	UFUNCTION(Blueprintcallable, Category = "SLMechatronics")
	virtual FString GetDebugString(int32 PortIndex);
	virtual void DebugDraw();
	virtual void DebugPrint();
};
