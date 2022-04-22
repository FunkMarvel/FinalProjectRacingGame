// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuHUD.h"

#include "Blueprint/UserWidget.h"
#include "MainMenuWidgets/MainMenuHUDWidget.h"

AMainMenuHUD::AMainMenuHUD()
{
}

void AMainMenuHUD::DrawHUD()
{
	Super::DrawHUD();
}

void AMainMenuHUD::BeginPlay()
{
	Super::BeginPlay();

	if (MainMenuClass)
	{
		MainMenuHUDWidget = CreateWidget<UMainMenuHUDWidget>(GetWorld(), MainMenuClass);

		if (MainMenuHUDWidget)
		{
			MainMenuHUDWidget->AddToViewport();
		}
	}
}
