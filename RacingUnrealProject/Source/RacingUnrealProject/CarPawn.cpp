// Fill out your copyright notice in the Description page of Project Settings.


#include "CarPawn.h"
#include "Components/StaticMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/SpringArmComponent.h" 
#include "Camera/CameraComponent.h" 
#include "Kismet/KismetMathLibrary.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/ArrowComponent.h"
#include "CameraEffecttComponent.h"
#include "DebugLog.h"
#include "GravitySplineActor.h"
#include "HighGravityZone.h"
#include "NeckComponent.h"
#include "RacingUnrealProjectGameModeBase.h"
#include "Grappling/PhysicsGrapplingComponent.h"
#include "Camera/CameraModifier.h"
#include "Chaos/KinematicTargets.h"
#include "Checkpoint/Checkpoint.h"
#include "Components/SplineComponent.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "Grappling/GrappableWidgetComponent.h"


// Sets default values
ACarPawn::ACarPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SetRootComponent(SphereComp);
	SphereComp->SetSimulatePhysics(true);
	SphereComp->SetEnableGravity(false);
	SphereComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	SphereComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	SphereComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Ignore);
	//SphereComp->BodyInstance.bLockRotation = true;

	CarMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Car Mesh"));
	CarMesh->SetupAttachment(GetRootComponent());
	CarMesh->SetSimulatePhysics(false);
	//SetRootComponent(CarMesh);
	CarMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("Camera Boom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->bEnableCameraLag = true;
	CameraBoom->CameraLagSpeed = 17.f;
	CameraBoom->bEnableCameraRotationLag = true;
	CameraBoom->CameraRotationLagSpeed = 20.f;

	MainCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	MainCamera->SetupAttachment(CameraBoom);

	SecondCameraTransfrom = CreateDefaultSubobject<UArrowComponent>(TEXT("SecondCameraTransfrom"));
	SecondCameraTransfrom->SetupAttachment(GetRootComponent());
	SecondCameraTransfrom->ArrowColor = FColor::Blue;
	SecondCameraTransfrom->ArrowSize = 1.f;

	ArrowRayCastStart = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow RayCastStart"));
	ArrowRayCastStart->SetupAttachment(GetRootComponent());
	
	GrappleHookSphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("GrappleHookSphereComp"));
	GrappleHookSphereComponent->SetupAttachment(GetRootComponent());
	GrappleHookSphereComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GrappleHookSphereComponent->SetNotifyRigidBodyCollision(true);
	
	GrappleHookMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GrapplingHookMesh"));
	GrappleHookMesh->SetupAttachment(GrappleHookSphereComponent);
	GrappleHookMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GrappleHookMesh->SetEnableGravity(false);

	GrappleSensor = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GrappleSensor"));
	GrappleSensor->SetupAttachment(GrappleHookSphereComponent);
	GrappleSensor->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	GrappleSensor->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	GrappleSensor->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Overlap);
	GrappleSensor->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel1);

	// neck
	NeckSpline = CreateDefaultSubobject<USplineComponent>(TEXT("NeckSpline"));
	NeckSpline->SetupAttachment(SphereComp);
	

	// actor componentns
    PhysicsGrappleComponent = CreateDefaultSubobject<UPhysicsGrapplingComponent>(TEXT("PhysicsGrappleComponent"));
	CameraEffectComponent = CreateDefaultSubobject<UCameraEffecttComponent>(TEXT("CameraEffectComponent"));
	NeckComponent = CreateDefaultSubobject<UNeckComponent>(TEXT("NeckSplineComponent"));
	GrappableWidgetComponent = CreateDefaultSubobject<UGrappableWidgetComponent>(TEXT("GrappableWidgetComponent"));
	
}

