// Fill out your copyright notice in the Description page of Project Settings.


#include "DeviceComponents/SLMDeviceGearBox.h"

#include "SLMechatronicsGameplayTags.h"
#include "SLMechatronicsSubsystem.h"


// Sets default values for this component's properties
USLMDeviceGearBox::USLMDeviceGearBox()
{
	PrimaryComponentTick.bCanEverTick = false;
	
	Ports = {
		FSLMPort(TAG_SLMechatronics_Domain_Drivetrain, FSLMData(0, 1)),
		FSLMPort(TAG_SLMechatronics_Domain_Drivetrain, FSLMData(0, 1))
	};
	GearRatio = 1;
}


void USLMDeviceGearBox::Simulate(float DeltaTime)
{
	//From BP Math
	const auto [FirstShaftAngularVelocity,FirstShaftMOI] = Subsystem->GetNetworkData(FirstShaftIndex);
	const auto [SecondShaftAngularVelocity,SecondShaftMOI] = Subsystem->GetNetworkData(SecondShaftIndex);

	
	const float EffectiveFirstShaftMOI = GearRatio * GearRatio * FirstShaftMOI;
	const float EffectiveFirstShaftAngularVelocity = FirstShaftAngularVelocity / GearRatio;
	const float SecondShaftAngularVelocityOut = (EffectiveFirstShaftAngularVelocity * EffectiveFirstShaftMOI + SecondShaftAngularVelocity * SecondShaftMOI) / (EffectiveFirstShaftMOI + SecondShaftMOI);
	const float FirstShaftAngularVelocityOut = SecondShaftAngularVelocityOut * GearRatio;

	Subsystem->SetNetworkValue(FirstShaftIndex, FirstShaftAngularVelocityOut);
	Subsystem->SetNetworkValue(SecondShaftIndex, SecondShaftAngularVelocityOut);
	
	Super::Simulate(DeltaTime);
}

// Called when the game starts
void USLMDeviceGearBox::BeginPlay()
{
	Super::BeginPlay();
	FirstShaftIndex = Ports[0].PortIndex;
	SecondShaftIndex = Ports[1].PortIndex;
}

