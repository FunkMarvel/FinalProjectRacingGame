// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RaceTimerWidget.generated.h"

/**
 * @brief in game HUD for time attack.
 */
UCLASS()
class RACINGUNREALPROJECT_API URaceTimerWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void UpdateTimer(float CurrentTime, float CurrentGoalTime, FSlateColor &SlateColor);

	UFUNCTION()
	void UpdateLapCounter(int32 CurrentNumLaps, int32 MaxNumLaps);

	UFUNCTION()
	void SetSpeedOMeter(float InSpeed);
	
protected:
	UPROPERTY(meta=(BindWidget))
		class UTextBlock* Timer{nullptr};

	UPROPERTY(meta=(BindWidget))
		class UTextBlock* LapCounter{nullptr};
	
	UPROPERTY(meta=(BindWidget))
		class UTextBlock* SpeedOMeter{nullptr};
};
