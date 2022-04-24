// Copyright Epic Games, Inc. All Rights Reserved.


#include "RacingUnrealProjectGameModeBase.h"

#include "DebugLog.h"

void ARacingUnrealProjectGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	PlayerPawn = GetWorld()->GetFirstPlayerController()->GetPawn();
}

void ARacingUnrealProjectGameModeBase::OnCompletedLap()
{
	DL_NORMAL(TEXT("Crossed finish line!"))
}
