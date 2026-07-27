// Copyright Spectrelight Studios, LLC

#include "Devices/SLMDeviceMotorGenerator.h"


UE_DEFINE_GAMEPLAY_TAG(TAG_SLM_System_Device_MotorGenerator, "SLM.System.Device.MotorGenerator");


FString FSLMDeviceSettingsMotorGenerator::GetDebugString() const
{
	FString Result;
	Result += FString::Format(TEXT("\n{0}, {1}"), {
								MaxPowerWatts, 
								ConstantTorqueAngVel});
	return Result;
}

uint32 FSLMDeviceSettingsMotorGenerator::GetDebugHash() const
{
	uint32 Hash = 0;
	Hash = HashCombine(Hash, GetTypeHash(FMath::RoundToInt(MaxPowerWatts * 100.0f)));
	Hash = HashCombine(Hash, GetTypeHash(FMath::RoundToInt(ConstantTorqueAngVel * 100.0f)));
	return Hash;
}

FString FSLMDeviceStateMotorGenerator::GetDebugString() const
{
	FString Result;
	//Result += FString::Format(TEXT("\n{0}, {1}"), {MaxPowerWatts, GearRatio, PortID_Rotation_Input, PortID_Rotation_Output, PortID_Signal_Shift});
	return Result;
}

uint32 FSLMDeviceStateMotorGenerator::GetDebugHash() const
{
	uint32 Hash = 0;
	//Hash = HashCombine(Hash, GetTypeHash(CurrentGear));
	//Hash = HashCombine(Hash, GetTypeHash(FMath::RoundToInt(GearRatio * 100.0f)));
	return Hash;
}

FSLMDeviceSystemMotorGenerator::FSLMDeviceSystemMotorGenerator()
{
	SystemTag = TAG_SLM_System_Device_MotorGenerator;
}

FGameplayTag FSLMDeviceSystemMotorGenerator::GetSystemTagStatic()
{
	return TAG_SLM_System_Device_MotorGenerator;
}

void FSLMDeviceSystemMotorGenerator::Initialize()
{
	DomainRotation = Manager->GetDomainSystem<FSLMDomainSystemRotation>();
	DomainElectricity = Manager->GetDomainSystem<FSLMDomainSystemElectricity>();
	DomainSignal = Manager->GetDomainSystem<FSLMDomainSystemSignal>();
}

void FSLMDeviceSystemMotorGenerator::PreSimulate_Impl(const int32 ID, const float DeltaTime)
{
	//auto& State = StateArray[ID];
	//const auto& Settings = SettingsArray[ID];
	//auto& Context = GameplayContextArray[ID];
}

void FSLMDeviceSystemMotorGenerator::Simulate_Impl(const int32 ID, const float DeltaTime, const float SubstepScalar)
{
	const auto& Settings = SettingsArray[ID];
	const auto& State = StateArray[ID];
	//auto& GameplayContext = GameplayContextArray[ID];
	
	auto& CrankParticle = DomainRotation->GetParticleRef(State.PortID_Rotation);
	auto& ElectricityParticle = DomainElectricity->GetParticleRef(State.PortID_Electricity);
	auto& Signal = DomainSignal->GetParticleRef(State.PortID_Signal_Throttle);
	
	const float Throttle = FMath::Clamp(Signal.Read(), -1.0, 1.0);
	const float AngVelForTorqueCalculation = FMath::Max(FMath::Abs(CrankParticle.AngularVelocity), Settings.ConstantTorqueAngVel);
	const float MaxTorque = Settings.MaxPowerWatts / AngVelForTorqueCalculation;
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
	//State.PowerFlowWatts = EnergyTransferred / DeltaTime;
}

void FSLMDeviceSystemMotorGenerator::PostSimulate_Impl(const int32 ID, const float DeltaTime)
{
	//auto& Settings = SettingsArray[ID];
	const auto& State = StateArray[ID];
	auto& GameplayContext = GameplayContextArray[ID];

	GameplayContext.CurrentThrottle = DomainSignal->GetParticleRef(State.PortID_Signal_Throttle).Read();
	GameplayContext.CurrentRPM = DomainRotation->GetParticleRef(State.PortID_Rotation).AngularVelocity * SLMRadToRPM;
}

