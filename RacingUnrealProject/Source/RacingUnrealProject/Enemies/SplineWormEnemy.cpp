// Fill out your copyright notice in the Description page of Project Settings.


#include "SplineWormEnemy.h"

#include "../GrappleSphereComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "Evaluation/IMovieSceneEvaluationHook.h"
#include "Kismet/KismetMathLibrary.h"
#include "RacingUnrealProject/DebugLog.h"

// Sets default values
ASplineWormEnemy::ASplineWormEnemy()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	Spline = CreateDefaultSubobject<USplineComponent>(TEXT("Spline"));
	SetRootComponent(Spline);

	SplineMeshComponents.Init(nullptr, 0);
	
	GrappleSphereComponent = CreateDefaultSubobject<UGrappleSphereComponent>(TEXT("GrappleSphereComponent"));
	GrappleSphereComponent->SetupAttachment(GetRootComponent());

	WormTargetMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WormTargetMesh"));
	WormTargetMesh->SetupAttachment(GrappleSphereComponent);
	WormTargetMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);


	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	TriggerBox->SetupAttachment(GetRootComponent());
	TriggerBox->SetBoxExtent(FVector(600.f, 150.f, 600.f));
	TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	TriggerBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	
	
}

// Called when the game starts or when spawned
void ASplineWormEnemy::BeginPlay()
{
	Super::BeginPlay();
	GrappleSphereComponent->OnReachedEvent.AddDynamic(this, &ASplineWormEnemy::OnGrappleReaced);
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ASplineWormEnemy::OnOverlap);
	
	
}

// Called every frame
void ASplineWormEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bPlayingAnim)
		return;

	if (!bHasInitSpline) {
		bHasInitSpline = true;
		InitSplineSegments();
	}
	
	UpdateSplineMeshComponent();
	UpdateHeadTransfrom(0.5f);

	//Offset += +DeltaTime * WormMoveSpeed;
	CurrentMoveTime += DeltaTime;
	CurrentWormDistance = MovmentCurveFloat->GetFloatValue(CurrentMoveTime / WormMoveDuration);
	CurrentWormDistance *= Spline->GetSplineLength();
	CurrentWormDistance -= GetWormRealLength(); // converts to the movment is based on the end of the worm
		
    // checking if we have reached the end
    if ( Spline->GetSplineLength() < SplineMeshComponents.Num() *
    	NeckSegmentLength + SplineMeshOverLap + CurrentWormDistance)
    {
	    bPlayingAnim = false;
    	GrappleSphereComponent->SetIsEnabled(true);
    }
	
}

void ASplineWormEnemy::UpdateSplineMeshComponent()
{

	float CurrentDistance = CurrentWormDistance;
	for (int32 i = 0; i < SplineMeshComponents.Num(); ++i)
	{
		// FVector StartLocation, EndLocation, StartTangent, EndTangent;

		/*StartLocation = Spline->GetLocationAtDistanceAlongSpline(CurrentDistance, ESplineCoordinateSpace::World);
		StartTangent = Spline->GetDirectionAtDistanceAlongSpline(CurrentDistance, ESplineCoordinateSpace::World);
		EndLocation = Spline->GetLocationAtDistanceAlongSpline(CurrentDistance + NeckSegmentLength + SplineMeshOverLap, 
		ESplineCoordinateSpace::World);
		EndTangent = Spline->GetDirectionAtDistanceAlongSpline(CurrentDistance + NeckSegmentLength + SplineMeshOverLap, 
		ESplineCoordinateSpace::World);*/

		//normalizing
		/*StartTangent.Normalize();
		EndTangent.Normalize();*/
		
		//setting the values
		SplineMeshComponents[i]->SetStartPosition(Spline->GetLocationAtDistanceAlongSpline(CurrentDistance, ESplineCoordinateSpace::World), false);
		SplineMeshComponents[i]->SetStartTangent(Spline->GetDirectionAtDistanceAlongSpline(CurrentDistance, ESplineCoordinateSpace::World), false);

		const float OffsetDistance = CurrentDistance+ NeckSegmentLength + SplineMeshOverLap;
		
		SplineMeshComponents[i]->SetEndPosition(Spline->GetLocationAtDistanceAlongSpline(OffsetDistance, 
		ESplineCoordinateSpace::World), false);
		SplineMeshComponents[i]->SetEndTangent(Spline->GetDirectionAtDistanceAlongSpline(OffsetDistance, 
		ESplineCoordinateSpace::World), true);
		
		CurrentDistance += NeckSegmentLength;
	}
	
}

