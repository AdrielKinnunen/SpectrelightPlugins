// Copyright Spectrelight Studios, LLC

#include "Devices/SLMDeviceSimpleEngine.h"

FSLMDeviceModelSimpleEngine USLMDeviceComponentSimpleEngine::GetDeviceState()
{
    return Subsystem->GetDeviceState(DeviceIndex);
}

void USLMDeviceComponentSimpleEngine::BeginPlay()
{
    Super::BeginPlay();
    const AActor* OwningActor = GetOwner();
    DeviceSettings.Port_Rotation_Crankshaft.PortMetaData.AssociatedActor = OwningActor;
    DeviceSettings.Port_Signal_Throttle.PortMetaData.AssociatedActor = OwningActor;

    Subsystem = GetWorld()->GetSubsystem<USLMDeviceSubsystemSimpleEngine>();
    DeviceIndex = Subsystem->AddDevice(DeviceSettings);
}

void USLMDeviceComponentSimpleEngine::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Subsystem->RemoveDevice(DeviceIndex);
    Super::EndPlay(EndPlayReason);
}

void USLMDeviceSubsystemSimpleEngine::OnWorldBeginPlay(UWorld& InWorld)
{
    DomainRotation = GetWorld()->GetSubsystem<USLMDomainRotation>();
    DomainSignal = GetWorld()->GetSubsystem<USLMDomainSignal>();
    Super::OnWorldBeginPlay(InWorld);
}

void USLMDeviceSubsystemSimpleEngine::PreSimulate(const float DeltaTime)
{
}

void USLMDeviceSubsystemSimpleEngine::Simulate(const float DeltaTime, const float SubstepScalar)
{
	SCOPED_NAMED_EVENT(SLMSimpleEngineSimulate, FColor::Green);

    for (auto& Model : DeviceModels)
    {
        const FSLMDataRotation Crank = DomainRotation->GetData(Model.Index_Rotation_Crankshaft);
        const float Throttle = FMath::Clamp(DomainSignal->ReadByPortIndex(Model.Index_Signal_Throttle), 0, 1.0);
    	const float RPM = Crank.GetRPM();
        if (RPM > Model.RevLimiter)
        {
	        Model.bIgnition = false;
        }
    	if (RPM < Model.RevLimiter - 500)
    	{
    		Model.bIgnition = true;
    	}
    	
        const float Torque = Model.bIgnition * Model.MaxTorque * Throttle - Model.DragTorque * (1 - Throttle);
        const float CrankRPS_Out = Crank.AngularVelocity + Torque * DeltaTime / Crank.MomentOfInertia;

        DomainRotation->SetAngularVelocity(Model.Index_Rotation_Crankshaft, CrankRPS_Out);
    }
}

void USLMDeviceSubsystemSimpleEngine::PostSimulate(const float DeltaTime)
{
}

int32 USLMDeviceSubsystemSimpleEngine::AddDevice(FSLMDeviceSimpleEngine Device)
{
    Device.DeviceModel.Index_Rotation_Crankshaft = DomainRotation->AddPort(Device.Port_Rotation_Crankshaft);
    Device.DeviceModel.Index_Signal_Throttle = DomainSignal->AddPort(Device.Port_Signal_Throttle);
    return DeviceModels.Add(Device.DeviceModel);
}

void USLMDeviceSubsystemSimpleEngine::RemoveDevice(const int32 DeviceIndex)
{
    DomainRotation->RemovePort(DeviceModels[DeviceIndex].Index_Rotation_Crankshaft);
    DomainSignal->RemovePort(DeviceModels[DeviceIndex].Index_Signal_Throttle);
    DeviceModels.RemoveAt(DeviceIndex);
}

FSLMDeviceModelSimpleEngine USLMDeviceSubsystemSimpleEngine::GetDeviceState(const int32 DeviceIndex)
{
    return DeviceModels[DeviceIndex];
}
