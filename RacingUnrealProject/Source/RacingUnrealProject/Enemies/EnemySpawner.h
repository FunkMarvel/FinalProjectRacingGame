// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemySpawner.generated.h"

UCLASS()
class RACINGUNREALPROJECT_API AEnemySpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemySpawner();
	

	UPROPERTY(EditAnywhere, Category="Trigger|Start Volume")
		class UBoxComponent* TriggerVolume;

	UPROPERTY(EditAnywhere, Category="Trigger|End Volume")
		class UBoxComponent* ExitVolume;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Spawning")
		TSubclassOf<class ABaseEnemyActor> EnemyClassToSpawn;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Spawning")
		int32 NumberOfEnemiesToSpawn{1};
	int32 NumberOfSpawnedEnemies{0};
	int32 NumberOfDestroyedEnemies{0};

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Spawning")
		float SpawnForwardOffset{10000.f};
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Spawning")
		float SpawnHeight{0.5f*SpawnForwardOffset};

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Spawning")
		float SpawnSpacing{1000.f};

	
	UFUNCTION()
	void OnTriggerOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnTriggerOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void OnExitOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnExitOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY()
		TArray<class ABaseEnemyActor*> EnemyActors;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnEnemyDeath(int32 EnemyIndex);

};
