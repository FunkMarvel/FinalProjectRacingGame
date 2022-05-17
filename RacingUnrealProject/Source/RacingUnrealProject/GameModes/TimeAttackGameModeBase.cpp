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
}

void ATimeAttackGameModeBase::BeginTimer()
{
	ToggleTiming(true);
}

void ATimeAttackGameModeBase::OnPressPause()
{
	Super::OnPressPause();
	bGamePaused = !bGamePaused;
	if (TimeAttackHUD) TimeAttackHUD->TogglePauseMenu(bGamePaused);
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

void ATimeAttackGameModeBase::OnCompletedLap()
{
	Super::OnCompletedLap();
	if (TimeAttackHUD) TimeAttackHUD->SetLapCounter(CurrentLap, NumberOfLaps);
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
		if (TimeAttackHUD) TimeAttackHUD->ToggleEndMenu(true);
	}
	CurrentPlayerData.PlayerTime = RaceTimer;
	FPlayerData* BestPlayer = RacingGameInstance->GetBestTimePlayer();
	if (BestPlayer)
	{
		TimeAttackHUD->SetBestTime(CurrentPlayerData.PlayerTime, BestPlayer->PlayerTime);
	}
	else
	{
		TimeAttackHUD->SetBestTime(CurrentPlayerData.PlayerTime, CurrentPlayerData.PlayerTime);
	}
	RacingGameInstance->SavePlayerData(CurrentPlayerData);
	if(SaveGame()) DL_NORMAL("Save Succesful");
}
