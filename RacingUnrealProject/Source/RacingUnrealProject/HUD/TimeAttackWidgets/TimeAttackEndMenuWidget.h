// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TimeAttackEndMenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class RACINGUNREALPROJECT_API UTimeAttackEndMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UFUNCTION()
	FText GetTimeTextFromFloat(float NewTime);

	void SetTimeText(FText CurrentTimeText, FText BestTimeText, FSlateColor SlateColor);

	UFUNCTION()
	virtual void OnPlayAgain();

	UFUNCTION()
	virtual void OnBackToMenu();
	
protected:
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* BestTime{nullptr};

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* CurrentTime{nullptr};

	UPROPERTY(meta=(BindWidget))
	class UButton* PlayAgainButton{nullptr};

	UPROPERTY(meta=(BindWidget))
	class UButton* ReturnToMenuButton{nullptr};
};
