// Fill out your copyright notice in the Description page of Project Settings.


#include "SLFluidDynamicsProbeComponent.h"
#include "SLFluidDynamicsSubsystem.h"

// Sets default values for this component's properties
USLFluidDynamicsProbeComponent::USLFluidDynamicsProbeComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void USLFluidDynamicsProbeComponent::BeginPlay()
{
	Super::BeginPlay();
	GetWorld()->GetSubsystem<USLFluidDynamicsSubsystem>()->RegisterProbeComponent(this);
}

void USLFluidDynamicsProbeComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorld()->GetSubsystem<USLFluidDynamicsSubsystem>()->DeRegisterProbeComponent(this);
	Super::EndPlay(EndPlayReason);
}


// // Called every frame
// void USLFluidDynamicsProbeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
// {
// 	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
//
// 	// ...
// }
