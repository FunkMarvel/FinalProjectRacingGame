// Fill out your copyright notice in the Description page of Project Settings.


#include "Crab.h"

#include "Components/SplineComponent.h"

// Sets default values
ACrab::ACrab()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	Spline = CreateDefaultSubobject<USplineComponent>(TEXT("Spline"));
	SetRootComponent(Spline);
	
	CrabMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CrabMesh"));
	CrabMesh->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void ACrab::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACrab::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	float Length = Spline->GetSplineLength();

	CurrentDistance += MoveSpeed * GetWorld()->GetDeltaSeconds();

	FVector Location;
	FRotator Rotation;
	ESplineCoordinateSpace::Type CoorSpace = ESplineCoordinateSpace::World;
	Location = Spline->GetLocationAtDistanceAlongSpline(CurrentDistance, CoorSpace);
	Rotation = Spline->GetRotationAtDistanceAlongSpline(CurrentDistance, CoorSpace);

	CrabMesh->SetWorldLocation(Location);
	CrabMesh->SetWorldRotation(Rotation);


	if (CurrentDistance > Spline->GetSplineLength())
		CurrentDistance = 0.f;
}

