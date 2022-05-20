// Fill out your copyright notice in the Description page of Project Settings.


#include "TimeAttackGameModeBase.h"

#include "RacingGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "RacingUnrealProject/DebugLog.h"
#include "RacingUnrealProject/GameStartSequenceActor.h"
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
		// delays timing of player until countdown finishes
		GameStartSequence = Cast<AGameStartSequenceActor>(StartSequenceActors[0]);
		GameStartSequence->StartSequenceFinishedEvent.AddDynamic(this, &ATimeAttackGameModeBase::BeginTimer);
	}
	if (PlayerController)
	{
		FInputModeGameOnly InputMode{};
		InputMode.SetConsumeCaptureMouseDown(true);
		PlayerController->SetInputMode(InputMode);
	}

	TimeAttackHUD = Cast<ATimeAttackHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());
	CurrentBestGoal = EGoals::Gold;
}

/**
 * @brief Starts timing of player.
 */
void ATimeAttackGameModeBase::BeginTimer()
{
	ToggleTiming(true);
}

/**
 * @brief toggles pause menu.
 */
void ATimeAttackGameModeBase::OnPressPause()
{
	Super::OnPressPause();
	bGamePaused = !bGamePaused;
	if (TimeAttackHUD) TimeAttackHUD->TogglePauseMenu(bGamePaused); // stops timing when paused.
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

/**
 * @brief counts up lap counter.
 */
void ATimeAttackGameModeBase::OnCompletedLap()
{
	Super::OnCompletedLap();
	if (TimeAttackHUD) TimeAttackHUD->SetLapCounter(CurrentLap, NumberOfLaps);
}

/**
 * @brief toggles end state and saves time to instance and file.
 */
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
		if (TimeAttackHUD) TimeAttackHUD->ToggleEndMenu(true);
	}
	CurrentPlayerData.PlayerTime = RaceTimer;
	FPlayerData* BestPlayer = RacingGameInstance->GetBestTimePlayer();
	FSlateColor CurrentColor{ChangeGoalColor(CurrentBestGoal)};
	if (BestPlayer)
	{
		TimeAttackHUD->SetBestTime(CurrentPlayerData.PlayerTime, BestPlayer->PlayerTime, CurrentColor);
	}
	else
	{
		TimeAttackHUD->SetBestTime(CurrentPlayerData.PlayerTime, CurrentPlayerData.PlayerTime, CurrentColor);
	}
	RacingGameInstance->SavePlayerData(CurrentPlayerData);
	if(SaveGame()) DL_NORMAL("Save Succesful");
}
