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

	void ToggleEndMenu(bool bShowMenu);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets|In Game HUD")
		TSubclassOf<class UUserWidget> TimerWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets|End Menu")
		TSubclassOf<class UUserWidget> TimeAttackEndMenuClass;

protected:
	UPROPERTY()
		class URaceTimerWidget* RaceTimerWidget{nullptr};

	UPROPERTY()
		class ATimeAttackGameModeBase* GameModeBase{nullptr};

	UPROPERTY()
		class UTimeAttackEndMenuWidget* TimeAttackEndMenuWidget{nullptr};
};
