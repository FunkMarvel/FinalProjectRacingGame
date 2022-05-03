// Fill out your copyright notice in the Description page of Project Settings.


#include "SplineWormEnemy.h"

#include "DrawDebugHelpers.h"
#include "NiagaraComponentPool.h"
#include "NiagaraComponentPool.h"
#include "../GrappleSphereComponent.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "RacingUnrealProject/DebugLog.h"
#include "RacingUnrealProject/EnterExitTrigger.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "RacingUnrealProject/CarPawn.h"

// Sets default values
ASplineWormEnemy::ASplineWormEnemy()
{
	
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	Spline = CreateDefaultSubobject<USplineComponent>(TEXT("Spline"));
	SetRootComponent(Spline);

	SplineMeshComponents.Init(nullptr, 0);
	NiagaraComponents.Init(nullptr, 0);
	
	GrappleSphereComponent = CreateDefaultSubobject<UGrappleSphereComponent>(TEXT("GrappleSphereComponent"));
	GrappleSphereComponent->SetupAttachment(GetRootComponent());

	WormTargetMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WormTargetMesh"));
	WormTargetMesh->SetupAttachment(GrappleSphereComponent);
	WormTargetMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	WormHeadMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WormHeadMesh"));
	WormHeadMesh->SetupAttachment(GetRootComponent());
	WormHeadMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	
}

// Called when the game starts or when spawned
void ASplineWormEnemy::BeginPlay()
{
	Super::BeginPlay();

	
	GrappleSphereComponent->OnReachedEvent.AddDynamic(this, &ASplineWormEnemy::OnGrappleReaced);

	if (EnterTrigger) {
		EnterTrigger->EventTriggerEnterExit.AddDynamic(this, &ASplineWormEnemy::StartWorm);
	}

	if (ExitTrigger) {
		ExitTrigger->EventTriggerEnterExit.AddDynamic(this, &ASplineWormEnemy::ResetWorm);
	}

	ResetWorm();
	
}

// Called every frame
void ASplineWormEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// APawn* CarPawn = GetWorld()->GetFirstPlayerController()->GetPawn();
	// FVector Location = UKismetMathLibrary::InverseTransformLocation(NiagaraComponent->GetComponentTransform(),
	// 	CarPawn->GetActorLocation());
	// NiagaraComponent->SetVectorParameter("endLocation", CarPawn->GetActorLocation());
	// NiagaraComponent->SetVectorParameter("startLocation", CarPawn->GetActorLocation() + FVector::UpVector * 200.f);
	// NiagaraComponent->SetVectorParameter("startTangent",  FVector::UpVector * 1.f+ FVector::LeftVector * 1.f);
	// NiagaraComponent->SetVectorParameter("endTangent", FVector::UpVector * -1.f + FVector::LeftVector * -1.f);

	
	
	if (bIdle) {
		CurrentMoveTime += DeltaTime;
		HandleIdleAnimation();
	}

	
	if (!bPlayingAnim)
		return;

	if (!bHasInitSpline) {
		bHasInitSpline = true;
		InitSplineSegments();
		InitNiagaraParticleComponents(SplineMeshComponents.Num());
	}

	//meshes
	UpdateSplineMeshComponent();
	UpdateHeadTransfrom();
	UpdateTargetTransfrom(HeadPlacement);

	//particle systems
	UpdateNiagaraParticleComponents();

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

void ASplineWormEnemy::InitNiagaraParticleComponents(int numOfComps) {
	for (int i = 0; i < numOfComps; ++i) {
		UNiagaraComponent* NewNiagaraComponent = NewObject<UNiagaraComponent>(this);
		if (NewNiagaraComponent) {
			//essensial
			NewNiagaraComponent->RegisterComponent();
			NewNiagaraComponent->SetMobility(EComponentMobility::Movable);
			NewNiagaraComponent->SetAsset(NiagaraSystem);

			//adds to array
			NiagaraComponents.Emplace(NewNiagaraComponent);
		}
	}
	
}

