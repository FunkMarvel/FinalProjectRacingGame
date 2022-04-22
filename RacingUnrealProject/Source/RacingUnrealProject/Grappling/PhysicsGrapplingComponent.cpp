// Fill out your copyright notice in the Description page of Project Settings.


#include "PhysicsGrapplingComponent.h"
#include "RacingUnrealProject/CarPawn.h"
#include "DrawDebugHelpers.h"
#include "RacingUnrealProject/GrappleSphereComponent.h"
#include "RacingUnrealProject/GravitySplineActor.h"
#include "RacingUnrealProject/NeckComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/SphereComponent.h"
#include "Components/SplineComponent.h"
#include "RacingUnrealProject/Grappling/GrappableWidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "RacingUnrealProject/DebugLog.h"

// Sets default values for this component's properties
UPhysicsGrapplingComponent::UPhysicsGrapplingComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UPhysicsGrapplingComponent::BeginPlay()
{
	Super::BeginPlay();

	
	
	
	if (GetOwner()->IsA(ACarPawn::StaticClass()))
	{
		CarPawn = Cast<ACarPawn>(GetOwner());
		StartLocationGrappleMesh = CarPawn->GrappleHookSphereComponent->GetRelativeLocation();
	}
}


void UPhysicsGrapplingComponent::HandleTargetHomingComp()
{
	//Handle TargetGraooableComponent
	TArray<UPrimitiveComponent*> OverlappingComponents;
	CarPawn->GrappleSensor->GetOverlappingComponents(OverlappingComponents);
	
	// UE_LOG(LogTemp, Warning, TEXT("%d"), OverlappingComponents.Num())
	
	TArray<UGrappleSphereComponent*> GrappableSphereComponents;
	GrappableSphereComponents.Init(nullptr, 0);
	
	for (int32 i = 0; i < OverlappingComponents.Num(); i++)
	{
		if (OverlappingComponents[i]->IsA(UGrappleSphereComponent::StaticClass()))
		{
			GrappableSphereComponents.Add(Cast<UGrappleSphereComponent>(OverlappingComponents[i]));		
		}
	}
	//UE_LOG(LogTemp, Warning, TEXT("Overlapping comps %i"), OverlappingComponents.Num())

	if (GrappableSphereComponents.Num() > 0)
	{
		if (GrappableSphereComponents[0]->IsEnabled())
		{
			
			TargetGrappableComponent = GrappableSphereComponents[0];
			CarPawn->GrappableWidgetComponent->PlaceWidget(TargetGrappableComponent);
			FoundHomingTargetEvent.Broadcast(TargetGrappableComponent);
		}
	}
	else // array size 0
	{
		TargetGrappableComponent = nullptr;
		LostHomingTargetEvent.Broadcast();
	}
}

// Called every frame
void UPhysicsGrapplingComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	CurrentStateTime += GetWorld()->GetDeltaSeconds();
	
	switch (CurrentGrappleState)
	{
	case EGrappleStates::InActive:
		InActiveState();
		break;
	case EGrappleStates::Traveling:
		TravelingState();
		break;
	case EGrappleStates::Knockoff:
		KnockoffState();
		break;
	case EGrappleStates::Hooked:
		HookedState();
		break;
	case EGrappleStates::HookedEatable:
		HookedEatableState();
		break;
	case EGrappleStates::Returning:
		ReturningState();
		break;
	}

	//bEnterState = false;

	
	
}

void UPhysicsGrapplingComponent::HandleGravity()
{
	USphereComponent* GHSComponent = nullptr;
	GHSComponent = CarPawn->GrappleHookSphereComponent;
	if (GHSComponent->IsSimulatingPhysics())
	{
		FVector Force = CarPawn->GravitySplineActive->GetFixedUpVectorFromLocation(GHSComponent->GetComponentLocation());
		GHSComponent->AddForce(-Force * GrappleGravityForce, NAME_None, true);
	}
}

bool UPhysicsGrapplingComponent::IsGrappleInsideOfRange()
{
	float SqrDistance = (CarPawn->SphereComp->GetComponentLocation() - CarPawn->GrappleHookSphereComponent->GetComponentLocation()).SizeSquared();

	//using squared distance since it cheaper
	if (SqrDistance < MaxGrappleDistance * MaxGrappleDistance)
		return true;

	return false;
}

