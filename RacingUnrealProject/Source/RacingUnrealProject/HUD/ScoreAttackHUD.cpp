// Fill out your copyright notice in the Description page of Project Settings.


#include "ScoreAttackHUD.h"

#include "ScoreAttackWidgets/ScoreCounterWidget.h"
#include "PauseMenuWidget.h"
#include "Components/Button.h"
#include "RacingUnrealProject/CarPawn.h"
#include "ScoreAttackWidgets/ScoreAttackEndMenuWidget.h"

void AScoreAttackHUD::BeginPlay()
{
	Super::BeginPlay();
	GameModeBase = Cast<AScoreAttackGameModeBase>(GetWorld()->GetAuthGameMode());

	if (ScoreWidgetClass)
	{
		ScoreCounterWidget = CreateWidget<UScoreCounterWidget>(GetWorld(), ScoreWidgetClass);
		ScoreCounterWidget->AddToViewport();
		ScoreCounterWidget->SetVisibility(ESlateVisibility::Visible);
		ScoreCounterWidget->UpdateLapCounter(GameModeBase->CurrentLap, GameModeBase->NumberOfLaps);
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
		ACarPawn* PlayerPawn = Cast<ACarPawn>(GetWorld()->GetFirstPlayerController()->GetPawn());
		if (PlayerPawn)
		{
			PauseMenuWidget->ResetToCheckpoint->OnClicked.AddDynamic(PlayerPawn, &ACarPawn::ResetCarToLastCheckpoint);
			PauseMenuWidget->ResetToCheckpoint->OnClicked.AddDynamic(this, &AScoreAttackHUD::OnResume);
		}
	}
}

void AScoreAttackHUD::SetLapCounter(int32 CurrentLap, int32 MaxNumLaps)
{
	ScoreCounterWidget->UpdateLapCounter(CurrentLap, MaxNumLaps);
}

void AScoreAttackHUD::SetScore(int32 CurrentScore)
{
	ScoreCounterWidget->UpdateScore(CurrentScore);
}

void AScoreAttackHUD::SetBestScore(int32 CurrentScore, int32 BestScore)
{
	FString CurrentScoreString{FString::Printf(TEXT("%05d"), CurrentScore)};
	FString BestScoreString{FString::Printf(TEXT("%05d"), BestScore)};
	ScoreAttackEndMenuWidget->SetTimeText(FText::FromString(CurrentScoreString), FText::FromString(BestScoreString));
}

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

void AScoreAttackHUD::OnResume()
{
	GameModeBase->OnPressPause();
}
