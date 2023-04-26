
// Copyright Spectrelight Studios, LLC

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "SLMechatronicsTypes.generated.h"


USTRUCT(BlueprintType)
struct FSLMData
{
	GENERATED_BODY()
	//FSLMData(){}
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	float Value = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	float Capacity = 1;
};

USTRUCT(BlueprintType)
struct FSLMPort
{
	GENERATED_BODY()
	//FSLMPort(){}
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	FGameplayTag Domain;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	FSLMData Data;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	FName ComponentName;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	FName SocketName;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	FVector Offset;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	int32 Index = -1;
};