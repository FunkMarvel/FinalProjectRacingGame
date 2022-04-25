// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../RacingUnrealProjectGameModeBase.h"
#include "ScoreAttackGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class RACINGUNREALPROJECT_API AScoreAttackGameModeBase : public ARacingUnrealProjectGameModeBase
{
	GENERATED_BODY()

public:
	AScoreAttackGameModeBase();

	virtual void BeginPlay() override;

	virtual void OnPressPause() override;

	virtual void OnCompletedLap() override;
	
	virtual void GameEndState() override;

protected:
	int32 CurrentScore{};
};
