// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../RacingUnrealProjectGameModeBase.h"
#include "MainMenuGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class RACINGUNREALPROJECT_API AMainMenuGameModeBase : public ARacingUnrealProjectGameModeBase
{
	GENERATED_BODY()

public:
	AMainMenuGameModeBase();
	
	virtual void BeginPlay() override;

protected:
	UPROPERTY()
		class URacingGameInstance* GameInstance{nullptr};
};
