// Copyright Epic Games, Inc. All Rights Reserved.


#include "RacingUnrealProjectGameModeBase.h"

#include "CarPawn.h"

#include "Checkpoint/Checkpoint.h"
#include "Components/ArrowComponent.h"
#include "Components/SphereComponent.h"
#include "GameModes/RacingGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "SaveGameObjects/RacingSaveGame.h"

void ARacingUnrealProjectGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	PlayerPawn = Cast<ACarPawn>(GetWorld()->GetFirstPlayerController()->GetPawn());
	RacingGameInstance = Cast<URacingGameInstance>(GetGameInstance());
	PlayerController = GetWorld()->GetFirstPlayerController();
	if (RacingGameInstance)
	{
		NumberOfLaps = RacingGameInstance->NumberOfLaps;
	}
}

/**
 * @brief Increments or decrements lap count
 */
void ARacingUnrealProjectGameModeBase::OnCompletedLap()
{
	if (bGameEnded) return;
	FVector PlayerToGoalDir{(GoalCheckpoint->GetActorLocation() - PlayerPawn->GetActorLocation()).GetSafeNormal()};
	if (CurrentLap >= NumberOfLaps) GameEndState();
	if (FVector::DotProduct(PlayerToGoalDir, GoalCheckpoint->GetSpawnArrow()->GetForwardVector()) > 0.f)
	{
		if (bFirstLap)
		{	// player passes through start line twice on first lap, so first crossing is ignored.
			bFirstLap = false;
			return;
		}
		CurrentLap++;  // increments lap if start line is crossed in right direction.
	}
	else if (FVector::DotProduct(PlayerToGoalDir, GoalCheckpoint->GetSpawnArrow()->GetForwardVector()) < 0.f)
	{
		CurrentLap--; // decrements lap if start line is crossed in wrong direction.
	}
}

/**
 * @brief Switches on end game menu, and decelerates ship.
 */
void ARacingUnrealProjectGameModeBase::GameEndState()
{
	if (bGameEnded) return;
	bGameEnded = true;
	PlayerPawn->EnterState(EVehicleState::Finished);
}

/**
 * @brief Toggles paused state and pause menu.
 */
void ARacingUnrealProjectGameModeBase::OnPressPause()
{
	if (FName(GetWorld()->GetMapName()) == RacingGameInstance->LevelNames[0]) return;  // no need to pause main menu
	if (!bGamePaused)  // following code ensures no forces accumulate on vehicle while game is paused:
	{
		// stores current velocity and spin of vehicle before zeroing all physics
		CurrentVel = PlayerPawn->SphereComp->GetPhysicsLinearVelocity();
		CurrentAngVel = PlayerPawn->SphereComp->GetPhysicsAngularVelocityInRadians();
		PlayerPawn->SphereComp->SetSimulatePhysics(false);
	}
	else
	{
		// enables physics and applies previous state
		PlayerPawn->SphereComp->SetSimulatePhysics(true);
		PlayerPawn->SphereComp->SetPhysicsLinearVelocity(CurrentVel);
		PlayerPawn->SphereComp->SetPhysicsAngularVelocityInRadians(CurrentAngVel);
	}
}

/**
 * @brief Adds points to score tracker if in ScoreAttack game mode.
 * @param Score number of points to add.
 */
void ARacingUnrealProjectGameModeBase::AddScore(int32 Score)
{
	// this is just a base class for polymorphism, actual implementation in ScoreAttackGameModeBase.
}

/**
 * @brief Attempts to save high score and best time to file.
 * @return bool - true if save succeeded and false if save failed.
 */
bool ARacingUnrealProjectGameModeBase::SaveGame()
{
	if (URacingSaveGame* RacingSaveGame = Cast<URacingSaveGame>(UGameplayStatics::CreateSaveGameObject(URacingSaveGame::StaticClass())))
	{
		RacingSaveGame->SetGameData(RacingGameInstance->GetBestTimePlayer()->PlayerTime, RacingGameInstance->GetBestScorePlayer()->PlayerScore);

		if (UGameplayStatics::SaveGameToSlot(RacingSaveGame, RacingGameInstance->DefaultSaveSlotName, RacingGameInstance->DefaultSaveSlotIndex))
			return true;
	}
	return false;
}

/**
 * @brief Attempts to load high score and best time from file.
 * @return bool - true if load succeeded and false if load failed.
 */
bool ARacingUnrealProjectGameModeBase::LoadGame()
{
	if (URacingSaveGame* RacingSaveGame =
		Cast<URacingSaveGame>(UGameplayStatics::LoadGameFromSlot(RacingGameInstance->DefaultSaveSlotName,
			RacingGameInstance->DefaultSaveSlotIndex)))
	{
		FPlayerData BestPlayer{RacingSaveGame->BestPlayerTime, RacingSaveGame->BestPlayerScore};
		RacingGameInstance->SavePlayerData(BestPlayer);
		return true;
	}
	return false;
}

/**
 * @brief Gives a FSlateColor for coloring text based on current goal reached.
 * @param CurrentGoal Current goal reached.
 * @return FSlateColor - white if no goal reached, else returns bronze, silver or gold depending on current goal reached.
 */
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
