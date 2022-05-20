// Fill out your copyright notice in the Description page of Project Settings.


#include "DroneActor.h"

#include "SpikyBallEnemyActor.h"
#include "../CarPawn.h"
#include "../GrappleSphereComponent.h"
#include "../GravitySplineActor.h"
#include "Components/SphereComponent.h"

/**
 * @brief Class for drone actor.
 */
ADroneActor::ADroneActor()
{
	GrappleSphereComponent = CreateDefaultSubobject<UGrappleSphereComponent>(TEXT("GrappleSphereComponent"));
	GrappleSphereComponent->SetupAttachment(GetRootComponent());
	GrappleSphereComponent->SetIsEnabled(true);
	GrappleSphereComponent->SetIsEatable(true);

	// collider used for AI:
	SensorSphere = CreateDefaultSubobject<USphereComponent>(TEXT("SensorSphere"));
	SensorSphere->SetupAttachment(GetRootComponent());
	SensorSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SensorSphere->SetCollisionResponseToChannel(ECC_GameTraceChannel3, ECR_Overlap);
	SensorSphere->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel3);
	SensorSphere->SetSphereRadius(AvoidanceRadius);
}

void ADroneActor::BeginPlay()
{
	Super::BeginPlay();
	GrappleSphereComponent->OnGrappleHitEvent.AddDynamic(this, &ADroneActor::Grappled);
	GrappleSphereComponent->OnReachedEvent.AddDynamic(this, &ADroneActor::Reached);
	SensorSphere->OnComponentBeginOverlap.AddDynamic(this, &ADroneActor::OnOverlap);
}

void ADroneActor::Tick(float DeltaSeconds)
{
	TArray<AActor*> Poop;
	SensorSphere->GetOverlappingActors(Poop);
	Super::Tick(DeltaSeconds);

	switch (CurrentState)
	{
	case EDroneState::Intercepting:
		InterceptingState();
		break;
	case EDroneState::Attacking:
		AttackingState();
		break;
	case EDroneState::Idle:
		if (PlayerPawn && GravitySplineActive) ChangeState(Intercepting);
		break;
	}

	// spawns spiky ball to drop if needed:
	if (DroppableEnemyClass && bDropEnemy && !bHasSpawned)
	{
		FAttachmentTransformRules AttachmentTransformRules(EAttachmentRule::SnapToTarget, true);
		AttachmentTransformRules.LocationRule = EAttachmentRule::KeepWorld;
		AttachmentTransformRules.ScaleRule = EAttachmentRule::KeepWorld;
		
		FVector SpawnLocation{GetActorLocation() - SpawnOffset*GetActorUpVector()};
		DroppedEnemyActor = GetWorld()->SpawnActor<ASpikyBallEnemyActor>(DroppableEnemyClass, SpawnLocation, GetActorRotation());
		
		if (GravitySplineActive) DroppedEnemyActor->GravitySplineActive = GravitySplineActive;
		DroppedEnemyActor->AttachToActor(this, AttachmentTransformRules);
		DroppedEnemyActor->ChangeState(ASpikyBallEnemyActor::Idle);
		bHasSpawned = true;
	}
}

void ADroneActor::InterceptingState()
{
	if (bEnteringState)
	{
		// all drones spend same time to reach target no matter the distance.
		InterceptSpeed = (TargetLocation-GetActorLocation()).Size()/InterceptTime;
		bEnteringState = false;
	}
	
	SetActorRotation((TargetLocation - GetActorLocation()).Rotation());
	Move(TargetLocation);
	if (GetActorLocation().Equals(TargetLocation))
	{
		ChangeState(EDroneState::Attacking);
	}
}

void ADroneActor::AttackingState()
{
	if (bEnteringState)
	{
		bEnteringState = false;
	}

	// look at player
	FRotator CurrentRot{GetActorRotation()};
	FRotator TargetRot{(PlayerPawn->GetActorLocation()-GetActorLocation()).Rotation()};
	FRotator NewRot{FMath::RInterpTo(CurrentRot, TargetRot, GetWorld()->GetDeltaSeconds(), 5.f)};
	SetActorRotation(NewRot);

	// drops spiky ball:
	if (DroppedEnemyActor && DropTimer >= DropTime && bDropEnemy && bHasSpawned)
	{
		// FDetachmentTransformRules DetachmentTransformRules(EDetachmentRule::KeepWorld, false);
		DroppedEnemyActor->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		DroppedEnemyActor->SphereComp->SetSimulatePhysics(true);
		DroppedEnemyActor->SphereComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		DroppedEnemyActor->GravitySplineActive = GravitySplineActive;
		DroppedEnemyActor->RotateSphereComponentToLocalUpVector();
		DroppedEnemyActor->ChangeState(ASpikyBallEnemyActor::Airborne);
		bDropEnemy = false;
	}
	else if (DropTimer < DropTime && bDropEnemy && bHasSpawned)
	{
		DropTimer += GetWorld()->GetDeltaSeconds();
	}
}

void ADroneActor::Grappled(FTransform SphereCompTransform)
{
	if (GameModeBase) GameModeBase->AddScore(ScoreValue/2);
}

void ADroneActor::Reached(float AddSpeedAmount)
{
	if (this) HandleDeath();
}

/**
 * @brief Handles death of drone.
 */
void ADroneActor::HandleDeath()
{
	Super::HandleDeath();
	if (IsValid(DroppedEnemyActor)) DroppedEnemyActor->Destroy();
	if (IsValid(this)) Destroy();
}

/**
 * @brief Adjusts target location to avoid colliding with other drones.
 */
void ADroneActor::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                            UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA<ADroneActor>())
	{
		TargetLocation += AvoidanceRadius*(GetActorLocation() - OtherActor->GetActorLocation()).GetSafeNormal();
	}
}

void ADroneActor::Move(FVector Target)
{
	InterceptTimer += GetWorld()->GetDeltaSeconds();
	SphereComp->AddRelativeLocation((TargetLocation-GetActorLocation()).GetSafeNormal()*InterceptSpeed*GetWorld()->GetDeltaSeconds());
	if (InterceptTimer >= InterceptTime)
	{
		ChangeState(EDroneState::Attacking);
	}
}
