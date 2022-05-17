// Fill out your copyright notice in the Description page of Project Settings.


#include "RacingSaveGame.h"

#include "RacingUnrealProject/GameModes/RacingGameInstance.h"

URacingSaveGame::URacingSaveGame()
{
}

void URacingSaveGame::SetGameData(TArray<FPlayerData> InPlayerDataArray)
{
	PlayerTimes.Reserve(InPlayerDataArray.Num());
	PlayerScores.Reserve(InPlayerDataArray.Num());
	for (FPlayerData &PlayerData : InPlayerDataArray)
	{
		PlayerScores.Add(PlayerData.PlayerScore);
		PlayerTimes.Add(PlayerData.PlayerTime);
	}
}
