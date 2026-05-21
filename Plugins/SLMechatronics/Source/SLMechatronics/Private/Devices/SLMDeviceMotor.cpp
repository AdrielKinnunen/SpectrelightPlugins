// Copyright Spectrelight Studios, LLC
#if 0
#include "Devices/SLMDeviceMotor.h"

#include "Net/UnrealNetwork.h"


void USLMDeviceSubsystemMotorGenerator::Client_AddOrChangeDescriptor(const int32 HandleID, const FInstancedStruct& Payload)
{
	Client_AddOrChangeDescriptor_Impl(HandleID, Payload);
}

void USLMDeviceSubsystemMotorGenerator::Client_RemoveDescriptor(const int32 HandleID)
{
	Client_RemoveDescriptor_Impl(HandleID);
}

void USLMDeviceSubsystemMotorGenerator::Client_AddOrChangeState(const int32 HandleID, const FInstancedStruct& Payload)
{
	Client_AddOrChangeState_Impl(HandleID, Payload);
}

void USLMDeviceSubsystemMotorGenerator::Client_RemoveState(const int32 HandleID)
{
	Client_RemoveState_Impl(HandleID);
}

void USLMDeviceSubsystemMotorGenerator::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
}

FString USLMDeviceSubsystemMotorGenerator::GetDebugString(const bool Verbose)
{
	return GetDebugString_Impl(Verbose);
}

uint32 USLMDeviceSubsystemMotorGenerator::GetDebugHash()
{
	return GetDebugHash_Impl();
}

void USLMDeviceSubsystemMotorGenerator::PostInitialize()
{
	Super::PostInitialize();
	DomainRotation = GetWorld()->GetSubsystem<USLMDomainRotation>();
	DomainElectricity = GetWorld()->GetSubsystem<USLMDomainElectricity>();
	DomainSignal = GetWorld()->GetSubsystem<USLMDomainSignal>();
}

void USLMDeviceSubsystemMotorGenerator::PreSimulate(const float DeltaTime)
{
}

void USLMDeviceSubsystemMotorGenerator::Simulate(const float DeltaTime, const float SubstepScalar)
{
	for (auto& Model : DeviceModels)
	{
		FSLMDataRotation& CrankParticle = DomainRotation->GetParticleRef(Model.State.PortID_Rotation);
		FSLMDataElectricity& ElectricityParticle = DomainElectricity->GetParticleRef(Model.State.PortID_Electricity);
		const float Throttle = FMath::Clamp(DomainSignal->ReadValue(Model.State.PortID_Signal_Throttle), -1.0, 1.0);

		const float AngVelForTorqueCalculation = FMath::Max(FMath::Abs(CrankParticle.AngularVelocity), Model.Settings.ConstantTorqueAngVel);
		const float MaxTorque = Model.Settings.MaxPowerWatts / AngVelForTorqueCalculation;
		const float TorqueDemand = Throttle * MaxTorque;

		//Motoring = Negative energy transfer
		//Generating = positive energy transfer
		//TODO: Incorporate efficiency
		const float EnergyDemand = -1 * TorqueDemand * DeltaTime * CrankParticle.AngularVelocity;
		const float EnergyTransferred = ElectricityParticle.TransferEnergyClamped(EnergyDemand);

		float TorqueMultiplier = 1.0;
		if (!FMath::IsNearlyZero(EnergyDemand))
		{
			TorqueMultiplier = EnergyTransferred / EnergyDemand;
		}
		const float ActualTorque = TorqueDemand * TorqueMultiplier;
		CrankParticle.AddTorque(ActualTorque, DeltaTime);
		Model.State.PowerFlow = EnergyTransferred / DeltaTime;
	}
}

void USLMDeviceSubsystemMotorGenerator::PostSimulate(const float DeltaTime)
{
	PullRepState();
}

void USLMDeviceSubsystemMotorGenerator::RegisterPorts(const FSLMPortSettingsMotorGenerator& PortSettings, FSLMModelStateMotorGenerator& ModelState, const FSLMPortAddressesMotorGenerator& Addresses) const
{
	ModelState.PortID_Rotation = DomainRotation->AddPort(PortSettings.Port_Rotation, Addresses.Address_Rotation);
	ModelState.PortID_Electricity = DomainElectricity->AddPort(PortSettings.Port_Electricity, Addresses.Address_Electricity);
	ModelState.PortID_Signal_Throttle = DomainSignal->AddPort(PortSettings.Port_Signal_Throttle, Addresses.Address_Signal_Throttle);
}