// Called when the game starts or when spawned
void ACarPawn::BeginPlay()
{
	Super::BeginPlay();
	
	// Hit and phyus
	SphereComp->OnComponentHit.AddDynamic(this, &ACarPawn::OnHitt);
	SphereComp->OnComponentBeginOverlap.AddDynamic(this, &ACarPawn::OnBeginOverLap);
	SphereComp->OnComponentEndOverlap.AddDynamic(this, &ACarPawn::OnEndOverLap);

	GrappleHookSphereComponent->OnComponentHit.AddDynamic(PhysicsGrappleComponent, &UPhysicsGrapplingComponent::OnGrappleHit);
	
	// CameraEffectComponent->SetCameraCurrent(MainCamera);

	//deathzone variables
	StartPlayerLocation = GetActorLocation();
	
	//setting camera lag
	OnStartCameraLag = FVector2D(CameraBoom->CameraLagSpeed, CameraBoom->CameraRotationLagSpeed);
	StartCameraBoomLength = CameraBoom->TargetArmLength;
	TargetCameraBoomLength = StartCameraBoomLength;

	//neck
	//detaches the neck spline so it dosent follow
	
	
	//UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.4f);
}

void ACarPawn::RotateSphereCompToLocalUpVector()
{
	//rotates sphere
	// FRotator NewSphereRot = UKismetMathLibrary::MakeRotFromZX(LocalUpVector, GetActorForwardVector());
	// SphereComp->SetWorldRotation(NewSphereRot);

	FRotator TargetRot = UKismetMathLibrary::MakeRotFromZX(LocalUpVector, GetActorForwardVector());
	FRotator NewRotation = FMath::RInterpTo(SphereComp->GetComponentRotation(), TargetRot,
		GetWorld()->GetDeltaSeconds(), 50.f);
	SphereComp->SetWorldRotation(NewRotation);
}

void ACarPawn::ApplyGravity()
{
	//gravity
	if (GravitySplineActive != nullptr)
	{
		FVector HoverForce(0.f);
		if (SphereComp->IsSimulatingPhysics())
		{
			
			float ScaleHeight{(HoverHeight - DistanceToGround())/HoverForceReduction};
			FVector HeightVelocity{
				FVector::DotProduct(SphereComp->GetPhysicsLinearVelocity(),LocalUpVector)*LocalUpVector
			};
			FVector GravityForceVector{-LocalUpVector * GravityForce * GravityMod};
			if (IsGrounded())
			{
				HoverForce = (-GravityForceVector * UKismetMathLibrary::Exp(ScaleHeight) -
					HoverDampingFactor * HeightVelocity).GetClampedToMaxSize(10000.f); // * UKismetMathLibrary::Exp(ScaleHeight)
			}
			SphereComp->AddForce(GravityForceVector+HoverForce, FName(), true);
		}
	}
}

void ACarPawn::TiltCarMesh(FVector AsymVector)
{
	//orients the mesh
	
	FRotator NewRot = UKismetMathLibrary::MakeRotFromZX(
		LocalUpVector + AsymVector * GetWorld()->GetDeltaSeconds() * 0.003f,
		GetActorForwardVector()
		);
	
	CarMesh->SetWorldRotation(
	FMath::RInterpTo(CarMesh->GetComponentRotation(),
        NewRot,
        UGameplayStatics::GetWorldDeltaSeconds(this),
        5.f
	));
	//clamps the roll rotation
	const float ClampValue = 45.f;
	FRotator LocalRot = CarMesh->GetRelativeRotation();
	LocalRot.Roll = FMath::Clamp(LocalRot.Roll, -ClampValue, ClampValue);

	
	CarMesh->SetRelativeRotation(LocalRot);
	
	
	if (abs(LocalRot.Roll) > 41.f)
	{
		if (LocalRot.Roll > 0.f)
			MaxTurnBpEvent(true);
		else
			MaxTurnBpEvent(false);
	}
	else
		NotMaxTurnBpEvent();
}

void ACarPawn::HandleAsymFriction(const FVector& AsymVector)
{
	if (SphereComp->IsSimulatingPhysics())
	{
		SphereComp->AddForce(AsymVector* 30.f);
	}
}

// Called every frame
void ACarPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	// state machine
	switch (CurrentVehicleState)
	{
	case EVehicleState::Driving:
		StateDriving();
		break;
	case EVehicleState::AirBorne:
		StateAirBorne();
		break;
	case EVehicleState::Dashing:
		StateDashing();
		break;
	case EVehicleState::Grappling:
		StateGrappling();
		break;
		
	}


	SphereComp->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
	//sets bEnterstate to false so it wont run until we enter a new state
	//bEnterState = false;
	
	//DrawDebugLine(GetWorld(), SphereComp->GetComponentLocation(), SphereComp->GetComponentLocation() + SphereComp->GetUpVector() * 300.f, FColor::Green, false, 0.5f);
	//DrawDebugLine(GetWorld(), SphereComp->GetComponentLocation(), SphereComp->GetComponentLocation() + SphereComp->GetRightVector() * 300.f, FColor::Green, false, 0.5f);
	

}

