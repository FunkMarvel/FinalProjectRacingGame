// Fill out your copyright notice in the Description page of Project Settings.


#include "RacingGameInstance.h"

#include "Kismet/GameplayStatics.h"

void URacingGameInstance::ChangeLevel(FName LevelName)
{
	UGameplayStatics::OpenLevel(GetWorld(), LevelName);
}
