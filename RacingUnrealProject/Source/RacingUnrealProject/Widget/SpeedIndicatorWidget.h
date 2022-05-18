// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "SpeedIndicatorWidget.generated.h"

/**
 * 
 */
UCLASS()
class RACINGUNREALPROJECT_API USpeedIndicatorWidget : public UUserWidget
{
	GENERATED_BODY()

	//my deseg

private:

	UPROPERTY(EditAnywhere, meta =(BindWidget/*, AllowPrivateAccess = "true"*/))
	class UProgressBar* SpeedProgressBar = nullptr;

	UPROPERTY(EditAnywhere, meta = (BindWidget/*, AllowPrivateAccess = "true"*/))
	class UImage* MaxSpeedImage = nullptr;

	//textures
	UPROPERTY(EditAnywhere, Category = "SpeedIndicator", meta =(AllowPrivateAccess = "true"))
		class UTexture2D* MaxSpeedTex = nullptr;
	UPROPERTY(EditAnywhere, Category = "SpeedIndicator", meta =(AllowPrivateAccess = "true"))
		class UTexture2D* NotMaxSpeedTex = nullptr;

	// Outer Element
	UPROPERTY(EditAnywhere, meta = (BindWidget/*, AllowPrivateAccess = "true"*/))
		class UImage* OuterImage = nullptr;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
		class UMaterial* OuterImageMaterial = nullptr;
	UPROPERTY()
		class UMaterialInstanceDynamic* OuterImageInstanceDynamic = nullptr;
	
	
	//variables
	UPROPERTY(meta = (AllowPrivateAccess = "true"))
		float fProgressBarChangeSpeed = 1.f;
protected:

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	virtual void NativeConstruct() override;
	
public:
	
	UFUNCTION()
		void SetMaxSpeedIndicatorOn(bool bMaxSpeed);

	UFUNCTION()
		void SetPercentage(const float Percent);

	UFUNCTION()
		void PsudoBeginPlay();
	
	UPROPERTY()
		class ACarPawn* CarPawn = nullptr;
};


