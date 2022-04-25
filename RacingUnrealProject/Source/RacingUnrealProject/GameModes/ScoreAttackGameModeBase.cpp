// Fill out your copyright notice in the Description page of Project Settings.


#include "ScoreAttackGameModeBase.h"

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
}

void AScoreAttackGameModeBase::OnPressPause()
{
	Super::OnPressPause();
	
}

void AScoreAttackGameModeBase::OnCompletedLap()
{
	Super::OnCompletedLap();
}

void AScoreAttackGameModeBase::GameEndState()
{
	Super::GameEndState();
}
