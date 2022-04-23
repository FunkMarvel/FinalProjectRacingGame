// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MainMenuHUD.generated.h"

/**
 * 
 */
UCLASS()
class RACINGUNREALPROJECT_API AMainMenuHUD : public AHUD
{
	GENERATED_BODY()

public:
	AMainMenuHUD();

	virtual void DrawHUD() override;

	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Menu Widgets")
		TSubclassOf<class UUserWidget> MainMenuClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Menu Widgets")
		TSubclassOf<class UUserWidget> LevelMenuClass;

protected:
	UPROPERTY()
	class UMainMenuHUDWidget* MainMenuHUDWidget{nullptr};

	UPROPERTY()
	class ULevelSelectMenuWidget* LevelSelectMenuWidget{nullptr};
	
	UPROPERTY()
	class URacingGameInstance* GameInstance{nullptr};

	UPROPERTY()
	class AMainMenuGameModeBase* GameModeBase{nullptr};

	// Main menu functions:
	UFUNCTION()
	void OnPressPlay();
	UFUNCTION()
	void OnPressSettings();
	UFUNCTION()
	void OnPressExit();

	// Level select menu functions:
	UFUNCTION()
	void OnPressTimeAttack();
	UFUNCTION()
	void OnPressScoreAttack();
	UFUNCTION()
	void OnPressBack();
};
