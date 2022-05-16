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
#include "Components/CapsuleComponent.h"
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
	// StaticMeshComponents.Init(nullptr, 0);
	
	GrappleSphereComponent = CreateDefaultSubobject<UGrappleSphereComponent>(TEXT("GrappleSphereComponent"));
	GrappleSphereComponent->SetupAttachment(GetRootComponent());

	WormTargetMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WormTargetMesh"));
	WormTargetMesh->SetupAttachment(GrappleSphereComponent);
	WormTargetMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	WormHeadMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WormHeadMesh"));
	WormHeadMesh->SetupAttachment(GetRootComponent());
	WormHeadMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ColliderCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	ColliderCapsule->SetupAttachment(GetRootComponent());
	ColliderCapsule->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	ColliderCapsule->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	
	
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
	
	CarPawn = GetWorld()->GetFirstPlayerController()->GetPawn();

	//sets collider capsule size
	float MaxX = NeckSegment->GetBoundingBox().Max.X - NeckSegment->GetBoundingBox().Min.X;
	ColliderCapsule->SetCapsuleHalfHeight(MaxX * 3.f);
	ColliderCapsule->SetCapsuleRadius(MaxX / 2.f);
}

// Called every frame
void ASplineWormEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	

	
	
	if (CurrentWormState == EWormState::Idle) {
		CurrentMoveTime += DeltaTime;
		HandleIdleAnimation();
		UpdateColliderCapsule();
	}
	else if (CurrentWormState == EWormState::UnInitialized) {
		return;
	}

	//if we reach here this means we splineWormEnemy is active


	
	//Collider Capsule
	UpdateColliderCapsule();
	
	//meshes
	UpdateSplineMeshComponent();
	// UpdateStaticMeshComponents();
	UpdateHeadTransfrom();
	UpdateTargetTransfrom(HeadPlacement);

	//particle systems
	UpdateNiagaraParticleComponents();

	//sets the current move time
	CurrentMoveTime += DeltaTime;
	CurrentWormDistance = MovmentCurveFloat->GetFloatValue(CurrentMoveTime / WormMoveDuration);
	CurrentWormDistance *= Spline->GetSplineLength();
	CurrentWormDistance -= GetWormRealLength(); // converts to the movment is based on the end of the worm

	//activate grapple target?
	if (CurrentMoveTime + PreActivateTargetTime > WormMoveDuration)
	{
		GrappleSphereComponent->SetIsEnabled(true);
	}
	
    // checking if we have reached the end
    if ( Spline->GetSplineLength() < SplineMeshComponents.Num() *
    	NeckSegmentLength + SplineMeshOverLap + CurrentWormDistance)
    {
	    CurrentWormState = EWormState::Idle;
    	
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

void ASplineWormEnemy::InitStaticMeshComponents() {
	//init spline segmensts
	int SegmentsToCreate = (WormGoalLength / NeckSegmentLength) - StaticMeshComponents.Num();

	DL_NORMAL("segments to create : " + FString::FromInt(SegmentsToCreate))
	
	for (int32 i = 0; i < SegmentsToCreate; i++)
	{
		UStaticMeshComponent* NewStaticMesh = NewObject<UStaticMeshComponent>(this);
		if (NewStaticMesh)
		{
			//essensial
			NewStaticMesh->RegisterComponent();
			NewStaticMesh->SetMobility(EComponentMobility::Movable);
			NewStaticMesh->SetStaticMesh(NeckSegment);

			//other
			NewStaticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			// NewSplineMesh->SetMaterial(0, WormBodyMaterial);
			
			//adds to array
			StaticMeshComponents.Emplace(NewStaticMesh);
		}
	}
		
	


	//setting the sizes on each spline mesh segment
	//this cpould be optimized, but since its only called once per worm, i think its fine for now
	
	const float TotalLength = GetWormRealLength();
	for (int i = 0; i < StaticMeshComponents.Num(); ++i) {
		//setting sclae
		//start
		float CurveTime = (i * NeckSegmentLength) / TotalLength;
		float Size = WormSizeCurve->GetFloatValue(CurveTime);
		StaticMeshComponents[i]->SetWorldScale3D(FVector(Size));
		
		
		// //setting random roll
		// float randomRoll = FMath::RandRange(-RandomRotationAmoundt, RandomRotationAmoundt);
		// StaticMeshComponents[i]->SetEndRoll(randomRoll);
		// StaticMeshComponents[i]->SetStartRoll(randomRoll);
	}
}

void ASplineWormEnemy::UpdateStaticMeshComponents() {
	float CurrentDistancee = CurrentWormDistance;
	FVector Offset = FVector::ZeroVector;
	for (int i = 0; i < StaticMeshComponents.Num(); ++i) {

		DL_NORMAL("Update static mesh")
		
		//offset animation
		float SinValue = sin( CurrentDistanceAffector * CurrentDistancee + CurrentMoveTime * CurrentMoveTimeAffector);
		float CosValue = cos( CurrentDistanceAffector * CurrentDistancee + CurrentMoveTime * CurrentMoveTimeAffector);
		Offset = Spline->GetRightVectorAtDistanceAlongSpline(CurrentDistancee, CoorSpace) * CosValue +
			Spline->GetUpVectorAtDistanceAlongSpline(CurrentDistancee, CoorSpace) * SinValue;
		Offset *= MoveAmplitude;

		// Location
		FVector Location = Spline->GetLocationAtDistanceAlongSpline(CurrentDistancee, CoorSpace);
		StaticMeshComponents[i]->SetWorldLocation(Location + Offset);

		// Rotation
		FRotator Rotation = Spline->GetRotationAtDistanceAlongSpline(CurrentDistancee, CoorSpace);
		StaticMeshComponents[i]->SetWorldRotation(Rotation);
		
		CurrentDistancee += NeckSegmentLength;
	}
}

void ASplineWormEnemy::UpdateColliderCapsule() {
	//updating capsule collisiton
	
	float Distance = Spline->GetDistanceAlongSplineAtSplineInputKey(Spline->FindInputKeyClosestToWorldLocation(CarPawn->GetActorLocation()));
	if (Distance < CurrentWormDistance)
		Distance = CurrentWormDistance;
	else if (Distance > CurrentWormDistance + GetWormRealLength())
		Distance = CurrentWormDistance + GetWormRealLength();
	
	//location
	FVector Location = Spline->GetLocationAtDistanceAlongSpline(Distance, CoorSpace);
	ColliderCapsule->SetWorldLocation(Location);
	
	//rotation
	FRotator CapsuleRotation = UKismetMathLibrary::MakeRotFromZX(Spline->GetDirectionAtDistanceAlongSpline(Distance, CoorSpace), FVector::UpVector);
	ColliderCapsule->SetWorldRotation(CapsuleRotation);
	
	// DrawDebugSphere(GetWorld(), Location, 500.f, 32, FColor::Red);
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

void ASplineWormEnemy::GetTargetUpRightVector(FVector& Up, FVector& Forward, FVector& Location, float WormDistance) {
	float Distance = WormDistance + GetWormRealLength() * HeadPlacement;
	Location = Spline->GetLocationAtDistanceAlongSpline(Distance, CoorSpace);
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

	

	if (bInvertUpHeadAxis)
		Up = -Up;

	Up = UKismetMathLibrary::RotateAngleAxis(Up, RotateHeadAxis, Forward);
}

void ASplineWormEnemy::UpdateTargetTransfrom(float RatioOnSnake)
{	

	//rotation
	FVector Up;
	FVector Forward;
	FVector Location;
	GetTargetUpRightVector(Up, Forward, Location, CurrentWormDistance);

	Location += Forward * HeadDistanceFromBody;
	GrappleSphereComponent->SetWorldLocation(Location);
	
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

void ASplineWormEnemy::InitSplineMeshSegments() {
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

void ASplineWormEnemy::UpdateSplineMeshComponent()
{
	
	float CurrentDistance = CurrentWormDistance;

	FVector Offset = FVector::ZeroVector;
	
	for (int32 i = 0; i < SplineMeshComponents.Num(); ++i)
	{
		//setting the values
		SplineMeshComponents[i]->SetStartPosition(Offset + Spline->GetLocationAtDistanceAlongSpline(CurrentDistance, CoorSpace), false);
		SplineMeshComponents[i]->SetStartTangent(Spline->GetDirectionAtDistanceAlongSpline(CurrentDistance, CoorSpace), false);

		const float OffsetDistance = CurrentDistance + NeckSegmentLength + SplineMeshOverLap;

		//offset animation
		float SinValue = sin( CurrentDistanceAffector * CurrentDistance + CurrentMoveTime * CurrentMoveTimeAffector);
		float CosValue = cos( CurrentDistanceAffector * CurrentDistance + CurrentMoveTime * CurrentMoveTimeAffector);
		Offset = Spline->GetRightVectorAtDistanceAlongSpline(CurrentDistance, CoorSpace) * CosValue +
			Spline->GetUpVectorAtDistanceAlongSpline(CurrentDistance, CoorSpace) * SinValue;
		Offset *= MoveAmplitude;

		//end position
		SplineMeshComponents[i]->SetEndPosition(Offset + Spline->GetLocationAtDistanceAlongSpline(OffsetDistance, 
		CoorSpace), false);
		SplineMeshComponents[i]->SetEndTangent(Spline->GetDirectionAtDistanceAlongSpline(OffsetDistance, 
		CoorSpace), true);
		
		CurrentDistance += NeckSegmentLength;
	}
	
}

float ASplineWormEnemy::GetWormRealLength() const
{
	int TotalSegments = (WormGoalLength / NeckSegmentLength);
	return float(TotalSegments) * NeckSegmentLength + SplineMeshOverLap;
	// return SplineMeshComponents.Num() * NeckSegmentLength + SplineMeshOverLap;
}

void ASplineWormEnemy::OnGrappleReaced(float Addspeed)
{
	DL_NORMAL("Finished grappling worm!")
	ColliderCapsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	// Destroy();
}

void ASplineWormEnemy::StartWorm() {
	ResetWorm();
	
	CurrentWormState = EWormState::Active;
	WormTargetMesh->SetVisibility(true, false);
	WormHeadMesh->SetVisibility(true, false);
	ColliderCapsule->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	InitSplineMeshSegments();
	// InitStaticMeshComponents();
	InitNiagaraParticleComponents(SplineMeshComponents.Num());
	DL_NORMAL("StartWOrm!")
}

void ASplineWormEnemy::ResetWorm() {
	CurrentWormState = EWormState::UnInitialized;
	ColliderCapsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
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

void ASplineWormEnemy::VisualizeWormEnemy() {
	//drawing the direction the player will be sent
	float DistanceOnWorm = Spline->GetSplineLength() - GetWormRealLength();
	FVector Up, Forward, Location;
	GetTargetUpRightVector(Up, Forward, Location, DistanceOnWorm);
	GrappleSphereComponent->SetWorldLocation(Location);
	GrappleSphereComponent->SetWorldRotation(UKismetMathLibrary::MakeRotFromXZ(-Forward, Up));

	
	UWorld* World = GetWorld();
	// forward -> Direction of player when finished grappling
	DrawDebugLine(World, Location, Location - Forward * 4000.f, FColor::Cyan, false, 1.2f, 0, 200.f);
	// up
	DrawDebugLine(World, Location, Location + Up * 1000.f, FColor::Red, false, 1.2f, 0, 200.f);
	
}


