// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuHUDWidget.generated.h"

/**
 * 
 */
UCLASS()
class RACINGUNREALPROJECT_API UMainMenuHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
		class UButton* PlayGameButton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
		class UButton* SettingsButton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
		class UButton* ExitGameButton;
};
