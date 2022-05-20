// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GravitySplineActor.h"
#include "TunnelGravitySplineActor.generated.h"

/**
 * 
 */
UCLASS()
class RACINGUNREALPROJECT_API ATunnelGravitySplineActor : public AGravitySplineActor
{
	GENERATED_BODY()

	//my deseg
public:

	virtual FVector GetFixedUpVectorFromLocation(FVector OrgPos) override;

	virtual void Tick(float DeltaSeconds) override;
	
	
};
