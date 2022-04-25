// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "RacingGameInstance.generated.h"

USTRUCT()
struct FPlayerData
{
	GENERATED_BODY()

	// FString PlayerName{};
	float PlayerTime{};
	int32 PlayerScore{};

	FPlayerData() {}
	// FPlayerData(FString NewPlayerName) { PlayerName = NewPlayerName; }
	FPlayerData(float NewPlayerTime) { PlayerTime = NewPlayerTime; }
	FPlayerData(int32 NewPlayerScore) { PlayerScore = NewPlayerScore; }
	FPlayerData(float NewPlayerTime, int32 NewPlayerScore) : FPlayerData(NewPlayerTime) { PlayerScore = NewPlayerScore; }

	bool operator==(const FPlayerData &OtherPlayer)
	{
		return (PlayerScore == OtherPlayer.PlayerScore && FMath::IsNearlyEqual(PlayerTime, OtherPlayer.PlayerTime));
	}
	friend bool operator==(const FPlayerData &FirstPlayer, const FPlayerData &OtherPlayer)
	{
		return (FirstPlayer.PlayerScore == OtherPlayer.PlayerScore && FMath::IsNearlyEqual(FirstPlayer.PlayerTime, OtherPlayer.PlayerTime));
	}
};

/**
 * 
 */
UCLASS()
class RACINGUNREALPROJECT_API URacingGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	URacingGameInstance();

	enum ELevels
	{
		MainMenu,
		TimeAttack,
		ScoreAttack
	};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Levels")
		TArray<FName> LevelNames{};
	
	void ChangeLevel(FName LevelName);
	
	void SavePlayerData(FPlayerData NewPlayerData);

	
	FPlayerData* GetBestTimePlayer();
	FPlayerData* GetBestScorePlayer();

protected:
	TArray<FPlayerData> PlayerArray;
};