void UPhysicsGrapplingComponent::FireGrapplingHook()
{
	
	EnterState(EGrappleStates::Traveling);
	
}

void UPhysicsGrapplingComponent::RetractGrapplingHook()
{
	//ResetTemporalVariables();
	if (CurrentGrappleState != EGrappleStates::HookedEatable)
	{
		EnterState(EGrappleStates::Returning);
		
	}
}

void UPhysicsGrapplingComponent::ResetTemporalVariables()
{
	OnHookedDirection = FVector::ZeroVector;
	OnHookedVehicleTransfrom = FTransform::Identity;
	OnHookedSpeed = 0.f;
	TargetGrappableComponent = nullptr;
	MoveToTargetModifier = 1.f;
	CurrentHookedTime = 0.f;
	KnockOffHitResult =	FHitResult();

	CarPawn->GrappleHookMesh->SetRelativeScale3D(FVector(3.f));
	
}

/**
 * @brief When the grappleComponent hits a grappleHit channel
 * @param OverlappedComponent 
 * @param OtherActor 
 * @param OtherComp 
 * @param OtherBodyIndex 
 * @param bFromSweep 
 * @param SweepResult 
 */
void UPhysicsGrapplingComponent::OnGrappleHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	return;
	UE_LOG(LogTemp, Warning, TEXT("HIT"))
	if (CurrentGrappleState != EGrappleStates::Traveling)
		return;
	
	if (!OtherComp->IsA(UGrappleSphereComponent::StaticClass()))
		return;
	
	UGrappleSphereComponent* GrappleSphereComponent = OtherActor->FindComponentByClass<UGrappleSphereComponent>(); // better? Better

	if (GrappleSphereComponent->IsEnabled())
	{
		GrappleSphereComponent->OnGrapple();
		TargetGrappableComponent = GrappleSphereComponent;

		//eatable v grappable logic
		if (GrappleSphereComponent->IsEatable())
			EnterState(EGrappleStates::HookedEatable);
		else
			EnterState(EGrappleStates::Hooked);
	}
		
	
	
}

void UPhysicsGrapplingComponent::EnterState(EGrappleStates NewState)
{
	bEnterState = true;
	CurrentStateTime = 0.f;
	CurrentGrappleState = NewState;
}

void UPhysicsGrapplingComponent::InActiveState()
{
	if (bDebugGrappleState)
		GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Cyan, "Inactive State", true);
	if (bEnterState){
		bEnterState = false;
		
		//CarPawn->GrapplingHookMesh->AddForce(CarPawn->GetActorForwardVector() * 10000000.f, FName(""), true);
		CarPawn->GrappleHookSphereComponent->AttachToComponent(GetOwner()->GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		CarPawn->GrappleHookSphereComponent->SetRelativeLocation(StartLocationGrappleMesh);
		CarPawn->GrappleHookSphereComponent->SetRelativeRotation(FRotator::ZeroRotator);
		ResetTemporalVariables();
		CarPawn->GrappleHookMesh->SetRelativeRotation(FRotator::ZeroRotator);

		//CarPawn->GrappleSensor->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		
	}
	FVector BaseSplineLocation = CarPawn->SphereComp->GetComponentLocation();
	CarPawn->NeckComponent->UpdateSplinePointsLocations(BaseSplineLocation, BaseSplineLocation, false);
	CarPawn->NeckComponent->UpdateSplinePointsTangents(FVector::ZeroVector, FVector::ZeroVector, false);
	CarPawn->NeckComponent->UpdateSplineMesh();

	// setting the Sensor mesh to match the camera forward vector
	FRotator NewSensorRot = CarPawn->MainCamera->GetForwardVector().Rotation();
	CarPawn->GrappleSensor->SetWorldRotation(NewSensorRot);
	
	HandleTargetHomingComp();
	//DL_NORMAL("Finn denne meldingen og slett den Anders! >:)")
}

