// Copyright Spectrelight Studios, LLC
/*
#pragma once

#include "CoreMinimal.h"
#include "SLMDeviceBase.h"
#include "Domains/SLMDomainRotation.h"
#include "SLMDeviceDoubleDifferential.generated.h"

class USLMDeviceSubsystemDoubleDifferential;

USTRUCT(BlueprintType)
struct FSLMDeviceModelDoubleDifferential
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	float DriveRatio = 1.0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	float SteerRatio = 1.0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	int32 Index_Rotation_Drive = -1;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	int32 Index_Rotation_Steer = -1;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	int32 Index_Rotation_Left = -1;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	int32 Index_Rotation_Right = -1;
};


USTRUCT(BlueprintType)
struct FSLMDeviceDoubleDifferential
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	FSLMDeviceModelDoubleDifferential DeviceModel;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	FSLMPortRotation Port_Rotation_Drive;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	FSLMPortRotation Port_Rotation_Steer;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	FSLMPortRotation Port_Rotation_Left;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SLMechatronics")
	FSLMPortRotation Port_Rotation_Right;
};


UCLASS(ClassGroup=("SLMechatronics"), meta=(BlueprintSpawnableComponent))
class SLMECHATRONICS_API USLMDeviceComponentDoubleDifferential : public USLMDeviceComponentBase
{
	GENERATED_BODY()
	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	USLMDeviceSubsystemDoubleDifferential* Subsystem;	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SLMechatronics")
	FSLMDeviceDoubleDifferential DeviceSettings;

	UFUNCTION(BlueprintCallable, Category = "SLMechatronics")
	FSLMDeviceModelDoubleDifferential GetDeviceState();
	
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};


UCLASS()
class SLMECHATRONICS_API USLMDeviceSubsystemDoubleDifferential : public USLMDeviceSubsystemBase
{
	GENERATED_BODY()
public:
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	virtual void PreSimulate(const float DeltaTime) override;
	virtual void Simulate(const float DeltaTime) override;
	virtual void PostSimulate(const float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "SLMechatronics")
	int32 AddDevice(FSLMDeviceDoubleDifferential Device);
	UFUNCTION(BlueprintCallable, Category = "SLMechatronics")
	void RemoveDevice(const int32 DeviceIndex);
	UFUNCTION(BlueprintCallable, Category = "SLMechatronics")
	FSLMDeviceModelDoubleDifferential GetDeviceState(const int32 DeviceIndex);
	
private:
	TWeakObjectPtr<USLMDomainRotation> DomainRotation;
	TSparseArray<FSLMDeviceModelDoubleDifferential> DeviceModels;
	TSparseArray<USLMDeviceComponentDoubleDifferential*> DeviceComponents;
};
*/
