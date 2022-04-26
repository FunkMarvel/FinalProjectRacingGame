// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ScoreCounterWidget.generated.h"

/**
 * 
 */
UCLASS()
class RACINGUNREALPROJECT_API UScoreCounterWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void UpdateScore(float CurrentTime);

	UFUNCTION()
	void UpdateLapCounter(int32 CurrentNumLaps, int32 MaxNumLaps);

protected:
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* Score{nullptr};

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* LapCounter{nullptr};
};