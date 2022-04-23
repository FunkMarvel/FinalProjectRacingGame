// Fill out your copyright notice in the Description page of Project Settings.


#include "SplineWormEnemy.h"

#include "../GrappleSphereComponent.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ASplineWormEnemy::ASplineWormEnemy()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	Spline = CreateDefaultSubobject<USplineComponent>(TEXT("Spline"));
	SetRootComponent(Spline);

	SplineMeshComponents.Init(nullptr, 0);

	WormTargetMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WormTargetMesh"));
	WormTargetMesh->SetupAttachment(GetRootComponent());
	WormTargetMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	GrappleSphereComponent = CreateDefaultSubobject<UGrappleSphereComponent>(TEXT("GrappleSphereComponent"));
	GrappleSphereComponent->SetupAttachment(WormTargetMesh);
	
	
}

// Called when the game starts or when spawned
void ASplineWormEnemy::BeginPlay()
{
	Super::BeginPlay();
	GrappleSphereComponent->OnReachedEvent.AddDynamic(this, &ASplineWormEnemy::OnGrappleReaced);
}

// Called every frame
void ASplineWormEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateSplineMeshComponent();

	if (!bPlayingAnim)
		return;


	//Offset += +DeltaTime * WormMoveSpeed;
	CurrentMoveTime += DeltaTime;
	Offset = MovmentCurveFloat->GetFloatValue(CurrentMoveTime / WormMoveDuration);
	Offset *= Spline->GetSplineLength();
		
    // checking if we have reached the end
    if ( Spline->GetSplineLength() < SplineMeshComponents.Num() * NeckSegmentLength + SplineMeshOverLap + Offset)
    {
	    bPlayingAnim = false;
    }
	
}

void ASplineWormEnemy::UpdateSplineMeshComponent()
{
	
	int SegmentsToCreate = (WormLength / NeckSegmentLength) - SplineMeshComponents.Num();

	if (SegmentsToCreate > 0)
	{
		for (int32 i = 0; i < SegmentsToCreate; i++)
		{
			USplineMeshComponent* NewSplineMesh = NewObject<USplineMeshComponent>(this);
			if (NewSplineMesh)
			{
				NewSplineMesh->RegisterComponent();
				NewSplineMesh->SetMobility(EComponentMobility::Movable);
				NewSplineMesh->SetStaticMesh(NeckSegment);

				NewSplineMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
				SplineMeshComponents.Emplace(NewSplineMesh);
			}
		}
		
	}
	else if (SegmentsToCreate < 0)
	{
		for (int32 i = 0; i < abs(SegmentsToCreate); i++)
		{
			int32 LastIndex = SplineMeshComponents.Num() - 1;
			SplineMeshComponents[LastIndex]->DestroyComponent();
			SplineMeshComponents.RemoveAt(LastIndex);
		}
	}

	float CurrentDistance = Offset;
	for (int i = 0; i < SplineMeshComponents.Num(); ++i)
	{
		FVector StartLocation, EndLocation, StartTangent, EndTangent;

		StartLocation = Spline->GetLocationAtDistanceAlongSpline(CurrentDistance, ESplineCoordinateSpace::World);
		StartTangent = Spline->GetTangentAtDistanceAlongSpline(CurrentDistance, ESplineCoordinateSpace::World);
		EndLocation = Spline->GetLocationAtDistanceAlongSpline(CurrentDistance + NeckSegmentLength + SplineMeshOverLap, 
		ESplineCoordinateSpace::World);
		EndTangent = Spline->GetTangentAtDistanceAlongSpline(CurrentDistance + NeckSegmentLength + SplineMeshOverLap, 
		ESplineCoordinateSpace::World);

		//normalizing
		StartTangent.Normalize();
		EndTangent.Normalize();
	
		
		//setting the values
		SplineMeshComponents[i]->SetStartPosition(StartLocation, false);
		SplineMeshComponents[i]->SetStartTangent(StartTangent, false);
		SplineMeshComponents[i]->SetEndPosition(EndLocation, false);
		SplineMeshComponents[i]->SetEndTangent(EndTangent, true);
		
		CurrentDistance += NeckSegmentLength;
	}
	
}

void ASplineWormEnemy::OnGrappleReaced(float Addspeed)
{
	UE_LOG(LogTemp, Warning, TEXT("Destroyed Worm"))
	Destroy();
}


