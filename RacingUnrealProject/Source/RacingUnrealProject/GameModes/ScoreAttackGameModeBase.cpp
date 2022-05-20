// Fill out your copyright notice in the Description page of Project Settings.


#include "ScoreAttackGameModeBase.h"

#include "Kismet/GameplayStatics.h"
#include "RacingUnrealProject/DebugLog.h"
#include "RacingUnrealProject/HUD/ScoreAttackHUD.h"

/**
 * @brief game mode base for score attack.
 */
AScoreAttackGameModeBase::AScoreAttackGameModeBase()
{
}

void AScoreAttackGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	if (PlayerController)
	{
		FInputModeGameOnly InputMode{};
		InputMode.SetConsumeCaptureMouseDown(true);
		PlayerController->SetInputMode(InputMode);
	}
	AttackHUD = Cast<AScoreAttackHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());
}

/**
 * @brief toggles pause menu.
 */
void AScoreAttackGameModeBase::OnPressPause()
{
	Super::OnPressPause();
	bGamePaused = !bGamePaused;
	if (AttackHUD) AttackHUD->TogglePauseMenu(bGamePaused);
	if (bGamePaused)
	{
		PlayerController->SetShowMouseCursor(true);
		FInputModeGameAndUI InputMode{};
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);
		PlayerController->SetInputMode(InputMode);
	}
	else
	{
		FInputModeGameOnly InputMode{};
		PlayerController->SetShowMouseCursor(false);
		InputMode.SetConsumeCaptureMouseDown(true);
		PlayerController->SetInputMode(InputMode);
	}
	UGameplayStatics::SetGamePaused(GetWorld(), bGamePaused);
}

void AScoreAttackGameModeBase::OnCompletedLap()
{
	Super::OnCompletedLap();
	AttackHUD->SetLapCounter(CurrentLap, NumberOfLaps);
}

/**
 * @brief toggles end menu and saves game to instance and file.
 */
void AScoreAttackGameModeBase::GameEndState()
{
	Super::GameEndState();
	if (PlayerController)
	{
		PlayerController->SetShowMouseCursor(true);
		FInputModeUIOnly InputMode{};
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);
		PlayerController->SetInputMode(InputMode);
		if (AttackHUD) AttackHUD->ToggleEndMenu(true);
	}
	CurrentPlayerData.PlayerScore = CurrentScore;
	FPlayerData* BestPlayer = RacingGameInstance->GetBestScorePlayer();
	FSlateColor CurrentColor{ChangeGoalColor(CurrentBestGoal)};
	if (BestPlayer)
	{
		AttackHUD->SetBestScore(CurrentPlayerData.PlayerScore, BestPlayer->PlayerScore, CurrentColor);
	}
	else
	{
		AttackHUD->SetBestScore(CurrentPlayerData.PlayerScore, CurrentPlayerData.PlayerScore, CurrentColor);
	}
	RacingGameInstance->SavePlayerData(CurrentPlayerData);
	if(SaveGame()) DL_NORMAL("Save Succesful");
}

/**
 * @brief Adds points to score tracker.
 * @param Score int32 - number of points to add.
 */
void AScoreAttackGameModeBase::AddScore(int32 Score)
{
	Super::AddScore(Score);
	
	int32 CurrentGoalScore{};
	FSlateColor SlateColor = ChangeGoalColor(EGoals::None);

	CurrentScore += Score;

	// changes color and displayed goal when exceeding the previous goal.
	if (RacingGameInstance->ScoresToBeat.Num() > EGoals::Gold)
	{
		if (CurrentScore >= RacingGameInstance->ScoresToBeat[CurrentBestGoal]) ++CurrentBestGoal;
		CurrentGoalScore = RacingGameInstance->ScoresToBeat[CurrentBestGoal];
		SlateColor = ChangeGoalColor(CurrentBestGoal);
	}
	
	AttackHUD->SetScore(CurrentScore, CurrentGoalScore, SlateColor);
}
