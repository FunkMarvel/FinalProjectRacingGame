// Fill out your copyright notice in the Description page of Project Settings.


#include "SpikyBallEnemyActor.h"

#include "../CarPawn.h"
#include "../GravitySplineActor.h"
#include "Components/SphereComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "RacingUnrealProject/DebugLog.h"

ASpikyBallEnemyActor::ASpikyBallEnemyActor() : ABaseEnemyActor()
{
	SphereComp->SetSimulatePhysics(true);
	SphereComp->SetEnableGravity(false);
	SphereComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	SphereComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	SphereComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Block);
	SphereComp->SetSphereRadius(200.f);
}

void ASpikyBallEnemyActor::BeginPlay()
{
	Super::BeginPlay();
	LocalUpVector = GetActorUpVector();
	SphereComp->OnComponentBeginOverlap.AddDynamic(this, &ASpikyBallEnemyActor::OnBeginOverLap);
	AGravitySplineActor* ClosestGravitySpline{};
	ClosestGravitySpline = GetClosestGravitySpline();
	if (ClosestGravitySpline)
	{
		GravitySplineActive = ClosestGravitySpline;
	}
	SetUpVectorAsSplineUpAxis();
	RotateSphereComponentToLocalUpVector();
}

void ASpikyBallEnemyActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	// if (CurrentState == EBallState::Spiked && GetToPlayerVector(false).Size() > TargetDistance) { Destroy(); }
	// else if (GetToPlayerVector(false).Size() > TargetDistance) { return; }
	if (GravitySplineActive) LocalUpVector = GravitySplineActive->GetFixedUpVectorFromLocation(GetActorLocation());
	
	switch (CurrentState)
	{
	case EBallState::Airborne:
		AirborneState();
		break;
	case EBallState::OnGround:
		GroundedState();
		break;
	case EBallState::Spiked:
		SpikedState();
		break;
	case EBallState::Idle:
		// DL_NORMAL(TEXT("Idle"));
		break;
	}
}

void ASpikyBallEnemyActor::OnBeginOverLap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                          UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                          const FHitResult& SweepResult)
{
	Super::OnBeginOverLap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	if (OtherActor->IsA<ASpikyBallEnemyActor>())
	{
		SphereComp->AddImpulse(SweepResult.ImpactNormal*FVector::DotProduct(SphereComp->GetPhysicsLinearVelocity(),
			SweepResult.ImpactNormal));
	}
}

void ASpikyBallEnemyActor::SetUpVectorAsSplineUpAxis()
{
	if (GravitySplineActive)
	{
		LocalUpVector = GravitySplineActive->GetFixedUpVectorFromLocation(SphereComp->GetComponentLocation());
		FRotator NewRot = UKismetMathLibrary::MakeRotFromZX(LocalUpVector, GetActorForwardVector());
		SetActorRotation(NewRot);
	}
}

void ASpikyBallEnemyActor::RotateSphereComponentToLocalUpVector()
{
	FRotator NewSphereRot = UKismetMathLibrary::MakeRotFromZX(LocalUpVector, GetActorForwardVector());
	SphereComp->SetWorldRotation(NewSphereRot);
}

void ASpikyBallEnemyActor::ChangeState(EBallState NewState)
{
	bEnteringState = true;
	CurrentState = NewState;
}

void ASpikyBallEnemyActor::AirborneState()
{
	if (bEnteringState) bEnteringState = false;
	if (IsGrounded()) ChangeState(EBallState::OnGround);
	ApplyGravity();
	// DL_NORMAL(TEXT("Airborne"));
}

void ASpikyBallEnemyActor::GroundedState()
{
	if (bEnteringState)
	{
		// if (SphereComp->IsSimulatingPhysics())
		// {
		// 	SphereComp->SetSimulatePhysics(false);
		// 	SphereComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		// }
		GetWorld()->GetTimerManager().SetTimer(SpikeTimerHandle, this, &ASpikyBallEnemyActor::TriggerSpikes,
		                                       SpikeTimer);
		bEnteringState = false;
	}
	LookAtPlayer();
	ApplyGravity();
	// DL_NORMAL(TEXT("Grounded"));
}

void ASpikyBallEnemyActor::SpikedState()
{
	if (bEnteringState)
	{
		bEnteringState = false;
	}
	Move();
	ApplyGravity();
	// UE_LOG(LogTemp, Warning, TEXT("Spiked"));
	// DL_NORMAL(TEXT("Spiked"));
}

void ASpikyBallEnemyActor::LookAtPlayer()
{
	FRotator CurrentRot{GetActorRotation()};
	FVector NewDir{(PlayerPawn->GetActorLocation() - GetActorLocation()).GetSafeNormal()};
	FRotator NewRot = FMath::RInterpTo(CurrentRot, NewDir.Rotation(), GetWorld()->GetDeltaSeconds(), 0.75f);

	SetActorRotation(NewRot, ETeleportType::TeleportPhysics);
}

void ASpikyBallEnemyActor::Move()
{
	FVector MoveDir{(PlayerPawn->GetActorLocation() - GetActorLocation()).GetSafeNormal()};
	// MoveDir.Z = 0.f;
	if (SphereComp->GetPhysicsLinearVelocity().Size() <= MaxSpeed)
	{
		SphereComp->AddForce(MoveDir * Acceleration, NAME_None, true);
	}
}

void ASpikyBallEnemyActor::ApplyGravity()
{
	SphereComp->AddForce(-LocalUpVector * Acceleration * 3.f, NAME_None, true);
}
