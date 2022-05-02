// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PauseMenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class RACINGUNREALPROJECT_API UPauseMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta=(BindWidget))
	class UButton* ResumeButton{nullptr};

	UPROPERTY(meta=(BindWidget))
	class UButton* ReturnToMenuButton{nullptr};

	UPROPERTY(meta=(BindWidget))
	class UButton* ResetToCheckpoint{nullptr};
};
