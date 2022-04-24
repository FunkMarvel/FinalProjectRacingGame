// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "RacingUnrealProjectGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class RACINGUNREALPROJECT_API ARacingUnrealProjectGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

private:
	UPROPERTY()
		class ACheckpoint* LastCheckpoint{nullptr};
	
public:
	UFUNCTION()
		class ACheckpoint* GetLastCheckpoint() const {return LastCheckpoint; }
	UFUNCTION()
		void SetLastCheckpoint(class ACheckpoint* NewLastCheckpoint) {LastCheckpoint = NewLastCheckpoint; }
	

	UFUNCTION()
		virtual void OnCompletedLap();
};
