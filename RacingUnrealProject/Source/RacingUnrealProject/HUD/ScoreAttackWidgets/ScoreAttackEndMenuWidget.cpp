// Fill out your copyright notice in the Description page of Project Settings.


#include "ScoreAttackEndMenuWidget.h"

#include "Components/Button.h"
#include "RacingUnrealProject/GameModes/RacingGameInstance.h"

void UScoreAttackEndMenuWidget::NativeConstruct()
{
	UUserWidget::NativeConstruct();
	PlayAgainButton->OnClicked.AddDynamic(this, &UScoreAttackEndMenuWidget::OnPlayAgain);
	ReturnToMenuButton->OnClicked.AddDynamic(this, &UScoreAttackEndMenuWidget::OnBackToMenu);
}

void UScoreAttackEndMenuWidget::OnPlayAgain()
{
	URacingGameInstance* GameInstance = Cast<URacingGameInstance>(GetWorld()->GetGameInstance());
	if (GameInstance) GameInstance->ChangeLevel(GameInstance->LevelNames[GameInstance->ScoreAttack]);
}

void UScoreAttackEndMenuWidget::OnBackToMenu()
{
	Super::OnBackToMenu();
}
