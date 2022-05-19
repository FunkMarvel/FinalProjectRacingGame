// Fill out your copyright notice in the Description page of Project Settings.


#include "BasicEatableActor.h"

#include "DrawDebugHelpers.h"
#include "../GrappleSphereComponent.h"
#include "Components/BoxComponent.h"
#include "RacingUnrealProject/EnterExitTrigger.h"
#include "RacingUnrealProject/RacingUnrealProjectGameModeBase.h"

// Sets default values
ABasicEatableActor::ABasicEatableActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MainMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	MainMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetRootComponent(MainMesh);
	
	GrappleSphereComponent = CreateDefaultSubobject<UGrappleSphereComponent>(TEXT("GrappleSphereComponent"));
	GrappleSphereComponent->SetupAttachment(GetRootComponent());
	GrappleSphereComponent->SetIsEatable(true);
	
	
		
}

// Called when the game starts or when spawned
void ABasicEatableActor::BeginPlay()
{
	Super::BeginPlay();
	StartLocation = GetActorLocation();
	GrappleSphereComponent->OnReachedEvent.AddDynamic(this, &ABasicEatableActor::OnReached);

	if (RespawnTrigger) {
		RespawnTrigger->EventTriggerEnterExit.AddDynamic(this, &ABasicEatableActor::Respawn);
	}
}

// Called every frame
void ABasicEatableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABasicEatableActor::VisualizeTrigger() {
	if (RespawnTrigger == nullptr)
		return;

	//enter spline
	const FVector StartLoc = GetActorLocation();
	const FVector EnterTriggerLoc =  RespawnTrigger->GetActorLocation();
	DrawDebugLine(GetWorld(), StartLoc, EnterTriggerLoc, FColor::Green, false, 1.f, 0,100.f);
	
}

void ABasicEatableActor::OnReached(float AddSpeedAmount)
{
	MainMesh->SetVisibility(false);
	GrappleSphereComponent->SetIsEatable(false);
	GrappleSphereComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ARacingUnrealProjectGameModeBase* GameModeBase{Cast<ARacingUnrealProjectGameModeBase>(GetWorld()->GetAuthGameMode())};

	if (IsValid(GameModeBase))
	{
		GameModeBase->AddScore(ScoreValue);
	}
}

void ABasicEatableActor::Respawn() {
	SetActorLocation(StartLocation);
	MainMesh->SetVisibility(true);
	GrappleSphereComponent->SetIsEatable(true);
	GrappleSphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

