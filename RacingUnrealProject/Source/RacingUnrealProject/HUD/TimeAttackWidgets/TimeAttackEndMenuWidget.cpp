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

FText UTimeAttackEndMenuWidget::GetTimeTextFromFloat(float NewTime)
{
	int32 Minutes = FMath::FloorToInt(NewTime / 60);
	int32 Seconds = FMath::FloorToInt(FMath::Fmod(NewTime, 60.f));
	int32 CentiSeconds = FMath::FloorToInt((FMath::Fmod(NewTime, 60.f) - Seconds)*100);
	FString TimeString{FString::Printf(TEXT("%02d:%02d:%02d"), Minutes, Seconds, CentiSeconds)};

	return FText::FromString(TimeString);
}

void UTimeAttackEndMenuWidget::SetTimeText(FText CurrentTimeText, FText BestTimeText)
{
	if (CurrentTime) CurrentTime->SetText(CurrentTimeText);
	if (BestTime) BestTime->SetText(BestTimeText);
}

void UTimeAttackEndMenuWidget::OnPlayAgain()
{
	URacingGameInstance* GameInstance{};
	GameInstance = Cast<URacingGameInstance>(GetGameInstance());
	if (GameInstance) GameInstance->ChangeLevel(GameInstance->LevelNames[GameInstance->TimeAttack]);
}

void UTimeAttackEndMenuWidget::OnBackToMenu()
{
	URacingGameInstance* GameInstance{};
	GameInstance = Cast<URacingGameInstance>(GetGameInstance());
	if (GameInstance) GameInstance->ChangeLevel(GameInstance->LevelNames[GameInstance->MainMenu]);
}
