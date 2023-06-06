// Fill out your copyright notice in the Description page of Project Settings.


#include "DeviceComponents/SLMDeviceSimpleWheel.h"

#include "SLMechatronicsGameplayTags.h"
#include "SLMechatronicsSubsystem.h"


// Sets default values for this component's properties
USLMDeviceSimpleWheel::USLMDeviceSimpleWheel()
{
	PrimaryComponentTick.bCanEverTick = false;
	
	Ports = {
		FSLMPort(TAG_SLMechatronics_Domain_Drivetrain, FSLMData(0, 5))
		//FSLMPort(TAG_SLMechatronics_Domain_Drivetrain, FSLMData(0, 5)),
		//FSLMPort(TAG_SLMechatronics_Domain_Signal, FSLMData(0, 0)),
		//FSLMPort(TAG_SLMechatronics_Domain_Signal, FSLMData(0, 0))
	};
}


void USLMDeviceSimpleWheel::PreSimulate(float DeltaTime)
{
	ImpulseBudget = NormalImpulseMagnitude * FrictionCoefficient;
	DirectionWheelAxis = WheelColliderComponent->GetRightVector();
	DirectionLong = FVector::CrossProduct(DirectionWheelAxis, ContactPatchNormal);
	DirectionLat = FVector::CrossProduct(ContactPatchNormal, DirectionLong);
	WheelVelocity = WheelColliderComponent->GetComponentVelocity();

	Super::PreSimulate(DeltaTime);
}

void USLMDeviceSimpleWheel::Simulate(float DeltaTime)
{
	const auto [WheelRPM,WheelMOI] = Subsystem->GetNetworkData(DrivetrainIndex);
	//const auto SteerSignal = Subsystem->GetNetworkData(SteerIndex).Capacity;
	//const auto BrakeSignal = Subsystem->GetNetworkData(BrakeIndex).Capacity;
	//const float WheelRPM = 20;
		
	//Velocities
	SlipVelocityWorld = FVector::VectorPlaneProject((DirectionLong * WheelRadiuscm * WheelRPM - WheelVelocity), ContactPatchNormal);
	const FVector DesiredImpulse = SlipVelocityWorld * WheelMass;
	const FVector OutputImpulse = DesiredImpulse.GetClampedToMaxSize(ImpulseBudget);
	WheelColliderComponent->AddImpulse(OutputImpulse);

	DrawDebugLine(GetWorld(), ContactPatchLocation, ContactPatchLocation + SlipVelocityWorld, FColor::Green, false, -1, 0, 5);
	DrawDebugLine(GetWorld(), ContactPatchLocation, ContactPatchLocation + ContactPatchNormal * 300, FColor::Blue, false, -1, 0, 5);
	DrawDebugLine(GetWorld(), ContactPatchLocation, ContactPatchLocation + OutputImpulse, FColor::Red, false, -1, 0, 5);
	
	const float WheelRPMOut = WheelRPM;
	Subsystem->SetNetworkValue(DrivetrainIndex, WheelRPMOut);
	
	Super::Simulate(DeltaTime);
}

void USLMDeviceSimpleWheel::PostSimulate(float DeltaTime)
{
	bIsTouching = false;
	Super::PostSimulate(DeltaTime);
}

void USLMDeviceSimpleWheel::OnComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	bIsTouching = true;
	NormalImpulse.ToDirectionAndLength(ContactPatchNormal, NormalImpulseMagnitude);
	ContactPatchLocation = Hit.Location;
}


// Called when the game starts
void USLMDeviceSimpleWheel::BeginPlay()
{
	Super::BeginPlay();
	DrivetrainIndex = Ports[0].PortIndex;
	//SteerIndex = Ports[0].PortIndex;
	//BrakeIndex = Ports[0].PortIndex;
	
	for(UActorComponent* Component : GetOwner()->GetComponents())
	{
		if(Component->GetFName() == WheelColliderComponentName)
		{
			WheelColliderComponent = Cast<UPrimitiveComponent>(Component);
		}
	}

	if (WheelColliderComponent)
	{
		WheelColliderComponent->OnComponentHit.AddDynamic(this, &USLMDeviceSimpleWheel::OnComponentHit);
		WheelMass = WheelColliderComponent->GetMass();
	}
}

