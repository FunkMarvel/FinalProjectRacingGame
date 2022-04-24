// Fill out your copyright notice in the Description page of Project Settings.


#include "TimeAttackGameModeBase.h"

#include "RacingGameInstance.h"
#include "Components/ArrowComponent.h"
#include "Kismet/GameplayStatics.h"
#include "RacingUnrealProject/GameStartSequenceActor.h"
#include "RacingUnrealProject/Checkpoint/Checkpoint.h"
#include "RacingUnrealProject/HUD/TimeAttackHUD.h"

ATimeAttackGameModeBase::ATimeAttackGameModeBase()
{
}

void ATimeAttackGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> StartSequenceActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGameStartSequenceActor::StaticClass(), StartSequenceActors);

	if (StartSequenceActors.Num() >= 1)
	{
		GameStartSequence = Cast<AGameStartSequenceActor>(StartSequenceActors[0]);
		GameStartSequence->StartSequenceFinishedEvent.AddDynamic(this, &ATimeAttackGameModeBase::BeginTimer);
	}
	if (PlayerController)
	{
		FInputModeGameOnly InputMode{};
		InputMode.SetConsumeCaptureMouseDown(true);
		PlayerController->SetInputMode(InputMode);
	}
}

void ATimeAttackGameModeBase::BeginTimer()
{
	ToggleTiming(true);
	TimeAttackHUD = Cast<ATimeAttackHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());
}

void ATimeAttackGameModeBase::OnCompletedLap()
{
	FVector PlayerToGoalDir{(GoalCheckpoint->GetActorLocation() - PlayerPawn->GetActorLocation()).GetSafeNormal()};
	if (FVector::DotProduct(PlayerToGoalDir, GoalCheckpoint->GetSpawnArrow()->GetForwardVector()) > 0.f)
	{
		CurrentLap++;
	}
	else if (FVector::DotProduct(PlayerToGoalDir, GoalCheckpoint->GetSpawnArrow()->GetForwardVector()) < 0.f)
	{
		CurrentLap--;
	}
	if (TimeAttackHUD) TimeAttackHUD->SetLapCounter(CurrentLap, NumberOfLaps);
	if (CurrentLap >= NumberOfLaps) GameEndState();
}

void ATimeAttackGameModeBase::GameEndState()
{
	Super::GameEndState();
	ToggleTiming(false);
	if (PlayerController)
	{
		PlayerController->SetShowMouseCursor(true);
		FInputModeUIOnly InputMode{};
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);
		PlayerController->SetInputMode(InputMode);
	}
	RacingGameInstance->SaveTime(RaceTimer);
}
