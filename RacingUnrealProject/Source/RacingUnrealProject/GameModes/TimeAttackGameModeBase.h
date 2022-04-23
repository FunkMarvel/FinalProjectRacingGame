// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../RacingUnrealProjectGameModeBase.h"
#include "TimeAttackGameModeBase.generated.h"

/**
 * 
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

	UFUNCTION()
	void OnTimerLoop();

protected:
	UPROPERTY()
		class AGameStartSequenceActor* GameStartSequence{nullptr};
	
	FTimerHandle RaceTimerHandle;
};
