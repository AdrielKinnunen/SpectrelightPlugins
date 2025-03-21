﻿// Copyright Spectrelight Studios, LLC

#include "Devices/SLMDeviceEngine.h"

FSLMDeviceModelEngine USLMDeviceComponentEngine::GetDeviceState()
{
	return Subsystem->GetDeviceState(DeviceIndex);
}

void USLMDeviceComponentEngine::BeginPlay()
{
	Super::BeginPlay();
	const AActor* OwningActor = GetOwner();
	DeviceSettings.Port_Rotation_Crankshaft.PortMetaData.AssociatedActor = OwningActor;
	DeviceSettings.Port_Signal_Throttle.PortMetaData.AssociatedActor = OwningActor;
	DeviceSettings.Port_Air_Intake.PortMetaData.AssociatedActor = OwningActor;
	DeviceSettings.Port_Air_Exhaust.PortMetaData.AssociatedActor = OwningActor;
	
	Subsystem = GetWorld()->GetSubsystem<USLMDeviceSubsystemEngine>();
	DeviceIndex = Subsystem->AddDevice(DeviceSettings);
}

void USLMDeviceComponentEngine::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Subsystem->RemoveDevice(DeviceIndex);
	Super::EndPlay(EndPlayReason);
}

void USLMDeviceSubsystemEngine::OnWorldBeginPlay(UWorld& InWorld)
{
	DomainRotation = GetWorld()->GetSubsystem<USLMDomainRotation>();
	DomainSignal = GetWorld()->GetSubsystem<USLMDomainSignal>();
	DomainAir = GetWorld()->GetSubsystem<USLMDomainAir>();
	Super::OnWorldBeginPlay(InWorld);
}

void USLMDeviceSubsystemEngine::PreSimulate(const float DeltaTime)
{
}

