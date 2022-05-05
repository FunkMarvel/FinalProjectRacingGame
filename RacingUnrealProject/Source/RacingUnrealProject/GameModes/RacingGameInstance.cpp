// Fill out your copyright notice in the Description page of Project Settings.


#include "RacingGameInstance.h"

#include "Kismet/GameplayStatics.h"

URacingGameInstance::URacingGameInstance()
{
	PlayerArray.Reserve(1);
}

void URacingGameInstance::ChangeLevel(FName LevelName)
{
	UGameplayStatics::OpenLevel(GetWorld(), LevelName);
}

void URacingGameInstance::SavePlayerData(FPlayerData NewPlayerData)
{
	if (PlayerArray.Contains(NewPlayerData)) return;
	PlayerArray.Add(NewPlayerData);
}

FPlayerData* URacingGameInstance::GetBestTimePlayer()
{
	if (PlayerArray.Num() > 0)
	{
		Algo::Sort(PlayerArray, [](const FPlayerData A, const FPlayerData B) { return A.PlayerTime < B.PlayerTime; });

		return &PlayerArray[0];
	}
	return nullptr;
}

FPlayerData* URacingGameInstance::GetBestScorePlayer()
{
	if (PlayerArray.Num() > 0)
	{
		Algo::Sort(PlayerArray, [](const FPlayerData A, const FPlayerData B) { return A.PlayerScore > B.PlayerScore; });

		return &PlayerArray[0];
	}
	return nullptr;
}

void URacingGameInstance::Init() {
	Super::Init();
}

void URacingGameInstance::Shutdown() {
	Super::Shutdown();
}

void URacingGameInstance::StartGameInstance() {
	Super::StartGameInstance();
}