void ASplineWormEnemy::UpdateHeadTransfrom(float RatioOnSnake)
{	
	ESplineCoordinateSpace::Type CoorSpace = ESplineCoordinateSpace::World; 

	//location
	float Distance = CurrentWormDistance + GetWormRealLength() * RatioOnSnake;
	FVector Location = Spline->GetLocationAtDistanceAlongSpline(Distance, CoorSpace);
	

	//rotation
	FVector Up;
	FVector Forward;

	switch (CurrentHeadAxis) {
	case ESplineWormHeadAxis::Right:
		Up = Spline->GetUpVectorAtDistanceAlongSpline(Distance, CoorSpace);
		Forward = -Spline->GetRightVectorAtDistanceAlongSpline(Distance, CoorSpace);
		break;
	case ESplineWormHeadAxis::Left:
		Up = Spline->GetUpVectorAtDistanceAlongSpline(Distance, CoorSpace);
		Forward = Spline->GetRightVectorAtDistanceAlongSpline(Distance, CoorSpace);
		break;
	case ESplineWormHeadAxis::Up:
		Up = Spline->GetRightVectorAtDistanceAlongSpline(Distance, CoorSpace);
		Forward = -Spline->GetUpVectorAtDistanceAlongSpline(Distance, CoorSpace);
		break;
	case ESplineWormHeadAxis::Down:
		Up = -Spline->GetRightVectorAtDistanceAlongSpline(Distance, CoorSpace);
		Forward = Spline->GetUpVectorAtDistanceAlongSpline(Distance, CoorSpace);
		break;
		default:
		Up = Spline->GetUpVectorAtDistanceAlongSpline(Distance, CoorSpace);
		Forward = -Spline->GetRightVectorAtDistanceAlongSpline(Distance, CoorSpace);
			break;
	}
	
	Location += Forward * HeadDistanceFromBody;
	GrappleSphereComponent->SetWorldLocation(Location);

	if (bInvertUpHeadAxis)
		Up = -Up;
	
	FRotator Rotation = UKismetMathLibrary::MakeRotFromXZ(-Forward, Up);
	GrappleSphereComponent->SetWorldRotation(Rotation);
}

void ASplineWormEnemy::InitSplineSegments() {
	//init spline segmensts
	int SegmentsToCreate = (WormGoalLength / NeckSegmentLength) - SplineMeshComponents.Num();

	DL_NORMAL("segments to create : " + FString::FromInt(SegmentsToCreate))
	if (SegmentsToCreate > 0) {
		for (int32 i = 0; i < SegmentsToCreate; i++)
		{
			USplineMeshComponent* NewSplineMesh = NewObject<USplineMeshComponent>(this);
			if (NewSplineMesh)
			{
				NewSplineMesh->RegisterComponent();
				NewSplineMesh->SetMobility(EComponentMobility::Movable);
				NewSplineMesh->SetStaticMesh(NeckSegment);

				NewSplineMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
				SplineMeshComponents.Emplace(NewSplineMesh);
			}
		}
		
	}
	else if (SegmentsToCreate < 0) {
		for (int32 i = 0; i < abs(SegmentsToCreate); i++) {
			int32 LastIndex = SplineMeshComponents.Num() - 1;
			SplineMeshComponents[LastIndex]->DestroyComponent();
			SplineMeshComponents.RemoveAt(LastIndex);
		}
	}
}

float ASplineWormEnemy::GetWormRealLength() const
{
	float Tip = 0.f;
	Tip += SplineMeshComponents.Num() * NeckSegmentLength + SplineMeshOverLap;
	return Tip;
}

void ASplineWormEnemy::OnGrappleReaced(float Addspeed)
{
	UE_LOG(LogTemp, Warning, TEXT("Destroyed Worm"))
	Destroy();
}

void ASplineWormEnemy::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	bPlayingAnim = true;
}


