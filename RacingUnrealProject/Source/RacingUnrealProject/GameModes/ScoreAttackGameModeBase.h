// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../RacingUnrealProjectGameModeBase.h"
#include "ScoreAttackGameModeBase.generated.h"

/**
 * @brief score attack game mode base.
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

	virtual void AddScore(int32 Score) override;

	int32 GetCurrentGoalScore()
	{
		if (RacingGameInstance) return RacingGameInstance->ScoresToBeat[CurrentBestGoal];
		return 0;
	}

protected:
	
	int32 CurrentScore{};

	class AScoreAttackHUD* AttackHUD{nullptr};
};
