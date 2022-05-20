// Fill out your copyright notice in the Description page of Project Settings.


#include "ScoreAttackHUD.h"

#include "ScoreAttackWidgets/ScoreCounterWidget.h"
#include "PauseMenuWidget.h"
#include "Components/Button.h"
#include "RacingUnrealProject/CarPawn.h"
#include "RacingUnrealProject/Widget/SpeedIndicatorWidget.h"
#include "ScoreAttackWidgets/ScoreAttackEndMenuWidget.h"

void AScoreAttackHUD::BeginPlay()
{
	Super::BeginPlay();
	GameModeBase = Cast<AScoreAttackGameModeBase>(GetWorld()->GetAuthGameMode());

	// sets up widgets:
	if (ScoreWidgetClass)
	{
		ScoreCounterWidget = CreateWidget<UScoreCounterWidget>(GetWorld(), ScoreWidgetClass);
		ScoreCounterWidget->AddToViewport();
		ScoreCounterWidget->SetVisibility(ESlateVisibility::Visible);
		ScoreCounterWidget->UpdateLapCounter(GameModeBase->CurrentLap, GameModeBase->NumberOfLaps);

		FSlateColor SlateColor{GameModeBase->ChangeGoalColor(EGoals::None)};
		ScoreCounterWidget->UpdateScore(0, GameModeBase->GetCurrentGoalScore(), SlateColor);
	}

	if (ScoreAttackEndMenuClass)
	{
		ScoreAttackEndMenuWidget = CreateWidget<UScoreAttackEndMenuWidget>(GetWorld(), ScoreAttackEndMenuClass);
		ScoreAttackEndMenuWidget->AddToViewport();
		ScoreAttackEndMenuWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
	
	if (PauseMenuClass)
	{
		PauseMenuWidget = CreateWidget<UPauseMenuWidget>(GetWorld(), PauseMenuClass);
		PauseMenuWidget->AddToViewport();
		PauseMenuWidget->SetVisibility(ESlateVisibility::Collapsed);
		PauseMenuWidget->ResumeButton->OnClicked.AddDynamic(this, &AScoreAttackHUD::OnResume);
		PauseMenuWidget->ReturnToMenuButton->OnClicked.AddDynamic(ScoreAttackEndMenuWidget, &UScoreAttackEndMenuWidget::OnBackToMenu);
		
		PlayerPawn = Cast<ACarPawn>(GetWorld()->GetFirstPlayerController()->GetPawn());
		if (PlayerPawn && PauseMenuWidget)
		{
			// sets up pause menu buttons.
			PauseMenuWidget->ResetToCheckpoint->OnClicked.AddDynamic(PlayerPawn, &ACarPawn::ResetCarToLastCheckpoint);
			PauseMenuWidget->ResetToCheckpoint->OnClicked.AddDynamic(this, &AScoreAttackHUD::OnResume);
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

/**
 * @brief update lap counter
 * @param CurrentLap number of current lap
 * @param MaxNumLaps number of total laps
 */
void AScoreAttackHUD::SetLapCounter(int32 CurrentLap, int32 MaxNumLaps)
{
	ScoreCounterWidget->UpdateLapCounter(CurrentLap, MaxNumLaps);
}

/**
 * @brief updates score counter
 * @param CurrentScore current score
 * @param CurrentGoalScore current goal to beat
 * @param CurrentColor color of current goal
 */
void AScoreAttackHUD::SetScore(int32 CurrentScore, int32 CurrentGoalScore, FSlateColor &CurrentColor)
{
	ScoreCounterWidget->UpdateScore(CurrentScore, CurrentGoalScore, CurrentColor);
}

/**
 * @brief sets scores on end menu
 * @param CurrentScore score achieved by player
 * @param BestScore high score
 * @param CurrentColor color of highest goal beat by player
 */
void AScoreAttackHUD::SetBestScore(int32 CurrentScore, int32 BestScore, FSlateColor &CurrentColor)
{
	FString CurrentScoreString{FString::Printf(TEXT("%05d"), CurrentScore)};
	FString BestScoreString{FString::Printf(TEXT("%05d"), BestScore)};
	ScoreAttackEndMenuWidget->SetTimeText(FText::FromString(CurrentScoreString), FText::FromString(BestScoreString), CurrentColor);
}

/**
 * @brief toggles end menu
 */
void AScoreAttackHUD::ToggleEndMenu(bool bShowMenu)
{
	if (ScoreWidgetClass && PauseMenuWidget && ScoreAttackEndMenuWidget && !PauseMenuWidget->IsVisible())
	{
		if (bShowMenu)
		{
			ScoreCounterWidget->SetVisibility(ESlateVisibility::Collapsed);
			ScoreAttackEndMenuWidget->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			ScoreCounterWidget->SetVisibility(ESlateVisibility::Visible);
			ScoreAttackEndMenuWidget->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

/**
 * @brief toggles pause menu
 */
void AScoreAttackHUD::TogglePauseMenu(bool bShowMenu)
{
	if (ScoreWidgetClass && PauseMenuWidget && ScoreAttackEndMenuWidget && !ScoreAttackEndMenuWidget->IsVisible())
	{
		if (bShowMenu)
		{
			ScoreCounterWidget->SetVisibility(ESlateVisibility::Collapsed);
			PauseMenuWidget->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			ScoreCounterWidget->SetVisibility(ESlateVisibility::Visible);
			PauseMenuWidget->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

void AScoreAttackHUD::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (ScoreCounterWidget) ScoreCounterWidget->SetSpeedOMeter(PlayerPawn->GetCurrentForwardSpeed());
}

/**
 * @brief resumes game if paused.
 */
void AScoreAttackHUD::OnResume()
{
	GameModeBase->OnPressPause();
}
