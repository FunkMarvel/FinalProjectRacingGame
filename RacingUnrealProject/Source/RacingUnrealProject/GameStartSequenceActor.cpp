// Fill out your copyright notice in the Description page of Project Settings.


#include "GameStartSequenceActor.h"

#include "CarPawn.h"
#include "DebugLog.h"
#include "Camera/CameraComponent.h"
#include "Components/ArrowComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AGameStartSequenceActor::AGameStartSequenceActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowCompoennt"));
	SetRootComponent(ArrowComponent);

	CameraSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraSpringArm"));
	CameraSpringArm->SetupAttachment(GetRootComponent());

	ShowoffCamema = CreateDefaultSubobject<UCameraComponent>(TEXT("ShowoffCamera"));
	ShowoffCamema->SetupAttachment(CameraSpringArm);
}

// Called when the game starts or when spawned
void AGameStartSequenceActor::BeginPlay()
{
	Super::BeginPlay();
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	CarPawn = Cast<ACarPawn>(PlayerController->GetPawn());
	
	//sets this as the active camera!
	PlayerController->SetViewTargetWithBlend(this, 0.f);

	PlayerController->DisableInput(PlayerController);
	
	//seting up timer
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AGameStartSequenceActor::OnShowoffFinshed,
		ShowoffYawCurve->FloatCurve.GetLastKey().Time - 0.5f, false);
	// DL_NORMAL(FString::SanitizeFloat(ShowoffYawCurve->FloatCurve.GetLastKey().Time))
	
}

// Called every frame
void AGameStartSequenceActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	CurrentAnimationTime += GetWorld()->GetDeltaSeconds();

	CameraSpringArm->SetRelativeRotation(FRotator(0,ShowoffYawCurve->GetFloatValue(CurrentAnimationTime),0));
	
}
void AGameStartSequenceActor::OnShowoffFinshed()
{
	APawn* PlayerPawn = UGameplayStatics::GetPlayerController(this, 0)->GetPawn();
	UGameplayStatics::GetPlayerController(this, 0)->SetViewTargetWithBlend(PlayerPawn,
		1.f, EViewTargetBlendFunction::VTBlend_Cubic);
	SetLifeSpan(1.f);
}

