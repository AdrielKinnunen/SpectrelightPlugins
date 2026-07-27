// Copyright Spectrelight Studios, LLC. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"


class USLMManager;


class SLMECHATRONICS_API FSLMSystemBase
{
public:
	virtual ~FSLMSystemBase() = default;
	virtual void Initialize() = 0;

	
	void CheckForCleanUp();
	
	virtual void PreSimulate(const float DeltaTime);
	virtual void Simulate(const float DeltaTime, const float SubstepScalar);
	virtual void PostSimulate(const float DeltaTime);
	
	virtual void DebugDraw();
	virtual uint32 GetDebugHash(const bool bVerbose) = 0;
	virtual FString GetDebugString(const bool bVerbose) = 0;
	
	FColor DebugColor = FColor::Black;
	FGameplayTag SystemTag = FGameplayTag::EmptyTag;
	bool bIsServer = false;
	
	USLMManager* Manager = nullptr;
	
protected:
	bool bNeedsCleanUp = false;
	
private:
	virtual void CleanUp() = 0;
};
