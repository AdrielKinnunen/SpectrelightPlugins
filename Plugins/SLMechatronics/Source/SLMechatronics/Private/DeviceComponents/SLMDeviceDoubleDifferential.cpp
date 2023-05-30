// Fill out your copyright notice in the Description page of Project Settings.


#include "DeviceComponents/SLMDeviceDoubleDifferential.h"

#include "SLMechatronicsGameplayTags.h"
#include "SLMechatronicsSubsystem.h"


// Sets default values for this component's properties
USLMDeviceDoubleDifferential::USLMDeviceDoubleDifferential()
{
	PrimaryComponentTick.bCanEverTick = false;
	
	Ports = {
		FSLMPort(TAG_SLMechatronics_Domain_Drivetrain, FSLMData(0, 1)),
		FSLMPort(TAG_SLMechatronics_Domain_Drivetrain, FSLMData(0, 1)),
		FSLMPort(TAG_SLMechatronics_Domain_Drivetrain, FSLMData(0, 1)),
		FSLMPort(TAG_SLMechatronics_Domain_Drivetrain, FSLMData(0, 1))
	};
	DriveRatio = 1;
}


void USLMDeviceDoubleDifferential::Simulate(float DeltaTime)
{
	const auto [A,L] = Subsystem->GetNetworkData(DriveShaftIndex);
	const auto [B,M] = Subsystem->GetNetworkData(SteerShaftIndex);
	const auto [C,N] = Subsystem->GetNetworkData(LeftShaftIndex);
	const auto [D,O] = Subsystem->GetNetworkData(RightShaftIndex);
	
	const float Divisor = L*(M+N+O) + M*(N+O) + 4*N*O;
	
	const float W = (A*L*(M+N+O) + B*M*(N-O) - C*M*N - 2*C*N*O + D*M*O + 2*D*N*O) / Divisor;
	const float X = (A*L*(N-O) + B*M*(L+N+O) + C*L*N + 2*C*N*O + D*L*O + 2*D*N*O) / Divisor;
	const float Y = X - W;
	const float Z = X + W;

	Subsystem->SetNetworkValue(DriveShaftIndex, W);
	Subsystem->SetNetworkValue(SteerShaftIndex, X);
	Subsystem->SetNetworkValue(LeftShaftIndex, Y);
	Subsystem->SetNetworkValue(RightShaftIndex, Z);
	
	Super::Simulate(DeltaTime);
}

// Called when the game starts
void USLMDeviceDoubleDifferential::BeginPlay()
{
	Super::BeginPlay();
	DriveShaftIndex = Ports[0].PortIndex;
	SteerShaftIndex = Ports[1].PortIndex;
	LeftShaftIndex = Ports[2].PortIndex;
	RightShaftIndex = Ports[3].PortIndex;
}

