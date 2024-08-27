// Copyright Spectrelight Studios, LLC

#include "Devices/SLMDevicePropeller.h"


FSLMDeviceModelPropeller USLMDeviceComponentPropeller::GetDeviceState()
{
    return Subsystem->GetDeviceState(DeviceIndex);
}

FSLMDeviceCosmeticsPropeller USLMDeviceComponentPropeller::GetDeviceCosmetics()
{
    return Subsystem->GetDeviceCosmetics(DeviceIndex);
}

void USLMDeviceComponentPropeller::SetPrimitive(UPrimitiveComponent* Primitive)
{
	Subsystem->SetPrimitive(DeviceIndex, Primitive);
}

void USLMDeviceComponentPropeller::BeginPlay()
{
    Super::BeginPlay();
    const AActor* OwningActor = GetOwner();

	//DeviceSettings.DeviceModel.Primitive = OwningActor->GetComponent

	
    DeviceSettings.Port_Rotation_Drive.PortMetaData.AssociatedActor = OwningActor;
	
    Subsystem = GetWorld()->GetSubsystem<USLMDeviceSubsystemPropeller>();
    DeviceIndex = Subsystem->AddDevice(DeviceSettings);
}

void USLMDeviceComponentPropeller::EndPlay(EEndPlayReason::Type EndPlayReason)
{
    Subsystem->RemoveDevice(DeviceIndex);
    Super::EndPlay(EndPlayReason);
}

void USLMDeviceSubsystemPropeller::OnWorldBeginPlay(UWorld& InWorld)
{
    DomainRotation = GetWorld()->GetSubsystem<USLMDomainRotation>();
    Super::OnWorldBeginPlay(InWorld);
}

void USLMDeviceSubsystemPropeller::PreSimulate(float DeltaTime)
{
	SCOPED_NAMED_EVENT(SLMPropellerPreSimulate, FColor::Green);

    for (auto& Propeller : DeviceModels)
    {
        if (Propeller.Primitive)
        {
            Propeller.DirectionPropellerAxis = Propeller.Primitive->GetForwardVector();
            Propeller.AxialSpeed_ms = FVector::DotProduct(Propeller.Primitive->GetComponentVelocity(), Propeller.DirectionPropellerAxis);
        	Propeller.ForceAccumulator_N = 0;
        }
    }
}

void USLMDeviceSubsystemPropeller::Simulate(float DeltaTime, float SubstepScalar)
{
	SCOPED_NAMED_EVENT(SLMPropellerSimulate, FColor::Green);

    for (auto& Propeller : DeviceModels)
    {
        const float AngVel_rads = DomainRotation->GetData(Propeller.Index_Rotation_Drive).AngularVelocity;
    	const float BladePitch_rad = FMath::DegreesToRadians(Propeller.BladePitch_Deg); 
    	const float LiftCoefficient = FMath::Sin(BladePitch_rad);
    	const float DragCoefficient = 2*FMath::Sin(0.5*BladePitch_rad);
    	const float AirDensity_kgm3 = 1.225;
    	
    	const float EffectiveRadius_m = .0075*Propeller.Radius_cm;
    	const float EffectiveSpeed_ms = AngVel_rads*EffectiveRadius_m;
    	const float EffectiveSpeedSquared_ms = EffectiveSpeed_ms*EffectiveSpeed_ms*FMath::Sign(EffectiveSpeed_ms);
    	const float EffectiveArea = 0.1*UE_PI*EffectiveRadius_m*EffectiveRadius_m;
    	const float Memo = 0.5*AirDensity_kgm3*EffectiveArea*EffectiveSpeedSquared_ms;

		const float ThrustForce_N = Memo * LiftCoefficient;
		const float DragForce_N = Memo * DragCoefficient;
    	
    	const float DragTorque_Nm = -1*DragForce_N * EffectiveRadius_m; 
    	const float DragAngularImpulse = DragTorque_Nm*DeltaTime;
    	DomainRotation->AddAngularImpulse(Propeller.Index_Rotation_Drive, DragAngularImpulse);
    	Propeller.ForceAccumulator_N += ThrustForce_N * SubstepScalar;
    }
}

void USLMDeviceSubsystemPropeller::PostSimulate(float DeltaTime)
{
	SCOPED_NAMED_EVENT(SLMPropellerPostSimulate, FColor::Green);

    for (auto It = DeviceModels.CreateIterator(); It; ++It)
    {
        if (It->Primitive)
        {
            const FVector Force = 100 * It->ForceAccumulator_N * It->DirectionPropellerAxis;
        	const FVector Location = It->Primitive->GetComponentLocation();
        	It->Primitive->AddForceAtLocation(Force, Location);
            
            //Debug
            //const FVector DrawDebugStartPoint = It->ContactPatchLocation + FVector(0, 0, 120);
            //DrawDebugLine(GetWorld(), DrawDebugStartPoint, DrawDebugStartPoint + It->ContactPatchNormal * It->NormalImpulseMagnitude * 0.1, FColor::Blue, false, -1, 0, 5);
            //DrawDebugLine(GetWorld(), DrawDebugStartPoint, DrawDebugStartPoint + It->ImpulseAccumulator * 0.1, FColor::Red, false, -1, 0, 5);

        }
        DeviceCosmetics[It.GetIndex()].AngularVelocityDegrees = FMath::RadiansToDegrees(DomainRotation->GetData(It->Index_Rotation_Drive).AngularVelocity);
    }
}

int32 USLMDeviceSubsystemPropeller::AddDevice(FSLMDevicePropeller Device)
{
    Device.DeviceModel.Index_Rotation_Drive = DomainRotation->AddPort(Device.Port_Rotation_Drive);
    DeviceCosmetics.Add(FSLMDeviceCosmeticsPropeller());
    return DeviceModels.Add(Device.DeviceModel);
}

void USLMDeviceSubsystemPropeller::RemoveDevice(int32 DeviceIndex)
{
    DomainRotation->RemovePort(DeviceModels[DeviceIndex].Index_Rotation_Drive);

    DeviceCosmetics.RemoveAt(DeviceIndex);
    DeviceModels.RemoveAt(DeviceIndex);
}

FSLMDeviceModelPropeller USLMDeviceSubsystemPropeller::GetDeviceState(int32 DeviceIndex)
{
    return DeviceModels[DeviceIndex];
}

FSLMDeviceCosmeticsPropeller USLMDeviceSubsystemPropeller::GetDeviceCosmetics(int32 DeviceIndex)
{
    return DeviceCosmetics[DeviceIndex];
}

void USLMDeviceSubsystemPropeller::SetPrimitive(int32 DeviceIndex, UPrimitiveComponent* Primitive)
{
	DeviceModels[DeviceIndex].Primitive = Primitive;
}
