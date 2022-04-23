// Fill out your copyright notice in the Description page of Project Settings.


#include "TimeAttackHUD.h"

ATimeAttackHUD::ATimeAttackHUD()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ATimeAttackHUD::BeginPlay()
{
	Super::BeginPlay();
}

void ATimeAttackHUD::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}