// Called to bind functionality to input
void ACarPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Axis bindings
	PlayerInputComponent->BindAxis("MoveForward", this, &ACarPawn::MoveXAxis);
	PlayerInputComponent->BindAxis("MoveRight", this, &ACarPawn::MoveYAxis);
	PlayerInputComponent->BindAxis("LookRight", this, &ACarPawn::LookXAxis);
	PlayerInputComponent->BindAxis("LookUp", this, &ACarPawn::LookYAxis);
	
	// Action binding
	//action.bConsumeInput = false;
	PlayerInputComponent->BindAction("Fire", EInputEvent::IE_Pressed, this, &ACarPawn::ToggleGrappleHook);
	PlayerInputComponent->BindAction("Up", EInputEvent::IE_Pressed, this, &ACarPawn::SetGameSpeedUp);
	PlayerInputComponent->BindAction("Down", EInputEvent::IE_Pressed, this, &ACarPawn::SetGameSpeedDown);
	PlayerInputComponent->BindAction("Space", EInputEvent::IE_Pressed, CameraEffectComponent, &UCameraEffecttComponent::SpaceCamera).bConsumeInput = false;
	// PlayerInputComponent->BindAction("Space", EInputEvent::IE_Pressed, this, &ACarPawn::OnSpacePressed);
}

void ACarPawn::UpdateCameraBoomLength()
{
	float newVal = FMath::FInterpTo(CameraBoom->TargetArmLength, TargetCameraBoomLength, GetWorld()->GetDeltaSeconds(), 5.f);
	CameraBoom->TargetArmLength = newVal;
	return;
}

void ACarPawn::EnterState(EVehicleState NewState)
{
	bEnterState = true;
	CurrentVehicleState = NewState;
}

void ACarPawn::StateDriving()
{
	if (bEnterState)
	{
		bEnterState = false;
		//CameraEffectComponent->BoostCameraModifier->DisableModifier(false);
	}
	ApplyGravity();
	SetUpVectorAsSplineUpAxis();
	// LocalUpVector = GetUpVectorFromUnderCar();
	
	if (IsGrounded())
	{
		
		
		const FVector AsymVector = CalcAsymVector();
		HandleAsymFriction(AsymVector);
		TiltCarMesh(AsymVector);
	
		// rotates sphere comp
		RotateSphereCompToLocalUpVector();
	}
	else
	{
		EnterState(EVehicleState::AirBorne);
	}

	UpdateCameraBoomLength();
	//HandleMaxTurnWithSpline();
	DrawDebugLine(GetWorld(), SphereComp->GetComponentLocation(), SphereComp->GetComponentLocation() + LocalUpVector * 10000.f, FColor::Red, false);
	
	//should we be grappling
	if (PhysicsGrappleComponent->ValidGrappleState())
	{
		EnterState(EVehicleState::Grappling);
	}
}

