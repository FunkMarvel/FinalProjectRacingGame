// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemySpawner.h"

#include <string>

#include "BaseEnemyActor.h"
#include "DroneActor.h"
#include "Components/BoxComponent.h"
#include "../DebugLog.h"
#include "Components/ArrowComponent.h"

// Sets default values
AEnemySpawner::AEnemySpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	TriggerVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerVolume"));
	SetRootComponent(TriggerVolume);
	TriggerVolume->bSelectable = true;
	
	ExitVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("ExitVolume"));
	ExitVolume->SetupAttachment(GetRootComponent());
	ExitVolume->bSelectable = true;
	
	SpawnPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("SpawnPoint"));
	SpawnPoint->SetupAttachment(GetRootComponent());
	SpawnPoint->bSelectable = true;

	TargetLocation = CreateDefaultSubobject<UArrowComponent>(TEXT("TargetLocation"));
	TargetLocation->SetupAttachment(GetRootComponent());
	TargetLocation->bSelectable = true;
	
}

void AEnemySpawner::OnTriggerOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (EnemyClassToSpawn && OtherActor->IsA<APawn>() && NumberOfSpawnedEnemies < NumberOfEnemiesToSpawn)
	{
		int32 Alternator{1};
		FVector SidewaysOffset{SpawnSpacing*SpawnPoint->GetRightVector()};
		FRotator SpawnOrientation{SpawnPoint->GetComponentRotation()};
		FVector SpawnLocation{SpawnPoint->GetComponentLocation()};
		for (int32 i{}; i < NumberOfEnemiesToSpawn; i++)
		{
			EnemyActors[i] = GetWorld()->SpawnActor<ABaseEnemyActor>(
				EnemyClassToSpawn, SpawnLocation + Alternator*((i+2-NumberOfEnemiesToSpawn%2)/2)*SidewaysOffset,
				SpawnOrientation
				);
			EnemyActors[i]->EnemyDeath.AddDynamic(this, &AEnemySpawner::OnEnemyDeath);
			EnemyActors[i]->SetEnemyArrayIndex(i);
			EnemyActors[i]->SetTargetLocation(TargetLocation->GetComponentLocation());
			NumberOfSpawnedEnemies++;
			Alternator *= -1;
		}
	}
	DL_NORMAL(FString::FromInt(NumberOfSpawnedEnemies) + " of " +
		FString::FromInt(NumberOfEnemiesToSpawn));
}

void AEnemySpawner::OnTriggerOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	
}

void AEnemySpawner::OnExitOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
}

void AEnemySpawner::OnExitOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->IsA<APawn>())
	{
		for (ABaseEnemyActor* Enemy : EnemyActors)
		{
			DL_NORMAL(TEXT("Destroyed"));
			if (Enemy) Enemy->HandleDeath();
		}
		NumberOfSpawnedEnemies = 0;
		NumberOfDestroyedEnemies = 0;
	}
}

// Called when the game starts or when spawned
void AEnemySpawner::BeginPlay()
{
	Super::BeginPlay();
	TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &AEnemySpawner::OnTriggerOverlapBegin);
	TriggerVolume->OnComponentEndOverlap.AddDynamic(this, &AEnemySpawner::OnTriggerOverlapEnd);

	ExitVolume->OnComponentBeginOverlap.AddDynamic(this, &AEnemySpawner::OnExitOverlapBegin);
	ExitVolume->OnComponentEndOverlap.AddDynamic(this, &AEnemySpawner::OnExitOverlapEnd);

	EnemyActors.Init(nullptr, NumberOfEnemiesToSpawn);
}


// Called every frame
void AEnemySpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AEnemySpawner::OnEnemyDeath(int32 EnemyIndex)
{
	EnemyActors[EnemyIndex] = nullptr;
	NumberOfDestroyedEnemies++;
	if (NumberOfDestroyedEnemies >= NumberOfSpawnedEnemies)
	{
		NumberOfDestroyedEnemies = 0;
		NumberOfSpawnedEnemies = 0;
	}
	DL_NORMAL("Enemy number " + FString::FromInt(EnemyIndex) + " died.");
	DL_NORMAL(FString::FromInt(NumberOfDestroyedEnemies) + " of " +
		FString::FromInt(NumberOfSpawnedEnemies) + " remaining.");
}