void UPhysicsGrapplingComponent::TravelingState()
{
	if (bDebugGrappleState)
		GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Cyan, "Traveling State", true);
	if (bEnterState)
	{
		bEnterState = false;
		//CarPawn->GrappleSensor->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

		CarPawn->GrappleHookSphereComponent->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
		CarPawn->GrappleHookSphereComponent->SetSimulatePhysics(true);
		CarPawn->GrappleHookSphereComponent->SetPhysicsLinearVelocity(CarPawn->MainCamera->GetForwardVector() * FireGrappleSpeed);
		//CarPawn->GrappleHookSphereComponent->AddImpulse(CarPawn->MainCamera->GetForwardVector() * FireGrappleSpeed, NAME_None, true);
		CarPawn->GrappleSensor->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

		CarPawn->GrappleHookMesh->SetRelativeScale3D(FVector(10.f));

		//event
		if (TargetGrappableComponent)
		{
			BeginHomingEvent.Broadcast();
			
		}
		
	}
	// UE_LOG(LogTemp, Warning, TEXT("current GrappleSphereSpeed %f"), CarPawn->GrappleHookSphereComponent->GetPhysicsLinearVelocity().Size())

	//updates the sensor to match grappleSphere velocity
	FRotator NewRot = UKismetMathLibrary::MakeRotFromXZ(CarPawn->GrappleHookSphereComponent->GetPhysicsLinearVelocity(), CarPawn->SphereComp->GetUpVector());
	CarPawn->GrappleSensor->SetWorldRotation(NewRot);
	CarPawn->GrappleHookMesh->SetWorldRotation(NewRot);

	//applies gravity
	HandleGravity();
	
	//updates spline
	FVector StartLocation, EndLocation = FVector::ZeroVector;
	
	StartLocation = CarPawn->CarMesh->GetComponentLocation();
	EndLocation = CarPawn->GrappleHookMesh->GetComponentLocation();

	float Distance = (StartLocation - EndLocation).Size();
	
	FVector StartTangent = CarPawn->CarMesh->GetForwardVector() * Distance;
	FVector EndTangent = CarPawn->GrappleHookMesh->GetForwardVector() * Distance;

	//new method for end tangent
	EndTangent = FVector::CrossProduct( CarPawn->GrappleHookSphereComponent->GetPhysicsLinearVelocity(),
		CarPawn->SphereComp->GetForwardVector());
	EndTangent = EndTangent.GetSafeNormal();
	EndTangent *= Distance;
	
	CarPawn->NeckComponent->UpdateSplinePointsLocations(StartLocation, EndLocation, false);
	CarPawn->NeckComponent->UpdateSplinePointsTangents(StartTangent, EndTangent, true);
	
	//updates spline mesh
	CarPawn->NeckComponent->UpdateSplineMesh();

	//this also handles the entering of states
	HandleRayTraceLogic();
	
	if (!IsGrappleInsideOfRange())
	{
		EnterState(EGrappleStates::Returning);
	}
	
	if (TargetGrappableComponent)
	{
		// UE_LOG(LogTemp, Warning, TEXT("Homing"))
		FVector Vel = CarPawn->GrappleHookSphereComponent->GetPhysicsLinearVelocity();
		FVector ToTarget = TargetGrappableComponent->GetComponentLocation() - CarPawn->GrappleHookSphereComponent->GetComponentLocation();
		FVector Cross = FVector::CrossProduct(Vel, ToTarget);
		Cross = Cross.GetSafeNormal();
		float Angle = ACarPawn::UnsignedAngle(Vel, ToTarget);
		if (Angle > 5.f)
		{
			Vel = Vel.RotateAngleAxis(UGameplayStatics::GetWorldDeltaSeconds(this) * GrappleRotationSpeed, Cross);
			CarPawn->GrappleHookSphereComponent->SetPhysicsLinearVelocity(Vel);
		}
		else
		{
			const float speed = Vel.Size();
			Vel = ToTarget.GetSafeNormal() * speed;
			CarPawn->GrappleHookSphereComponent->SetPhysicsLinearVelocity(Vel);
		}
		
		//sets the shark head
		FRotator NewHeadRot = UKismetMathLibrary::MakeRotFromXZ(Vel.GetSafeNormal(), CarPawn->LocalUpVector);
		CarPawn->GrappleHookMesh->SetWorldRotation(NewHeadRot);
		
	}
}