void ACarPawn::StateGrappling()
{
	if (bEnterState)
	{
		bEnterState = false;
		SphereComp->SetSimulatePhysics(false);
		
		// CameraBoom->CameraLagSpeed = GrapplingCameraLag.X;
		// CameraBoom->CameraRotationLagSpeed = GrapplingCameraLag.Y;
		CameraEffectComponent->BoostCameraModifier->EnableModifier();
		// UGameplayStatics::PlayWorldCameraShake(GetWorld(), CameraEffectComponent->CameraShake, SphereComp->GetComponentLocation(),
		// 	  0.f, 1000.f, 1.f);
		
	}

	
	
	//orients the sphere comp
	
	float StartDistance = (PhysicsGrappleComponent->GetOnHookedVehicleTransform().GetLocation() -  GrappleHookMesh->GetComponentLocation()).Size();
	float CurrentDistance = (SphereComp->GetComponentLocation() -  GrappleHookMesh->GetComponentLocation()).Size();
	float lerpFactor = CurrentDistance / StartDistance; // at start will be 1, and will progress towards 0
	lerpFactor = 1.f -lerpFactor; // at start will be 0, will progress towards 1

	if (PhysicsGrappleComponent->GetTargetComponent())
	{
		FRotator NewRot = UKismetMathLibrary::RLerp(
			PhysicsGrappleComponent->GetOnHookedVehicleTransform().Rotator(),
			PhysicsGrappleComponent->GetTargetComponent()->GetComponentRotation(),
			lerpFactor,
			true);
		SphereComp->SetWorldRotation(NewRot);
	}
	CameraBoom->SetRelativeRotation(FRotator(-5.f, 0.f, 0.f));
	
	//psudo on exit
	if (PhysicsGrappleComponent->ValidGrappleState() == false)
	{
		// sets velocity
		SphereComp->SetSimulatePhysics(true);
		FVector NewVel = PhysicsGrappleComponent->GetOnHookedDirection() * PhysicsGrappleComponent->GetOnHookedVelocitySize();
		//CameraEffectComponent->PlayCameraEffect();
		SphereComp->SetPhysicsLinearVelocity(NewVel);

		//sets rotation speed
		// CameraBoom->CameraLagSpeed = OnStartCameraLag.X;
		// CameraBoom->CameraRotationLagSpeed = OnStartCameraLag.Y;
		
		EnterState(EVehicleState::AirBorne);
	}
	
}

void ACarPawn::StateAirBorne()
{
	if (bEnterState)
	{
		bEnterState = false;
		
	}
	SetUpVectorAsSplineUpAxis();
	// LocalUpVector = GetUpVectorFromUnderCar();
	
	RotateSphereCompToLocalUpVector();
	ApplyGravity();
	TiltCarMesh(FVector::ZeroVector);

	
	if (IsGrounded())
	{
		
		EnterState(EVehicleState::Driving);
	}

	if (IsOutOfBounds())
	{
		ARacingUnrealProjectGameModeBase* GameMode = Cast<ARacingUnrealProjectGameModeBase>(GetWorld()->GetAuthGameMode());
		if (GameMode && GameMode->GetLastCheckpoint())
		{
			ACheckpoint* CP = GameMode->GetLastCheckpoint();
			SetActorLocation(CP->GetSpawnArrow()->GetComponentLocation());
			SetActorRotation(CP->GetSpawnArrow()->GetComponentRotation());
			
			if (CP->GetCheckpointGravitySpline())
				GravitySplineActive = CP->GetCheckpointGravitySpline();
			else
				DL_NORMAL( "gravity spline is selected" + CP->GetName());
			
			EnterState(EVehicleState::AirBorne);
			SphereComp->SetPhysicsLinearVelocity(FVector::ZeroVector);
		}
		
	}
	//shoud we be in grapple state
	if (PhysicsGrappleComponent->ValidGrappleState())
	{
		EnterState(EVehicleState::Grappling);
	}
	
}

void ACarPawn::StateDashing()
{
	if (bEnterState)
	{
		bEnterState = false;
	}
}

void ACarPawn::ToggleGrappleHook()
{
	if (PhysicsGrappleComponent->GetCurrentGrappleState() == EGrappleStates::InActive)
	{
		//EnterState(EVehicleState::Grappling);
		PhysicsGrappleComponent->FireGrapplingHook();
	}
	else
	{
		// EnterState(EVehicleState::AirBorne);
		PhysicsGrappleComponent->RetractGrapplingHook();
	}
	
}

FVector ACarPawn::CalcAsymVector()
{
	// Got help from Anders so its physics based
	FVector NegativeVelocity = -SphereComp->GetPhysicsLinearVelocity();
	FVector FrictionForce = NegativeVelocity.GetSafeNormal() * AsymForce; // a big number

	FVector CalcAsymForce = GetActorRightVector() * FVector::DotProduct(SphereComp->GetRightVector(), FrictionForce);

	float cutoffspeed = 700.f;
	if (NegativeVelocity.SizeSquared() < cutoffspeed* cutoffspeed)
	{
		CalcAsymForce *= NegativeVelocity.Size() / 5000.f;
		// CalcAsymForce = FVector::ZeroVector;
		// DL_NORMAL("UnderCutoffSpeed")
	}
	
		
	return CalcAsymForce;
}

