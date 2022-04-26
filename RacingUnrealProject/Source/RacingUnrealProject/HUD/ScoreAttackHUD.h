// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "RacingUnrealProject/GameModes/ScoreAttackGameModeBase.h"
#include "ScoreAttackHUD.generated.h"

/**
 * 
 */
UCLASS()
class RACINGUNREALPROJECT_API AScoreAttackHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	void SetLapCounter(int32 CurrentLap, int32 MaxNumLaps);

	void SetBestScore(int32 CurrentScore, int32 BestScore);

	void ToggleEndMenu(bool bShowMenu);

	void TogglePauseMenu(bool bShowMenu);

	UFUNCTION()
		void OnResume();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets|In Game HUD")
	TSubclassOf<class UUserWidget> ScoreWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets|End Menu")
	TSubclassOf<class UUserWidget> ScoreAttackEndMenuClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets|Pause Menu")
	TSubclassOf<class UUserWidget> PauseMenuClass;

protected:
	AScoreAttackGameModeBase* GameModeBase{nullptr};

	UPROPERTY()
		class UPauseMenuWidget* PauseMenuWidget{nullptr};

	UPROPERTY()
		class UScoreCounterWidget* ScoreCounterWidget{nullptr};

	UPROPERTY()
		class UScoreAttackEndMenuWidget* ScoreAttackEndMenuWidget{nullptr};
};
