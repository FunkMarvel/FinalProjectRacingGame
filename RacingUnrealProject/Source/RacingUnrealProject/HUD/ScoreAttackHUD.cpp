// Fill out your copyright notice in the Description page of Project Settings.


#include "ScoreAttackHUD.h"

#include "ScoreAttackWidgets/ScoreCounterWidget.h"
#include "PauseMenuWidget.h"

void AScoreAttackHUD::BeginPlay()
{
	Super::BeginPlay();
	GameModeBase = Cast<AScoreAttackGameModeBase>(GetWorld()->GetAuthGameMode());

	if (PauseMenuClass)
	{
		PauseMenuWidget = CreateWidget<UPauseMenuWidget>(GetWorld(), PauseMenuClass);
		PauseMenuWidget->AddToViewport();
		PauseMenuWidget->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (ScoreWidgetClass)
	{
		ScoreCounterWidget = CreateWidget<UScoreCounterWidget>(GetWorld(), ScoreWidgetClass);
		ScoreCounterWidget->AddToViewport();
		ScoreCounterWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void AScoreAttackHUD::SetLapCounter(int32 CurrentLap, int32 MaxNumLaps)
{
}

void AScoreAttackHUD::SetBestTime(float CurrentTime, float BestTime)
{
}

void AScoreAttackHUD::ToggleEndMenu(bool bShowMenu)
{
}

void AScoreAttackHUD::TogglePauseMenu(bool bShowMenu)
{
	if (ScoreWidgetClass && PauseMenuWidget)
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
}
