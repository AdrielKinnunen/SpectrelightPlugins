// Copyright Spectrelight Studios, LLC. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SLMSubsystemBase.generated.h"



UCLASS()
class SLMECHATRONICS_API USLMSubsystemBase : public UWorldSubsystem
{
	GENERATED_BODY()
	friend class USLMManager;
	
	virtual void PreSimulate(const float DeltaTime);
	virtual void Simulate(const float DeltaTime, const float SubstepScalar);
	virtual void PostSimulate(const float DeltaTime);
	
	virtual void DebugDraw();
	virtual uint32 GetDebugHash();
	virtual FString GetDebugString(bool Verbose);
	
protected:
	FColor DebugColor = FColor::Black;

};