void USLMDeviceSubsystemEngine::Simulate(const float DeltaTime, const float SubstepScalar)
{
    for (const auto& Model : DeviceModels)
    {
    	//Setup
    	const float Throttle = DomainSignal->ReadByPortIndex(Model.Index_Signal_Throttle);
		const float ThermalEfficiency = 1 - FMath::Pow(Model.CompressionRatio, 1 - SLMGammaAir);
    	//TODO Implement the rest of the engine lmao
    	
    	const FSLMDataRotation Crank = DomainRotation->GetData(Model.Index_Rotation_Crankshaft);
    	const bool bIsNormalDirection = Crank.AngularVelocity >= 0.0;
    	const int32 FromPort = bIsNormalDirection ? Model.Index_Air_Intake : Model.Index_Air_Exhaust;
    	const int32 ToPort = bIsNormalDirection ? Model.Index_Air_Exhaust : Model.Index_Air_Intake;
    	const float DirectionScalar = bIsNormalDirection ? 1.0 : -1.0;
    	FSLMDataAir Intake = DomainAir->GetData(FromPort);
    	FSLMDataAir Exhaust = DomainAir->GetData(ToPort);
    	//Air Transfer
    	const float VolumeMoved = Model.DisplacementPerRev * FMath::Abs(Crank.AngularVelocity) * DeltaTime * SLMRadToRev;
    	const float ExpansionRatio = (Intake.Volume + VolumeMoved) / Intake.Volume;
    	const float TransferRatio = VolumeMoved / (Intake.Volume + VolumeMoved);
    	const float MolesMoved = Intake.Moles * TransferRatio;
    	const float EnergyMoved = Intake.Energy * TransferRatio;
    	//Torque and Work, work is always work DONE on the shaft
    	const float PressureDifference = Intake.GetPressure() - Exhaust.GetPressure();
    	const float ShaftTorque = PressureDifference * Model.DisplacementPerRev * SLMRadToRev;
    	const float ShaftWork = VolumeMoved * PressureDifference;
    	const float ExpansionWork = Intake.Energy * (1 - FMath::Pow(ExpansionRatio, 1 - SLMGammaAir));
    	const float HeatIntoExhaust = ExpansionWork - ShaftWork;
    	//Update State
    	DomainAir->AddEnergyAndMoles(FromPort, -EnergyMoved - ExpansionWork, -MolesMoved);
    	DomainAir->AddEnergyAndMoles(ToPort, EnergyMoved + HeatIntoExhaust, MolesMoved);
    	DomainRotation->AddTorque(Model.Index_Rotation_Crankshaft, ShaftTorque * DirectionScalar, DeltaTime);

    	/*
    	const float LitersMoved = Model.DisplacementPerRev * FMath::Abs(Crank.AngularVelocity) * DeltaTime * SLMRadToRev;
    	const float PumpingTorque = (Intake.Pressure_bar - Exhaust.Pressure_bar) * Model.DisplacementPerRev * SLMRadToRev * 100 * TorqueDirectionMultiplier;
    	float CombustionTorque = 0;

	    if (LitersMoved > 0.0001)
	    {
	    	//Intake
	    	const float IntakeOriginalVolume = Intake.Volume_l;
	    	Intake.CompressOrExpandToVolume(Intake.Volume_l + LitersMoved);
	    	FSLMDataAir Charge = Intake;
	    	Intake.Volume_l = IntakeOriginalVolume;
	    	Intake.UpdateMoles();
	    	Charge.Volume_l = LitersMoved;
	    	Charge.UpdateMoles();

	    	//Compression
	    	const float EnergyBeforeCompression = Charge.GetInternalEnergy();
	    	Charge.CompressOrExpandToVolume(LitersMoved / Model.CompressionRatio);
	    	const float EnergyAfterCompression = Charge.GetInternalEnergy();
    	
	    	//Ignition
	    	const float OxygenGrams = Charge.GetMassGrams() * Charge.OxygenRatio;
	    	const float FuelGrams = OxygenGrams * SLMFuelPerAirGrams * Throttle;
	    	Charge.AddInternalEnergyJoules(SLMFuelJoulesPerGram * FuelGrams);
	    	const float EnergyAfterCombustion = Charge.GetInternalEnergy();

	    	//Expansion
	    	Charge.CompressOrExpandToVolume(LitersMoved);
	    	const float EnergyAfterExpansion = Charge.GetInternalEnergy();
	    	const float NetWork = EnergyBeforeCompression - EnergyAfterCompression + EnergyAfterCombustion - EnergyAfterExpansion;
	    	const float Power = NetWork / DeltaTime;
	    	CombustionTorque = Power / FMath::Abs(Crank.AngularVelocity);

	    	//Exhaust
	    	const float ExhaustOriginalVolume = Exhaust.Volume_l;
	    	Charge.CompressOrExpandToPressure(Exhaust.Pressure_bar);
	    	Exhaust.MixWith(Charge);
	    	Exhaust.CompressOrExpandToVolume(ExhaustOriginalVolume);
	    }

    	DomainAir->SetData(FromPort, Intake);
    	DomainAir->SetData(ToPort, Exhaust);
    	DomainRotation->AddTorque(Model.Index_Rotation_Crankshaft, PumpingTorque + CombustionTorque, DeltaTime);
    	//DomainRotation->SetAngularVelocity(Model.Index_Rotation_Crankshaft, 10);

    	
    	//Thermo formulation
    	const FSLMDataRotation Crank = DomainRotation->GetData(Model.Index_Rotation_Crankshaft);
    	const bool bIsNormalDirection = Crank.AngularVelocity >= 0.0;
    	const int32 FromPort = bIsNormalDirection ? Model.Index_Air_Intake : Model.Index_Air_Exhaust;
    	const int32 ToPort = bIsNormalDirection ? Model.Index_Air_Exhaust : Model.Index_Air_Intake;
    	FSLMDataAir Intake = DomainAir->GetData(FromPort);
    	FSLMDataAir Exhaust = DomainAir->GetData(ToPort);

    	const float LitersMoved = Model.DisplacementPerRev * FMath::Abs(Crank.AngularVelocity) * DeltaTime * SLMRadToRev;
    	const float MolesPerLiter = Intake.N_Moles / Intake.Volume_l;
		const float MolesMoved = MolesPerLiter * LitersMoved;

    	const float OldIntakePressure = Intake.Pressure_bar;
    	Intake.N_Moles -= MolesMoved;
    	Intake.UpdatePressure();
    	
    	Exhaust.N_Moles += MolesMoved;
    	
    	DomainAir->SetData(FromPort, Intake);
    	DomainAir->SetData(ToPort, Exhaust);
    	const float PumpingTorque = (Intake.Pressure_bar - Exhaust.Pressure_bar) * Model.DisplacementPerRev * SLMRadToRev * 100 * ((Crank.AngularVelocity >= 0) * 2 - 1); //TODO Make this better
    	DomainRotation->AddTorque(Model.Index_Rotation_Crankshaft, PumpingTorque, DeltaTime);

    	

    	// Update moles in the source and destination volumes
    	moles_source -= mole_flow_rate * delta_time;
    	moles_dest += mole_flow_rate * delta_time;

    	// Update pressures using the ideal gas law in terms of moles
    	pressure_source = (moles_source * R_u * temperature_source) / source_volume;
    	pressure_dest = (moles_dest * R_u * temperature_dest) / dest_volume;

    	// Update temperatures adiabatically
    	temperature_source *= pow((pressure_source / pressure_source), (gamma - 1) / gamma);
    	temperature_dest *= pow((pressure_dest / pressure_dest), (gamma - 1) / gamma);
    	*/    	
    }
}

void USLMDeviceSubsystemEngine::PostSimulate(const float DeltaTime)
{
}

int32 USLMDeviceSubsystemEngine::AddDevice(FSLMDeviceEngine Device)
{
	Device.DeviceModel.Index_Rotation_Crankshaft = DomainRotation->AddPort(Device.Port_Rotation_Crankshaft);
	Device.DeviceModel.Index_Signal_Throttle = DomainSignal->AddPort(Device.Port_Signal_Throttle);
	Device.DeviceModel.Index_Air_Intake = DomainAir->AddPort(Device.Port_Air_Intake);
	Device.DeviceModel.Index_Air_Exhaust = DomainAir->AddPort(Device.Port_Air_Exhaust);
	return DeviceModels.Add(Device.DeviceModel);
}

void USLMDeviceSubsystemEngine::RemoveDevice(const int32 DeviceIndex)
{
	DomainRotation->RemovePort(DeviceModels[DeviceIndex].Index_Rotation_Crankshaft);
	DomainSignal->RemovePort(DeviceModels[DeviceIndex].Index_Signal_Throttle);
	DomainAir->RemovePort(DeviceModels[DeviceIndex].Index_Air_Intake);
	DomainAir->RemovePort(DeviceModels[DeviceIndex].Index_Air_Exhaust);
	DeviceModels.RemoveAt(DeviceIndex);
}

FSLMDeviceModelEngine USLMDeviceSubsystemEngine::GetDeviceState(const int32 DeviceIndex)
{
	return DeviceModels[DeviceIndex];
}
