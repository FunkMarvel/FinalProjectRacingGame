// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GravitySplineActor.h"
#include "ArcGravitySplineActor.generated.h"

/**
 * 
 */
UCLASS()
class RACINGUNREALPROJECT_API AArcGravitySplineActor : public AGravitySplineActor
{
	GENERATED_BODY()
public:
	AArcGravitySplineActor();
protected:
	virtual void BeginPlay() override;
public:
	virtual void Tick(float DeltaSeconds) override;

	//my deseg
public:
	UPROPERTY(EditAnywhere)
		class USplineComponent* SplineCompTwo = nullptr;


	virtual FVector GetFixedUpVectorFromLocation(FVector OrgPos) override;

	UFUNCTION()
		FVector GetFixedUpVectorFromLocationAndSpline(FVector OrgPos, USplineComponent* SplineComponent);
	
};
