// Fill out your copyright notice in the Description page of Project Settings.


#include "RacingGameInstance.h"

#include "Kismet/GameplayStatics.h"

URacingGameInstance::URacingGameInstance()
{
	TimeAttackTimes.Reserve(1);
	ScoreAttackScores.Reserve(1);
}

void URacingGameInstance::ChangeLevel(FName LevelName)
{
	UGameplayStatics::OpenLevel(GetWorld(), LevelName);
}

void URacingGameInstance::SaveTime(float TimeToSave)
{
	TimeAttackTimes.Add(TimeToSave);
}

void URacingGameInstance::SaveScore(uint32 ScoreToSave)
{
	ScoreAttackScores.Add(ScoreToSave);
}