void UPhysicsGrapplingComponent::KnockoffState()
{
	if (bDebugGrappleState)
		GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Cyan, "Knockoff State", true);
	if (bEnterState)
	{
		bEnterState = false;

		USphereComponent* GHSComponent = CarPawn->GrappleHookSphereComponent;
		GHSComponent->SetSimulatePhysics(true);
		FVector Direction = KnockOffHitResult.ImpactNormal;
		Direction += ((CarPawn->GrappleHookSphereComponent->GetComponentLocation() - CarPawn->SphereComp->GetComponentLocation())).GetSafeNormal();
		Direction += FMath::VRand() * 0.3f;
		Direction *= KnockoffForce;
		GHSComponent->SetPhysicsLinearVelocity(Direction);

		if (KnockOffParticleSystem)
		{
			FRotator NewRotation = UKismetMathLibrary::MakeRotFromZX(KnockOffHitResult.ImpactNormal,
				(CarPawn->GrappleHookSphereComponent->GetComponentLocation() - CarPawn->SphereComp->GetComponentLocation()));
			UGameplayStatics::SpawnEmitterAtLocation(this, KnockOffParticleSystem, CarPawn->GrappleHookSphereComponent->GetComponentLocation(),
				NewRotation);
		}
		else
			DL_ERROR("NoParticleSystem assigned! (Knockoff State)")
	}

	// appling gravity
	HandleGravity();

	
	//setting the spline points
	FVector StartLocation, EndLocation, StartTangent, EndTangent;

	StartLocation = CarPawn->SphereComp->GetComponentLocation();
	EndLocation = CarPawn->GrappleHookSphereComponent->GetComponentLocation();
	float Distance = (StartLocation - EndLocation).Size();
	
	StartTangent = CarPawn->SphereComp->GetForwardVector() * Distance;
	EndTangent = CarPawn->GrappleHookSphereComponent->GetForwardVector() * Distance;
	
	CarPawn->NeckComponent->UpdateSplinePointsLocations(StartLocation, EndLocation, false);
	CarPawn->NeckComponent->UpdateSplinePointsTangents(StartTangent, EndTangent, true);

	
	//updates spline mesh
	CarPawn->NeckComponent->UpdateSplineMesh();
	
	if (CurrentStateTime > knockoffDuration)
	{
		EnterState(EGrappleStates::Returning);
	}
}

