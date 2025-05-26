// Copyright Spectrelight Studios, LLC. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SLMDomainBase.h"
#include "GameFramework/Actor.h"
#include "SLMReplicationHelper.generated.h"


class USLMechatronicsSubsystem;

UCLASS()
class SLMECHATRONICS_API ASLMReplicationHelper : public AActor
{
	GENERATED_BODY()
public:
	ASLMReplicationHelper();
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	USLMechatronicsSubsystem* SLMSubsystem;
protected:
	virtual void BeginPlay() override;
public:
	UPROPERTY(ReplicatedUsing=OnRep_ReplicatedConnections, BlueprintReadWrite)
	TArray<FSLMConnectionByMetaData> ReplicatedConnections;
	
	UFUNCTION()
	void OnRep_ReplicatedConnections();
};
