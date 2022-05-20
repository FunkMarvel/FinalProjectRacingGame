// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../TimeAttackWidgets/TimeAttackEndMenuWidget.h"
#include "ScoreAttackEndMenuWidget.generated.h"

/**
 * @brief end menu
 */
UCLASS()
class RACINGUNREALPROJECT_API UScoreAttackEndMenuWidget : public UTimeAttackEndMenuWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	virtual void OnPlayAgain() override;

	virtual void OnBackToMenu() override;

	
};
