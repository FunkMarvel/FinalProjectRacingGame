// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "RacingSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class RACINGUNREALPROJECT_API URacingSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TArray<float> PlayerTimes;

	UPROPERTY()
	TArray<int32> PlayerScores;

	URacingSaveGame();

	void SetGameData(TArray<struct FPlayerData> InPlayerDataArray);
};
