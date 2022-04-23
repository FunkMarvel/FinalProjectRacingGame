// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuGameModeBase.h"

#include "RacingGameInstance.h"

AMainMenuGameModeBase::AMainMenuGameModeBase()
{
}

void AMainMenuGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	GameInstance = Cast<URacingGameInstance>(GetGameInstance());
}
