// Fill out your copyright notice in the Description page of Project Settings.


#include "GameStartSequenceActor.h"

#include "CarPawn.h"
#include "DebugLog.h"
#include "Camera/CameraComponent.h"
#include "Components/ArrowComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Widget/GrappableWidget.h"

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

	if (bSkipSequence) // if we want to skip this, just destroy the object
	{
		StartSequenceFinishedEvent.Broadcast();
		Destroy();
		return;
	}
	
	PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	CarPawn = Cast<ACarPawn>(PlayerController->GetPawn());

	
	// CarPawn->SpacePressedEvent.AddDynamic(this, &AGameStartSequenceActor::Skip);
	
	//sets camera on this actor as the active camera
	PlayerController->SetViewTargetWithBlend(this, 0.f);

	
	CarPawn->DisableInput(PlayerController);
	
	
	//seting up timer
	
	GetWorld()->GetTimerManager().SetTimer(ShowOffTimerHandle, this, &AGameStartSequenceActor::OnShowoffFinshed,
		ShowoffYawCurve->FloatCurve.GetLastKey().Time - 0.5f, false);

	
}

void AGameStartSequenceActor::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	// PlayerInputComponent->BindAction("Space",IE_Pressed, this, &AGameStartSequenceActor::Skip);
	//does not work as this is not the active player controller
}

// Called every frame
void AGameStartSequenceActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	CurrentShowOffTime += GetWorld()->GetDeltaSeconds();
	CameraSpringArm->SetRelativeRotation(FRotator(0,ShowoffYawCurve->GetFloatValue(CurrentShowOffTime),0));
}
void AGameStartSequenceActor::OnShowoffFinshed()
{
	float BlendTime = 2.f;
	
	UGameplayStatics::GetPlayerController(this, 0)->SetViewTargetWithBlend(CarPawn,
		BlendTime, EViewTargetBlendFunction::VTBlend_Cubic);

	CountDownWidget->AddToViewport();
	CountDownWidget->PlayCloseAnimationCpp();
	
	
	GetWorld()->GetTimerManager().SetTimer(BlendFinishedTimerHandle, this, &AGameStartSequenceActor::BlendFinished, BlendTime, false);
}

void AGameStartSequenceActor::BlendFinished()
{
	CarPawn->EnableInput(PlayerController);
	SetLifeSpan(0.1f);
	StartSequenceFinishedEvent.Broadcast();
}

void AGameStartSequenceActor::Skip()
{
	GetWorld()->GetTimerManager().ClearTimer(ShowOffTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(BlendFinishedTimerHandle);
	UGameplayStatics::GetPlayerController(this, 0)->SetViewTargetWithBlend(CarPawn,
		0.f, EViewTargetBlendFunction::VTBlend_Cubic);
	BlendFinished();
	DL_NORMAL("hehe skip!")
}