void ASplineWormEnemy::UpdateNiagaraParticleComponents() {

	FVector startLocation, endLocation, startTangent, endTangent = FVector::ZeroVector;
	float _Distance = CurrentWormDistance;
	for (int i = 0; i < NiagaraComponents.Num(); ++i) {

		startLocation = Spline->GetLocationAtDistanceAlongSpline(_Distance, CoorSpace);
		startTangent = Spline->GetDirectionAtDistanceAlongSpline(_Distance, CoorSpace);
		
		_Distance += NeckSegmentLength;

		endLocation = Spline->GetLocationAtDistanceAlongSpline(_Distance, CoorSpace);
		endTangent = Spline->GetDirectionAtDistanceAlongSpline(_Distance, CoorSpace);

		//scaling tangens
		float _Scale = 0.5f;
		endTangent *= 0.f;
		startTangent *= _Scale;

		
		//sets parameters
		NiagaraComponents[i]->SetVectorParameter("startLocation", startLocation);
		NiagaraComponents[i]->SetVectorParameter("endLocation", endLocation);
		NiagaraComponents[i]->SetVectorParameter("startTangent",  startTangent);
		NiagaraComponents[i]->SetVectorParameter("endTangent", endTangent);
	}
}

void ASplineWormEnemy::HandleIdleAnimation()  {
	
	
	
	for (int i = 0; i < SplineMeshComponents.Num(); ++i) {
		float CurrentDistance = i * NeckSegmentLength + (Spline->GetSplineLength() - GetWormRealLength()); // offsets until end of line
		
		//offset animation
		float SinValue = sin(  IdleTimeAffector * CurrentMoveTime + CurrentDistance);
		float CosValue = cos( IdleTimeAffector * CurrentMoveTime + CurrentDistance);
		FVector Offset = Spline->GetRightVectorAtDistanceAlongSpline(CurrentDistance, CoorSpace) * CosValue +
			Spline->GetUpVectorAtDistanceAlongSpline(CurrentDistance, CoorSpace) * SinValue;
		Offset *= IdleAmplitude;
		
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
				NewSplineMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
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

void ASplineWormEnemy::StartWorm() {
	//TODO implement
	bPlayingAnim = true;
	WormTargetMesh->SetVisibility(true, false);
	WormHeadMesh->SetVisibility(true, false);
	DL_NORMAL("StartWOrm!")
}

void ASplineWormEnemy::ResetWorm() {
	bPlayingAnim = false;
	bIdle = false;
	bHasInitSpline = false;

	GrappleSphereComponent->SetIsEatable(false);
	
	CurrentMoveTime = 0.f;
	CurrentWormDistance = 0.f;

	WormTargetMesh->SetVisibility(false, false);
	WormHeadMesh->SetVisibility(false, false);

	//deletes all segments
	for (int i = 0; i < SplineMeshComponents.Num(); ++i) {
		SplineMeshComponents[i]->DestroyComponent();
		// SplineMeshComponents.RemoveAt(i);
	}
	SplineMeshComponents.Empty();
	

	//deletes all particle systems
	for (int i = 0; i < NiagaraComponents.Num(); ++i) {
		NiagaraComponents[i]->DestroyComponent();
	}
	NiagaraComponents.Empty();
	

	DL_NORMAL("ResetWorm!!")
}

void ASplineWormEnemy::VisualizeTriggers() {
	//enter spline
	FVector StartLoc = GetActorLocation();
	
	if (EnterTrigger) {
		FVector EnterTriggerLoc =  EnterTrigger->GetActorLocation();
		DrawDebugLine(GetWorld(), StartLoc, EnterTriggerLoc, FColor::Green, false, 1.f, 0,100.f);
	}

	
	if (ExitTrigger) {
		FVector ExitTriggerLoc = ExitTrigger->GetActorLocation();
		DrawDebugLine(GetWorld(), StartLoc, ExitTriggerLoc, FColor::Red, false, 1.f, 0,100.f);
	}
	return;
	
}


