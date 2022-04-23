// Fill out your copyright notice in the Description page of Project Settings.


#include "TimeAttackHUD.h"

#include "Blueprint/UserWidget.h"
#include "RacingUnrealProject/GameModes/TimeAttackGameModeBase.h"
#include "TimeAttackWidgets/RaceTimerWidget.h"

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
		GameModeBase->BeginTimer();
		RaceTimerWidget->AddToViewport();
	}
}

void ATimeAttackHUD::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (GameModeBase->IsTiming()) GameModeBase->RaceTimer += DeltaSeconds;
	if (RaceTimerWidget) RaceTimerWidget->UpdateTimer(GameModeBase->RaceTimer);
}
