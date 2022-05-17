// Copyright Epic Games, Inc. All Rights Reserved.


#include "RacingUnrealProjectGameModeBase.h"

#include "DebugLog.h"
#include "Checkpoint/Checkpoint.h"
#include "Components/ArrowComponent.h"
#include "GameModes/RacingGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "SaveGameObjects/RacingSaveGame.h"

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
	if (bGameEnded) return;
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
	if (bGameEnded) return;
	bGameEnded = true;
}

void ARacingUnrealProjectGameModeBase::OnPressPause()
{
}

void ARacingUnrealProjectGameModeBase::AddScore(int32 Score)
{
}

bool ARacingUnrealProjectGameModeBase::SaveGame()
{
	if (URacingSaveGame* RacingSaveGame = Cast<URacingSaveGame>(UGameplayStatics::CreateSaveGameObject(URacingSaveGame::StaticClass())))
	{
		RacingSaveGame->SetGameData(RacingGameInstance->GetPlayerDataArray());

		if (UGameplayStatics::SaveGameToSlot(RacingSaveGame, RacingGameInstance->DefaultSaveSlotName, RacingGameInstance->DefaultSaveSlotIndex))
			return true;
	}
	return false;
}

bool ARacingUnrealProjectGameModeBase::LoadGame()
{
	if (URacingSaveGame* RacingSaveGame =
		Cast<URacingSaveGame>(UGameplayStatics::LoadGameFromSlot(RacingGameInstance->DefaultSaveSlotName,
			RacingGameInstance->DefaultSaveSlotIndex)))
	{
		for (int i{}; i < RacingSaveGame->PlayerScores.Num(); i++)
		{
			RacingGameInstance->SavePlayerData(FPlayerData(RacingSaveGame->PlayerTimes[i], RacingSaveGame->PlayerScores[i]));
		}
		return true;
	}
	return false;
}

FSlateColor ARacingUnrealProjectGameModeBase::ChangeGoalColor(EGoals CurrentGoal)
{
	FSlateColor SlateColor;
	switch (CurrentBestGoal)
	{
	case EGoals::None:
		SlateColor = FSlateColor(FLinearColor(FColorList::White));
		break;
	case EGoals::Bronze:
		SlateColor = FSlateColor(FLinearColor(FColorList::Bronze));
		break;
	case EGoals::Silver:
		SlateColor = FSlateColor(FLinearColor(FColorList::Silver));
		break;
	case EGoals::Gold:
		SlateColor = FSlateColor(FLinearColor(FColorList::Gold));
		break;
	}
	return SlateColor;
}