void FSLMDeviceSystemMotorGenerator::RegisterPorts_Impl(const int32 DeviceID, const FSLMDevicePortAddressesMotorGenerator& Addresses)
{
	auto& State = StateArray[DeviceID];
	const auto& Descriptor = DescriptorArray[DeviceID];
	const auto& Ports = DescriptorArray[DeviceID].Ports;
	
	State.PortID_Rotation			= DomainRotation->RegisterPort(Ports.Port_Rotation, Addresses.Address_Rotation, Descriptor.AssociatedActor.Get());
	State.PortID_Electricity		= DomainElectricity->RegisterPort(Ports.Port_Electricity, Addresses.Address_Electricity, Descriptor.AssociatedActor.Get());
	State.PortID_Signal_Throttle	= DomainSignal->RegisterPort(Ports.Port_Signal_Throttle, Addresses.Address_Signal_Throttle, Descriptor.AssociatedActor.Get());
}

void FSLMDeviceSystemMotorGenerator::RemovePorts_Impl(const FSLMDevicePortAddressesMotorGenerator& Addresses) const
{
	DomainRotation		->RemovePort(Addresses.Address_Rotation);
	DomainElectricity	->RemovePort(Addresses.Address_Electricity);
	DomainSignal		->RemovePort(Addresses.Address_Signal_Throttle);
}

void FSLMDeviceSystemMotorGenerator::GetPortAddresses_Impl(const int32 DeviceID, FSLMDevicePortAddressesMotorGenerator& OutAddresses) const
{
	OutAddresses.Address_Rotation			= MakePortAddress(this, DeviceID, DomainRotation, 0);
	OutAddresses.Address_Electricity		= MakePortAddress(this, DeviceID, DomainElectricity, 0);
	OutAddresses.Address_Signal_Throttle	= MakePortAddress(this, DeviceID, DomainSignal, 0);
}

void FSLMDeviceSystemMotorGenerator::GetRepState_Impl(const int32 DeviceID, FSLMDeviceRepStateMotorGenerator& OutRepState) const
{
}

void FSLMDeviceSystemMotorGenerator::ApplyRepState_Impl(const int32 DeviceID, const FSLMDeviceRepStateMotorGenerator& RepState)
{
}

void USLMDeviceComponentMotorGenerator::BeginPlay()
{
	Super::BeginPlay();
	if (GetOwner()->HasAuthority() && !Manager->bRemappingContextOpen)
	{
		DefaultDeviceDescriptor.AssociatedActor = GetOwner();
		DeviceAddress = DeviceSystem->AddEditDevice(DefaultDeviceDescriptor);
		OnRep_DeviceAddress(DeviceAddress);
	}
}

void USLMDeviceComponentMotorGenerator::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (GetOwner()->HasAuthority() && bDeviceResolved)
	{
		DeviceSystem->RemoveDevice(DeviceAddress);
	}
	Super::EndPlay(EndPlayReason);
}

void USLMDeviceComponentMotorGenerator::OnManagerReady()
{
	DeviceSystem = Manager->GetDeviceSystem<FSLMDeviceSystemMotorGenerator>();
}

void USLMDeviceComponentMotorGenerator::OnDeviceResolved()
{
	
}

FSLMDeviceDescriptorMotorGenerator USLMDeviceComponentMotorGenerator::GetDeviceDescriptor() const
{
	return DeviceSystem->GetDescriptor(DeviceAddress);
}

void USLMDeviceComponentMotorGenerator::EditDevice(const FSLMDeviceDescriptorMotorGenerator& Descriptor)
{
	DeviceSystem->AddEditDevice(Descriptor, DeviceAddress);
}

FSLMDevicePortAddressesMotorGenerator USLMDeviceComponentMotorGenerator::GetPortAddresses() const
{
	return DeviceSystem->GetPortAddresses(DeviceAddress);
}

FSLMDeviceGameplayContextMotorGenerator USLMDeviceComponentMotorGenerator::GetGameplayContext() const
{
	if (DeviceSystem->DeviceExists(DeviceAddress))
	{
		return DeviceSystem->GetGameplayContextRef(DeviceAddress);
	}
	return FSLMDeviceGameplayContextMotorGenerator();
}