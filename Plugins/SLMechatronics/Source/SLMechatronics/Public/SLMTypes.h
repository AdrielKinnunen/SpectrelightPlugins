// Copyright Spectrelight Studios, LLC. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "StructUtils/InstancedStruct.h"
#include "SLMTypes.generated.h"


class USLMDomainSubsystemBase;
class USLMDeviceSubsystemBase;





USTRUCT(BlueprintType)
struct FSLMDeviceAddress
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	TSubclassOf<USLMDeviceSubsystemBase> DeviceClass;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	int32 DeviceID = INDEX_NONE;
	
	FString GetDebugString() const;
	bool IsValid() const;
};
uint32 GetTypeHash(const FSLMDeviceAddress& Address);
bool operator==(const FSLMDeviceAddress& A, const FSLMDeviceAddress& B);
FSLMDeviceAddress MakeDeviceAddress(const USLMDeviceSubsystemBase* Device, const int32 DeviceID);



USTRUCT(BlueprintType)
struct FSLMPortAddress
{
	GENERATED_BODY()
	
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	//TSubclassOf<USLMDeviceSubsystemBase> DeviceClass;
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	//int32 DeviceID = INDEX_NONE;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	FSLMDeviceAddress DeviceAddress;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	TSubclassOf<USLMDomainSubsystemBase> DomainClass;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	int32 PortID = INDEX_NONE;
	
	FString GetDebugString() const;
	bool IsValid() const;
};
uint32 GetTypeHash(const FSLMPortAddress& Address);
bool operator==(const FSLMPortAddress& A, const FSLMPortAddress& B);
FSLMPortAddress MakePortAddress(const USLMDeviceSubsystemBase* Device, const USLMDomainSubsystemBase* Domain, const int32 DeviceID, const int32 PortID);


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
uint32 GetTypeHash(const FSLMConnection& Connection);
bool operator==(const FSLMConnection& A, const FSLMConnection& B);



USTRUCT(BlueprintType)
struct FSLMPortMetaData
{
    GENERATED_BODY()

	FSLMPortMetaData()
    {
		DeviceName = NAME_None;
		PortName = NAME_None;
		SceneComponentName = NAME_None;
		SocketName = NAME_None;
		OffsetLocal = FVector::ZeroVector;
		AssociatedActor = nullptr;
		AssociatedSceneComponent = nullptr;
	}


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	FName DeviceName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	FName PortName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	FName SceneComponentName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
    FName SocketName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	FVector OffsetLocal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	const AActor* AssociatedActor;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "SLMechatronics")
	const USceneComponent* AssociatedSceneComponent;
};



USTRUCT()
struct FSLMDeviceSnapshot
{
	GENERATED_BODY()

	UPROPERTY()
	FSLMDeviceAddress Address;

	UPROPERTY()
	FInstancedStruct Descriptor;
};