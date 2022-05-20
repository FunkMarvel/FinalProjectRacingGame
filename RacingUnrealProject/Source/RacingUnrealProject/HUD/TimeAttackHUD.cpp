// Fill out your copyright notice in the Description page of Project Settings.


#include "TimeAttackHUD.h"

#include "PauseMenuWidget.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "RacingUnrealProject/CarPawn.h"
#include "RacingUnrealProject/GameModes/TimeAttackGameModeBase.h"
#include "RacingUnrealProject/Widget/SpeedIndicatorWidget.h"
#include "TimeAttackWidgets/RaceTimerWidget.h"
#include "TimeAttackWidgets/TimeAttackEndMenuWidget.h"

ATimeAttackHUD::ATimeAttackHUD()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ATimeAttackHUD::BeginPlay()
{
	Super::BeginPlay();
	GameModeBase = Cast<ATimeAttackGameModeBase>(GetWorld()->GetAuthGameMode());

	// setting up widgets:
	if (TimerWidgetClass)
	{
		RaceTimerWidget = CreateWidget<URaceTimerWidget>(GetWorld(), TimerWidgetClass);
		RaceTimerWidget->AddToViewport();
		RaceTimerWidget->UpdateLapCounter(GameModeBase->CurrentLap, GameModeBase->NumberOfLaps);
		// setting initial goal to beat as lowest time.
		CurrentGoalColor = GameModeBase->ChangeGoalColor(GameModeBase->CurrentBestGoal);
	}

	if (TimeAttackEndMenuClass)
	{
		TimeAttackEndMenuWidget = CreateWidget<UTimeAttackEndMenuWidget>(GetWorld(), TimeAttackEndMenuClass);
		TimeAttackEndMenuWidget->AddToViewport();
		TimeAttackEndMenuWidget->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (PauseMenuClass)
	{
		PauseMenuWidget = CreateWidget<UPauseMenuWidget>(GetWorld(), PauseMenuClass);
		PauseMenuWidget->AddToViewport();
		PauseMenuWidget->SetVisibility(ESlateVisibility::Collapsed);
		PauseMenuWidget->ResumeButton->OnClicked.AddDynamic(this, &ATimeAttackHUD::OnResume);
		PauseMenuWidget->ReturnToMenuButton->OnClicked.AddDynamic(TimeAttackEndMenuWidget, &UTimeAttackEndMenuWidget::OnBackToMenu);
		PlayerPawn = Cast<ACarPawn>(GetWorld()->GetFirstPlayerController()->GetPawn());
		if (PlayerPawn)
		{
			PauseMenuWidget->ResetToCheckpoint->OnClicked.AddDynamic(PlayerPawn, &ACarPawn::ResetCarToLastCheckpoint);
			PauseMenuWidget->ResetToCheckpoint->OnClicked.AddDynamic(this, &ATimeAttackHUD::OnResume);
		}
	}

	if (SpeedIndicatorClass) {
		SpeedIndicatorWidget = CreateWidget<USpeedIndicatorWidget>(GetWorld(), SpeedIndicatorClass);
		SpeedIndicatorWidget->AddToViewport();
		SpeedIndicatorWidget->SetVisibility(ESlateVisibility::HitTestInvisible);

		if (PlayerPawn) SpeedIndicatorWidget->CarPawn = PlayerPawn;

		SpeedIndicatorWidget->PsudoBeginPlay();
	}
}

void ATimeAttackHUD::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (GameModeBase->IsTiming()) GameModeBase->RaceTimer += DeltaSeconds;

	// setting initial goal to beat as lowest time.
	// if player does not beat level in time, easier goal is set.
	if (GameModeBase->RaceTimer >= GameModeBase->GetCurrentGoalTime())
	{
		--GameModeBase->CurrentBestGoal;
		CurrentGoalColor = GameModeBase->ChangeGoalColor(GameModeBase->CurrentBestGoal);
	}
	
	if (RaceTimerWidget) RaceTimerWidget->UpdateTimer(GameModeBase->RaceTimer, GameModeBase->GetCurrentGoalTime(), CurrentGoalColor);
	if (RaceTimerWidget) RaceTimerWidget->SetSpeedOMeter(PlayerPawn->GetCurrentForwardSpeed());
}

/**
 * @brief update lap counter
 * @param CurrentLap number of current lap
 * @param MaxNumLaps number of total laps
 */
void ATimeAttackHUD::SetLapCounter(int32 CurrentLap, int32 MaxNumLaps)
{
	RaceTimerWidget->UpdateLapCounter(CurrentLap, MaxNumLaps);
}

void ATimeAttackHUD::SetBestTime(float CurrentTime, float BestTime, FSlateColor SlateColor)
{
	TimeAttackEndMenuWidget->SetTimeText(
		TimeAttackEndMenuWidget->GetTimeTextFromFloat(CurrentTime),
		TimeAttackEndMenuWidget->GetTimeTextFromFloat(BestTime), SlateColor);
}

/**
 * @brief toggles end menu
 */
void ATimeAttackHUD::ToggleEndMenu(bool bShowMenu)
{
	if (PauseMenuWidget && !PauseMenuWidget->IsVisible())
	{
		if (bShowMenu)
		{
			if (RaceTimerWidget) RaceTimerWidget->SetVisibility(ESlateVisibility::Collapsed);
			if (TimeAttackEndMenuWidget) TimeAttackEndMenuWidget->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			if (RaceTimerWidget) RaceTimerWidget->SetVisibility(ESlateVisibility::Visible);
			if (TimeAttackEndMenuWidget) TimeAttackEndMenuWidget->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

/**
 * @brief toggles pause menu
 */
void ATimeAttackHUD::TogglePauseMenu(bool bShowMenu)
{
	if (TimeAttackEndMenuWidget && !TimeAttackEndMenuWidget->IsVisible())
	{
		if (bShowMenu)
		{
			if (RaceTimerWidget) RaceTimerWidget->SetVisibility(ESlateVisibility::Collapsed);
			if (PauseMenuWidget) PauseMenuWidget->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			if (RaceTimerWidget) RaceTimerWidget->SetVisibility(ESlateVisibility::Visible);
			if (PauseMenuWidget) PauseMenuWidget->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

/**
 * @brief resumes game if paused.
 */
void ATimeAttackHUD::OnResume()
{
	GameModeBase->OnPressPause();
}
