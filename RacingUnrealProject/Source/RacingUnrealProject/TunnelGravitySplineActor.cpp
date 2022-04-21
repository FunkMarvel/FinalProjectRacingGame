// Fill out your copyright notice in the Description page of Project Settings.


#include "TunnelGravitySplineActor.h"

#include "Components/SplineComponent.h"

FVector ATunnelGravitySplineActor::GetFixedUpVectorFromLocation(FVector OrgPos)
{
	
	const FVector NormalUpVector = Super::GetFixedUpVectorFromLocation(OrgPos); // incase we need it for later!
	
	
	FVector ClosestSplineLocation = SplineComp->FindLocationClosestToWorldLocation(OrgPos, ESplineCoordinateSpace::World);
	FVector NewUpVector = (ClosestSplineLocation - OrgPos).GetSafeNormal();

	return NewUpVector;
}

void ATunnelGravitySplineActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}
