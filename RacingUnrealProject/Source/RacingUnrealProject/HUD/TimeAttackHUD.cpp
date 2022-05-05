// Fill out your copyright notice in the Description page of Project Settings.


#include "TimeAttackHUD.h"

#include "PauseMenuWidget.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "RacingUnrealProject/CarPawn.h"
#include "RacingUnrealProject/GameModes/TimeAttackGameModeBase.h"
#include "RacingUnrealProject/Widget/SpeedIndicatorWidget.h"
#include "TimeAttackWidgets/MinimapWidget.h"
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
		ACarPawn* PlayerPawn = Cast<ACarPawn>(GetWorld()->GetFirstPlayerController()->GetPawn());
		if (PlayerPawn)
		{
			PauseMenuWidget->ResetToCheckpoint->OnClicked.AddDynamic(PlayerPawn, &ACarPawn::ResetCarToLastCheckpoint);
			PauseMenuWidget->ResetToCheckpoint->OnClicked.AddDynamic(this, &ATimeAttackHUD::OnResume);
		}
	}

	if (/*MinimapClass*/ false) {
		MinimapWidget = CreateWidget<UMinimapWidget>(GetWorld(), MinimapClass);
		MinimapWidget->AddToViewport();
		MinimapWidget->SetVisibility(ESlateVisibility::Visible);
		MinimapWidget->CarPawn = GetWorld()->GetFirstPlayerController()->GetPawn();
		MinimapWidget->PlayerStartLocation = MinimapWidget->CarPawn->GetActorLocation();
	}

	if (SpeedIndicatorClass) {
		SpeedIndicatorWidget = CreateWidget<USpeedIndicatorWidget>(GetWorld(), SpeedIndicatorClass);
		SpeedIndicatorWidget->AddToViewport();
		SpeedIndicatorWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
		if (GetWorld()->GetFirstPlayerController()->GetPawn()->IsA(ACarPawn::StaticClass())) {
			SpeedIndicatorWidget->CarPawn = Cast<ACarPawn>(GetWorld()->GetFirstPlayerController()->GetPawn());
			
		}
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
	if (PauseMenuWidget && !PauseMenuWidget->IsVisible())
	{
		if (bShowMenu)
		{
			if (RaceTimerWidget) RaceTimerWidget->SetVisibility(ESlateVisibility::Collapsed);
			if (TimeAttackEndMenuWidget) TimeAttackEndMenuWidget->SetVisibility(ESlateVisibility::Visible);
			if (MinimapWidget) MinimapWidget->SetVisibility(ESlateVisibility::Collapsed);
		}
		else
		{
			if (RaceTimerWidget) RaceTimerWidget->SetVisibility(ESlateVisibility::Visible);
			if (TimeAttackEndMenuWidget) TimeAttackEndMenuWidget->SetVisibility(ESlateVisibility::Collapsed);
			if (MinimapWidget) MinimapWidget->SetVisibility(ESlateVisibility::Visible);
		}
	}
}

void ATimeAttackHUD::TogglePauseMenu(bool bShowMenu)
{
	if (TimeAttackEndMenuWidget && !TimeAttackEndMenuWidget->IsVisible())
	{
		if (bShowMenu)
		{
			if (RaceTimerWidget) RaceTimerWidget->SetVisibility(ESlateVisibility::Collapsed);
			if (PauseMenuWidget) PauseMenuWidget->SetVisibility(ESlateVisibility::Visible);
			if (MinimapWidget) MinimapWidget->SetVisibility(ESlateVisibility::Collapsed);
		}
		else
		{
			if (RaceTimerWidget) RaceTimerWidget->SetVisibility(ESlateVisibility::Visible);
			if (PauseMenuWidget) PauseMenuWidget->SetVisibility(ESlateVisibility::Collapsed);
			if (MinimapWidget) MinimapWidget->SetVisibility(ESlateVisibility::Visible);
		}
	}
}

void ATimeAttackHUD::OnResume()
{
	GameModeBase->OnPressPause();
}
