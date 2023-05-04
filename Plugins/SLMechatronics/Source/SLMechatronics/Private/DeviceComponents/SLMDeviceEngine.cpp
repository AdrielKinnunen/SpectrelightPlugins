// Fill out your copyright notice in the Description page of Project Settings.


#include "DeviceComponents/SLMDeviceEngine.h"

#include "SLMechatronicsGameplayTags.h"
#include "SLMechatronicsSubsystem.h"

// Sets default values for this component's properties
USLMDeviceEngine::USLMDeviceEngine()
{
	PrimaryComponentTick.bCanEverTick = false;
	
	Ports = {
		//FSLMPort(TAG_SLMechatronics_Domain_Drivetrain, FSLMData(0, 1)),
		FSLMPort(TAG_SLMechatronics_Domain_Drivetrain, FSLMData(0, 1))
	};
	//GearRatio = 1;
}


void USLMDeviceEngine::Simulate(float DeltaTime)
{
	
	//Value = RPM
	const auto [CrankRPM,CrankMOI] = Subsystem->GetNetworkData(CrankIndex);
	
	const float CrankMomentum = CrankRPM * CrankMOI;
	const float Torque = MaxTorque * Throttle * (CrankRPM < MaxRPM) - 0.2 * CrankRPM;

	const float CrankMomentumOut = CrankMomentum + Torque * DeltaTime;
	const float CrankRPMOut = CrankMomentumOut / CrankMOI;

	Subsystem->SetNetworkValue(CrankIndex, CrankRPMOut);
	
	
	
	
	
	
	
	
	/*    Value = Momentum
	const auto [CrankMomentum,CrankMOI] = Subsystem->GetNetworkData(CrankIndex);

	const float RPM = CrankMomentum / CrankMOI;
	const float Torque = MaxTorque * Throttle * (RPM < MaxRPM) - 0.1 * RPM;

	const float CrankMomentumOut = CrankMomentum + Torque * DeltaTime;

	Subsystem->SetNetworkValue(CrankIndex, CrankMomentumOut);
	*/
	Super::Simulate(DeltaTime);
}


// Called when the game starts
void USLMDeviceEngine::BeginPlay()
{
	Super::BeginPlay();
	CrankIndex = Ports[0].PortIndex;
}

