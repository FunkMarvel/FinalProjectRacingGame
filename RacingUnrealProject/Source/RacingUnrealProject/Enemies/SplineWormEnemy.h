// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SplineComponent.h"
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

	// my deseg
public:
	UPROPERTY(EditAnywhere, Category = "Meshes")
	class UStaticMeshComponent* WormTargetMesh = nullptr;
	UPROPERTY(EditAnywhere, Category = "Meshes")
	class UStaticMeshComponent* WormHeadMesh = nullptr;
	UPROPERTY(EditAnywhere, Category = "Spline")
		class UGrappleSphereComponent* GrappleSphereComponent = nullptr;
	UPROPERTY(EditAnywhere, Category = "Spline")
		class UBoxComponent* TriggerBox = nullptr;
	UPROPERTY(EditAnywhere, Category = "Spline")
		class USplineComponent* Spline = nullptr;
	
private:
	
	// float curves
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"), Category = "Spline")
		class UCurveFloat* MovmentCurveFloat = nullptr;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"), Category = "Spline")
		class UCurveFloat* WormSizeCurve = nullptr;

	//static meshes
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"), Category = "Spline")
		class UStaticMesh* NeckSegment = nullptr;

	//not editor visible
	UPROPERTY()
		TArray<class USplineMeshComponent*> SplineMeshComponents;
	


	//funcs
	UFUNCTION()
		void UpdateSplineMeshComponent();
	
	UFUNCTION()
		void HandleIdleAnimation();
	
	UFUNCTION()
	/**
	 * @brief 
	 * @param RatioOnSnake 1 means at the tip, 0 means at the end
	 */
		void UpdateTargetTransfrom(float RatioOnSnake);

	UFUNCTION()
	    void UpdateHeadTransfrom();
	
	UFUNCTION()
		void InitSplineSegments();
	UFUNCTION()
		float GetWormRealLength() const;
public:
	
	UPROPERTY(EditAnywhere, Category = "Spline")
		float NeckSegmentLength = 500.f;
	UPROPERTY(EditAnywhere, Category = "Spline")
		float WormGoalLength = 2000.f;
	UPROPERTY(EditAnywhere, Category = "Spline")
		float SplineMeshOverLap = 50.f;
	UPROPERTY(EditAnywhere, Category = "Spline")
		float WormMoveDuration = 5.f;
	UPROPERTY(EditAnywhere, Category = "Spline")
		float HeadDistanceFromBody = 500.f;
	UPROPERTY(EditAnywhere, Category = "Spline")
		ESplineWormHeadAxis CurrentHeadAxis = ESplineWormHeadAxis::Right;
	UPROPERTY(meta = (ToolTip = "If the Upvector for the CarPawn is upside down at the end of the grapple, Use this to invert"),
		EditAnywhere, Category = "Spline")
	bool bInvertUpHeadAxis = false;
	UPROPERTY(EditAnywhere, Category = "Spline", meta = (ClampMin = 0.f, ClampMax = 180.f))
		float RandomRotationAmoundt = 20.f;

	//move animation varibales
	UPROPERTY(EditAnywhere, Category = "Spline|MoveAnimation")
	float CurrentDistanceAffector = 0.001f;
	UPROPERTY(EditAnywhere, Category = "Spline|MoveAnimation")
	float CurrentMoveTimeAffector = 10.f;
	UPROPERTY(EditAnywhere, Category = "Spline|MoveAnimation")
	float MoveAmplitude = 100.f;


	//other
	ESplineCoordinateSpace::Type CoorSpace = ESplineCoordinateSpace::World;
private:
	//animation
	UPROPERTY(EditAnywhere, Category = "Spline")
		bool bPlayingAnim = false;
	UPROPERTY()
		bool bIdle = false;
	UPROPERTY(EditAnywhere, Category = "Spline")
		bool bHasInitSpline = false;
	UPROPERTY(meta = (AllowPrivateAccess = "true", ToolTip = "1 means at head, 0 is a back", ClampMin = 0.f, ClampMax = 1.f), EditAnywhere, Category = "Spline")
		float HeadPlacement = 0.5f;
	

	// not visible variables
	UPROPERTY(/*meta = ( ToolTip = "Is the distance along spline the end of the worm is"), EditAnywhere, Category = "Spline"*/)
		float CurrentWormDistance = 0.f;
	UPROPERTY()
		float CurrentMoveTime = 0.f;



	//grapple events
	UFUNCTION()
		void OnGrappleReaced(float Addspeed);
	UFUNCTION()
		void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
};


