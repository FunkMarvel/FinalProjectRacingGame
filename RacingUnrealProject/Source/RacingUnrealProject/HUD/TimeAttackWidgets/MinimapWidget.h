// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "MinimapWidget.generated.h"

/**
 * 
 */
UCLASS()
class RACINGUNREALPROJECT_API UMinimapWidget : public UUserWidget
{
	GENERATED_BODY()
	// my deseg --------------
	
protected:
	virtual void NativeConstruct() override;
	
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
public:
	UPROPERTY(EditAnywhere, meta =(BindWidget), BlueprintReadWrite)
		UImage* PointerImage = nullptr;
	UPROPERTY(EditAnywhere, meta =(BindWidget), BlueprintReadWrite)
		UImage* MapImage = nullptr;

	UPROPERTY()
		APawn* CarPawn = nullptr;

	UPROPERTY()
		FVector PlayerStartLocation = FVector::ZeroVector;
};
