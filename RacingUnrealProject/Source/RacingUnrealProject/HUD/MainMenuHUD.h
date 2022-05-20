// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MainMenuHUD.generated.h"

/**
 * @brief main menu HUD class.
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

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Menu Widgets")
		TSubclassOf<class UUserWidget> SettingsMenuClass;

protected:
	UPROPERTY()
	class UMainMenuHUDWidget* MainMenuHUDWidget{nullptr};

	UPROPERTY()
	class ULevelSelectMenuWidget* LevelSelectMenuWidget{nullptr};

	UPROPERTY()
	class USettingsMenu* SettingsMenuWidget{nullptr};
	
	UPROPERTY()
	class URacingGameInstance* GameInstance{nullptr};

	UPROPERTY()
	class AMainMenuGameModeBase* GameModeBase{nullptr};

	int32 CurrentNumLaps{};

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

	// Settings menu functions:
	UFUNCTION()
	void OnPressPlus();

	UFUNCTION()
	void OnPressMinus();

	UFUNCTION()
	void OnPressSave();

	UFUNCTION()
	void OnPressReturnToMenu();
};
