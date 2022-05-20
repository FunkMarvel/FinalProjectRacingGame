// Fill out your copyright notice in the Description page of Project Settings.


#include "ArcGravitySplineActor.h"

#include "CarPawn.h"
#include "DebugLog.h"
#include "DrawDebugHelpers.h"
#include "Components/SplineComponent.h"
#include "Kismet/KismetMathLibrary.h"

AArcGravitySplineActor::AArcGravitySplineActor() {
	PrimaryActorTick.bCanEverTick = true;
	SplineCompTwo = CreateDefaultSubobject<USplineComponent>(TEXT("SplineCompTwo"));
	SplineCompTwo->SetupAttachment(GetRootComponent());
}

void AArcGravitySplineActor::BeginPlay() {
	Super::BeginPlay();
}

void AArcGravitySplineActor::Tick(float DeltaSeconds) {
	Super::Tick(DeltaSeconds);
}

FVector AArcGravitySplineActor::GetFixedUpVectorFromLocation(FVector OrgPos) {

	FVector FirstUpVector = GetFixedUpVectorFromLocationAndSpline(OrgPos, SplineComp);
	FVector FirstSplineLocation = SplineComp->FindLocationClosestToWorldLocation(OrgPos, ESplineCoordinateSpace::World);
	float FirstDistance = (OrgPos - FirstSplineLocation).Size();
	
	FVector SecondUpVector = GetFixedUpVectorFromLocationAndSpline(OrgPos, SplineCompTwo);
	FVector SecondSplineLocation = SplineCompTwo->FindLocationClosestToWorldLocation(OrgPos, ESplineCoordinateSpace::World);
	float SecondDistance = (OrgPos - SecondSplineLocation).Size();

	float _MaxDistance = (FirstSplineLocation - SecondSplineLocation).Size();
	
	float FirstRatio = FirstDistance/_MaxDistance;
	float SecondRatio = SecondDistance/_MaxDistance;

	// DrawDebugSphere(GetWorld(), FirstSplineLocation, 150.f, 32, FColor::Red);
	// DrawDebugSphere(GetWorld(), SecondSplineLocation, 150.f, 32, FColor::Red);
	//
	// DrawDebugLine(GetWorld(), FirstSplineLocation , FirstSplineLocation + FirstUpVector * 300.f, FColor::Red, false, -1, 0, 50.f);
	// DrawDebugLine(GetWorld(), SecondSplineLocation , SecondSplineLocation + SecondUpVector * 300.f, FColor::Red, false, -1, 0, 50.f);
	
	//early bail if we are farther away than max distance
	if (FirstRatio > 1.005f || SecondRatio > 1.005f) {
		DL_NORMAL("Fruther away that man distance")
		if (FirstRatio > SecondRatio) { // first point is longer away than second. This means second point is closer
			return SecondUpVector;
		}
		else {
			return FirstUpVector;
		}
	}

	// thingking from first to second
	FVector ToSecondFromFirst = SecondSplineLocation - FirstSplineLocation;
	FVector ToPlayerFromFirst = OrgPos - FirstSplineLocation;
	// DrawDebugLine(GetWorld(), FirstSplineLocation , FirstSplineLocation + ToSecondFromFirst, FColor::Red, false, -1, 0, 50.f);
	// DrawDebugLine(GetWorld(), FirstSplineLocation , FirstSplineLocation + ToPlayerFromFirst, FColor::Red, false, -1, 0, 50.f);

	float Angle = ACarPawn::UnsignedAngle(ToSecondFromFirst.GetSafeNormal(), ToPlayerFromFirst.GetSafeNormal());
	DL_NORMAL(FString::SanitizeFloat(Angle))
	
	float Distance = FMath::Cos(FMath::DegreesToRadians(Angle)) * ToPlayerFromFirst.Size();
	float Lerp = Distance / _MaxDistance;

	return UKismetMathLibrary::VLerp(FirstUpVector, SecondUpVector, Lerp);
	
}

FVector AArcGravitySplineActor::GetFixedUpVectorFromLocationAndSpline(FVector OrgPos, USplineComponent* SplineComponent){
	
	const int closest = GetClosestSplinePoint(SplineComponent, OrgPos);
	const float PointDistance = SplineComponent->GetDistanceAlongSplineAtSplinePoint(closest);

	const float OrgPosInputKey = SplineComponent->FindInputKeyClosestToWorldLocation(OrgPos);
	const float OrgPosDistance = SplineComponent->GetDistanceAlongSplineAtSplineInputKey(OrgPosInputKey);

	int SecClosest = 0;
	if (OrgPosDistance > PointDistance) // OrgPoint is further along spline
		{
		SecClosest = closest + 1;
		}
	else
	{
		SecClosest = closest - 1;
		if (SecClosest == -1)
			return SplineComponent->GetUpVectorAtSplinePoint(0, ESplineCoordinateSpace::World);
	}

	const float SecPointDistance = SplineComponent->GetDistanceAlongSplineAtSplinePoint(SecClosest);

	const float SegmentDistance = PointDistance - SecPointDistance;
	const float p1 = abs(PointDistance - OrgPosDistance);

	float lerp = p1/SegmentDistance;
	lerp = abs(lerp);

	FVector newUpVector = UKismetMathLibrary::VLerp(SplineComponent->GetUpVectorAtSplinePoint(closest, ESplineCoordinateSpace::World),
		SplineComponent->GetUpVectorAtSplinePoint(SecClosest, ESplineCoordinateSpace::World),
		lerp);
	
	return newUpVector.GetSafeNormal();
}
