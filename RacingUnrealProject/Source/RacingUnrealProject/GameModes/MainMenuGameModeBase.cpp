// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuGameModeBase.h"

#include "Kismet/GameplayStatics.h"
#include "RacingUnrealProject/DebugLog.h"
#include "RacingUnrealProject/SaveGameObjects/SavedSettings.h"

AMainMenuGameModeBase::AMainMenuGameModeBase()
{
}

void AMainMenuGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	if (PlayerController)
	{
		PlayerController->SetShowMouseCursor(true);
		FInputModeUIOnly InputMode{};
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);
		PlayerController->SetInputMode(InputMode);
	}

	if (RacingGameInstance)
	{
		if (LoadGame()) { DL_NORMAL("Game Loaded Succesfully"); }
		if (LoadSettings()) { DL_NORMAL("Settings Loaded Succesfully"); }
	}
}

bool AMainMenuGameModeBase::SaveSettings()
{
	if (USavedSettings* SavedSettings = Cast<USavedSettings>(UGameplayStatics::CreateSaveGameObject(USavedSettings::StaticClass())))
	{
		SavedSettings->AddSettingsToSave(RacingGameInstance->NumberOfLaps);

		if (UGameplayStatics::SaveGameToSlot(SavedSettings, RacingGameInstance->SettingsSaveSlotName, RacingGameInstance->SettingsSaveSlotIndex))
			return true;
	}
	return false;
}

bool AMainMenuGameModeBase::LoadSettings()
{
	if (USavedSettings* SavedSettings =
	Cast<USavedSettings>(UGameplayStatics::LoadGameFromSlot(RacingGameInstance->SettingsSaveSlotName,
		RacingGameInstance->SettingsSaveSlotIndex)))
	{
		RacingGameInstance->NumberOfLaps = SavedSettings->NumberOfLaps;
		return true;
	}
	return false;
}