void USLMDeviceSubsystemMotorGenerator::RemovePorts(const FSLMPortAddressesMotorGenerator& Addresses) const
{
	DomainRotation->RemovePort(Addresses.Address_Rotation);
	DomainElectricity->RemovePort(Addresses.Address_Electricity);
	DomainSignal->RemovePort(Addresses.Address_Signal_Throttle);
}

void USLMDeviceSubsystemMotorGenerator::HandleToAddresses(const FSLMHandleMotorGenerator Handle, FSLMPortAddressesMotorGenerator& Addresses) const
{
	Addresses.Address_Rotation			= MakePortAddress(this, DomainRotation, Handle.ID, 0);
	Addresses.Address_Electricity		= MakePortAddress(this, DomainElectricity, Handle.ID, 1);
	Addresses.Address_Signal_Throttle	= MakePortAddress(this, DomainSignal, Handle.ID, 0);
}

void USLMDeviceSubsystemMotorGenerator::ModelToCosmeticState(const FSLMModelMotorGenerator& Model, FSLMCosmeticStateMotorGenerator& CosmeticState) const
{
	CosmeticState.PowerFlowWatts = Model.State.PowerFlow;
	CosmeticState.AngVelDegS = DomainRotation->GetParticleRef(Model.State.PortID_Rotation).AngularVelocity * SLMRadToDeg;
}

void USLMDeviceSubsystemMotorGenerator::ModelToRepState(const FSLMModelMotorGenerator& Model, FSLMRepStateMotorGenerator& RepState)
{
}

void USLMDeviceSubsystemMotorGenerator::RepStateToModel(const FSLMRepStateMotorGenerator& RepState, FSLMModelMotorGenerator& Model)
{
}

void USLMDeviceSubsystemMotorGenerator::InputToModel(const FSLMInputMotorGenerator& Input, FSLMModelMotorGenerator& Model)
{
}




USLMDeviceComponentMotorGenerator::USLMDeviceComponentMotorGenerator()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void USLMDeviceComponentMotorGenerator::BeginPlay()
{
	Super::BeginPlay();
	Subsystem = GetWorld()->GetSubsystem<USLMDeviceSubsystemMotorGenerator>();
	if (GetOwner()->HasAuthority())
	{
		const AActor* Owner = GetOwner();
		DeviceDescriptor.PortSettings.Port_Rotation.PortMetaData.AssociatedActor = Owner;
		DeviceDescriptor.PortSettings.Port_Electricity.PortMetaData.AssociatedActor = Owner;
		DeviceDescriptor.PortSettings.Port_Signal_Throttle.PortMetaData.AssociatedActor = Owner;
		Handle = Subsystem->AddDevice(DeviceDescriptor);
	}
}

void USLMDeviceComponentMotorGenerator::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (GetOwner()->HasAuthority())
	{
		Subsystem->RemoveDevice(Handle);
	}
	Super::EndPlay(EndPlayReason);
}

void USLMDeviceComponentMotorGenerator::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(USLMDeviceComponentMotorGenerator, Handle);
}

FSLMModelSettingsMotorGenerator USLMDeviceComponentMotorGenerator::GetDeviceSettings() const
{
	return Subsystem->GetModelSettings(Handle);
}

void USLMDeviceComponentMotorGenerator::SetDeviceSettings(const FSLMModelSettingsMotorGenerator& Settings) const
{
	Subsystem->SetModelSettings(Handle, Settings);
}

void USLMDeviceComponentMotorGenerator::ApplyInput(const FSLMInputMotorGenerator& Input) const
{
	Subsystem->ApplyInput(Handle, Input);
}

FSLMCosmeticStateMotorGenerator USLMDeviceComponentMotorGenerator::GetCosmeticState() const
{
	return Subsystem->GetCosmeticState(Handle);
}

FSLMPortAddressesMotorGenerator USLMDeviceComponentMotorGenerator::GetPortAddresses() const
{
	return Subsystem->GetPortAddresses(Handle);
}
#endif