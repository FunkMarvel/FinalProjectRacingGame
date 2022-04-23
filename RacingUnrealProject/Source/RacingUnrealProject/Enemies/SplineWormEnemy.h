// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "RacingUnrealProject/Enums/Enums.h"

#include "SplineWormEnemy.generated.h"


UCLASS()
class RACINGUNREALPROJECT_API ASplineWormEnemy : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASplineWormEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UPROPERTY(EditAnywhere, Category = "Spline")
		class USplineComponent* Spline = nullptr;
	UPROPERTY(EditAnywhere, Category = "Spline")
		class UStaticMeshComponent* WormTargetMesh = nullptr;
	UPROPERTY(EditAnywhere, Category = "Spline", BlueprintReadOnly)
		class UGrappleSphereComponent* GrappleSphereComponent = nullptr;
	UPROPERTY(EditAnywhere, Category = "Spline")
		class UBoxComponent* TriggerBox = nullptr;
	
	
	UPROPERTY(EditAnywhere, Category = "Spline")
		class UStaticMesh* NeckSegment = nullptr;
	UPROPERTY(EditAnywhere, Category = "Spline")
		class UCurveFloat* MovmentCurveFloat;
	UPROPERTY()
		TArray<class USplineMeshComponent*> SplineMeshComponents;

	UFUNCTION()
		void UpdateSplineMeshComponent();

	UFUNCTION()
	/**
	 * @brief 
	 * @param RatioOnSnake 1 means at the tip, 0 means at the end
	 */
		void UpdateHeadTransfrom(float RatioOnSnake);
	UFUNCTION()
		float GetWormRealLength() const;
	
	UPROPERTY(EditAnywhere, Category = "Spline")
		float NeckSegmentLength = 500.f;
	UPROPERTY(EditAnywhere, Category = "Spline")
		float WormGoalLength = 2000.f;
	UPROPERTY(EditAnywhere, Category = "Spline")
		float SplineMeshOverLap = 50.f;
	UPROPERTY(EditAnywhere, Category = "Spline")
		float WormMoveDuration = 5.f;
	UPROPERTY(meta = (ToolTip = "Is the distance along spline the end of the worm is")
		, EditAnywhere, Category = "Spline")
		float CurrentWormDistance = 0.f;
	UPROPERTY(EditAnywhere, Category = "Spline")
		float HeadDistanceFromBody = 500.f;
	UPROPERTY(EditAnywhere, Category = "Spline")
		ESplineWormHeadAxis CurrentHeadAxis = ESplineWormHeadAxis::Right;
	
	UPROPERTY(EditAnywhere, Category = "Spline")
		bool bPlayingAnim = false;
	UPROPERTY()
		float CurrentMoveTime = 0.f;
private:
	UFUNCTION()
		void OnGrappleReaced(float Addspeed);
	UFUNCTION()
		void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
};
