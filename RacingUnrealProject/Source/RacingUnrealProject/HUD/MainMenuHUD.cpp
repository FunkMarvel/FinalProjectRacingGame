// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuHUD.h"

#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Kismet/KismetSystemLibrary.h"
#include "MainMenuWidgets/LevelSelectMenuWidget.h"
#include "MainMenuWidgets/MainMenuHUDWidget.h"
#include "MainMenuWidgets/SettingsMenu.h"
#include "RacingUnrealProject/GameModes/MainMenuGameModeBase.h"
#include "RacingUnrealProject/GameModes/RacingGameInstance.h"

AMainMenuHUD::AMainMenuHUD()
{
}

void AMainMenuHUD::DrawHUD()
{
	Super::DrawHUD();
}

void AMainMenuHUD::BeginPlay()
{
	Super::BeginPlay();

	GameInstance = Cast<URacingGameInstance>(GetGameInstance());
	if (GameInstance) CurrentNumLaps = GameInstance->NumberOfLaps;
	
	GameModeBase = Cast<AMainMenuGameModeBase>(GetWorld()->GetAuthGameMode());

	if (MainMenuClass)
	{
		MainMenuHUDWidget = CreateWidget<UMainMenuHUDWidget>(GetWorld(), MainMenuClass);

		if (MainMenuHUDWidget)
		{
			MainMenuHUDWidget->PlayGameButton->OnClicked.AddDynamic(this, &AMainMenuHUD::OnPressPlay);
			MainMenuHUDWidget->SettingsButton->OnClicked.AddDynamic(this, &AMainMenuHUD::OnPressSettings);
			MainMenuHUDWidget->ExitGameButton->OnClicked.AddDynamic(this, &AMainMenuHUD::OnPressExit);
			MainMenuHUDWidget->AddToViewport();
		}
	}

	if (LevelMenuClass)
	{
		LevelSelectMenuWidget = CreateWidget<ULevelSelectMenuWidget>(GetWorld(), LevelMenuClass);

		if (LevelSelectMenuWidget)
		{
			LevelSelectMenuWidget->TimeAttackButton->OnClicked.AddDynamic(this, &AMainMenuHUD::OnPressTimeAttack);
			LevelSelectMenuWidget->ScoreAttackButton->OnClicked.AddDynamic(this, &AMainMenuHUD::OnPressScoreAttack);
			LevelSelectMenuWidget->BackButton->OnClicked.AddDynamic(this, &AMainMenuHUD::OnPressBack);
			LevelSelectMenuWidget->AddToViewport();
			LevelSelectMenuWidget->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	if (SettingsMenuClass)
	{
		SettingsMenuWidget = CreateWidget<USettingsMenu>(GetWorld(), SettingsMenuClass);
		SettingsMenuWidget->PlusButton->OnClicked.AddDynamic(this, &AMainMenuHUD::OnPressPlus);
		SettingsMenuWidget->MinusButton->OnClicked.AddDynamic(this, &AMainMenuHUD::OnPressMinus);
		SettingsMenuWidget->SaveButton->OnClicked.AddDynamic(this, &AMainMenuHUD::OnPressSave);
		SettingsMenuWidget->ReturnToMenuButton->OnClicked.AddDynamic(this, &AMainMenuHUD::OnPressReturnToMenu);
		SettingsMenuWidget->AddToViewport();
		SettingsMenuWidget->UpdateText(CurrentNumLaps);
		SettingsMenuWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void AMainMenuHUD::OnPressPlay()
{
	if (MainMenuHUDWidget && LevelSelectMenuWidget)
	{
		MainMenuHUDWidget->SetVisibility(ESlateVisibility::Collapsed);
		LevelSelectMenuWidget->SetVisibility(ESlateVisibility::Visible);
	}
}

void AMainMenuHUD::OnPressSettings()
{
	CurrentNumLaps = GameInstance->NumberOfLaps;
	if (MainMenuHUDWidget && SettingsMenuWidget)
	{
		SettingsMenuWidget->SetVisibility(ESlateVisibility::Visible);
		MainMenuHUDWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void AMainMenuHUD::OnPressExit()
{
	UKismetSystemLibrary::QuitGame(GetWorld(), GetWorld()->GetFirstPlayerController(), EQuitPreference::Quit, false);
}

void AMainMenuHUD::OnPressTimeAttack()
{
	if (GameInstance->LevelNames.Num() >= 2)
	{
		GameInstance->ChangeLevel(GameInstance->LevelNames[1]);
	}
}

void AMainMenuHUD::OnPressScoreAttack()
{
	if (GameInstance->LevelNames.Num() >= 3)
	{
		GameInstance->ChangeLevel(GameInstance->LevelNames[2]);
	}
}

void AMainMenuHUD::OnPressBack()
{
	if (MainMenuHUDWidget && LevelSelectMenuWidget)
	{
		LevelSelectMenuWidget->SetVisibility(ESlateVisibility::Collapsed);
		MainMenuHUDWidget->SetVisibility(ESlateVisibility::Visible);
	}
}

void AMainMenuHUD::OnPressPlus()
{
	if (CurrentNumLaps < 5) CurrentNumLaps++;
	SettingsMenuWidget->UpdateText(CurrentNumLaps);
}

void AMainMenuHUD::OnPressMinus()
{
	if (CurrentNumLaps > 3) CurrentNumLaps--;
	SettingsMenuWidget->UpdateText(CurrentNumLaps);
}

void AMainMenuHUD::OnPressSave()
{
	GameInstance->NumberOfLaps = CurrentNumLaps;
}

void AMainMenuHUD::OnPressReturnToMenu()
{
	CurrentNumLaps = GameInstance->NumberOfLaps;
	if (MainMenuHUDWidget && LevelSelectMenuWidget)
	{
		SettingsMenuWidget->SetVisibility(ESlateVisibility::Collapsed);
		MainMenuHUDWidget->SetVisibility(ESlateVisibility::Visible);
	}
}
