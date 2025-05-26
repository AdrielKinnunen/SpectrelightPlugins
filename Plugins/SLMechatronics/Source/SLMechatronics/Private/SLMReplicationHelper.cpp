// Copyright Spectrelight Studios, LLC. All Rights Reserved.


#include "SLMReplicationHelper.h"

#include "SLMSubsystem.h"
#include "Net/UnrealNetwork.h"

ASLMReplicationHelper::ASLMReplicationHelper()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	bAlwaysRelevant = true;
}

void ASLMReplicationHelper::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASLMReplicationHelper, ReplicatedConnections);
}

void ASLMReplicationHelper::BeginPlay()
{
	Super::BeginPlay();
	SLMSubsystem = GetWorld()->GetSubsystem<USLMechatronicsSubsystem>();
}

void ASLMReplicationHelper::OnRep_ReplicatedConnections()
{
	UE_LOG(LogTemp, Log, TEXT("C++ OnRep Fired, ReplicatedConnections has %i entries."), ReplicatedConnections.Num());
	for (FSLMConnectionByMetaData Connection : ReplicatedConnections)
	{
		SLMSubsystem->MakeConnectionByMetadata(Connection);
	}
}

void ASLMReplicationHelper::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
