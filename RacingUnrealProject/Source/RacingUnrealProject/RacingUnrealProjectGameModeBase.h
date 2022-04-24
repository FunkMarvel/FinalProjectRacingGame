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

protected:
	UPROPERTY()
		class ACheckpoint* LastCheckpoint{nullptr};

	UPROPERTY()
		class ACheckpoint* GoalCheckpoint{nullptr};

	UPROPERTY()
		class APawn* PlayerPawn{nullptr};

	UPROPERTY()
		class APlayerController* PlayerController{nullptr};

	UPROPERTY()
		class URacingGameInstance* RacingGameInstance{nullptr};
	
public:
	virtual void BeginPlay() override;
	
	UFUNCTION()
		class ACheckpoint* GetLastCheckpoint() const {return LastCheckpoint; }
	UFUNCTION()
		void SetLastCheckpoint(class ACheckpoint* NewLastCheckpoint) {LastCheckpoint = NewLastCheckpoint; }

	UFUNCTION()
		void SetGoalCheckpoint(class ACheckpoint* NewGoal) { GoalCheckpoint = NewGoal; }
	
	UFUNCTION()
		virtual void OnCompletedLap();

	UFUNCTION()
		virtual void GameEndState();
};
