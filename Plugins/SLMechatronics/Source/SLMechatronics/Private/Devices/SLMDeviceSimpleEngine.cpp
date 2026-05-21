// Copyright Spectrelight Studios, LLC
#if 0
#include "Devices/SLMDeviceSimpleEngine.h"
#include "Net/UnrealNetwork.h"



void FSLMSimpleEngineRepArrayDescriptor::PostReplicatedAdd(const TArrayView<int32>& AddedIndices, int32 FinalSize) const
{
	Subsystem->PostRepAddOrChangeDescriptor(AddedIndices, Items);
}

void FSLMSimpleEngineRepArrayDescriptor::PostReplicatedChange(const TArrayView<int32>& ChangedIndices, int32 FinalSize) const
{
	Subsystem->PostRepAddOrChangeDescriptor(ChangedIndices, Items);
}

void FSLMSimpleEngineRepArrayDescriptor::PreReplicatedRemove(const TArrayView<int32>& RemovedIndices, int32 FinalSize) const
{
	Subsystem->PreRepRemoveDescriptor(RemovedIndices, Items);
}

void FSLMSimpleEngineRepArrayState::PostReplicatedAdd(const TArrayView<int32>& AddedIndices, int32 FinalSize) const
{
	Subsystem->PostRepAddOrChangeState(AddedIndices, Items);
}

void FSLMSimpleEngineRepArrayState::PostReplicatedChange(const TArrayView<int32>& ChangedIndices, int32 FinalSize) const
{
	Subsystem->PostRepAddOrChangeState(ChangedIndices, Items);
}

void FSLMSimpleEngineRepArrayState::PreReplicatedRemove(const TArrayView<int32>& RemovedIndices, int32 FinalSize)
{
}

void USLMDeviceSubsystemSimpleEngine::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
	if (GetWorld()->GetNetMode() != NM_Client)
	{
		Replicator = InWorld.SpawnActor<ASLMDeviceReplicatorSimpleEngine>();
	}
}

FString USLMDeviceSubsystemSimpleEngine::GetDebugString(const bool Verbose)
{
	return GetDebugString_Impl(Verbose);
}

uint32 USLMDeviceSubsystemSimpleEngine::GetDebugHash()
{
	return GetDebugHash_Impl();
}

void USLMDeviceSubsystemSimpleEngine::PostInitialize()
{
	Super::PostInitialize();
	DomainRotation = GetWorld()->GetSubsystem<USLMDomainRotation>();
	DomainSignal = GetWorld()->GetSubsystem<USLMDomainSignal>();
}

void USLMDeviceSubsystemSimpleEngine::PreSimulate(const float DeltaTime)
{
}

void USLMDeviceSubsystemSimpleEngine::Simulate(const float DeltaTime, const float SubstepScalar)
{
	for (auto& Model : DeviceModels)
	{
		FSLMDataRotation& CrankParticle = DomainRotation->GetParticleRef(Model.State.PortID_Rotation_Crankshaft);
		const float Throttle = FMath::Clamp(DomainSignal->ReadValue(Model.State.PortID_Signal_Throttle), 0, 1.0);
		const float RPM = CrankParticle.GetRPM();
		if (RPM > Model.Settings.MaxRPM)
		{
			Model.State.bIgnition = false;
		}
		if (RPM < Model.Settings.MaxRPM - 500)
		{
			Model.State.bIgnition = true;
		}
		const float DragTorque = 0.1 * Model.Settings.MaxTorque;
		const float Torque = Throttle * Model.State.bIgnition * Model.Settings.MaxTorque + (1 - Throttle) * DragTorque * FMath::Sign(Model.Settings.IdleRPM - RPM);

		CrankParticle.AddTorque(Torque, DeltaTime);
	}
}

void USLMDeviceSubsystemSimpleEngine::PostSimulate(const float DeltaTime)
{
	PullRepState();
}

void USLMDeviceSubsystemSimpleEngine::RegisterPorts(const FSLMSimpleEnginePortSettings& PortSettings, FSLMSimpleEngineModelState& ModelState, const FSLMSimpleEngineAddresses& Addresses) const
{
	ModelState.PortID_Rotation_Crankshaft = DomainRotation->AddPort(PortSettings.Port_Rotation_Crankshaft, Addresses.Address_Rotation_Crankshaft);
	ModelState.PortID_Signal_Throttle = DomainSignal->AddPort(PortSettings.Port_Signal_Throttle, Addresses.Address_Signal_Throttle);
}

void USLMDeviceSubsystemSimpleEngine::RemovePorts(const FSLMSimpleEngineAddresses& Addresses) const
{
	DomainRotation->RemovePort(Addresses.Address_Rotation_Crankshaft);
	DomainSignal->RemovePort(Addresses.Address_Signal_Throttle);
}

void USLMDeviceSubsystemSimpleEngine::HandleToAddresses(const FSLMSimpleEngineHandle Handle, FSLMSimpleEngineAddresses& Addresses) const
{
	Addresses.Address_Rotation_Crankshaft	= MakePortAddress(this, DomainRotation, Handle.ID, 1);
	Addresses.Address_Signal_Throttle		= MakePortAddress(this, DomainSignal, Handle.ID, 0);
}

