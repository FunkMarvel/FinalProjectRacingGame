// Fill out your copyright notice in the Description page of Project Settings.


#include "GravitySplineActor.h"

#include <string>

#include "DebugLog.h"
#include "DrawDebugHelpers.h"
#include "Components/SplineComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h" 
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AGravitySplineActor::AGravitySplineActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	ArrowComp = CreateDefaultSubobject<UArrowComponent>(TEXT("ARROW COMP"));
	SetRootComponent(ArrowComp);

	SplineComp = CreateDefaultSubobject<USplineComponent>(TEXT("Gravity Spline Component"));
	SplineComp->SetupAttachment(ArrowComp);

	//setting up UBoxComponents
	BoxSplineOne = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Spline One"));
	BoxSplineOne->SetupAttachment(GetRootComponent());
	BoxSplineOne->SetBoxExtent(FVector(200.f));

	BoxSplineTwo = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Spline Two"));
	BoxSplineTwo->SetupAttachment(GetRootComponent());
	BoxSplineTwo->SetBoxExtent(FVector(200.f));
}

// Called when the game starts or when spawned
void AGravitySplineActor::BeginPlay()
{
	Super::BeginPlay();
	SplineComp->bDrawDebug = true;
	
}

// Called every frame
void AGravitySplineActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//VisualiseUpVectors(200, SplineComp);

	

}

int32 AGravitySplineActor::GetClosestSplinePoint(USplineComponent* splineEval, FVector Location)
{
	
	float MaxDist = 9999999999.f;
	int index = -1;
	for (int32 i = 0; i < splineEval->GetNumberOfSplinePoints(); i++)
	{
		float Dist = (splineEval->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::World) - Location).SizeSquared();
		if (Dist < MaxDist)
		{
			MaxDist = Dist;
			index = i;
		}
	}
	
	return index;

	FVector loc = splineEval->GetLocationAtSplinePoint(index, ESplineCoordinateSpace::World);

	DrawDebugSphere(GetWorld(), loc , 70.f, 32,
		FColor::Emerald, false, 1.f);

	

	//FVector up = (splineEval->GetRotationAtSplinePoint(index, ESplineCoordinateSpace::World).Add(90.f, 0.f, 0.f)).Vector();
	FVector up = splineEval->GetUpVectorAtSplinePoint(index, ESplineCoordinateSpace::World);

	DrawDebugLine(GetWorld(), loc, loc + up * 400.f, FColor::Red, false);


	return index;
}

void AGravitySplineActor::VisualiseUpVectors(int Segments, USplineComponent* SplineToUse)
{
	float Length = SplineComp->GetSplineLength();
	for (int32 i = 0; i < 200; i++)
	{
		float dist = Length * (i / (float)Segments);


		FVector Loc = SplineComp->GetLocationAtDistanceAlongSpline(dist, ESplineCoordinateSpace::World);
		 
		FVector UpVec = GetFixedUpVector(Loc);
		
		DrawDebugLine(GetWorld(), Loc, Loc + UpVec * 200.f, FColor::Blue, false, 0.4f, (uint8)0U, 10.f);
		
		
		/*FVector Up = FVector(0.f);
		switch (EUpVectorAxis)
		{
		case EGravitySplineAxis::Axis_Y:
			Up = SplineComp->GetRightVectorAtDistanceAlongSpline(dist, ESplineCoordinateSpace::World);
			break;
		case EGravitySplineAxis::Axis_Z:
			Up = SplineComp->GetUpVectorAtDistanceAlongSpline(dist, ESplineCoordinateSpace::World);
			break;
			
		default:
			break;
		}

		FVector Pos = SplineComp->GetLocationAtDistanceAlongSpline(dist, ESplineCoordinateSpace::World);
		//UE_LOG(LogTemp, Warning, TEXT("did create debug %f"), dist)
		DrawDebugLine(GetWorld(), Pos, Pos + Up * 200.f, FColor::Emerald, false, 0.4f, (uint8)0U, 40.f);


		//alsodraws a blue on in the forward direction
		float SplineLength = SplineComp->GetSplineLength();
		FVector ForwardPos = SplineComp->GetLocationAtDistanceAlongSpline(SplineLength, ESplineCoordinateSpace::World);
		FVector ForwardForwardVec = SplineComp->GetDirectionAtDistanceAlongSpline(SplineLength, ESplineCoordinateSpace::World);

		DrawDebugLine(GetWorld(), ForwardPos, ForwardPos + ForwardForwardVec * 200.f, FColor::Blue, false, 0.4f, (uint8)0U, 40.f);
	*/
	}
}

FVector AGravitySplineActor::GetAdjustedUpVectorFromLocation(FVector Loc)
{
	FVector ReturnUpVector;
	switch (EUpVectorAxis)
	{
	case EGravitySplineAxis::Axis_Y:
		ReturnUpVector = SplineComp->FindRightVectorClosestToWorldLocation(Loc, ESplineCoordinateSpace::World);
		break;
	case EGravitySplineAxis::Axis_Z:
		ReturnUpVector = SplineComp->FindUpVectorClosestToWorldLocation(Loc, ESplineCoordinateSpace::World);
		break;
	default:
		break;
	}
	

	return ReturnUpVector;
}

FVector AGravitySplineActor::GetFixedUpVector(FVector OrgPos)
{
	int closest = GetClosestSplinePoint(SplineComp, OrgPos);
	float PointDistance = SplineComp->GetDistanceAlongSplineAtSplinePoint(closest);

	float OrgPosInputKey = SplineComp->FindInputKeyClosestToWorldLocation(OrgPos);
	float OrgPosDistance = SplineComp->GetDistanceAlongSplineAtSplineInputKey(OrgPosInputKey);

	int SecClosest = 0;
	if (OrgPosDistance > PointDistance) // OrgPoint is further along spline
	{
		SecClosest = closest + 1;
	}
	else
	{
		SecClosest = closest - 1;
		if (SecClosest == -1)
			SecClosest = 0;
	}

	float SecPointDistance = SplineComp->GetDistanceAlongSplineAtSplinePoint(SecClosest);

	float SegmentDistance = PointDistance - SecPointDistance;
	float p1 = abs(PointDistance - OrgPosDistance);
	//float p2 = abs(SecPointDistance - OrgPosDistance);

	float lerp = p1/SegmentDistance;
	lerp = abs(lerp);
	DL_NORMAL(FString::SanitizeFloat(lerp))

	FVector newUpVector = UKismetMathLibrary::VLerp(SplineComp->GetUpVectorAtSplinePoint(closest, ESplineCoordinateSpace::World),
		SplineComp->GetUpVectorAtSplinePoint(SecClosest, ESplineCoordinateSpace::World),
		lerp);


	//debugging

	/*DrawDebugSphere(GetWorld(), SplineComp->GetLocationAtSplinePoint(closest, ESplineCoordinateSpace::World), 50.f, 32, FColor::Blue,
		false);
	DrawDebugSphere(GetWorld(), SplineComp->GetLocationAtSplinePoint(SecClosest, ESplineCoordinateSpace::World), 50.f, 32, FColor::Blue,
		false);

	
	FVector locc = SplineComp->GetLocationAtDistanceAlongSpline(OrgPosDistance, ESplineCoordinateSpace::World);
	DrawDebugLine(GetWorld(), locc, locc + newUpVector * 2000.f, FColor::Red, false, 1.4f,
		(uint8)0U, 20.f);*/
	
	return newUpVector.GetSafeNormal();
}

