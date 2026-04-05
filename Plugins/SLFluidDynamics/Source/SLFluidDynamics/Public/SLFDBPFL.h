// Copyright Spectrelight Studios, LLC. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SLFDBPFL.generated.h"

/**
 * 
 */
UCLASS()
class SLFLUIDDYNAMICS_API USLFDBPFL : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	UFUNCTION(BlueprintCallable, Category="Test")
	static int32 AddIntsTogether(const int32 A, const int32 B);
};
