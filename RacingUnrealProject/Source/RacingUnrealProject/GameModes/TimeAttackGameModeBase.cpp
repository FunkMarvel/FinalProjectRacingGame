// Fill out your copyright notice in the Description page of Project Settings.


#include "TimeAttackGameModeBase.h"

#include "Kismet/GameplayStatics.h"
#include "RacingUnrealProject/GameStartSequenceActor.h"

ATimeAttackGameModeBase::ATimeAttackGameModeBase()
{
}

void ATimeAttackGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> StartSequenceActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGameStartSequenceActor::StaticClass(), StartSequenceActors);

	if (StartSequenceActors.Num() >= 1)
	{
		GameStartSequence = Cast<AGameStartSequenceActor>(StartSequenceActors[0]);
		GameStartSequence->StartSequenceFinishedEvent.AddDynamic(this, &ATimeAttackGameModeBase::BeginTimer);
	}
}

void ATimeAttackGameModeBase::BeginTimer()
{
}
