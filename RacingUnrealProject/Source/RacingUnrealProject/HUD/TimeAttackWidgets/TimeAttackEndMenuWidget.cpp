// Fill out your copyright notice in the Description page of Project Settings.


#include "TimeAttackEndMenuWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "RacingUnrealProject/GameModes/RacingGameInstance.h"

void UTimeAttackEndMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();
	PlayAgainButton->OnClicked.AddDynamic(this, &UTimeAttackEndMenuWidget::OnPlayAgain);
	ReturnToMenuButton->OnClicked.AddDynamic(this, &UTimeAttackEndMenuWidget::OnBackToMenu);
}

/**
 * @brief Turns float into time text
 * @param NewTime 
 * @return 
 */
FText UTimeAttackEndMenuWidget::GetTimeTextFromFloat(float NewTime)
{
	int32 Minutes = FMath::FloorToInt(NewTime / 60);
	int32 Seconds = FMath::FloorToInt(FMath::Fmod(NewTime, 60.f));
	int32 CentiSeconds = FMath::FloorToInt((FMath::Fmod(NewTime, 60.f) - Seconds)*100);
	FString TimeString{FString::Printf(TEXT("%02d:%02d:%02d"), Minutes, Seconds, CentiSeconds)};

	return FText::FromString(TimeString);
}

/**
 * @brief sets time text
 * @param CurrentTimeText player's achieved time
 * @param BestTimeText best time
 * @param SlateColor color of achieved time
 */
void UTimeAttackEndMenuWidget::SetTimeText(FText CurrentTimeText, FText BestTimeText, FSlateColor SlateColor)
{
	if (CurrentTime)
	{
		CurrentTime->SetText(CurrentTimeText);
		CurrentTime->SetColorAndOpacity(SlateColor);
	}
	if (BestTime) BestTime->SetText(BestTimeText);
}

/**
 * @brief play again
 */
void UTimeAttackEndMenuWidget::OnPlayAgain()
{
	URacingGameInstance* GameInstance{};
	GameInstance = Cast<URacingGameInstance>(GetGameInstance());
	if (GameInstance) GameInstance->ChangeLevel(GameInstance->LevelNames[GameInstance->TimeAttack]);
}

/**
 * @brief quit to menu
 */
void UTimeAttackEndMenuWidget::OnBackToMenu()
{
	URacingGameInstance* GameInstance{};
	GameInstance = Cast<URacingGameInstance>(GetGameInstance());
	if (GameInstance) GameInstance->ChangeLevel(GameInstance->LevelNames[GameInstance->MainMenu]);
}