float ACarPawn::CaltAsymForce()
{
	return 0.0f;
}

void ACarPawn::MoveXAxis(const float Value)
{
	//guard cluase
	if (CurrentVehicleState != EVehicleState::Driving)
	{
		return;
	}
	
	float TimeIndependentValue = Value /** UGameplayStatics::GetWorldDeltaSeconds(this)*/;
	if (!SphereComp->IsSimulatingPhysics()) // guard cluase
		return;

	if (IsMovingForward()) // is moving forward
	{
		if (TimeIndependentValue > 0.f) // accelerating
		{
			if (IsUnderMaxSpeed(false))
				SphereComp->AddForce(GetActorForwardVector() * AccelerationForce * TimeIndependentValue, NAME_None, true);
			
		}
		else // deaccelerating
		{
			SphereComp->AddForce(GetActorForwardVector() * DeaccelerationForce * TimeIndependentValue, NAME_None, true);
		}
	}
	else // moving bacward
	{
		if (TimeIndependentValue > 0.f) // accelerating
		{
			SphereComp->AddForce(GetActorForwardVector() * AccelerationForce * TimeIndependentValue, NAME_None, true);
		}
		else // still deaccelerating
		{
			if (IsUnderMaxSpeed(false))
				SphereComp->AddForce(GetActorForwardVector() * DeaccelerationForce * 0.2f * TimeIndependentValue, NAME_None, true);
			
		}
	}

	//handle damping
	if (abs(Value) < 0.05f) {
		ApplyDrag();
	}
	
}

void ACarPawn::MoveYAxis(float Value)
{
	//guard cluase
	if (CurrentVehicleState != EVehicleState::Driving)
	{
		//return;
	}
	
	FVector Forwardd = SphereComp->GetForwardVector();
	FVector Upp = SphereComp->GetUpVector();
	Forwardd =Forwardd.RotateAngleAxis(Value * TurnSpeed * UGameplayStatics::GetWorldDeltaSeconds(this), Upp);

	FRotator NewRot = UKismetMathLibrary::MakeRotFromXZ(Forwardd, Upp);
	SphereComp->SetWorldRotation(NewRot);


}

void ACarPawn::LookXAxis(float Value)
{
	FRotator OldRotation = CameraBoom->GetRelativeRotation();
	float Yaw = OldRotation.Yaw + CameraLookSpeed* Value * UGameplayStatics::GetWorldDeltaSeconds(this);
	Yaw = FMath::Clamp(Yaw, -MaxYawLookAngle, MaxYawLookAngle);

	OldRotation.Yaw = Yaw;
	CameraBoom->SetRelativeRotation(OldRotation);
}

void ACarPawn::LookYAxis(float Value)
{
	FRotator OldRotation = CameraBoom->GetRelativeRotation();
	float Pitch = OldRotation.Pitch + CameraLookSpeed* Value * UGameplayStatics::GetWorldDeltaSeconds(this);
	Pitch = FMath::Clamp(Pitch, -MaxPichLookAngle, MaxPichLookAngle);

	OldRotation.Pitch = Pitch;
	CameraBoom->SetRelativeRotation(OldRotation);
}

/// <summary>
/// Returns angle in radians
/// </summary>
/// <param name="v1"></param>
/// <param name="v2"></param>
/// <param name="axis"></param>
/// <returns></returns>
float ACarPawn::SignedAngleAxis(FVector v1, FVector v2, FVector axis)
{
	v1.Normalize();
	v2.Normalize();
	float DotProduct = FVector::DotProduct(v1, v2);
	float LengthCombined = (v1.Size() * v2.Size());
	float Angle = UKismetMathLibrary::Acos(DotProduct/ LengthCombined);
	
	
	FVector CrossProduct = FVector::CrossProduct(v1, v2);
	CrossProduct = CrossProduct.GetSafeNormal();
	float SignedFactor = FVector::DotProduct(CrossProduct, axis);

	Angle *= SignedFactor;
	Angle = FMath::RadiansToDegrees(Angle);
	return Angle;
	
}

