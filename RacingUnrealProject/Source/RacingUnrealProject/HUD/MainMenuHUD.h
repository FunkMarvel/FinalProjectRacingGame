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

protected:
	UPROPERTY()
	class UMainMenuHUDWidget* MainMenuHUDWidget{nullptr};
	
	UPROPERTY()
	class URacingGameInstance* GameInstance{nullptr};

	UPROPERTY()
	class AMainMenuGameModeBase* GameModeBase{nullptr};

	UFUNCTION()
	void OnPressPlay();
	void OnPressSettings();
	void OnPressExit();
};
