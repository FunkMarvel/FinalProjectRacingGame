// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Checkpoint.generated.h"

UCLASS()
class RACINGUNREALPROJECT_API ACheckpoint : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACheckpoint();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//my deseg
private:
	//components
	UPROPERTY(meta = (AllowPrivateAccess = "true"), EditAnywhere, Category = "Checkpoint")
		class UArrowComponent* BaseArrow{};
	UPROPERTY(meta = (AllowPrivateAccess = "true"), EditAnywhere, Category = "Checkpoint")
		class UStaticMeshComponent* MainMesh{};
	UPROPERTY(meta = (AllowPrivateAccess = "true"), EditAnywhere, Category = "Checkpoint")
		class UBoxComponent* BoxComponent{};
	UPROPERTY(meta = (AllowPrivateAccess = "true"), EditAnywhere, Category = "Checkpoint")
		class UArrowComponent* SpawnArrow{};
	UPROPERTY(meta = (AllowPrivateAccess = "true"), EditAnywhere, Category = "Checkpoint")
		class USkeletalMeshComponent* PillarOneMesh = nullptr;
	UPROPERTY(meta = (AllowPrivateAccess = "true"), EditAnywhere, Category = "Checkpoint")
		class USkeletalMeshComponent* PillarTwoMesh = nullptr;

	//variables
	UPROPERTY()
		class ARacingUnrealProjectGameModeBase* GameModee{};
	UPROPERTY(meta = (AllowPrivateAccess = "ture"), EditAnywhere, Category = "Checkpoint")
		class AGravitySplineActor* GravitySplineActor{};
	UPROPERTY(meta = (AllowPrivateAccess = "ture"), EditAnywhere, Category = "Checkpoint|Enter")
		class AEnterExitTrigger* EnterTrigger = nullptr;
	UPROPERTY(meta = (AllowPrivateAccess = "ture"), EditAnywhere, Category = "Checkpoint|Enter")
		class UAnimationAsset* OpenAnimation = nullptr;
	UPROPERTY(meta = (AllowPrivateAccess = "ture"), EditAnywhere, Category = "Checkpoint|Enter")
	class UAnimationAsset* IdleAnimation = nullptr;

	
	//funcs
	UFUNCTION()
		void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void OnTriggerEnter();
	
	
public:
	// public funcs
	UFUNCTION()
		UArrowComponent* GetSpawnArrow() const {return SpawnArrow; }
	UFUNCTION()
		AGravitySplineActor* GetCheckpointGravitySpline() const {return GravitySplineActor; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Track position")
		bool bStartAndFinishLine{false};

};
