// Fill out your copyright notice in the Description page of Project Settings.


#include "TimeAttackEndMenuWidget.h"

#include "Components/TextBlock.h"

void UTimeAttackEndMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

FText UTimeAttackEndMenuWidget::GetTimeTextFromFloat(float NewTime)
{
	int32 Minutes = FMath::FloorToInt(NewTime / 60);
	int32 Seconds = FMath::FloorToInt(FMath::Fmod(NewTime, 60.f));
	int32 CentiSeconds = FMath::FloorToInt((FMath::Fmod(NewTime, 60.f) - Seconds)*100);
	FString TimeString{FString::Printf(TEXT("%02d:%02d:%02d"), Minutes, Seconds, CentiSeconds)};

	return FText::FromString(TimeString);
}
