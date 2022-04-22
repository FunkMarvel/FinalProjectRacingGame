// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseEnemyActor.h"
#include "Sockets.h"
#include "DroneActor.generated.h"

/**
 * 
 */
UCLASS()
class RACINGUNREALPROJECT_API ADroneActor : public ABaseEnemyActor
{
	GENERATED_BODY()

public:
	ADroneActor();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement")
		float ForwardOffset{5000.f};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement")
		float AvoidanceRadius{700.f};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement")
		float InterceptTime{3.f};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collider|Sensor")
		class USphereComponent* SensorSphere{nullptr};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
		TSubclassOf<class ASpikyBallEnemyActor> DroppableEnemyClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawning")
		float SpawnOffset{500.f};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
		float DropTime{1.f};
	float DropTimer{};
	
	float InterceptTimer{};
	float InterceptSpeed{};

	UPROPERTY(EditAnywhere, Category = "Collider|GrappleTarget")
		class UGrappleSphereComponent* GrappleSphereComponent{nullptr};

	UFUNCTION()
		void Grappled(FTransform SphereCompTransform);

	UFUNCTION()
		void Reached(float AddSpeedAmount);

	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
protected:
	FVector TargetLocation{};
	ASpikyBallEnemyActor* DroppedEnemyActor{nullptr};

	enum EDroneState
	{
		Idle,
		Intercepting,
		Attacking
	};

	EDroneState CurrentState{EDroneState::Idle};
	bool bEnteringState{false};

	void ChangeState(EDroneState NewState)
	{
		bEnteringState = true;
		CurrentState = NewState;
	}

	void InterceptingState();
	void AttackingState();


	void Move(FVector Target);
};
