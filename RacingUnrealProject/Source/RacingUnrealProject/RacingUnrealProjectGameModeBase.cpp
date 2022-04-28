// Copyright Epic Games, Inc. All Rights Reserved.


#include "RacingUnrealProjectGameModeBase.h"

#include "Checkpoint/Checkpoint.h"
#include "Components/ArrowComponent.h"
#include "GameModes/RacingGameInstance.h"

void ARacingUnrealProjectGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	PlayerPawn = GetWorld()->GetFirstPlayerController()->GetPawn();
	RacingGameInstance = Cast<URacingGameInstance>(GetGameInstance());
	PlayerController = GetWorld()->GetFirstPlayerController();
	if (RacingGameInstance)
	{
		NumberOfLaps = RacingGameInstance->NumberOfLaps;
	}
}

void ARacingUnrealProjectGameModeBase::OnCompletedLap()
{
	FVector PlayerToGoalDir{(GoalCheckpoint->GetActorLocation() - PlayerPawn->GetActorLocation()).GetSafeNormal()};
	if (CurrentLap >= NumberOfLaps) GameEndState();
	if (FVector::DotProduct(PlayerToGoalDir, GoalCheckpoint->GetSpawnArrow()->GetForwardVector()) > 0.f)
	{
		if (bFirstLap)
		{
			bFirstLap = false;
			return;
		}
		CurrentLap++;
	}
	else if (FVector::DotProduct(PlayerToGoalDir, GoalCheckpoint->GetSpawnArrow()->GetForwardVector()) < 0.f)
	{
		CurrentLap--;
	}
}

void ARacingUnrealProjectGameModeBase::GameEndState()
{
}

void ARacingUnrealProjectGameModeBase::OnPressPause()
{
}

void ARacingUnrealProjectGameModeBase::AddScore(int32 Score)
{
}
