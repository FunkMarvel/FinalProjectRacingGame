// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GameModes/RacingGameInstance.h"
#include "RacingUnrealProjectGameModeBase.generated.h"

UENUM()
enum EGoals
{
	None,
	Bronze,
	Silver,
	Gold
};

inline EGoals& operator++(EGoals& CurrentGoal) { switch (CurrentGoal)
{
case EGoals::None:
	return CurrentGoal = EGoals::Bronze;
case EGoals::Bronze:
	return CurrentGoal = EGoals::Silver;
case EGoals::Silver:
	return CurrentGoal = EGoals::Gold;
case EGoals::Gold:
	return CurrentGoal = EGoals::Gold;
}
	return CurrentGoal;
}

inline EGoals& operator--(EGoals& CurrentGoal) { switch (CurrentGoal)
{
case EGoals::None:
	return CurrentGoal = EGoals::None;
case EGoals::Bronze:
	return CurrentGoal = EGoals::None;
case EGoals::Silver:
	return CurrentGoal = EGoals::Bronze;
case EGoals::Gold:
	return CurrentGoal = EGoals::Silver;
}
	return CurrentGoal;
}

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

	FPlayerData CurrentPlayerData{};
	bool bFirstLap{true};
	bool bGameEnded{false};
	
public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Laps")
		int32 NumberOfLaps{3};
	int32 CurrentLap{1};
	
	EGoals CurrentBestGoal{EGoals::None};
	
	bool bGamePaused{false};
	
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

	UFUNCTION()
		virtual void OnPressPause();

	UFUNCTION()
		virtual void AddScore(int32 Score);

	UFUNCTION()
		bool SaveGame();

	UFUNCTION()
		bool LoadGame();

	UFUNCTION()
		FSlateColor ChangeGoalColor(EGoals CurrentGoal);
};