void UPhysicsGrapplingComponent::HookedState()
{
	if (bDebugGrappleState)
		GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Cyan, "Hooked State", true);
	
	CurrentHookedTime += GetWorld()->GetDeltaSeconds();
	static float TargetReturningTime = 1.f;
	if (bEnterState)
	{
		bEnterState = false;
		//UE_LOG(LogTemp, Warning, TEXT("Disables physics"))
		CarPawn->GrappleHookSphereComponent->SetSimulatePhysics(false);
		CarPawn->GrappleHookSphereComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		
		OnHookedSpeed = CarPawn->SphereComp->GetPhysicsLinearVelocity().Size();
		if (OnHookedSpeed >= HighestOnHookedSpeed) // clamps the velocity
			OnHookedSpeed = HighestOnHookedSpeed;
		else if (OnHookedSpeed <= LowestOnHookedSpeed)
			OnHookedSpeed = LowestOnHookedSpeed;
		
		
		OnHookedDirection = (CarPawn->GrappleHookSphereComponent->GetComponentLocation() - CarPawn->GetActorLocation()).GetSafeNormal();
		OnHookedVehicleTransfrom = CarPawn->GetTransform();

		//neck spline

		//detaches spline so is wont move
		CarPawn->NeckSpline->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		CarPawn->NeckComponent->UpdateSplinePointsLocations(CarPawn->SphereComp->GetComponentLocation(),
			CarPawn->GrappleHookSphereComponent->GetComponentLocation(), false);
		
		TargetReturningTime = CarPawn->NeckComponent->Spline->GetSplineLength() / GetOnHookedVelocitySize(); // v = s / t -> t = s / v
		TargetReturningTime *= 0.6f; // 40% faster
		TargetReturningTime = FMath::Clamp(TargetReturningTime, 0.4f, 4.f); // clamps
	}

	
	// updates spline
	CarPawn->NeckComponent->UpdateSplineEndPosition(GetTargetComponent()->GetComponentLocation());
	FVector StartTangent = GetOnHookedVelocitySize() * CarPawn->SphereComp->GetForwardVector();
	FVector EndTangent = TargetGrappableComponent->GetForwardVector() * 5000.f;
	CarPawn->NeckComponent->UpdateSplinePointsTangents(StartTangent, EndTangent, true);
	
	float Lerp = CurrentHookedTime / TargetReturningTime; // 0 to 1

	Lerp = HookedMovementCurve->GetFloatValue(Lerp); // places on curve

	Lerp = Lerp * CarPawn->NeckSpline->GetSplineLength(); // mulitplies to get actual length
	
	// get corrent length along spline
	MoveTowardsGrapple(Lerp);

	//updates spline mesh
	float StartLength, EndLength = 0.f;
	EndLength = CarPawn->NeckSpline->GetSplineLength();
	StartLength = Lerp;
	CarPawn->NeckComponent->UpdateSplineMesh(StartLength, EndLength);

	//updates grapplehookcomponent location
	if (TargetGrappableComponent)
	{
		CarPawn->GrappleHookSphereComponent->SetWorldLocation(TargetGrappableComponent->GetComponentLocation());
	}
	
	
	//cheks if we are close enough (on exit)
	if ((CarPawn->GrappleHookSphereComponent->GetComponentLocation() - CarPawn->SphereComp->GetComponentLocation())
		.SizeSquared() < 400.f*400.f)
	{
		// handlig grappleSphere and its events
		if (TargetGrappableComponent != nullptr)
		{
			TargetGrappableComponent->OnReached();

			if (TargetGrappableComponent->IsOverrideReleaseVelocity())
			{
				OnHookedDirection = TargetGrappableComponent->GetForwardVector();
			}
			OnHookedSpeed += TargetGrappableComponent->GetAddSpeed();
		}
		
		EnterState(EGrappleStates::Returning);
	}
}

void UPhysicsGrapplingComponent::HookedEatableState()
{
	if (bDebugGrappleState)
		GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Cyan, "Hooked Eatable State", true);
	if (bEnterState)
	{
		bEnterState = false;
		CarPawn->GrappleHookSphereComponent->SetSimulatePhysics(false);
		CarPawn->GrappleHookSphereComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		//neck
		CarPawn->NeckSpline->AttachToComponent(CarPawn->SphereComp, FAttachmentTransformRules::KeepWorldTransform);
	}
	
	float lerp = CurrentStateTime / ReturnEatableTime; // a lerp from 0 -> 1
	float invertedLerp = 1.f - lerp;
	float length = CarPawn->NeckSpline->GetSplineLength();

	length = length * invertedLerp;
	
	FVector NewPos = CarPawn->NeckSpline->GetLocationAtDistanceAlongSpline(length, ESplineCoordinateSpace::World);
	CarPawn->GrappleHookSphereComponent->SetWorldLocation(NewPos);

	
	//CarPawn->NeckComponent->UpdateSplinePoints();
	CarPawn->NeckComponent->UpdateSplineMesh(0.f, length);

	// sets the eatable components location and rotation
	TargetGrappableComponent->GetOwner()->SetActorLocation(CarPawn->GrappleHookSphereComponent->GetComponentLocation());
	TargetGrappableComponent->GetOwner()->SetActorRotation(CarPawn->GrappleHookSphereComponent->GetComponentRotation());

	//are we close to home
	float distanceSqr = (CarPawn->SphereComp->GetComponentLocation() -
		CarPawn->GrappleHookSphereComponent->GetComponentLocation()).SizeSquared();

	if (distanceSqr < 1000.f* 1000.f) // when its at us!
	{
		TargetGrappableComponent->OnReached();
		CarPawn->SphereComp->AddImpulse(CarPawn->SphereComp->GetForwardVector() * TargetGrappableComponent->GetAddSpeed(), NAME_None, true);
		
		EnterState(EGrappleStates::InActive);
	}
	
}