float ACarPawn::UnsignedAngle(FVector v1, FVector v2)
{
	v1.Normalize();
	v2.Normalize();
	float DotProduct = FVector::DotProduct(v1, v2);
	float LengthCombined = (v1.Size() * v2.Size());
	float Angle = UKismetMathLibrary::Acos(DotProduct / LengthCombined);
	
	Angle = FMath::RadiansToDegrees(Angle);
	return Angle;
}

bool ACarPawn::IsGrounded()
{
	FCollisionQueryParams TraceParams(FName(TEXT("")), false, GetOwner());

	FHitResult hit{};
	GetWorld()->LineTraceSingleByObjectType(
		hit,
		ArrowRayCastStart->GetComponentLocation(),
		ArrowRayCastStart->GetComponentLocation() - GetActorUpVector() * HoverCutOffHeight,
		FCollisionObjectQueryParams(ECollisionChannel::ECC_WorldStatic),
		TraceParams
	);
	
	if (hit.IsValidBlockingHit()/* && UnsignedAngle(GravitySplineActive->
		GetFixedUpVectorFromLocation(SphereComp->GetComponentLocation()), hit.Normal) < MaxGroundAngle*/) {
		return true;
	}
	return false;
}

float ACarPawn::DistanceToGround()
{
	FCollisionQueryParams TraceParams(FName(TEXT("")), false, GetOwner());
	FHitResult hit{};
	FVector StartLocation{ArrowRayCastStart->GetComponentLocation()};
	FVector EndLocation{StartLocation - GetActorUpVector()*20*HoverHeight};
	GetWorld()->LineTraceSingleByObjectType(
		hit,
		StartLocation,
		EndLocation,
		FCollisionObjectQueryParams(ECollisionChannel::ECC_WorldStatic),
		TraceParams
	);
	
	if (hit.IsValidBlockingHit()/* && UnsignedAngle(GravitySplineActive->
		GetFixedUpVectorFromLocation(SphereComp->GetComponentLocation()), hit.Normal) < MaxGroundAngle*/) {
		
		return hit.Distance;
	}
	
	return 1000*HoverHeight;
}

FVector ACarPawn::VelocityTowardsTarget(FVector StartLocation, FVector Velocity, FVector Target)
{
	float Speed = UKismetMathLibrary::Cos(UnsignedAngle(Velocity, Target - StartLocation)) * Velocity.Size();
	
	FVector VelocityTowards = (Target - StartLocation).GetSafeNormal() * Speed;
	return VelocityTowards;
}

FHitResult ACarPawn::ShootRayFromCenterOfScreen()
{
	FCollisionQueryParams TraceParams(FName(TEXT("")), false, GetOwner());
	
	FVector Start = MainCamera->GetComponentLocation()/* + LocalUpVector * 250.f*/;
	FVector End = MainCamera->GetComponentLocation() + MainCamera->GetForwardVector() * 1000000.f;
	

	FHitResult hit{};
	GetWorld()->LineTraceSingleByObjectType(
		hit,
		Start,
		End,
		FCollisionObjectQueryParams(ECollisionChannel::ECC_WorldStatic),
		TraceParams
	);
	return FHitResult();
}

void ACarPawn::SetUpVectorAsSplineUpAxis()
{
	// LocalUpVector = GetUpVectorFromUnderCar();
	LocalUpVector = GravitySplineActive->GetFixedUpVectorFromLocation(SphereComp->GetComponentLocation());
	
}

bool ACarPawn::IsMovingForward()
{
	const FVector VehicleForward = SphereComp->GetForwardVector();
	FVector VelocityDirection = VehicleForward;
	if (SphereComp->IsSimulatingPhysics())
	{
		VelocityDirection = SphereComp->GetPhysicsLinearVelocity().GetSafeNormal();
	}
	const float Angle = UnsignedAngle(VehicleForward, VelocityDirection);
	
	if (Angle <= 90.f)
		return true;
	
	return false;
}

float ACarPawn::GetSplineCarForwardAngle()
{
	FVector SplineForward = GravitySplineActive->SplineComp->FindDirectionClosestToWorldLocation(SphereComp->GetComponentLocation(), ESplineCoordinateSpace::World);
	FVector CarForward = SphereComp->GetForwardVector();
	float Angle = SignedAngleAxis(SplineForward, CarForward, LocalUpVector);
	

	return Angle;
}

