// Fill out your copyright notice in the Description page of Project Settings.

#include "SLFluidDynamicsSubsystem.h"

#include "Kismet/KismetSystemLibrary.h"

bool USLFluidDynamicsSubsystem::DoesSupportWorldType(const EWorldType::Type WorldType) const
{
    switch (WorldType)
    {
    case EWorldType::None: break;
    case EWorldType::Game: return true;
    case EWorldType::Editor: break;
    case EWorldType::PIE: return true;
    case EWorldType::EditorPreview: return true;
    case EWorldType::GamePreview: return true;
    case EWorldType::GameRPC: break;
    case EWorldType::Inactive: break;
    default: ;
    }
    return false;
}

void USLFluidDynamicsSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
}

void USLFluidDynamicsSubsystem::Deinitialize()
{
}

void USLFluidDynamicsSubsystem::RegisterProbeComponent(USLFluidDynamicsProbeComponent* ProbeComponentToRegister)
{
    ProbeComponentsArray.Add(ProbeComponentToRegister);
}

void USLFluidDynamicsSubsystem::DeRegisterProbeComponent(USLFluidDynamicsProbeComponent* ProbeComponentToDeRegister)
{
    ProbeComponentsArray.Remove(ProbeComponentToDeRegister);
}

void USLFluidDynamicsSubsystem::Tick(float DeltaTime)
{
    for (const USLFluidDynamicsProbeComponent* ProbeComponent : ProbeComponentsArray)
    {
        UPrimitiveComponent* AffectedComponent = Cast<UPrimitiveComponent>(ProbeComponent->GetAttachmentRoot());

        if (AffectedComponent != nullptr && AffectedComponent->IsSimulatingPhysics())
        {
            //GEngine->AddOnScreenDebugMessage(-1, 0.001f, FColor::Yellow, UKismetSystemLibrary::GetDisplayName(ProbeComponent));

            //Convenience
            const FSLFluidDynamicsProbeSettings ProbeSettings = ProbeComponent->FluidDynamicsProbeSettings;
            const FBodyInstance* AffectedComponentBodyInstance = AffectedComponent->GetBodyInstance();
            //const FTransform ProbeTransform = ProbeComponent->GetComponentTransform();
            const float Mass = AffectedComponent->GetMass();

            //Depth
            const float ProbeDepth = -ProbeComponent->GetComponentLocation().Z;
            const float ProbeDepthAlpha = FMath::Clamp(ProbeDepth / ProbeSettings.MaxBuoyancyDepth, 0.f, 1.f);

            //Velocity
            const FVector ProbeVelocity = AffectedComponentBodyInstance->GetUnrealWorldVelocityAtPoint(ProbeComponent->GetComponentLocation());
            const float ProbeVelocityLocalX = FVector::DotProduct(ProbeVelocity, ProbeComponent->GetForwardVector());
            const float ProbeVelocityLocalY = FVector::DotProduct(ProbeVelocity, ProbeComponent->GetRightVector());
            const float ProbeVelocityLocalZ = FVector::DotProduct(ProbeVelocity, ProbeComponent->GetUpVector());
            //Implement this later
            //const FVector ProbeLocalVelocity = ProbeTransform.InverseTransformVectorNoScale(ProbeVelocity);

            //Forces
            const FVector BuoyantForce = FVector(0.f, 0.f, 1.f) * ProbeDepthAlpha * ProbeSettings.MaxBuoyancy * Mass;
            const float DragForceLocalX = ProbeVelocityLocalX * ProbeSettings.WaterDrag.X;
            const float DragForceLocalY = ProbeVelocityLocalY * ProbeSettings.WaterDrag.Y;
            const float DragForceLocalZ = ProbeVelocityLocalZ * ProbeSettings.WaterDrag.Z;
            const FVector DragForce = ProbeDepthAlpha * -1 * Mass * (DragForceLocalX * ProbeComponent->GetForwardVector() + DragForceLocalY * ProbeComponent->GetRightVector() + DragForceLocalZ * ProbeComponent->GetUpVector());
            const FVector OutForce = BuoyantForce + DragForce;
            //AffectedComponent->AddForce(OutForce, NAME_None, true);
            AffectedComponent->AddForceAtLocation(OutForce, ProbeComponent->GetComponentLocation(), NAME_None);
        }
    }
    //GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Yellow, TEXT("Some debug message!"));
}

TStatId USLFluidDynamicsSubsystem::GetStatId() const
{
    return TStatId();
}
