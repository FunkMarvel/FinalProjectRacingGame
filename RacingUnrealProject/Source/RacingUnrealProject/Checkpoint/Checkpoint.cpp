// Fill out your copyright notice in the Description page of Project Settings.


#include "Checkpoint.h"

#include "../CarPawn.h"
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "RacingUnrealProject/EnterExitTrigger.h"
#include "RacingUnrealProject/RacingUnrealProjectGameModeBase.h"

// Sets default values
ACheckpoint::ACheckpoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BaseArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("BaseArrow"));
	SetRootComponent(BaseArrow);

	MainMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MainMesh"));
	MainMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MainMesh->SetupAttachment(GetRootComponent());

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent(Trigger)"));
	BoxComponent->SetupAttachment(GetRootComponent());
	// boxcomponent is set to overlap all by defualt. So dont have to set this manually at this time in cpp

	SpawnArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("PlayerSpawnArrow"));
	SpawnArrow->SetupAttachment(GetRootComponent());

	PillarOneMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("PillarOneMesh"));
	PillarOneMesh->SetupAttachment(GetRootComponent());

	PillarTwoMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("PillarTwoMesh"));
	PillarTwoMesh->SetupAttachment(GetRootComponent());

	
}

// Called when the game starts or when spawned
void ACheckpoint::BeginPlay()
{
	Super::BeginPlay();

	//setting up game mode
	AGameModeBase* BaseGameMode = GetWorld()->GetAuthGameMode();
	if (BaseGameMode->IsA(ARacingUnrealProjectGameModeBase::StaticClass()))
		GameModee = Cast<ARacingUnrealProjectGameModeBase>(BaseGameMode);

	if (bStartAndFinishLine) GameModee->SetGoalCheckpoint(this);
	
	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &ACheckpoint::OnBeginOverlap);

	if (EnterTrigger != nullptr) {
		EnterTrigger->EventTriggerEnterExit.AddDynamic(this, &ACheckpoint::OnTriggerEnter);
	}
}

// Called every frame
void ACheckpoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACheckpoint::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA(ACarPawn::StaticClass()))
	{
		UE_LOG(LogTemp, Warning, TEXT("Overlap!"))
		if (GameModee)
		{
			GameModee->SetLastCheckpoint(this);
			if (bStartAndFinishLine) GameModee->OnCompletedLap();
		}
	}
	
}

void ACheckpoint::OnTriggerEnter() {
	PillarOneMesh->PlayAnimation(OpenAnimation, false);
	PillarTwoMesh->PlayAnimation(OpenAnimation, false);

	
	//timer
	FTimerHandle Handle;
	FTimerDelegate Callback;
	Callback.BindLambda([this]
	{
		PillarOneMesh->PlayAnimation(IdleAnimation, false);
		PillarTwoMesh->PlayAnimation(IdleAnimation, false);
	});
	GetWorld()->GetTimerManager().SetTimer(Handle, Callback, 5.f, false);
}

