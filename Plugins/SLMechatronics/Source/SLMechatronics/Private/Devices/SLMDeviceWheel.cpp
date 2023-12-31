// Copyright Spectrelight Studios, LLC

#include "Devices/SLMDeviceWheel.h"

FSLMDeviceModelWheel USLMDeviceComponentWheel::GetDeviceState() const
{
	return Subsystem->GetDeviceState(DeviceIndex);
}

void USLMDeviceComponentWheel::BeginPlay()
{
	Super::BeginPlay();
	const AActor* OwningActor = GetOwner();
	DeviceSettings.Port_Rotation_Drive.PortMetaData.AssociatedActor = OwningActor;
	DeviceSettings.Port_Signal_Brake.PortMetaData.AssociatedActor = OwningActor;
	DeviceSettings.Port_Signal_Steer.PortMetaData.AssociatedActor = OwningActor;
	
	Subsystem = GetWorld()->GetSubsystem<USLMDeviceSubsystemWheel>();
	DeviceIndex = Subsystem->AddDevice(DeviceSettings);
}

void USLMDeviceComponentWheel::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Subsystem->RemoveDevice(DeviceIndex);
	Super::EndPlay(EndPlayReason);
}

void USLMDeviceSubsystemWheel::OnWorldBeginPlay(UWorld& InWorld)
{
	DomainRotation = GetWorld()->GetSubsystem<USLMDomainRotation>();
	DomainSignal = GetWorld()->GetSubsystem<USLMDomainSignal>();
	//HitResultBuffer.Reserve(128);
	Super::OnWorldBeginPlay(InWorld);
}

void USLMDeviceSubsystemWheel::PreSimulate(float DeltaTime)
{
	for (auto& Wheel:DeviceModels)
	{
		if (Wheel.Collider)
		{
			Wheel.WheelMass = Wheel.Collider->GetMass();
			Wheel.ImpulseBudget = Wheel.NormalImpulseMagnitude * Wheel.FrictionCoefficient;
			Wheel.DirectionWheelAxis = Wheel.Collider->GetRightVector();
			//Wheel.DirectionWheelAxis = FVector(0,1,0);
			Wheel.DirectionLong = FVector::CrossProduct(Wheel.DirectionWheelAxis, Wheel.ContactPatchNormal);
			Wheel.DirectionLat = FVector::CrossProduct(Wheel.ContactPatchNormal, Wheel.DirectionLong);
			Wheel.Velocity = Wheel.Collider->GetComponentVelocity();
			//Wheel.Velocity = FVector(100,0,0);
		}
	}
}

void USLMDeviceSubsystemWheel::Simulate(float DeltaTime)
{
	for (auto& Wheel:DeviceModels)
	{
		const auto [AngVel,WheelMOI] = DomainRotation->GetData(Wheel.Index_Mech_Drive);
		//const auto SteerSignal = Subsystem->GetNetworkData(SteerIndex).Capacity;
		//const auto BrakeSignal = Subsystem->GetNetworkData(BrakeIndex).Capacity;
		//const float WheelRPM = 20;
		
		//Velocities
		Wheel.SlipVelocityWorld = FVector::VectorPlaneProject((Wheel.DirectionLong * Wheel.Radius * AngVel - Wheel.Velocity), Wheel.ContactPatchNormal);
		const FVector DesiredImpulse = Wheel.SlipVelocityWorld * Wheel.WheelMass;
		const FVector OutputImpulse = DesiredImpulse.GetClampedToMaxSize(Wheel.ImpulseBudget);
		//Wheel.DirectionWheelAxis = OutputImpulse;
		if (Wheel.Collider)
		{
			Wheel.Collider->AddImpulse(OutputImpulse);	
		}
		const FVector DrawDebugStartPoint = Wheel.ContactPatchLocation + FVector(0,0,120);
		DrawDebugLine(GetWorld(), DrawDebugStartPoint, DrawDebugStartPoint + Wheel.SlipVelocityWorld, FColor::Green, false, -1, 0, 5);
		DrawDebugLine(GetWorld(), DrawDebugStartPoint, DrawDebugStartPoint + Wheel.ContactPatchNormal * Wheel.NormalImpulseMagnitude * 0.1, FColor::Blue, false, -1, 0, 5);
		DrawDebugLine(GetWorld(), DrawDebugStartPoint, DrawDebugStartPoint + OutputImpulse * 0.1, FColor::Red, false, -1, 0, 5);

		const float AngImpulse = FVector::DotProduct(OutputImpulse, Wheel.DirectionLong) * Wheel.Radius * Wheel.TestImpulseMultiplier;
		const float WheelRPMOut = AngVel + AngImpulse / WheelMOI;
		DomainRotation->SetAngularVelocity(Wheel.Index_Mech_Drive, WheelRPMOut);
	}
}

void USLMDeviceSubsystemWheel::PostSimulate(float DeltaTime)
{
}

int32 USLMDeviceSubsystemWheel::AddDevice(FSLMDeviceWheel Device)
{
	Device.DeviceModel.Index_Mech_Drive = DomainRotation->AddPort(Device.Port_Rotation_Drive);
	Device.DeviceModel.Index_Signal_Brake = DomainSignal->AddPort(Device.Port_Signal_Brake);
	Device.DeviceModel.Index_Signal_Steer = DomainSignal->AddPort(Device.Port_Signal_Steer);
	//Device.DeviceModel.Collider->OnComponentHit.AddUniqueDynamic(this, &ThisClass::)
	return DeviceModels.Add(Device.DeviceModel);
}

void USLMDeviceSubsystemWheel::RemoveDevice(const int32 DeviceIndex)
{
	DomainRotation->RemovePort(DeviceModels[DeviceIndex].Index_Mech_Drive);
	DomainSignal->RemovePort(DeviceModels[DeviceIndex].Index_Signal_Brake);
	DomainSignal->RemovePort(DeviceModels[DeviceIndex].Index_Signal_Steer);

	DeviceModels.RemoveAt(DeviceIndex);
}

FSLMDeviceModelWheel USLMDeviceSubsystemWheel::GetDeviceState(const int32 DeviceIndex)
{
	return DeviceModels[DeviceIndex];
}

void USLMDeviceSubsystemWheel::SendHitData(const int32 DeviceIndex, UPrimitiveComponent* Primitive, FVector Location, FVector Normal, FVector NormalImpulse)
{
	FSLMDeviceModelWheel Wheel = DeviceModels[DeviceIndex];
	Wheel.Collider = Primitive;
	Wheel.ContactPatchLocation = Location;
	Wheel.ContactPatchNormal = Normal;
	Wheel.NormalImpulseMagnitude = NormalImpulse.Length();
	DeviceModels[DeviceIndex] = Wheel;
}

/*
void USLMDeviceSubsystemWheel::AddHitResultToBuffer(FHitResult NewHitResult)
{
	HitResultBuffer.Add(NewHitResult);
}

void USLMDeviceSubsystemWheel::ProcessHitResult(FHitResult HitResult)
{
	const auto HitComponent = HitResult.Component;
	if (PrimitiveToIndex.Contains(HitComponent))
	{
		const int32 ModelIndex = PrimitiveToIndex.FindRef(HitComponent);
		DeviceModels[ModelIndex].Impu
	}
}
*/
