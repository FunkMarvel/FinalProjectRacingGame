// Fill out your copyright notice in the Description page of Project Settings.


#include "SpeedIndicatorWidget.h"

#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/SphereComponent.h"
#include "RacingUnrealProject/CarPawn.h"
#include "RacingUnrealProject/DebugLog.h"

void USpeedIndicatorWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime) {
	Super::NativeTick(MyGeometry, InDeltaTime);


	if (CarPawn) {
		float Percent = CarPawn->SphereComp->GetPhysicsLinearVelocity().Size() / CarPawn->GetMaxSpeed();
		//scaling down by 30% this means if we are over 30% speed we hit the max max speed and change speed icon
		Percent *= 0.7f;

		//lerp
		float Lerp = FMath::Lerp(SpeedProgressBar->Percent, Percent, GetWorld()->GetDeltaSeconds() * fProgressBarChangeSpeed);
		
		SetPercentage(Lerp);
		
		//handling image
		bool bMaxSpeed = Lerp >= 1.f ? true : false;
		SetMaxSpeedIndicatorOn(bMaxSpeed);

		
		
	}
}

void USpeedIndicatorWidget::NativeConstruct()
{
	Super::NativeConstruct();
	OuterImageInstanceDynamic = UMaterialInstanceDynamic::Create(OuterImageMaterial, this);
}

void USpeedIndicatorWidget::SetMaxSpeedIndicatorOn(bool bMaxSpeed) {
	//guard clause
	if (!NotMaxSpeedTex || !MaxSpeedTex || !MaxSpeedImage)
		return;

	
	if (bMaxSpeed) {
		MaxSpeedImage->SetBrushFromTexture(MaxSpeedTex);
	}
	else {
		MaxSpeedImage->SetBrushFromTexture(NotMaxSpeedTex);
	}
}

void USpeedIndicatorWidget::SetPercentage(const float Percent) {
	SpeedProgressBar->SetPercent(Percent);
	OuterImageInstanceDynamic->SetScalarParameterValue("Percent", Percent);
	
}

void USpeedIndicatorWidget::PsudoBeginPlay() {
	OuterImage->SetBrushFromMaterial(OuterImageInstanceDynamic);
	// OuterImage->Brush.SetResourceObject(OuterImageMaterial);
	
}
