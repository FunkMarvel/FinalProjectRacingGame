// Copyright Epic Games, Inc. All Rights Reserved.


#include "RacingUnrealProjectGameModeBase.h"

#include "DebugLog.h"
#include "GameModes/RacingGameInstance.h"

void ARacingUnrealProjectGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	PlayerPawn = GetWorld()->GetFirstPlayerController()->GetPawn();
	RacingGameInstance = Cast<URacingGameInstance>(GetGameInstance());
	PlayerController = GetWorld()->GetFirstPlayerController();
}

void ARacingUnrealProjectGameModeBase::OnCompletedLap()
{
	DL_NORMAL(TEXT("Crossed finish line!"))
}

void ARacingUnrealProjectGameModeBase::GameEndState()
{
}
