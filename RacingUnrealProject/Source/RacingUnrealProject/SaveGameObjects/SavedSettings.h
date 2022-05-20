// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SavedSettings.generated.h"

/**
 * @brief save object for settings.
 */
UCLASS()
class RACINGUNREALPROJECT_API USavedSettings : public USaveGame
{
	GENERATED_BODY()

public:
	void AddSettingsToSave(int32 NumberOfLapsToSave) {NumberOfLaps = NumberOfLapsToSave;}
	
	USavedSettings() { AddSettingsToSave(3); }
	USavedSettings(int32 NUmberOfLapsToSave) { AddSettingsToSave(NUmberOfLapsToSave); }
	
	UPROPERTY()
		int32 NumberOfLaps;

};
