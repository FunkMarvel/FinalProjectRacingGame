// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraEffecttComponent.h"

#include "CarPawn.h"
#include "DebugLog.h"
#include "Camera/CameraComponent.h"
#include "Camera/CameraModifier.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/ArrowComponent.h"
#include "GameFramework/SpringArmComponent.h" 

// Sets default values for this component's properties
UCameraEffecttComponent::UCameraEffecttComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UCameraEffecttComponent::BeginPlay()
{
	Super::BeginPlay();

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UCameraEffecttComponent::LateBeginPlay, 0.1f, false);
	
	if (GetOwner()->IsA(ACarPawn::StaticClass()))
		CarPawn = Cast<ACarPawn>(GetOwner());

	CameraCurrent = CarPawn->MainCamera;
	if (CameraCurrent)
		StartFOV = CameraCurrent->FieldOfView;


	// this is all the setup you ned for the boost modifier, it will de activate itself when being enabled
	
	BoostCameraModifier = UGameplayStatics::GetPlayerCameraManager(this, 0)->AddNewCameraModifier(BoostCameraModifierClass);
	BoostCameraModifier->DisableModifier(true);
	
	//setting up speed camera shake

	// UGameplayStatics::PlayWorldCameraShake(GetWorld(), CameraShake, CarPawn->SphereComp->GetComponentLocation(),
	// 	 	 0.f, 1000.f, 1.f);
}


// Called every frame
void UCameraEffecttComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	float Speed = 0.f;
	if (CarPawn->SphereComp->IsSimulatingPhysics())
	{
		Speed = CarPawn->SphereComp->GetPhysicsLinearVelocity().Size();
		
	}
	else
	{
		Speed = 10000.f;
	}
	float TargetFOV = Speed / 300.f + StartFOV;
	TargetFOV = FMath::Clamp(TargetFOV, 0.f, MaxFOV);

	TargetFOV = FMath::FInterpTo(CameraCurrent->FieldOfView, TargetFOV, GetWorld()->GetDeltaSeconds(), 5.f);

	// TargetFOV = FMath::Clamp(TargetFOV, 0.f, MaxFOV);
	
	CameraCurrent->SetFieldOfView(TargetFOV);

	//speed camera shake
	if (CarPawn->SphereComp->IsSimulatingPhysics() && SpeedCameraShake)
	{
		SpeedCameraShake->ShakeScale = CarPawn->SphereComp->GetPhysicsLinearVelocity().Size() * (1.f/SpeedShakeInverseIntensity);
	}
}

void UCameraEffecttComponent::SpaceCamera()
{
	if (bFirstCamera)
	{
		CarPawn->MainCamera->AttachToComponent(CarPawn->SecondCameraTransfrom, FAttachmentTransformRules::SnapToTargetIncludingScale);
	}
	else
	{
		CarPawn->MainCamera->AttachToComponent(CarPawn->CameraBoom, FAttachmentTransformRules::SnapToTargetIncludingScale);
	}
	
	bFirstCamera = !bFirstCamera;
}

void UCameraEffecttComponent::LateBeginPlay()
{
	SpeedCameraShake = UGameplayStatics::GetPlayerCameraManager(this, 0)->StartCameraShake(SpeedCameraShakeClass);
	
	// UGameplayStatics::PlayWorldCameraShake(GetWorld(), CameraShake, GetOwner()->GetActorLocation(),
	// 		  0.f, 1000.f, 1.f);
}


