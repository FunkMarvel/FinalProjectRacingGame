// Fill out your copyright notice in the Description page of Project Settings.


#include "DroneActor.h"

#include "SpikyBallEnemyActor.h"
#include "../CarPawn.h"
#include "../GrappleSphereComponent.h"
#include "../GravitySplineActor.h"
#include "../DebugLog.h"
#include "Components/SphereComponent.h"

ADroneActor::ADroneActor()
{
	GrappleSphereComponent = CreateDefaultSubobject<UGrappleSphereComponent>(TEXT("GrappleSphereComponent"));
	GrappleSphereComponent->SetupAttachment(GetRootComponent());
	GrappleSphereComponent->SetIsEnabled(true);
	GrappleSphereComponent->SetIsEatable(false);

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

	if (DroppableEnemyClass)
	{
		FAttachmentTransformRules AttachmentTransformRules(EAttachmentRule::SnapToTarget, true);
		AttachmentTransformRules.LocationRule = EAttachmentRule::KeepWorld;
		AttachmentTransformRules.ScaleRule = EAttachmentRule::KeepWorld;
		FVector SpawnLocation{GetActorLocation() - SpawnOffset*GetActorUpVector()};
		DroppedEnemyActor = GetWorld()->SpawnActor<ASpikyBallEnemyActor>(DroppableEnemyClass, SpawnLocation, GetActorRotation());
		DroppedEnemyActor->AttachToActor(this, AttachmentTransformRules);
		DroppedEnemyActor->ChangeState(ASpikyBallEnemyActor::Idle);
	}
	ScoreValue = 100;
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
	// if (!PlayerPawn)
	// {
	// 	PlayerPawn = Cast<ACarPawn>(GetWorld()->GetFirstPlayerController()->GetPawn());
	// }
	// else if (!GravitySplineActive)
 //    {
 //    	GravitySplineActive = PlayerPawn->GravitySplineActive;
 //    }
}

void ADroneActor::InterceptingState()
{
	if (bEnteringState)
	{
		InterceptSpeed = (TargetLocation-GetActorLocation()).Size()/InterceptTime;
		bEnteringState = false;
	}
	// TargetLocation = FVector::DotProduct(TargetLocation, PlayerPawn->GetActorForwardVector())*PlayerPawn->GetActorForwardVector() +
	// 	FVector::DotProduct(TargetLocation, PlayerPawn->GetActorRightVector())*PlayerPawn->GetActorRightVector() +
	// 		FVector::DotProduct(GetActorLocation(), PlayerPawn->GetActorUpVector())*PlayerPawn->GetActorUpVector();
	
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

	FRotator CurrentRot{GetActorRotation()};
	FRotator TargetRot{(PlayerPawn->GetActorLocation()-GetActorLocation()).Rotation()};
	FRotator NewRot{FMath::RInterpTo(CurrentRot, TargetRot, GetWorld()->GetDeltaSeconds(), 5.f)};
	SetActorRotation(NewRot);
	
	if (DroppedEnemyActor && DropTimer >= DropTime)
	{
		// FDetachmentTransformRules DetachmentTransformRules(EDetachmentRule::KeepWorld, false);
		DroppedEnemyActor->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		DroppedEnemyActor->SphereComp->SetSimulatePhysics(true);
		DroppedEnemyActor->SphereComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		DroppedEnemyActor->RotateSphereComponentToLocalUpVector();
		DroppedEnemyActor->ChangeState(ASpikyBallEnemyActor::Airborne);
		DroppedEnemyActor = nullptr;
	}
	else if (DropTimer < DropTime)
	{
		DropTimer += GetWorld()->GetDeltaSeconds();
	}
}

void ADroneActor::Grappled(FTransform SphereCompTransform)
{
	//TODO: Add behaviour for being grappled.
}

void ADroneActor::Reached(float AddSpeedAmount)
{
	//TODO: Add animation for destruction.
	HandleDeath();
}

void ADroneActor::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	DL_NORMAL("Overlapping")
	if (OtherActor->IsA<ADroneActor>())
	{
		DL_NORMAL("Adjusting target")
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
