// Fill out your copyright notice in the Description page of Project Settings.


#include "ScoreCounterWidget.h"

#include "Components/TextBlock.h"

void UScoreCounterWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (Score) Score->SetText(FText::FromString("00000"));
	if (LapCounter) LapCounter->SetText(FText::FromString("0/0"));
	if (SpeedOMeter) SpeedOMeter->SetText(FText::FromString("000 KM/H"));
}

void UScoreCounterWidget::UpdateScore(int32 CurrentScore, int32 CurrentGoalScore, FSlateColor &CurrentColor)
{
	FString ScoreString{FString::Printf(TEXT("Goal: %05d\nScore: %05d"), CurrentGoalScore, CurrentScore)};
	Score->SetText(FText::FromString(ScoreString));
	Score->SetColorAndOpacity(CurrentColor);
}

void UScoreCounterWidget::UpdateLapCounter(int32 CurrentNumLaps, int32 MaxNumLaps)
{
	FString LapString{FString::Printf(TEXT("%01d/%01d"), CurrentNumLaps, MaxNumLaps)};
	LapCounter->SetText(FText::FromString(LapString));
}

void UScoreCounterWidget::SetSpeedOMeter(float InSpeed)
{
	FString SpeedString{FString::Printf(TEXT("%03.0f KM/H"), 0.01*InSpeed*3.6)};
	if (SpeedOMeter) SpeedOMeter->SetText(FText::FromString(SpeedString));
}
