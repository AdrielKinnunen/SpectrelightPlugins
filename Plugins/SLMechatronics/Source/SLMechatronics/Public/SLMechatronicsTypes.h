
// Copyright Spectrelight Studios, LLC

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "SLMechatronicsTypes.generated.h"


USTRUCT(BlueprintType)
struct FSLMData
{
	GENERATED_BODY()

	FSLMData(){}
	FSLMData(const float Value, const float Capacity):Value(Value), Capacity(Capacity){}
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	float Value = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	float Capacity = 1;
};

USTRUCT(BlueprintType)
struct FSLMPortLocationData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	FName ComponentName;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	FName SocketName;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	FVector Offset;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	USceneComponent* SceneComponent = nullptr;
};

USTRUCT(BlueprintType)
struct FSLMPort
{
	GENERATED_BODY()
	
	FSLMPort(){}
	FSLMPort(const FGameplayTag Domain, const FSLMData Data):Domain(Domain), DefaultData(Data){}

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SLMechatronics")
	FName PortName;	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SLMechatronics", meta=(Categories="SLMechatronics.Domain"))
	FGameplayTag Domain;
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SLMechatronics", meta=(ShowOnlyInnerProperties))
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	FSLMData DefaultData;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	FSLMPortLocationData PortLocationData;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	int32 PortIndex = -1;
};