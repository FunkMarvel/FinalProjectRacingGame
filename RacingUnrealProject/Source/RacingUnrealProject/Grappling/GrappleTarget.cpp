// Fill out your copyright notice in the Description page of Project Settings.


#include "GrappleTarget.h"

#include "DrawDebugHelpers.h"
#include "RacingUnrealProject/GrappleSphereComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h" 
#include "RacingUnrealProject/EnterExitTrigger.h"

// Sets default values
AGrappleTarget::AGrappleTarget()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	
	
	RootArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("RootArrow"));
	SetRootComponent(RootArrow);

	MainMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MainSkeletalMesh"));
	MainMesh->SetupAttachment(GetRootComponent());

	GrappleSphereComponent = CreateDefaultSubobject<UGrappleSphereComponent>(TEXT("GrappleSphereComponent"));
	GrappleSphereComponent->SetupAttachment(MainMesh, FName("Grapple"));
	GrappleSphereComponent->SetIsEnabled(false);

	
	
	
}

// Called when the game starts or when spawned
void AGrappleTarget::BeginPlay()
{
	Super::BeginPlay();
	
	GrappleSphereComponent->OnReachedEvent.AddDynamic(this, &AGrappleTarget::OnReachedTarget);
	GrappleSphereComponent->OnGrappleHitEvent.AddDynamic(this, &AGrappleTarget::OnGrappleTarget);

	//enter exit
	if (EnterTrigger) {
		EnterTrigger->EventTriggerEnterExit.AddDynamic(this, &AGrappleTarget::OnEnterTrigger);
	}

	if (ExitTrigger) {
		ExitTrigger->EventTriggerEnterExit.AddDynamic(this, &AGrappleTarget::OnExitTrigger);
	}
	
}

// Called every frame
void AGrappleTarget::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	GrappleSphereComponent->SetRelativeLocation(FVector::ZeroVector);
	

}



void AGrappleTarget::OnReachedTarget(float AddSpeedAmount)
{
	//Destroy();
	
}

void AGrappleTarget::OnGrappleTarget(FTransform SphereCompTransfrom)
{
	
}

void AGrappleTarget::VisualizeTriggers() {
	if (EnterTrigger == nullptr || ExitTrigger == nullptr) {
		return;
	}
	//enter spline
	FVector StartLoc = GetActorLocation();
	FVector EnterTriggerLoc = EnterTrigger->GetActorLocation();
	FVector ExitTriggerLoc = ExitTrigger->GetActorLocation();

	//drawing rays
	DrawDebugLine(GetWorld(), StartLoc, EnterTriggerLoc, FColor::Green, false, 1.f, 0,100.f);
	DrawDebugLine(GetWorld(), StartLoc, ExitTriggerLoc, FColor::Red, false, 1.f, 0,100.f);
}

void AGrappleTarget::OnEnterTrigger() {
	MainMesh->SetVisibility(true, false);
	MainMesh->Play(false);
	GrappleSphereComponent->SetIsEnabled(true);
}

void AGrappleTarget::OnExitTrigger() {
	MainMesh->SetVisibility(false, false);
	GrappleSphereComponent->SetIsEnabled(false);
	
}

