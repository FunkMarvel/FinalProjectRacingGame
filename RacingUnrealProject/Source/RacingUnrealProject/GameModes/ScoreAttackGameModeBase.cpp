// Fill out your copyright notice in the Description page of Project Settings.


#include "ScoreAttackGameModeBase.h"

#include "Kismet/GameplayStatics.h"
#include "RacingUnrealProject/HUD/ScoreAttackHUD.h"

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
	FPlayerData* BestPlayer = RacingGameInstance->GetBestTimePlayer();
	if (BestPlayer)
	{
		AttackHUD->SetBestScore(CurrentPlayerData.PlayerScore, BestPlayer->PlayerScore);
	}
	else
	{
		AttackHUD->SetBestScore(CurrentPlayerData.PlayerScore, CurrentPlayerData.PlayerScore);
	}
	RacingGameInstance->SavePlayerData(CurrentPlayerData);
}

void AScoreAttackGameModeBase::AddScore(int32 Score)
{
	Super::AddScore(Score);
	CurrentScore += Score;
	AttackHUD->SetScore(CurrentScore);
}
