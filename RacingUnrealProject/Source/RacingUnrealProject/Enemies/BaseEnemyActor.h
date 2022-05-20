// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RacingUnrealProject/RacingUnrealProjectGameModeBase.h"
#include "BaseEnemyActor.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEnemyDeath, int32, EnemyIndex);

// parent class for drones and spiky balls.
UCLASS()
class RACINGUNREALPROJECT_API ABaseEnemyActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABaseEnemyActor();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collider")
	class USphereComponent* SphereComp{nullptr};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	class UStaticMeshComponent* CosmeticMesh{nullptr};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gravity|Spline")
	class AGravitySplineActor* GravitySplineActive{nullptr};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Target|Player Pawn")
	class ACarPawn* PlayerPawn{nullptr};

	UFUNCTION()
	virtual void OnBeginOverLap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                            UPrimitiveComponent* OtherComp,
	                            int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	FVector GetToPlayerVector(bool bNormalized = true);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
		bool bDropEnemy{false};
	
protected:
	FVector TargetLocation{};
	FVector LocalUpVector{};
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	AGravitySplineActor* GetClosestGravitySpline();

	bool IsGrounded();

	UPROPERTY(EditDefaultsOnly, Category = "Ground")
	class UArrowComponent* ArrowRayCastStart{nullptr};

	int32 EnemyArrayIndex;

	UPROPERTY(EditAnywhere, Category="Score")
		int32 ScoreValue{1};

	ARacingUnrealProjectGameModeBase* GameModeBase{nullptr};

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetEnemyArrayIndex(int32 Index) { EnemyArrayIndex = Index; }
	virtual void HandleDeath();

	void SetTargetLocation(FVector Target) { TargetLocation = Target; }
	FEnemyDeath EnemyDeath;
};