void USLMDeviceSubsystemSimpleEngine::ModelToCosmeticState(const FSLMSimpleEngineModel& Model, FSLMSimpleEngineCosmeticState& CosmeticState) const
{
	CosmeticState.Throttle = DomainSignal->ReadValue(Model.State.PortID_Signal_Throttle);
	CosmeticState.CrankshaftRPM = DomainRotation->GetParticleRef(Model.State.PortID_Rotation_Crankshaft).AngularVelocity * SLMRadToRPM;
}

void USLMDeviceSubsystemSimpleEngine::ModelToRepState(const FSLMSimpleEngineModel& Model, FSLMSimpleEngineRepState& RepState)
{
}

void USLMDeviceSubsystemSimpleEngine::RepStateToModel(const FSLMSimpleEngineRepState& RepState, FSLMSimpleEngineModel& Model)
{
}

void USLMDeviceSubsystemSimpleEngine::InputToModel(const FSLMSimpleEngineInput& Input, FSLMSimpleEngineModel& Model)
{
}



ASLMDeviceReplicatorSimpleEngine::ASLMDeviceReplicatorSimpleEngine()
{
	bReplicates = true;
	bAlwaysRelevant = true;
	SetReplicatingMovement(false);
}

void ASLMDeviceReplicatorSimpleEngine::PostInitializeComponents()
{
	RepArrayDescriptor.Subsystem = GetWorld()->GetSubsystem<USLMDeviceSubsystemSimpleEngine>();
	RepArrayState.Subsystem = GetWorld()->GetSubsystem<USLMDeviceSubsystemSimpleEngine>();
	Super::PostInitializeComponents();
}

void ASLMDeviceReplicatorSimpleEngine::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASLMDeviceReplicatorSimpleEngine, RepArrayDescriptor);
	DOREPLIFETIME(ASLMDeviceReplicatorSimpleEngine, RepArrayState);
}



FSLMSimpleEngineHandle USLMBPFLSimpleEngine::AddDeviceSimpleEngine(const UObject* WorldContextObject, const FSLMSimpleEngineDescriptor& Settings)
{
	return WorldContextObject->GetWorld()->GetSubsystem<USLMDeviceSubsystemSimpleEngine>()->AddDevice(Settings);
}

void USLMBPFLSimpleEngine::RemoveDevice(const UObject* WorldContextObject, const FSLMSimpleEngineHandle Handle)
{
	WorldContextObject->GetWorld()->GetSubsystem<USLMDeviceSubsystemSimpleEngine>()->RemoveDevice(Handle);
}

FSLMSimpleEngineModelSettings USLMBPFLSimpleEngine::GetDeviceSettings(const UObject* WorldContextObject, const FSLMSimpleEngineHandle Handle)
{
	return WorldContextObject->GetWorld()->GetSubsystem<USLMDeviceSubsystemSimpleEngine>()->GetModelSettings(Handle);
}

void USLMBPFLSimpleEngine::SetDeviceSettings(const UObject* WorldContextObject, const FSLMSimpleEngineHandle Handle, const FSLMSimpleEngineModelSettings& Settings)
{
	WorldContextObject->GetWorld()->GetSubsystem<USLMDeviceSubsystemSimpleEngine>()->SetModelSettings(Handle, Settings);
}

FSLMSimpleEngineAddresses USLMBPFLSimpleEngine::GetPortAddresses(const UObject* WorldContextObject, const FSLMSimpleEngineHandle Handle)
{
	return WorldContextObject->GetWorld()->GetSubsystem<USLMDeviceSubsystemSimpleEngine>()->GetPortAddresses(Handle);
}

void USLMBPFLSimpleEngine::ApplyInput(const UObject* WorldContextObject, const FSLMSimpleEngineHandle Handle, const FSLMSimpleEngineInput& Input)
{
	WorldContextObject->GetWorld()->GetSubsystem<USLMDeviceSubsystemSimpleEngine>()->ApplyInput(Handle, Input);
}

FSLMSimpleEngineCosmeticState USLMBPFLSimpleEngine::GetCosmeticState(const UObject* WorldContextObject, const FSLMSimpleEngineHandle Handle)
{
	return WorldContextObject->GetWorld()->GetSubsystem<USLMDeviceSubsystemSimpleEngine>()->GetCosmeticState(Handle);
}
















































/*
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
        if (RPM > Model.MaxRPM)
        {
	        Model.bIgnition = false;
        }
    	if (RPM < Model.MaxRPM - 500)
    	{
    		Model.bIgnition = true;
    	}
    	const float DragTorque = 0.1 * Model.MaxTorque;
        const float Torque = Throttle * Model.bIgnition * Model.MaxTorque + (1 - Throttle) * DragTorque * FMath::Sign(Model.IdleRPM - RPM);
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
*/


#endif