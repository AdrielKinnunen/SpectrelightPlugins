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
	const auto [B,M] = Subsystem->GetNetworkData(InputShaftIndex);
	const auto [C,N] = Subsystem->GetNetworkData(LeftShaftIndex);
	const auto [D,O] = Subsystem->GetNetworkData(RightShaftIndex);
	
	const float Divisor = M*N + M*O + 4*N*O;

	const float LeftShaftVel_Out = (-2*B*M*O + C*M*N + 4*C*N*O + D*M*O) / Divisor;
	const float RightShaftVel_Out = (2*B*M*N + C*M*N + D*M*O + 4*D*N*O) / Divisor;
	const float InputShaftVel_Out = 0.5 * (RightShaftVel_Out - LeftShaftVel_Out);

	Subsystem->SetNetworkValue(LeftShaftIndex, LeftShaftVel_Out);
	Subsystem->SetNetworkValue(RightShaftIndex, RightShaftVel_Out);
	Subsystem->SetNetworkValue(InputShaftIndex, InputShaftVel_Out);
	
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