// Fill out your copyright notice in the Description page of Project Settings.


#include "RacingUnrealProject/HUD/TimeAttackWidgets/MinimapWidget.h"

#include "Components/CanvasPanelSlot.h"
#include "Components/PanelSlot.h"

void UMinimapWidget::NativeConstruct() {
	Super::NativeConstruct();
}


void UMinimapWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime) {
	Super::NativeTick(MyGeometry, InDeltaTime);
	
	// UCanvasPanelSlot* slot = Cast<UCanvasPanelSlot>(PointerImage->Slot);
		
		
	FWidgetTransform trans = FWidgetTransform(FVector2D::ZeroVector, FVector2D::UnitVector,
		FVector2D::ZeroVector, 0.f);

	if (!CarPawn) {
		return;	
	}
	
	trans.Translation = FVector2D(CarPawn->GetActorLocation() - PlayerStartLocation);
	PointerImage->SetRenderTransform(trans);
	
	
	
	
}
