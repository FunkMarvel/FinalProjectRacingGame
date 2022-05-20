// Fill out your copyright notice in the Description page of Project Settings.


#include "NeckComponent.h"

#include "DrawDebugHelpers.h"
#include "Components/SphereComponent.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"

// Sets default values for this component's properties
UNeckComponent::UNeckComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UNeckComponent::BeginPlay()
{
	Super::BeginPlay();

	CarPawn = Cast<ACarPawn>(GetOwner());
	Spline = CarPawn->NeckSpline;

	SplineMeshComponents.Init(nullptr, 0);
	
}


// Called every frame
void UNeckComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	
}

void UNeckComponent::UpdateSplinePoints()
{
	//calc locations and tangents
	FVector StartLocation, StartTangent, EndLocation, EndTangent = FVector::ZeroVector;
	
	StartLocation = CarPawn->SharkBodyMesh->GetComponentLocation();
	EndLocation = CarPawn->SharkHeadMesh->GetComponentLocation();

	float Distance = (StartLocation - EndLocation).Size();
	
	StartTangent = CarPawn->SharkBodyMesh->GetForwardVector() * Distance;

	//new method for end tangent
	EndTangent = FVector::CrossProduct( CarPawn->GrappleHookSphereComponent->GetPhysicsLinearVelocity(),
		CarPawn->SphereComp->GetForwardVector());
	EndTangent = EndTangent.GetSafeNormal();
	EndTangent *= Distance;
	
	//Sets the values to the spline

	//sets values
	Spline->SetLocationAtSplinePoint(0, StartLocation, ESplineCoordinateSpace::World, false);
	Spline->SetTangentAtSplinePoint(0, StartTangent, ESplineCoordinateSpace::World, false);
	Spline->SetLocationAtSplinePoint(1, EndLocation, ESplineCoordinateSpace::World, false);
	Spline->SetTangentAtSplinePoint(1, EndTangent, ESplineCoordinateSpace::World, true);
}



void UNeckComponent::UpdateSplinePointsLocations(FVector Start, FVector End, bool bLerp)
{
	if (bLerp)
	{
		FVector PrevStart = Spline->GetTangentAtSplinePoint(0, ESplineCoordinateSpace::World);
		FVector PrevEnd = Spline->GetTangentAtSplinePoint(1, ESplineCoordinateSpace::World);

		Start = FMath::VInterpTo(PrevStart, Start, GetWorld()->GetDeltaSeconds(), 15.f);
		End = FMath::VInterpTo(PrevEnd, End, GetWorld()->GetDeltaSeconds(), 15.f);
	}
	Spline->SetLocationAtSplinePoint(0, Start, ESplineCoordinateSpace::World, false);
	Spline->SetLocationAtSplinePoint(1, End, ESplineCoordinateSpace::World, true);
}

void UNeckComponent::UpdateSplineStartPosition(FVector Start)
{
	Spline->SetLocationAtSplinePoint(0, Start, ESplineCoordinateSpace::World);
}

void UNeckComponent::UpdateSplineEndPosition(FVector End)
{
	Spline->SetLocationAtSplinePoint(1, End, ESplineCoordinateSpace::World);
}

void UNeckComponent::UpdateSplinePointsTangents(FVector StartTangent, FVector EndTangent, bool bLerp)
{
	if (bLerp)
	{
		FVector PrevStartTangent = Spline->GetTangentAtSplinePoint(0, ESplineCoordinateSpace::World);
		FVector PrevEndTangent = Spline->GetTangentAtSplinePoint(1, ESplineCoordinateSpace::World);

		StartTangent = FMath::VInterpTo(PrevStartTangent, StartTangent, GetWorld()->GetDeltaSeconds(), 15.f);
		EndTangent = FMath::VInterpTo(PrevEndTangent, EndTangent, GetWorld()->GetDeltaSeconds(), 15.f);
	}
	Spline->SetTangentAtSplinePoint(0, StartTangent, ESplineCoordinateSpace::World, false);
	Spline->SetTangentAtSplinePoint(1, EndTangent, ESplineCoordinateSpace::World, true);
	
}