void ACarPawn::HandleMaxTurnWithSpline()
{
	float Angle = GetSplineCarForwardAngle();

	
	if (Angle > MaxCar_SplineAngle)
	{
		FRotator CurrentRot = SphereComp->GetRelativeRotation();
		AddActorLocalRotation(FRotator(0.f,-MaxCar_SplineAngleCorrectionSpeed * UGameplayStatics::GetWorldDeltaSeconds(this),0.f));
	}
	else if (Angle < -MaxCar_SplineAngle)
	{
		AddActorLocalRotation(FRotator(0.f,MaxCar_SplineAngleCorrectionSpeed * UGameplayStatics::GetWorldDeltaSeconds(this),0.f));
	}
}

void ACarPawn::ApplyDrag() {
	const FVector Velocity = SphereComp->GetPhysicsLinearVelocity();
	if (SphereComp->IsSimulatingPhysics()) {
		FVector Direction = Velocity.GetSafeNormal();
		SphereComp->AddForce( -Direction * DragForce, NAME_None, true);
	}
}

bool ACarPawn::IsOutOfBounds()
{
	if (GravitySplineActive != nullptr)
	{
		FVector ClosestSplineLocation = GravitySplineActive->SplineComp->
		FindLocationClosestToWorldLocation(SphereComp->GetComponentLocation(), ESplineCoordinateSpace::World);

		float dist = (ClosestSplineLocation - SphereComp->GetComponentLocation()).Size();
		
		
		if (dist > MaxOutOfBoundsDistance)
		{
			return true;
		}
	}
	return false;
}

void ACarPawn::SetGameSpeedUp()
{
	float timedil = UGameplayStatics::GetGlobalTimeDilation(this);
	
		UGameplayStatics::SetGlobalTimeDilation( GetWorld(),timedil + 0.1f);
}

void ACarPawn::SetGameSpeedDown()
{
	float timedil = UGameplayStatics::GetGlobalTimeDilation(this);
	if (timedil - 0.1f > 0.05f)
	{
		UGameplayStatics::SetGlobalTimeDilation( GetWorld(),timedil - 0.1f);
	}
	else
	{
		UGameplayStatics::SetGlobalTimeDilation( GetWorld(),0.04f);
	}
	
}

bool ACarPawn::IsUnderMaxSpeed(bool bBuffer)
{
	float BufferMaxSpeed = MaxSpeed * MaxSpeed;
	float buffer = 0.f;
	buffer = BufferMaxSpeed * 0.05f * bBuffer;


	if (BufferMaxSpeed + buffer > SphereComp->GetPhysicsLinearVelocity().SizeSquared())
	{
		
		
		return true;
	}
	return false;
}

FVector ACarPawn::GetUpVectorFromUnderCar()
{
	FVector Start = SphereComp->GetComponentLocation();
	FVector End = Start -SphereComp->GetUpVector() * 1000.f;
	FCollisionQueryParams TraceParams(FName(TEXT("")), false, GetOwner());

	FHitResult hit{};
	GetWorld()->LineTraceSingleByObjectType(
		hit,
		Start,
		End,
		FCollisionObjectQueryParams(ECollisionChannel::ECC_WorldStatic),
		TraceParams
	);

	DrawDebugLine(GetWorld(), Start, End, FColor::Blue, false, 1.f, (uint8)0U, 10.f);
	if (hit.IsValidBlockingHit())
	{
		// DL_NORMAL("LineTraceChannelHit!")
		return hit.Normal;
	}
	return FVector::UpVector;
}

void ACarPawn::OnHitt(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	HitGroundBpEvent(SphereComp->GetPhysicsLinearVelocity().Size());
	
}

void ACarPawn::OnBeginOverLap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA<AGravitySplineActor>())
	{
		GravitySplineActive = Cast<AGravitySplineActor>(OtherActor);
	}
	else if (OtherActor->IsA<AHighGravityZone>())
	{
		GravityMod = Cast<AHighGravityZone>(OtherActor)->GetGravityModifier();
		
	}
}

void ACarPawn::OnEndOverLap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{

	if (OtherActor->IsA<AHighGravityZone>())
	{
		GravityMod = BaseGravMod;
	}
}



