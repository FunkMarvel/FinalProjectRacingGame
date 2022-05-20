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
	if (SpeedOMeter) SpeedOMeter->SetText(FText::FromString("000 KM/H"));
}

/**
 * @brief updates timer
 * @param CurrentTime current time in number of seconds
 * @param CurrentGoalTime current goal to achieve in seconds
 * @param SlateColor color of current goal
 */
void URaceTimerWidget::UpdateTimer(float CurrentTime, float CurrentGoalTime, FSlateColor &SlateColor)
{
	//calculate time in Minutes::Seconds::HundrethOfASecond :
	int32 GoalMinutes = FMath::FloorToInt(CurrentGoalTime / 60);
	int32 GoalSeconds = FMath::FloorToInt(FMath::Fmod(CurrentGoalTime, 60.f));
	int32 GoalCentiSeconds = FMath::FloorToInt((FMath::Fmod(CurrentGoalTime, 60.f) - GoalSeconds)*100);
	
	int32 Minutes = FMath::FloorToInt(CurrentTime / 60);
	int32 Seconds = FMath::FloorToInt(FMath::Fmod(CurrentTime, 60.f));
	int32 CentiSeconds = FMath::FloorToInt((FMath::Fmod(CurrentTime, 60.f) - Seconds)*100);
	
	FString TimeString{FString::Printf(TEXT("Goal: %02d:%02d:%02d\nCurrent: %02d:%02d:%02d"), GoalMinutes, GoalSeconds,
		GoalCentiSeconds, Minutes, Seconds, CentiSeconds)};
	
	if (Timer) Timer->SetText(FText::FromString(TimeString));
	if (Timer) Timer->SetColorAndOpacity(SlateColor);


	
}

/**
 * @brief Update lap counter
 * @param CurrentNumLaps 
 * @param MaxNumLaps 
 */
void URaceTimerWidget::UpdateLapCounter(int32 CurrentNumLaps, int32 MaxNumLaps)
{
	FString LapString{FString::Printf(TEXT("%01d/%01d"), CurrentNumLaps, MaxNumLaps)};
	if (LapCounter) LapCounter->SetText(FText::FromString(LapString));
}

/**
 * @brief Update SpeedOMeter
 * @param InSpeed speed in centimetre per second.
 */
void URaceTimerWidget::SetSpeedOMeter(float InSpeed)
{
	FString SpeedString{FString::Printf(TEXT("%03.0f KM/H"), 0.01*InSpeed*3.6)};
	if (SpeedOMeter) SpeedOMeter->SetText(FText::FromString(SpeedString));
}
