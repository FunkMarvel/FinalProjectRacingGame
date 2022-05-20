// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "RacingGameInstance.generated.h"

/// @brief  Game instance class for persistent variables.
USTRUCT()
struct FPlayerData
{
	GENERATED_BODY()

	
	// FString PlayerName{};
	UPROPERTY()
	float PlayerTime{};

	UPROPERTY()
	int32 PlayerScore{};

	FPlayerData() {}
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

protected:
	TArray<FPlayerData> PlayerArray;


	//this should stop pure virutal function form being called (Thanks Bjørn Joachim Olsen!!!)
	virtual void Init() override;
	virtual void Shutdown() override;
	virtual void StartGameInstance() override;
	
public:
	URacingGameInstance();

	enum ELevels
	{
		MainMenu,
		TimeAttack,
		ScoreAttack
	};

	const FString DefaultSaveSlotName{TEXT("DefaultSaveSlot")};
	const uint32 DefaultSaveSlotIndex{0};

	const FString SettingsSaveSlotName{TEXT("SettingsSaveSlot")};
	const uint32 SettingsSaveSlotIndex{1};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Levels")
		TArray<FName> LevelNames{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Goals")
		TArray<float> TimesToBeat{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Goals")
		TArray<int32> ScoresToBeat{};
	
	void ChangeLevel(FName LevelName);
	
	void SavePlayerData(FPlayerData NewPlayerData);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Levels")
		int32 NumberOfLaps{3};
	
	FPlayerData* GetBestTimePlayer();
	FPlayerData* GetBestScorePlayer();
	TArray<FPlayerData> GetPlayerDataArray() { return PlayerArray; }
};