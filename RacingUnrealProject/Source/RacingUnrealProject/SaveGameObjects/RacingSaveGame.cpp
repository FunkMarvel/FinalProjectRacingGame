// Fill out your copyright notice in the Description page of Project Settings.


#include "RacingSaveGame.h"

#include "RacingUnrealProject/GameModes/RacingGameInstance.h"

/**
 * @brief save object for high score and best time.
 */
URacingSaveGame::URacingSaveGame()
{
}

/**
 * @brief save object for high score and best time.
 * @param TimeToSave number of seconds to save
 * @param ScoreToSave number of points in score to save
 */
void URacingSaveGame::SetGameData(float TimeToSave, int32 ScoreToSave)
{
	BestPlayerTime = TimeToSave;
	BestPlayerScore = ScoreToSave;
}
