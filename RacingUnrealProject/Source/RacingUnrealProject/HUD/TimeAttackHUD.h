// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "TimeAttackHUD.generated.h"

/**
 * 
 */
UCLASS()
class RACINGUNREALPROJECT_API ATimeAttackHUD : public AHUD
{
	GENERATED_BODY()

public:
	ATimeAttackHUD();
	
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets|Time")
		TSubclassOf<class URaceTimerWidget> TimerWidgetClass;

protected:
	UPROPERTY()
		class URaceTimerWidget* RaceTimerWidget{nullptr};
};
