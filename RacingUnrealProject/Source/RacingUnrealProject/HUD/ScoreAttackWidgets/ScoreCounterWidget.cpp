// Fill out your copyright notice in the Description page of Project Settings.


#include "ScoreCounterWidget.h"

#include "Components/TextBlock.h"

void UScoreCounterWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UScoreCounterWidget::UpdateScore(int32 CurrentScore)
{
	FString ScoreString{FString::Printf(TEXT("%05d"), CurrentScore)};
	Score->SetText(FText::FromString(ScoreString));
}

void UScoreCounterWidget::UpdateLapCounter(int32 CurrentNumLaps, int32 MaxNumLaps)
{
	FString LapString{FString::Printf(TEXT("%01d/%01d"), CurrentNumLaps, MaxNumLaps)};
	LapCounter->SetText(FText::FromString(LapString));
}
