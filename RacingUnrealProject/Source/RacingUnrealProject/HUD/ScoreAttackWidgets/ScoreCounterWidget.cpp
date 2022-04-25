// Fill out your copyright notice in the Description page of Project Settings.


#include "ScoreCounterWidget.h"

void UScoreCounterWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UScoreCounterWidget::UpdateScore(float CurrentScore)
{
	FString ScoreString{FString::Printf(TEXT("%05d"), CurrentScore)};
}

void UScoreCounterWidget::UpdateLapCounter(int32 CurrentNumLaps, int32 MaxNumLaps)
{
}
