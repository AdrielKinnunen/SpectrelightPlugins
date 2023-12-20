// Copyright Spectrelight Studios, LLC
/*
#include "Devices/SLMDeviceWheel.h"

void USLMDeviceSubsystemWheel::OnWorldBeginPlay(UWorld& InWorld)
{
	DomainMech = GetWorld()->GetSubsystem<USLMDomainMech>();
	Super::OnWorldBeginPlay(InWorld);
}

void USLMDeviceSubsystemWheel::PreSimulate(float DeltaTime)
{
	for (auto& [WheelRadiuscm, FrictionCoefficient, ContactPatchLocation, ContactPatchNormal, NormalImpulseMagnitude, WheelMass, ImpulseBudget, DirectionWheelAxis, DirectionLong, DirectionLat, WheelVelocity, SlipVelocityWorld, SlipVelocityLocal, ContactPatchOrientation, Index_Mech_Drive] :Instances)
	{
		ImpulseBudget = NormalImpulseMagnitude * FrictionCoefficient;
		//Wheel.DirectionWheelAxis = Wheel.WheelColliderComponent->GetRightVector();
		DirectionWheelAxis = FVector(0,1,0);
		DirectionLong = FVector::CrossProduct(DirectionWheelAxis, ContactPatchNormal);
		DirectionLat = FVector::CrossProduct(ContactPatchNormal, DirectionLong);
		//WheelVelocity = WheelColliderComponent->GetComponentVelocity();
		WheelVelocity = FVector(100,0,0);
	}
}

void USLMDeviceSubsystemWheel::Simulate(float DeltaTime)
{
	for (auto& [WheelRadiuscm, FrictionCoefficient, ContactPatchLocation, ContactPatchNormal, NormalImpulseMagnitude, WheelMass, ImpulseBudget, DirectionWheelAxis, DirectionLong, DirectionLat, WheelVelocity, SlipVelocityWorld, SlipVelocityLocal, ContactPatchOrientation, Index_Mech_Drive] :Instances)
	{
		const auto [WheelRPM,WheelMOI] = DomainMech->GetNetworkData(Index_Mech_Drive);
		//const auto SteerSignal = Subsystem->GetNetworkData(SteerIndex).Capacity;
		//const auto BrakeSignal = Subsystem->GetNetworkData(BrakeIndex).Capacity;
		//const float WheelRPM = 20;
		
		//Velocities
		SlipVelocityWorld = FVector::VectorPlaneProject((DirectionLong * WheelRadiuscm * WheelRPM - WheelVelocity), ContactPatchNormal);
		const FVector DesiredImpulse = SlipVelocityWorld * WheelMass;
		const FVector OutputImpulse = DesiredImpulse.GetClampedToMaxSize(ImpulseBudget);
		DirectionWheelAxis = OutputImpulse;
		//WheelColliderComponent->AddImpulse(OutputImpulse);

		//DrawDebugLine(GetWorld(), ContactPatchLocation, ContactPatchLocation + SlipVelocityWorld, FColor::Green, false, -1, 0, 5);
		//DrawDebugLine(GetWorld(), ContactPatchLocation, ContactPatchLocation + ContactPatchNormal * 300, FColor::Blue, false, -1, 0, 5);
		//DrawDebugLine(GetWorld(), ContactPatchLocation, ContactPatchLocation + OutputImpulse, FColor::Red, false, -1, 0, 5);
	
		const float WheelRPMOut = WheelRPM;

		const FSLMDataMech Shaft1 = FSLMDataMech(WheelRPMOut, WheelMOI);
		
		DomainMech->SetNetworkData(Index_Mech_Drive, Shaft1);
	}
}

void USLMDeviceSubsystemWheel::PostSimulate(float DeltaTime)
{
}

void USLMDeviceSubsystemWheel::AddInstance(const FSLMDeviceModelWheel& Instance)
{
	Instances.Add(Instance);
}

USLMDeviceComponentWheel::USLMDeviceComponentWheel()
{
	PrimaryComponentTick.bCanEverTick = false;
	DomainMech = nullptr;
}

void USLMDeviceComponentWheel::BeginPlay()
{
	Super::BeginPlay();
	DomainMech = GetWorld()->GetSubsystem<USLMDomainMech>();
	DeviceModel.Index_Mech_Drive = DomainMech->AddPort(Port_Mech_Drive);
	//DeviceModel.Index_Signal_Steer = Subsystem->AddPort(Port_Signal_Steer);
	//DeviceModel.Index_Signal_Brake = Subsystem->AddPort(Port_Signal_Brake);
	GetWorld()->GetSubsystem<USLMDeviceSubsystemWheel>()->AddInstance(DeviceModel);
}

void USLMDeviceComponentWheel::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	DomainMech->RemovePort(DeviceModel.Index_Mech_Drive);
	//GetWorld()->GetSubsystem<USLMDeviceSubsystemWheel>()
	Super::EndPlay(EndPlayReason);
}

/*
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
#1#
*/
