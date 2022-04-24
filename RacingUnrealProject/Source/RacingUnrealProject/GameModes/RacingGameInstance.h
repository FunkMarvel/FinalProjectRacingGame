// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "RacingGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class RACINGUNREALPROJECT_API URacingGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	URacingGameInstance();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Levels")
		TArray<FName> LevelNames{};

	UFUNCTION()
		void ChangeLevel(FName LevelName);

	UFUNCTION()
		void SaveTime(float TimeToSave);

	UFUNCTION()
		void SaveScore(uint32 ScoreToSave);

protected:
	UPROPERTY(VisibleAnywhere)
		TArray<float> TimeAttackTimes{};

	UPROPERTY(VisibleAnywhere)
		TArray<uint32> ScoreAttackScores{};
};
