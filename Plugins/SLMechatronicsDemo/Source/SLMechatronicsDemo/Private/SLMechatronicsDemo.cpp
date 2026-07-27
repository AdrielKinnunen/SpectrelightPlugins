// Copyright Epic Games, Inc. All Rights Reserved.

#include "SLMechatronicsDemo.h"

#include "SLMManager.h"
#include "Domains/SLMDomainRotation.h"
#include "Domains/SLMDomainSignal.h"
#include "Domains/SLMDomainElectricity.h"
#include "Devices/SLMDeviceGearbox.h"
#include "Devices/SLMDeviceMotorGenerator.h"
#include "Devices/SLMDeviceProbe.h"

#define LOCTEXT_NAMESPACE "FSLMechatronicsDemoModule"

void FSLMechatronicsDemoModule::StartupModule()
{
	USLMManager::RegisterDomainSystem(MakeUnique<FSLMDomainSystemRotation>());
	USLMManager::RegisterDomainSystem(MakeUnique<FSLMDomainSystemSignal>());
	USLMManager::RegisterDomainSystem(MakeUnique<FSLMDomainSystemElectricity>());
	
	USLMManager::RegisterDeviceSystem(MakeUnique<FSLMDeviceSystemGearbox>());
	USLMManager::RegisterDeviceSystem(MakeUnique<FSLMDeviceSystemMotorGenerator>());
	USLMManager::RegisterDeviceSystem(MakeUnique<FSLMDeviceSystemProbe>());
}

void FSLMechatronicsDemoModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FSLMechatronicsDemoModule, SLMechatronicsDemo)