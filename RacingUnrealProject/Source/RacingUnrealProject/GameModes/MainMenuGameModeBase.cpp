// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuGameModeBase.h"

#include "RacingGameInstance.h"
#include "Kismet/GameplayStatics.h"

AMainMenuGameModeBase::AMainMenuGameModeBase()
{
}

void AMainMenuGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	GameInstance = Cast<URacingGameInstance>(GetGameInstance());

	
}

void AMainMenuGameModeBase::ChangeLevel(FName LevelName)
{
	UGameplayStatics::OpenLevel(GetWorld(), LevelName);
}
