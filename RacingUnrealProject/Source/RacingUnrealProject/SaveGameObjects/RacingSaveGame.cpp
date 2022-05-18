// Fill out your copyright notice in the Description page of Project Settings.


#include "RacingSaveGame.h"

#include "RacingUnrealProject/GameModes/RacingGameInstance.h"

URacingSaveGame::URacingSaveGame()
{
}

void URacingSaveGame::SetGameData(float TimeToSave, int32 ScoreToSave)
{
	BestPlayerTime = TimeToSave;
	BestPlayerScore = ScoreToSave;
}
