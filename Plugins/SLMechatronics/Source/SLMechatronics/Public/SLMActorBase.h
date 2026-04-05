// Copyright Spectrelight Studios, LLC. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SLMActorBase.generated.h"

UCLASS()
class SLMECHATRONICS_API ASLMActorBase : public AInfo
{
	GENERATED_BODY()

public:
	ASLMActorBase();
	
	//Core
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
	
protected:
	virtual void BeginPlay() override;
};
