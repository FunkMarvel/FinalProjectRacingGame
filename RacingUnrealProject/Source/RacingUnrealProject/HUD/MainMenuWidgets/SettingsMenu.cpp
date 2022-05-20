// Fill out your copyright notice in the Description page of Project Settings.


#include "SettingsMenu.h"

#include "Components/TextBlock.h"

/**
 * @brief Updates the displayed number of laps
 * @param NumberToDisplay selected number of laps.
 */
void USettingsMenu::UpdateText(int32 NumberToDisplay)
{
	FString NumString{FString::Printf(TEXT("Number of laps: %01d"), NumberToDisplay)};
	if (NumLapsTextBlock) NumLapsTextBlock->SetText(FText::FromString(NumString));
}
