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

	void SetScore(int32 CurrentScore, int32 CurrentGoalScore, FSlateColor &CurrentColor);

	void SetBestScore(int32 CurrentScore, int32 BestScore, FSlateColor &CurrentColor);

	void ToggleEndMenu(bool bShowMenu);

	void TogglePauseMenu(bool bShowMenu);

	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION()
		void OnResume();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets|In Game HUD")
	TSubclassOf<class UUserWidget> ScoreWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets|End Menu")
	TSubclassOf<class UUserWidget> ScoreAttackEndMenuClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets|Pause Menu")
	TSubclassOf<class UUserWidget> PauseMenuClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets|In Game HUD")
	TSubclassOf<class UUserWidget> SpeedIndicatorClass; 

protected:
	AScoreAttackGameModeBase* GameModeBase{nullptr};

	UPROPERTY()
		class UPauseMenuWidget* PauseMenuWidget{nullptr};

	UPROPERTY()
		class UScoreCounterWidget* ScoreCounterWidget{nullptr};

	UPROPERTY()
		class UScoreAttackEndMenuWidget* ScoreAttackEndMenuWidget{nullptr};

	UPROPERTY() 
		class USpeedIndicatorWidget* SpeedIndicatorWidget{nullptr};

	UPROPERTY()
		class ACarPawn* PlayerPawn{nullptr};
};
