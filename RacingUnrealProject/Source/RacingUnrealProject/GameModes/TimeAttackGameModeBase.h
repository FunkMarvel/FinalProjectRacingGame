// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../RacingUnrealProjectGameModeBase.h"
#include "TimeAttackGameModeBase.generated.h"

/**
 *  @brief game mode base for time attack.
 */
UCLASS()
class RACINGUNREALPROJECT_API ATimeAttackGameModeBase : public ARacingUnrealProjectGameModeBase
{
	GENERATED_BODY()

public:
	ATimeAttackGameModeBase();

	virtual void BeginPlay() override;

	UFUNCTION()
	void BeginTimer();

	virtual void OnPressPause() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Timer")
		float RaceTimer{0.f};


protected:
	UPROPERTY()
		class AGameStartSequenceActor* GameStartSequence{nullptr};

	bool bIsTiming{false};

	UPROPERTY()
		class ATimeAttackHUD* TimeAttackHUD{nullptr};

public:
	bool IsTiming() { return bIsTiming; }
	void ToggleTiming(bool bOn) { bIsTiming = bOn; }
	
	virtual void OnCompletedLap() override;

	virtual void GameEndState() override;

	float GetCurrentGoalTime()
	{
		if (RacingGameInstance) return RacingGameInstance->TimesToBeat[CurrentBestGoal]*RacingGameInstance->NumberOfLaps;
		return 0.f;
	}
};
