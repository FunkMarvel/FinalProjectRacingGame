// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
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
	class UStaticMesh* NeckSegment = nullptr;
	UPROPERTY(EditAnywhere, Category = "Spline")
	class UCurveFloat* MovmentCurveFloat;
	UPROPERTY()
	TArray<class USplineMeshComponent*> SplineMeshComponents;

	UFUNCTION(BlueprintCallable)
	void UpdateSplineMeshComponent();
	
	UPROPERTY(EditAnywhere, Category = "Spline")
	float NeckSegmentLength = 500.f;
	UPROPERTY(EditAnywhere, Category = "Spline")
	float WormLength = 2000.f;
	UPROPERTY(EditAnywhere, Category = "Spline")
	float SplineMeshOverLap = 50.f;
	UPROPERTY(EditAnywhere, Category = "Spline")
	float WormMoveDuration = 2000.f;
	UPROPERTY(EditAnywhere, Category = "Spline")
	float Offset = 0.f;
	
	UPROPERTY(EditAnywhere, Category = "Spline")
	bool bPlayingAnim = true;

	UPROPERTY()
	float CurrentMoveTime = 0.f;
private:
	UFUNCTION()
	void OnGrappleReaced(float Addspeed);
};
