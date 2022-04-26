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

	WormHeadMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WormHeadMesh"));
	WormHeadMesh->SetupAttachment(GetRootComponent());
	WormHeadMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

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
	if (bIdle) {
		CurrentMoveTime += DeltaTime;
		HandleIdleAnimation();
	}

	
	if (!bPlayingAnim)
		return;

	if (!bHasInitSpline) {
		bHasInitSpline = true;
		InitSplineSegments();
	}
	
	UpdateSplineMeshComponent();
	UpdateHeadTransfrom();
	UpdateTargetTransfrom(HeadPlacement);

	//sets the current move time
	CurrentMoveTime += DeltaTime;
	CurrentWormDistance = MovmentCurveFloat->GetFloatValue(CurrentMoveTime / WormMoveDuration);
	CurrentWormDistance *= Spline->GetSplineLength();
	CurrentWormDistance -= GetWormRealLength(); // converts to the movment is based on the end of the worm
		
    // checking if we have reached the end
    if ( Spline->GetSplineLength() < SplineMeshComponents.Num() *
    	NeckSegmentLength + SplineMeshOverLap + CurrentWormDistance)
    {
	    bPlayingAnim = false;
    	bIdle = true;
    	GrappleSphereComponent->SetIsEnabled(true);
    }
	
}

void ASplineWormEnemy::UpdateSplineMeshComponent()
{
	
	float CurrentDistance = CurrentWormDistance;

	FVector Offset = FVector::ZeroVector;
	
	for (int32 i = 0; i < SplineMeshComponents.Num(); ++i)
	{
		

		
		//setting the values
		SplineMeshComponents[i]->SetStartPosition(Offset + Spline->GetLocationAtDistanceAlongSpline(CurrentDistance, CoorSpace), false);
		SplineMeshComponents[i]->SetStartTangent(Spline->GetDirectionAtDistanceAlongSpline(CurrentDistance, CoorSpace), false);

		const float OffsetDistance = CurrentDistance+ NeckSegmentLength + SplineMeshOverLap;

		//offset animation
		float SinValue = sin( CurrentDistanceAffector * CurrentDistance + CurrentMoveTime * CurrentMoveTimeAffector);
		float CosValue = cos( CurrentDistanceAffector * CurrentDistance + CurrentMoveTime * CurrentMoveTimeAffector);
		Offset = Spline->GetRightVectorAtDistanceAlongSpline(CurrentDistance, CoorSpace) * CosValue +
			Spline->GetUpVectorAtDistanceAlongSpline(CurrentDistance, CoorSpace) * SinValue;
		Offset *= MoveAmplitude;
		
		SplineMeshComponents[i]->SetEndPosition(Offset + Spline->GetLocationAtDistanceAlongSpline(OffsetDistance, 
		CoorSpace), false);
		SplineMeshComponents[i]->SetEndTangent(Spline->GetDirectionAtDistanceAlongSpline(OffsetDistance, 
		CoorSpace), true);
		
		CurrentDistance += NeckSegmentLength;
	}
	
}

void ASplineWormEnemy::HandleIdleAnimation() {
	
	
	
	for (int i = 0; i < SplineMeshComponents.Num(); ++i) {
		float CurrentDistance = i * NeckSegmentLength + (Spline->GetSplineLength() - GetWormRealLength()); // offsets until end of line
		
		//offset animation
		float SinValue = sin(  CurrentMoveTimeAffector * CurrentMoveTime + CurrentDistance);
		float CosValue = cos( CurrentMoveTimeAffector * CurrentMoveTime + CurrentDistance);
		FVector Offset = Spline->GetRightVectorAtDistanceAlongSpline(CurrentDistance, CoorSpace) * CosValue +
			Spline->GetUpVectorAtDistanceAlongSpline(CurrentDistance, CoorSpace) * SinValue;
		Offset *= MoveAmplitude;
		
		SplineMeshComponents[i]->SetStartPosition(Offset +
			Spline->GetLocationAtDistanceAlongSpline(CurrentDistance, CoorSpace), false);
		SplineMeshComponents[i]->SetEndPosition(Offset +
			Spline->GetLocationAtDistanceAlongSpline(CurrentDistance + NeckSegmentLength, CoorSpace), true);

	}
}

void ASplineWormEnemy::UpdateTargetTransfrom(float RatioOnSnake)
{	

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

void ASplineWormEnemy::UpdateHeadTransfrom() {
	float Distance = GetWormRealLength() + CurrentWormDistance;
	FVector Location = Spline->GetLocationAtDistanceAlongSpline(Distance,
		ESplineCoordinateSpace::World);
	FRotator Rotation = Spline->GetRotationAtDistanceAlongSpline(Distance,
		ESplineCoordinateSpace::World);

	WormHeadMesh->SetWorldLocation(Location);
	WormHeadMesh->SetWorldRotation(Rotation);
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
				//essensial
				NewSplineMesh->RegisterComponent();
				NewSplineMesh->SetMobility(EComponentMobility::Movable);
				NewSplineMesh->SetStaticMesh(NeckSegment);

				//other
				//NewSplineMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
				// NewSplineMesh->SetMaterial(0, WormBodyMaterial);
				
				//adds to array
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


	//setting the sizes on each spline mesh segment
	//this cpould be optimized, but since its only called once per worm, i think its fine for now
	
	const float TotalLength = GetWormRealLength();
	for (int i = 0; i < SplineMeshComponents.Num(); ++i) {
		//setting sclae
			//start
		float CurveTime = (i * NeckSegmentLength) / TotalLength;
		float Size = WormSizeCurve->GetFloatValue(CurveTime);
		SplineMeshComponents[i]->SetStartScale(FVector2D(Size), false);
		
			//end
		CurveTime = (1 + i) * NeckSegmentLength / TotalLength;
		Size = WormSizeCurve->GetFloatValue(CurveTime);
		SplineMeshComponents[i]->SetEndScale(FVector2D(Size), true);

		//setting random roll
		float randomRoll = FMath::RandRange(-RandomRotationAmoundt, RandomRotationAmoundt);
		SplineMeshComponents[i]->SetEndRoll(randomRoll);
		SplineMeshComponents[i]->SetStartRoll(randomRoll);
	}
}

float ASplineWormEnemy::GetWormRealLength() const
{
	return SplineMeshComponents.Num() * NeckSegmentLength + SplineMeshOverLap;
}

void ASplineWormEnemy::OnGrappleReaced(float Addspeed)
{
	DL_NORMAL("Finished grappling worm!")
	// Destroy();
}

void ASplineWormEnemy::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	bPlayingAnim = true;
}


