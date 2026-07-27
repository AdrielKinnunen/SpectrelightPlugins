// Copyright Spectrelight Studios, LLC

#include "SLMDeviceBase.h"

#include "SLMManager.h"
#include "Net/UnrealNetwork.h"

USLMDeviceComponentBase::USLMDeviceComponentBase()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
	//bWantsInitializeComponent = true;
}

void USLMDeviceComponentBase::BeginPlay()
{
	Super::BeginPlay();
	Manager = GetWorld()->GetSubsystem<USLMManager>();
	OnManagerReady();
	TryResolveDevice(DeviceAddress);
}

void USLMDeviceComponentBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void USLMDeviceComponentBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(USLMDeviceComponentBase, DeviceAddress);
}

void USLMDeviceComponentBase::OnRep_DeviceAddress(const FSLMDeviceAddress OldDeviceAddress)
{
	if (HasBegunPlay())
	{
		TryResolveDevice(DeviceAddress);
	}
}

void USLMDeviceComponentBase::TryResolveDevice(const FSLMDeviceAddress NewDeviceAddress)
{
	if (!DeviceAddress.IsValid() || NewDeviceAddress != DeviceAddress)
	{
		return;
	}
	auto* System = Manager->GetDeviceSystemByTag(NewDeviceAddress.DeviceTag);
	if (System->DeviceExists(NewDeviceAddress))
	{
		System->OnDeviceAdded.RemoveAll(this);
		bDeviceResolved = true;
		OnDeviceResolved();
		return;
	}
	if (!System->OnDeviceAdded.IsBoundToObject(this))
	{
		System->OnDeviceAdded.AddUObject(this, &USLMDeviceComponentBase::TryResolveDevice);
	}
}

void USLMDeviceComponentBase::OnManagerReady()
{
}

void USLMDeviceComponentBase::OnDeviceResolved()
{
}

bool FSLMDeviceSystemBase::DeviceExists(const FSLMDeviceAddress& DeviceAddress) const
{
	const bool bDeviceTypeMatches = SystemTag.MatchesTagExact(DeviceAddress.DeviceTag);
	const bool bDeviceIDIsValid = OccupiedDeviceIDs.IsValidIndex(DeviceAddress.DeviceID) && OccupiedDeviceIDs[DeviceAddress.DeviceID];
	return bDeviceTypeMatches && bDeviceIDIsValid;
}
