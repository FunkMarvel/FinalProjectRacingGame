// Fill out your copyright notice in the Description page of Project Settings.


#include "RaceTimerWidget.h"

#include "Components/TextBlock.h"

void URaceTimerWidget::NativeConstruct()
{
	Super::NativeConstruct();
	Timer->SetText(FText::FromString("00:00:00"));
}

void URaceTimerWidget::UpdateTimer(float CurrentTime)
{
	int32 Minutes = FMath::FloorToInt(CurrentTime / 60);
	int32 Seconds = FMath::FloorToInt(FMath::Fmod(CurrentTime, 60.f));
	int32 CentiSeconds = FMath::FloorToInt((FMath::Fmod(CurrentTime, 60.f) - Seconds)*100);
	FString TimeString{FString::Printf(TEXT("%02d:%02d:%02d"), Minutes, Seconds, CentiSeconds)};
	FText TimeText{FText::FromString(TimeString)};
	Timer->SetText(TimeText);
}