void UNeckComponent::UpdateEndTangent(FVector EndTangent, bool bLerp)
{
	if (bLerp) {
		FVector PrevEndTangent = Spline->GetTangentAtSplinePoint(1, ESplineCoordinateSpace::World);

		EndTangent = FMath::VInterpTo(PrevEndTangent, EndTangent, GetWorld()->GetDeltaSeconds(), 15.f);
	}
	Spline->SetTangentAtSplinePoint(1, EndTangent, ESplineCoordinateSpace::World);
}

void UNeckComponent::UpdateStartTangent(FVector StartTangent, bool bLerp)
{
	if (bLerp) {
		FVector PrevStartTangent = Spline->GetTangentAtSplinePoint(0, ESplineCoordinateSpace::World);

		StartTangent = FMath::VInterpTo(PrevStartTangent, StartTangent, GetWorld()->GetDeltaSeconds(), 15.f);
	}
	Spline->SetTangentAtSplinePoint(0, StartTangent, ESplineCoordinateSpace::World);
}

void UNeckComponent::UpdateSplineMesh()
{
	UpdateSplineMesh(0, Spline->GetSplineLength());
	
}

void UNeckComponent::UpdateSplineMesh(float StartLength, float EndLength)
{
	//clamps input
	if (StartLength < 0.f || StartLength > Spline->GetSplineLength()) StartLength = 0.f;
	if (EndLength > Spline->GetSplineLength() || EndLength < 0.f) EndLength = Spline->GetSplineLength();
	
	//adds or removes segments to the array
	//int32 segments = CalculateNumberOfSegments();
	float Length = EndLength - StartLength;
	float segmentsf = Length / TargetSegmentsLength;
	int32 segments = (int)truncf(segmentsf) + 1;

	// if length is really short, have zero spline mesh segments
	if (Length < 10.f)
		segments = 0;	
	
	int32 SegmentsToCreate = segments - SplineMeshComponents.Num();
	
	if (SegmentsToCreate > 0)
	{
		for (int32 i = 0; i < SegmentsToCreate; i++)
		{
			//creates new USplineMesh
			USplineMeshComponent* NewSplineMesh = NewObject<USplineMeshComponent>(this);
			if (NewSplineMesh)
			{
				//essensials
				NewSplineMesh->RegisterComponent();
				NewSplineMesh->SetMobility(EComponentMobility::Movable);
				NewSplineMesh->SetStaticMesh(StaticMeshClass);

				//ads to array
				SplineMeshComponents.Emplace(NewSplineMesh);
			}
		}
		
	}
	else if (SegmentsToCreate < 0)
	{
		for (int32 i = 0; i < abs(SegmentsToCreate); i++)
		{
			//removes from array
			int32 LastIndex = SplineMeshComponents.Num() - 1;
			SplineMeshComponents[LastIndex]->DestroyComponent();
			SplineMeshComponents.RemoveAt(LastIndex);
		}
	}

	//sets the positions and tangets
	//float truncSplineLength = TargetSegmentsLength * SplineMeshComponents.Num();
	float lastLength = EndLength;
	for (int32 i = 0; i < SplineMeshComponents.Num(); i++)
	{
		float currentLength = TargetSegmentsLength * i;

		currentLength = Length - currentLength;
		currentLength += StartLength;

		SplineMeshComponents[i]->SetStartPosition(Spline->GetLocationAtDistanceAlongSpline(lastLength, ESplineCoordinateSpace::World), false);
		SplineMeshComponents[i]->SetStartTangent(Spline->GetDirectionAtDistanceAlongSpline(lastLength, ESplineCoordinateSpace::World) * SplineMeshTangentLength,  false);
		
		SplineMeshComponents[i]->SetEndPosition(Spline->GetLocationAtDistanceAlongSpline(currentLength, ESplineCoordinateSpace::World), false);
		SplineMeshComponents[i]->SetEndTangent(Spline->GetDirectionAtDistanceAlongSpline(currentLength, ESplineCoordinateSpace::World)* SplineMeshTangentLength, true);
		
		lastLength = currentLength;
	}
}

int32 UNeckComponent::CalculateNumberOfSegments()
{
	float Length = Spline->GetSplineLength();

	float segmentsf = Length/TargetSegmentsLength;
	int32 segments = (int)truncf(segmentsf);

	return segments;
}



