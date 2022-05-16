// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseEnemyActor.h"
#include "SpikyBallEnemyActor.generated.h"

/**
 * 
 */
UCLASS()
class RACINGUNREALPROJECT_API ASpikyBallEnemyActor : public ABaseEnemyActor
{
	GENERATED_BODY()

public:
	ASpikyBallEnemyActor();

	virtual void BeginPlay() override;
	void Tick(float DeltaSeconds) override;


protected:
	void SetUpVectorAsSplineUpAxis();

public:
	void RotateSphereComponentToLocalUpVector();

	enum EBallState
	{
		Idle,
		Airborne,
		OnGround,
		Spiked
	};
	
	EBallState CurrentState{Airborne};
	bool bEnteringState{false};

	void ChangeState(EBallState NewState);

protected:
	FTimerHandle SpikeTimerHandle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attack")
		float SpikeTimer{1.f};
	void TriggerSpikes();

	void AirborneState();
	void GroundedState();
	void SpikedState();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Rolling")
		float Acceleration{1500};

	void LookAtPlayer();
	void Move();
	void ApplyGravity();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Rolling")
		float MaxSpeed{2000.f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Rolling")
		float TargetDistance{12000.f};
};

inline void ASpikyBallEnemyActor::TriggerSpikes()
{
	ChangeState(EBallState::Spiked);
}
