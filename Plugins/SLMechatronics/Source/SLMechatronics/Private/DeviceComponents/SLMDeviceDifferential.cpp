// Fill out your copyright notice in the Description page of Project Settings.


#include "DeviceComponents/SLMDeviceDifferential.h"

#include "SLMechatronicsGameplayTags.h"
#include "SLMechatronicsSubsystem.h"

// Sets default values for this component's properties
USLMDeviceDifferential::USLMDeviceDifferential()
{
	PrimaryComponentTick.bCanEverTick = false;
	
	Ports = {
		FSLMPort(TAG_SLMechatronics_Domain_Drivetrain, FSLMData(0, 1)),
		FSLMPort(TAG_SLMechatronics_Domain_Drivetrain, FSLMData(0, 1)),
		FSLMPort(TAG_SLMechatronics_Domain_Drivetrain, FSLMData(0, 1))
	};
	FinalDriveRatio = 1;
}


void USLMDeviceDifferential::Simulate(float DeltaTime)
{
	const auto [ISV,ISM] = Subsystem->GetNetworkData(InputShaftIndex);
	const auto [LSV,LSM] = Subsystem->GetNetworkData(LeftShaftIndex);
	const auto [RSV,RSM] = Subsystem->GetNetworkData(RightShaftIndex);
	
	const float Divisor = ISM*LSM + ISM*RSM + 4*LSM*RSM;

	const float InputShaftVel_Out = (ISV*ISM*LSM + ISV*ISM*RSM + 2*LSV*LSM*RSM + 2*RSV*LSM*RSM) / Divisor;
	const float LeftShaftVel_Out = (2*ISV*ISM*RSM + LSV*ISM*LSM + 4*LSV*LSM*RSM - RSV*ISM*RSM) / Divisor;
	const float RightShaftVel_Out = (2*ISV*ISM*LSM - LSV*ISM*LSM + 4*RSV*LSM*RSM + RSV*ISM*RSM) / Divisor;

	Subsystem->SetNetworkValue(InputShaftIndex, InputShaftVel_Out);
	Subsystem->SetNetworkValue(LeftShaftIndex, LeftShaftVel_Out);
	Subsystem->SetNetworkValue(RightShaftIndex, RightShaftVel_Out);
	
	Super::Simulate(DeltaTime);
}

// Called when the game starts
void USLMDeviceDifferential::BeginPlay()
{
	Super::BeginPlay();
	InputShaftIndex = Ports[0].PortIndex;
	LeftShaftIndex = Ports[1].PortIndex;
	RightShaftIndex = Ports[2].PortIndex;
}