// Fill out your copyright notice in the Description page of Project Settings.


#include "TimeAttackHUD.h"

#include "PauseMenuWidget.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "RacingUnrealProject/GameModes/TimeAttackGameModeBase.h"
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

	if (TimerWidgetClass)
	{
		RaceTimerWidget = CreateWidget<URaceTimerWidget>(GetWorld(), TimerWidgetClass);
		RaceTimerWidget->AddToViewport();
		RaceTimerWidget->UpdateLapCounter(GameModeBase->CurrentLap, GameModeBase->NumberOfLaps);
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
	}
}

void ATimeAttackHUD::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (GameModeBase->IsTiming()) GameModeBase->RaceTimer += DeltaSeconds;
	if (RaceTimerWidget) RaceTimerWidget->UpdateTimer(GameModeBase->RaceTimer);
}

void ATimeAttackHUD::SetLapCounter(int32 CurrentLap, int32 MaxNumLaps)
{
	RaceTimerWidget->UpdateLapCounter(CurrentLap, MaxNumLaps);
}

void ATimeAttackHUD::SetBestTime(float CurrentTime, float BestTime)
{
	TimeAttackEndMenuWidget->SetTimeText(
		TimeAttackEndMenuWidget->GetTimeTextFromFloat(CurrentTime),
		TimeAttackEndMenuWidget->GetTimeTextFromFloat(BestTime));
}

void ATimeAttackHUD::ToggleEndMenu(bool bShowMenu)
{
	if (TimeAttackEndMenuWidget && RaceTimerWidget && PauseMenuWidget && !PauseMenuWidget->IsVisible())
	{
		if (bShowMenu)
		{
			RaceTimerWidget->SetVisibility(ESlateVisibility::Collapsed);
			TimeAttackEndMenuWidget->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			RaceTimerWidget->SetVisibility(ESlateVisibility::Visible);
			TimeAttackEndMenuWidget->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

void ATimeAttackHUD::TogglePauseMenu(bool bShowMenu)
{
	if (TimeAttackEndMenuWidget && RaceTimerWidget && PauseMenuWidget && !TimeAttackEndMenuWidget->IsVisible())
	{
		if (bShowMenu)
		{
			RaceTimerWidget->SetVisibility(ESlateVisibility::Collapsed);
			PauseMenuWidget->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			RaceTimerWidget->SetVisibility(ESlateVisibility::Visible);
			PauseMenuWidget->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

void ATimeAttackHUD::OnResume()
{
	GameModeBase->OnPressPause();
}
