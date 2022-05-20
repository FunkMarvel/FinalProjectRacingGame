// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "RacingSaveGame.generated.h"

/**
 * @brief save object for high score and best time.
 */
UCLASS()
class RACINGUNREALPROJECT_API URacingSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY()
	float BestPlayerTime;

	UPROPERTY()
	int32 BestPlayerScore;

	URacingSaveGame();

	void SetGameData(float TimeToSave, int32 ScoreToSave);
};
