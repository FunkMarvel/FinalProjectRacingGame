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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Timer")
		float RaceTimer{0.f};

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Laps")
		int32 NumberOfLaps{3};
	int32 CurrentLap{};

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
};
