// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseEnemyActor.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEnemyDeath, int32, EnemyIndex);

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

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	AGravitySplineActor* GetClosestGravitySpline();

	bool IsGrounded();

	UPROPERTY(EditDefaultsOnly, Category = "Car")
	class UArrowComponent* ArrowRayCastStart{nullptr};

	int32 EnemyArrayIndex;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetEnemyArrayIndex(int32 Index) { EnemyArrayIndex = Index; }
	void HandleDeath();

	FEnemyDeath EnemyDeath;
};
