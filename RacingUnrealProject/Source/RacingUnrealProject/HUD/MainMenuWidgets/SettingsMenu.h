// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SettingsMenu.generated.h"

/**
 * @brief settings menu
 */
UCLASS()
class RACINGUNREALPROJECT_API USettingsMenu : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta=(BindWidget))
	class UButton* PlusButton{nullptr};

	UPROPERTY(meta=(BindWidget))
	class UButton* MinusButton{nullptr};

	UPROPERTY(meta=(BindWidget))
	class UButton* SaveButton{nullptr};

	UPROPERTY(meta=(BindWidget))
	class UButton* ReturnToMenuButton{nullptr};

	void UpdateText(int32 NumberToDisplay);

protected:
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* NumLapsTextBlock{nullptr};
};
