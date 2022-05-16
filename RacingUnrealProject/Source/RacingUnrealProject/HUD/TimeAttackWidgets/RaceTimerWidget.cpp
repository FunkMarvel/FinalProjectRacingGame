// Fill out your copyright notice in the Description page of Project Settings.


#include "RaceTimerWidget.h"

#include "Components/TextBlock.h"

void URaceTimerWidget::NativeConstruct()
{
	Super::NativeConstruct();
	// Timer = CreateDefaultSubobject<UTextBlock>(TEXT("LapTimer"));
	// LapCounter = CreateDefaultSubobject<UTextBlock>(TEXT("LapCounter"));
	
	if (Timer) Timer->SetText(FText::FromString("00:00:00"));
	if (LapCounter) LapCounter->SetText(FText::FromString("0/0"));
}

void URaceTimerWidget::UpdateTimer(float CurrentTime)
{
	//timer text
	int32 Minutes = FMath::FloorToInt(CurrentTime / 60);
	int32 Seconds = FMath::FloorToInt(FMath::Fmod(CurrentTime, 60.f));
	int32 CentiSeconds = FMath::FloorToInt((FMath::Fmod(CurrentTime, 60.f) - Seconds)*100);
	FString TimeString{FString::Printf(TEXT("%02d:%02d:%02d"), Minutes, Seconds, CentiSeconds)};
	if (Timer) Timer->SetText(FText::FromString(TimeString));


	
}

void URaceTimerWidget::UpdateLapCounter(int32 CurrentNumLaps, int32 MaxNumLaps)
{
	FString LapString{FString::Printf(TEXT("%01d/%01d"), CurrentNumLaps, MaxNumLaps)};
	if (LapCounter) LapCounter->SetText(FText::FromString(LapString));
}
