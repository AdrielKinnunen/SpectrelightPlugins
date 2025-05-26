// Copyright Spectrelight Studios, LLC

#include "Devices/SLMDeviceWheel.h"

FSLMDeviceModelWheel USLMDeviceComponentWheel::GetDeviceState()
{
    return Subsystem->GetDeviceState(DeviceIndex);
}

FSLMDeviceCosmeticsWheel USLMDeviceComponentWheel::GetDeviceCosmetics()
{
    return Subsystem->GetDeviceCosmetics(DeviceIndex);
}

void USLMDeviceComponentWheel::SendHitData(UPrimitiveComponent* Primitive, FVector Location, FVector Normal, FVector NormalImpulse)
{
    Subsystem->SendHitData(DeviceIndex, Primitive, Location, Normal, NormalImpulse);
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

void USLMDeviceComponentWheel::EndPlay(EEndPlayReason::Type EndPlayReason)
{
    Subsystem->RemoveDevice(DeviceIndex);
    Super::EndPlay(EndPlayReason);
}

void USLMDeviceSubsystemWheel::OnWorldBeginPlay(UWorld& InWorld)
{
    DomainRotation = GetWorld()->GetSubsystem<USLMDomainRotation>();
    DomainSignal = GetWorld()->GetSubsystem<USLMDomainSignal>();
    //HitResultBuffer.Reserve(128);
	//TestThingCallback = GetWorld()->GetPhysicsScene()->GetSolver()->CreateAndRegisterSimCallbackObject_External<FTestThingCallback>();

	Super::OnWorldBeginPlay(InWorld);

}

void USLMDeviceSubsystemWheel::PreSimulate(float DeltaTime)
{
	SCOPED_NAMED_EVENT(SLMWheelPreSimulate, FColor::Green);

    for (auto& Wheel : DeviceModels)
    {
        if (Wheel.Collider)
        {
            const auto SteerSignal = FMath::Clamp(DomainSignal->ReadByPortIndex(Wheel.Index_Signal_Steer), -1, 1);
            Wheel.SteerAngle = FMath::FInterpConstantTo(Wheel.SteerAngle, Wheel.MaxSteerAngle * SteerSignal, DeltaTime, Wheel.SteerRate);

        	Wheel.DirectionWheelAxis = Wheel.Collider->GetRightVector().RotateAngleAxis(Wheel.SteerAngle, Wheel.Collider->GetUpVector());
            Wheel.DirectionLong = FVector::CrossProduct(Wheel.DirectionWheelAxis, Wheel.ContactPatchNormal);
            Wheel.DirectionLat = FVector::CrossProduct(Wheel.ContactPatchNormal, Wheel.DirectionLong);
        	Wheel.Velocity = Wheel.Collider->GetComponentVelocity();
        	Wheel.WheelMass = Wheel.Collider->GetMass();
        	const float GripScalar = 1 - FMath::Abs(FVector::DotProduct(Wheel.DirectionWheelAxis, Wheel.ContactPatchNormal));
        	//UE_LOG(LogTemp, Warning, TEXT("GripScalar is %f"), GripScalar);

            Wheel.ImpulseBudget = Wheel.NormalImpulseMagnitude * Wheel.FrictionCoefficient * GripScalar;

            Wheel.ImpulseAccumulator = FVector::ZeroVector;
        }
    }
}

void USLMDeviceSubsystemWheel::Simulate(float DeltaTime, float SubstepScalar)
{
	SCOPED_NAMED_EVENT(SLMWheelSimulate, FColor::Green);

    for (auto& Wheel : DeviceModels)
    {
    	switch (Wheel.TireModel)
    	{
    	case ETireModel::StaticFriction:
    		DoStaticFriction(Wheel, DeltaTime, SubstepScalar);
    		break; 
    	case ETireModel::Pacejka:
    		DoPacejka(Wheel, DeltaTime, SubstepScalar);
    		break; 
    	case ETireModel::Brush:
    		DoBrush(Wheel, DeltaTime, SubstepScalar);
    		break; 
    	default: break; 
    	}
    }
}

void USLMDeviceSubsystemWheel::PostSimulate(float DeltaTime)
{
	SCOPED_NAMED_EVENT(SLMWheelPostSimulate, FColor::Green);

    for (auto It = DeviceModels.CreateIterator(); It; ++It)
    {
    	DeviceCosmetics[It.GetIndex()].AngularVelocityDegrees = FMath::RadiansToDegrees(DomainRotation->GetData(It->Index_Mech_Drive).AngularVelocity);
    	DeviceCosmetics[It.GetIndex()].Load = It->NormalImpulseMagnitude / DeltaTime;
    	const float SlipSpeed = It->SlipSpeed;
    	const float Speed = It->Velocity.Size();
    	DeviceCosmetics[It.GetIndex()].SlipRatio = SlipSpeed / (Speed + 100);
    	
        if (It->Collider)
        {
            const FVector Impulse = It->ImpulseAccumulator;
            const FVector AngularImpulse = It->DirectionWheelAxis * FVector::DotProduct(It->DirectionLong, Impulse) * -1;
            //if (It->Collider->GetNetMode() != NM_Client)
            {
            	It->Collider->AddImpulseAtLocation(Impulse, It->ContactPatchLocation);
            	It->Collider->AddAngularImpulseInRadians(AngularImpulse);
            }

            
            //Debug
            //const FVector DrawDebugStartPoint = It->ContactPatchLocation + It->ContactPatchNormal * 120;
            //DrawDebugLine(GetWorld(), DrawDebugStartPoint, DrawDebugStartPoint + It->ContactPatchNormal * It->NormalImpulseMagnitude * 0.1, FColor::Blue, false, -1, 0, 5);
            //DrawDebugLine(GetWorld(), DrawDebugStartPoint, DrawDebugStartPoint + It->ImpulseAccumulator * 0.1, FColor::Red, false, -1, 0, 5);

            It->NormalImpulseMagnitude = 0;
        }
    }
}

int32 USLMDeviceSubsystemWheel::AddDevice(FSLMDeviceWheel Device)
{
    Device.DeviceModel.Index_Mech_Drive = DomainRotation->AddPort(Device.Port_Rotation_Drive);
    Device.DeviceModel.Index_Signal_Brake = DomainSignal->AddPort(Device.Port_Signal_Brake);
    Device.DeviceModel.Index_Signal_Steer = DomainSignal->AddPort(Device.Port_Signal_Steer);
    //Device.DeviceModel.Collider->OnComponentHit.AddUniqueDynamic(this, &ThisClass::)
    DeviceCosmetics.Add(FSLMDeviceCosmeticsWheel());
    return DeviceModels.Add(Device.DeviceModel);
}

void USLMDeviceSubsystemWheel::RemoveDevice(int32 DeviceIndex)
{
    DomainRotation->RemovePort(DeviceModels[DeviceIndex].Index_Mech_Drive);
    DomainSignal->RemovePort(DeviceModels[DeviceIndex].Index_Signal_Brake);
    DomainSignal->RemovePort(DeviceModels[DeviceIndex].Index_Signal_Steer);

    DeviceCosmetics.RemoveAt(DeviceIndex);
    DeviceModels.RemoveAt(DeviceIndex);
}

FSLMDeviceModelWheel USLMDeviceSubsystemWheel::GetDeviceState(int32 DeviceIndex)
{
    return DeviceModels[DeviceIndex];
}

FSLMDeviceCosmeticsWheel USLMDeviceSubsystemWheel::GetDeviceCosmetics(int32 DeviceIndex)
{
    return DeviceCosmetics[DeviceIndex];
}

void USLMDeviceSubsystemWheel::SendHitData(int32 DeviceIndex, UPrimitiveComponent* Primitive, FVector Location, FVector Normal, FVector NormalImpulse)
{
    FSLMDeviceModelWheel Wheel = DeviceModels[DeviceIndex];
    Wheel.Collider = Primitive;
    Wheel.ContactPatchLocation = Location;
    Wheel.ContactPatchNormal = Normal;
    Wheel.NormalImpulseMagnitude = NormalImpulse.Length();
    DeviceModels[DeviceIndex] = Wheel;
}

void USLMDeviceSubsystemWheel::DoStaticFriction(FSLMDeviceModelWheel& Wheel, float DeltaTime, float SubstepScalar)
{
	auto AngVel = DomainRotation->GetData(Wheel.Index_Mech_Drive).AngularVelocity;
	const auto WheelMOI = DomainRotation->GetData(Wheel.Index_Mech_Drive).MomentOfInertia;
	const auto BrakeSignal = FMath::Clamp(DomainSignal->ReadByPortIndex(Wheel.Index_Signal_Brake), 0, 1);

	//Brakes
	const float MaxBrakeAngularImpulse = Wheel.BrakeMaxTorque * BrakeSignal * DeltaTime;
	const float BrakeImpulseToStop = -1 * AngVel * WheelMOI;
	const float BrakeImpulseClamped = FMath::Clamp(BrakeImpulseToStop, -MaxBrakeAngularImpulse, MaxBrakeAngularImpulse);
	AngVel += BrakeImpulseClamped / WheelMOI;

	//Grip
	const float DesiredAngVel = FVector::DotProduct(Wheel.Velocity, Wheel.DirectionLong) / Wheel.Radius;
	const float AngularImpulseToStop = (DesiredAngVel - AngVel) * WheelMOI;
	const float LinearImpulseMaxSizeThisSubstep = FMath::Abs(AngularImpulseToStop * 10000 / Wheel.Radius);

	const float SlipSpeedLong = Wheel.Radius * AngVel - FVector::DotProduct(Wheel.Velocity, Wheel.DirectionLong);
	FVector ImpulseToStopLong = SlipSpeedLong * Wheel.WheelMass * Wheel.DirectionLong * SubstepScalar;
	const FVector ImpulseToStopLongClamped = ImpulseToStopLong.GetClampedToMaxSize(LinearImpulseMaxSizeThisSubstep);

	const float SlipSpeedLat = -1 * FVector::DotProduct(Wheel.Velocity, Wheel.DirectionLat);
	const FVector ImpulseToStopLat = SlipSpeedLat * Wheel.WheelMass * Wheel.DirectionLat * SubstepScalar;

	const FVector SlipVelocity = FVector(SlipSpeedLong, SlipSpeedLat * 0.1, 0);
	Wheel.SlipSpeed = SlipVelocity.Size();

	const FVector ImpulseToStop = ImpulseToStopLat + ImpulseToStopLongClamped;


	const float RemainingImpulseBudget = Wheel.ImpulseBudget - Wheel.ImpulseAccumulator.Length();
	const FVector ActualImpulse = ImpulseToStop.GetClampedToMaxSize(RemainingImpulseBudget);
	Wheel.ImpulseAccumulator += ActualImpulse;
	const float GripAngularImpulse = -0.0001 * FVector::DotProduct(ActualImpulse, Wheel.DirectionLong) * Wheel.Radius;

	//const float TotalImpulse = GripAngularImpulse + BrakeImpulseClamped;


	AngVel += GripAngularImpulse / WheelMOI;

	DomainRotation->SetAngularVelocity(Wheel.Index_Mech_Drive, AngVel);
}

void USLMDeviceSubsystemWheel::DoPacejka(FSLMDeviceModelWheel& Wheel, float DeltaTime, float SubstepScalar)
{
	auto AngVel = DomainRotation->GetData(Wheel.Index_Mech_Drive).AngularVelocity;


	
	const float SlipRatio = Wheel.Radius * AngVel / FVector::DotProduct(Wheel.Velocity, Wheel.DirectionLong) - 1;

	const float DesiredLongGs = FMath::GetMappedRangeValueClamped(FVector2D(-0.3,0.3),FVector2D(-1,1),SlipRatio);
	const auto DesiredImpulse = Wheel.DirectionLong * DesiredLongGs * Wheel.NormalImpulseMagnitude;

	const float RemainingImpulseBudget = Wheel.ImpulseBudget - Wheel.ImpulseAccumulator.Length();
	const FVector ActualImpulse = DesiredImpulse.GetClampedToMaxSize(RemainingImpulseBudget);
	Wheel.ImpulseAccumulator += ActualImpulse;
	const float GripAngularImpulse = -0.0001 * FVector::DotProduct(ActualImpulse, Wheel.DirectionLong) * Wheel.Radius;

	//const float TotalImpulse = GripAngularImpulse + BrakeImpulseClamped;


	//AngVel += GripAngularImpulse / WheelMOI;

	DomainRotation->SetAngularVelocity(Wheel.Index_Mech_Drive, AngVel);
}

void USLMDeviceSubsystemWheel::DoBrush(FSLMDeviceModelWheel& Wheel, float DeltaTime, float SubstepScalar)
{
	auto AngVel = DomainRotation->GetData(Wheel.Index_Mech_Drive).AngularVelocity;
	const auto WheelMOI = DomainRotation->GetData(Wheel.Index_Mech_Drive).MomentOfInertia;
	const auto BrakeSignal = FMath::Clamp(DomainSignal->ReadByPortIndex(Wheel.Index_Signal_Brake), 0, 1);

	//Brakes
	const float MaxBrakeAngularImpulse = Wheel.BrakeMaxTorque * BrakeSignal * DeltaTime;
	const float BrakeImpulseToStop = -1 * AngVel * WheelMOI;
	const float BrakeImpulseClamped = FMath::Clamp(BrakeImpulseToStop, -MaxBrakeAngularImpulse, MaxBrakeAngularImpulse);
	AngVel += BrakeImpulseClamped / WheelMOI;

	
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
