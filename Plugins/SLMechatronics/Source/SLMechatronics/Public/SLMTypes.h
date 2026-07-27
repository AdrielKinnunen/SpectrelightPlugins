// Copyright Spectrelight Studios, LLC. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "StructUtils/InstancedStruct.h"
#include "SLMTypes.generated.h"


class FSLMDomainSystemBase;
class FSLMDeviceSystemBase;


USTRUCT(BlueprintType)
struct FSLMDeviceAddress
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	FGameplayTag DeviceTag;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	int32 DeviceID = INDEX_NONE;
	
	FString GetDebugString() const;
	bool IsValid() const;
};
SLMECHATRONICS_API uint32 GetTypeHash(const FSLMDeviceAddress& Address);
SLMECHATRONICS_API bool operator==(const FSLMDeviceAddress& A, const FSLMDeviceAddress& B);
SLMECHATRONICS_API FSLMDeviceAddress MakeDeviceAddress(const FSLMDeviceSystemBase* DeviceSystem, const int32 DeviceID);





USTRUCT(BlueprintType)
struct FSLMPortAddress
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	FSLMDeviceAddress DeviceAddress;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	FGameplayTag DomainTag;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	int32 PortID = INDEX_NONE;
	
	FString GetDebugString() const;
	bool IsValid() const;
};
SLMECHATRONICS_API uint32 GetTypeHash(const FSLMPortAddress& Address);
SLMECHATRONICS_API bool operator==(const FSLMPortAddress& A, const FSLMPortAddress& B);
SLMECHATRONICS_API FSLMPortAddress MakePortAddress(const FSLMDeviceSystemBase* DeviceSystem, const int32 DeviceID, const FSLMDomainSystemBase* DomainSystem, const int32 PortID);





USTRUCT(BlueprintType)
struct FSLMConnection
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	FSLMPortAddress First;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	FSLMPortAddress Second;
	
	FString GetDebugString() const;
	bool IsValid() const;
};
SLMECHATRONICS_API uint32 GetTypeHash(const FSLMConnection& Connection);
SLMECHATRONICS_API bool operator==(const FSLMConnection& A, const FSLMConnection& B);







USTRUCT(BlueprintType)
struct SLMECHATRONICS_API FSLMSpatialContextAuthored
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	FName SceneComponentName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	FName SocketName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	FVector OffsetLocal;

	FString GetDebugString() const;
	uint32 GetDebugHash(const bool bVerbose) const;
};

USTRUCT(BlueprintType)
struct SLMECHATRONICS_API FSLMSpatialContextRuntime
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	TWeakObjectPtr<AActor> AssociatedActor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	TWeakObjectPtr<USceneComponent> AssociatedSceneComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	FName SocketName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	FVector OffsetLocal;

	FTransform GetWorldTransform() const;
	static FSLMSpatialContextRuntime MakeFromContextAuthored(const FSLMSpatialContextAuthored& ContextAuthored, AActor* AssociatedActor);
};



USTRUCT(BlueprintType)
struct FSLMDeviceSnapshot
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FName DeviceComponentName;
	UPROPERTY(BlueprintReadOnly)
	FSLMDeviceAddress DeviceAddress;
	UPROPERTY(BlueprintReadOnly)
	FInstancedStruct DeviceDescriptor;
};