void UPhysicsGrapplingComponent::ReturningState()
{	if (bDebugGrappleState)
		GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Cyan, "Returning State", true);
	
	
	if (bEnterState){
		bEnterState = false;
		CarPawn->GrappleHookSphereComponent->SetSimulatePhysics(false);
		CarPawn->GrappleHookSphereComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		//neck
		CarPawn->NeckSpline->AttachToComponent(CarPawn->SphereComp, FAttachmentTransformRules::KeepWorldTransform);
	}

	//are we close to home
	float distanceSqr = (CarPawn->SphereComp->GetComponentLocation() -
		CarPawn->GrappleHookSphereComponent->GetComponentLocation()).SizeSquared();

	
	if (distanceSqr < 1000.f* 1000.f)
	{
		EnterState(EGrappleStates::InActive);
		return;
	}

	float lerp = CurrentStateTime / ReturnStateTime; // a lerp from 0 -> 1
	float invertedLerp = 1.f - lerp;
	float length = CarPawn->NeckSpline->GetSplineLength();

	length = length * invertedLerp;
	
	FVector NewPos = CarPawn->NeckSpline->GetLocationAtDistanceAlongSpline(length, ESplineCoordinateSpace::World);
	CarPawn->GrappleHookSphereComponent->SetWorldLocation(NewPos);
	
	CarPawn->NeckComponent->UpdateSplineMesh(0.f, length);
}

void UPhysicsGrapplingComponent::MoveTowardsGrapple(float LengthAtSpline)
{
	FVector NewLocation = CarPawn->NeckSpline->GetLocationAtDistanceAlongSpline(LengthAtSpline, ESplineCoordinateSpace::World);
	CarPawn->SetActorLocation(NewLocation);

	//old method
	/*FVector Direction = CarPawn->GrappleHookSphereComponent->GetComponentLocation() - CarPawn->GetActorLocation();
	Direction = Direction.GetSafeNormal();
	CarPawn->AddActorWorldOffset(Direction * GetOnHookedVelocitySize() * MoveToTargetModifier * UGameplayStatics::GetWorldDeltaSeconds(this), false);*/
}

/**
 * @brief Checks is the car should be in grapple state
 * @return true if we should be in grapple state
 */
bool UPhysicsGrapplingComponent::ValidGrappleState()
{
	if (GetCurrentGrappleState() == EGrappleStates::Hooked && bEnterState == false)
	{
		return true;
	}
	return false;
}

void UPhysicsGrapplingComponent::HandleRayTraceLogic()
{
	FVector StartLoc = CarPawn->GrappleHookSphereComponent->GetComponentLocation();
	FVector EndLoc = StartLoc + CarPawn->GrappleHookSphereComponent->GetPhysicsLinearVelocity().GetSafeNormal() * RaycastRange;

	FCollisionQueryParams TraceParams(FName(TEXT("")), false, GetOwner());
	//FHitResult hit{};
	
	GetWorld()->LineTraceSingleByChannel(KnockOffHitResult,
		StartLoc, EndLoc,
		ECollisionChannel::ECC_GameTraceChannel1,
		TraceParams
		);
	
	// this is super ugly, but could not find another way
	if (!KnockOffHitResult.IsValidBlockingHit())
	{
		GetWorld()->LineTraceSingleByChannel(KnockOffHitResult,
		StartLoc, EndLoc,
		ECollisionChannel::ECC_MAX,
		TraceParams
		);
	}
 
	if (!KnockOffHitResult.IsValidBlockingHit())
		return;
	
	if (!KnockOffHitResult.Component->IsA(UGrappleSphereComponent::StaticClass()))
	{
		EnterState(EGrappleStates::Knockoff);
		return;
	}

	UGrappleSphereComponent* GrappleSphereComponent = KnockOffHitResult.Actor->FindComponentByClass<UGrappleSphereComponent>(); // better? Better

	if (GrappleSphereComponent->IsEnabled())
	{
		GrappleSphereComponent->OnGrapple();
		TargetGrappableComponent = GrappleSphereComponent;

		//eatable v grappable logic
		if (GrappleSphereComponent->IsEatable())
			EnterState(EGrappleStates::HookedEatable);
		else
			EnterState(EGrappleStates::Hooked);
	}
	
	
}



