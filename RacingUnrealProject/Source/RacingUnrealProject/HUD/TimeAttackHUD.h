// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "TimeAttackHUD.generated.h"

/**
 * 
 */
UCLASS()
class RACINGUNREALPROJECT_API ATimeAttackHUD : public AHUD
{
	GENERATED_BODY()

public:
	ATimeAttackHUD();
	
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	void SetLapCounter(int32 CurrentLap, int32 MaxNumLaps);

	void SetBestTime(float CurrentTime, float BestTime);

	void ToggleEndMenu(bool bShowMenu);

	void TogglePauseMenu(bool bShowMenu);

	UFUNCTION()
		void OnResume();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets|In Game HUD")
		TSubclassOf<class UUserWidget> TimerWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets|End Menu")
		TSubclassOf<class UUserWidget> TimeAttackEndMenuClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets|Pause Menu")
		TSubclassOf<class UUserWidget> PauseMenuClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets|Minimap")
		TSubclassOf<class UUserWidget> MinimapClass;
		
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets|In Game HUD")
		TSubclassOf<class UUserWidget> SpeedIndicatorClass; 

protected:
	UPROPERTY()
		class URaceTimerWidget* RaceTimerWidget{nullptr};

	UPROPERTY()
		class ATimeAttackGameModeBase* GameModeBase{nullptr};

	UPROPERTY()
		class UTimeAttackEndMenuWidget* TimeAttackEndMenuWidget{nullptr};

	UPROPERTY()
		class UPauseMenuWidget* PauseMenuWidget{nullptr};

	UPROPERTY()
		class UMinimapWidget* MinimapWidget{nullptr};
		
	UPROPERTY() 
		class USpeedIndicatorWidget* SpeedIndicatorWidget{nullptr};

	UPROPERTY()
		class ACarPawn* PlayerPawn{nullptr};
};
