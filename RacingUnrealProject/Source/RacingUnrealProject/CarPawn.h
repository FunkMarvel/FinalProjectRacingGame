// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enums/Enums.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/SpringArmComponent.h"
#include "CarPawn.generated.h"

//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHitGround, float, Speed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSpacePressedEvent);

UCLASS(config=Game, BlueprintType, Blueprintable)
class RACINGUNREALPROJECT_API ACarPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ACarPawn();

	UFUNCTION()
		void ResetCarToLastCheckpoint();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void RotateSphereCompToLocalUpVector();
	void ApplyGravity();
	void TiltCarMesh(FVector AsymVector);
	void HandleAsymFriction(const FVector& AsymVector);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// my deseg
public:
	UPROPERTY(EditDefaultsOnly, Category = "Car", BlueprintReadWrite)
	class USphereComponent* SphereComp{nullptr};
	// UPROPERTY(EditDefaultsOnly, Category = "Car")
	// class UStaticMeshComponent* CarMesh{nullptr};
	UPROPERTY(EditDefaultsOnly, Category = "Car")
	class USpringArmComponent* CameraBoom{nullptr};
	UPROPERTY(EditDefaultsOnly, Category = "Car")
	class UCameraComponent* MainCamera{nullptr};
	UPROPERTY(EditDefaultsOnly, Category = "Car")
	class UArrowComponent* SecondCameraTransfrom = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Car")
	class UArrowComponent* ArrowRayCastStart{nullptr};
	UPROPERTY(EditDefaultsOnly, Category = "Car")
	class USphereComponent* GrappleHookSphereComponent = nullptr;
	// UPROPERTY(EditDefaultsOnly, Category = "Car")
	// class UStaticMeshComponent* GrappleHookMesh = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Car")
	class UStaticMeshComponent* GrappleSensor = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Car")
	class UPhysicsGrapplingComponent* PhysicsGrappleComponent = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Car")
	class UCameraEffecttComponent* CameraEffectComponent = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Car")
	class UNeckComponent* NeckComponent = nullptr;

	//skeletal meshes
	UPROPERTY(EditDefaultsOnly, Category = "Car", BlueprintReadWrite)
	class USkeletalMeshComponent* SharkBodyMesh = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Car", BlueprintReadWrite)
	class USkeletalMeshComponent* SharkHeadMesh = nullptr;
	
	//Grapple widget comp
	UPROPERTY(EditDefaultsOnly, Category = "Car|Widget")
	class UGrappableWidgetComponent* GrappableWidgetComponent = nullptr;

	// spline neck
	UPROPERTY(EditDefaultsOnly, Category = "Car|Neck")
	class USplineComponent* NeckSpline = nullptr;

	//spline gravity
	UPROPERTY(EditAnywhere, Category = "Car|spline")
	class AGravitySplineActor* GravitySplineActive = nullptr;

	//music comp
	UPROPERTY(EditAnywhere, Category = "Car|Spline")
	class UMusicComponent* MusicComponent = nullptr;

	//values

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Car|Hover")
	float HoverHeight{100.f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Car|Hover")
	float HoverCutOffHeight{1.5f * HoverHeight};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Car|Hover")
	float HoverDampingFactor{4};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Car|Hover")
	float HoverForceReduction{20};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Car|Hover")
	float GravityForce{2720};

	UPROPERTY(meta = (AllowPrivateAccess = "true"), EditDefaultsOnly, Category = "Car|Hover")
	float GravityMod = 1.f;

	const float BaseGravMod = 1.f;

	UFUNCTION()
	void OnHitt(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse,
	            const FHitResult& Hit);
	FVector LocalUpVector = FVector(1.f, 1.f, 1.f);
	UFUNCTION()
	void OnBeginOverLap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                    int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnEndOverLap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                  int32 OtherBodyIndex);


private:
	//movement
	UPROPERTY(meta = (AllowPrivateAccess = "true"), EditDefaultsOnly, Category = "Car|Movment")
	float AccelerationForce = 1200.f;
	UPROPERTY(meta = (AllowPrivateAccess = "true"), EditDefaultsOnly, Category = "Car|Movment")
	float DeaccelerationForce = 900.f;
	UPROPERTY(meta = (AllowPrivateAccess = "true"), EditDefaultsOnly, Category = "Car|Movment")
	float DragForce = 700;
	UPROPERTY(meta = (AllowPrivateAccess = "true"), EditDefaultsOnly, Category = "Car|Movment")
	float MaxSpeed = 6900.f;
	UPROPERTY(meta = (AllowPrivateAccess = "true"), EditDefaultsOnly, Category = "Car|Movment")
	float TurnSpeed = 75.f;
	UPROPERTY(meta = (AllowPrivateAccess = "true"), EditDefaultsOnly, Category = "Car|Movment")
	float MaxGroundAngle = 75.f;
	UPROPERTY(meta = (AllowPrivateAccess = "true"), EditDefaultsOnly, Category = "Car|Movment")
	float MaxCar_SplineAngle = 85.f;
	UPROPERTY(meta = (AllowPrivateAccess = "true"), EditDefaultsOnly, Category = "Car|Movment")
	float MaxCar_SplineAngleCorrectionSpeed = 100.f;
	UPROPERTY(meta = (AllowPrivateAccess = "true"), EditDefaultsOnly, Category = "Car|Movment")
	float AsymForce = 300000.f;

	UPROPERTY()
	float YAxisValue = 0.f;
	
	//gettes for movment
public:
	UFUNCTION()
		float GetMaxSpeed() const {return MaxSpeed; }
	UFUNCTION()
		float GetCurrentForwardSpeed();
	UFUNCTION()
		float GetYAxisValue() const {return YAxisValue; }

	
	//Camera
private:
	UPROPERTY(meta = (AllowPrivateAccess = "true"), EditDefaultsOnly, Category = "Car|Camera")
	float CameraLookSpeed = 70.f; //gets set at begin play
	UPROPERTY(meta = (AllowPrivateAccess = "true"), EditAnywhere, Category = "Car|Camera")
	float MaxYawLookAngle = 90.f;
	UPROPERTY(meta = (AllowPrivateAccess = "true"), EditAnywhere, Category = "Car|Camera")
	float MaxPichLookAngle = 45.f;

	//getters setters for Cameraboom
public:
	UFUNCTION()
	void SetTargetCameraBoomLength(float NewCameraboomLength){CameraBoom->TargetArmLength = NewCameraboomLength; }
	UFUNCTION()
		float GetTargetCameraBoomLength() const {return CameraBoom->TargetArmLength; }

	

	//Deathzone
private:
	UPROPERTY()
	FVector StartPlayerLocation = FVector::ZeroVector;


	//state machince
	UPROPERTY(meta = (AllowPrivateAccess = "true"), EditAnywhere, Category = "Car|State")
	EVehicleState CurrentVehicleState = EVehicleState::AirBorne;
	UPROPERTY()
	float StateTime = 0.f;

	bool bEnterState = false;
	UFUNCTION()
	void EnterState(EVehicleState NewState);


	UFUNCTION()
	void StateDriving();

	UFUNCTION()
	void StateGrappling();
	UPROPERTY(EditAnywhere, Category = "Car|State")
	float FinishGrappleDistance = 100.f;

	UFUNCTION()
	void StateAirBorne();

	UFUNCTION()
	void StateDying();

	UFUNCTION()
	void ToggleGrappleHook();

	//TODO orgenize these :)


	//other funcs

	FVector CalcAsymVector();
	float CaltAsymForce();

	

	
	//action funcs
	void MoveXAxis(float TimeAdjustedValue);
	void MoveYAxis(float Value);
	void LookXAxis(float Value);
	void LookYAxis(float Value);

	UFUNCTION(BlueprintCallable)
	bool IsGrounded();
	float DistanceToGround();
	FVector VelocityTowardsTarget(FVector StartLocation, FVector Velocity, FVector Target);
	FHitResult ShootRayFromCenterOfScreen();
	void SetUpVectorAsSplineUpAxis();
	bool IsMovingForward();

	float GetSplineCarForwardAngle();
	void HandleMaxTurnWithSpline();
	void ApplyDrag();
	UFUNCTION()
	bool IsOutOfBounds();


	// #if WITH_EDITORONLY_DATA
	//TODO debug remove!
	UFUNCTION()
	void SetGameSpeedUp();
	UFUNCTION()
	void SetGameSpeedDown();
	// TODO debug remove end
	// #endif

	//patricles
	UPROPERTY(meta = (AllowPrivateAccess = "true"), EditAnywhere, Category = "Car|ParticleSystem")
		class UParticleSystem* DeathParticleSystem = nullptr;


public:
	static float SignedAngleAxis(FVector v1, FVector v2, FVector axis);
	static float UnsignedAngle(FVector v1, FVector v2);


	bool IsUnderMaxSpeed(bool bBuffer);

	UFUNCTION()
	FVector GetUpVectorFromUnderCar();

	//debug
public:
	UPROPERTY(EditAnywhere, Category = "Car|Debug")
	bool bDebugCarState = false;

	//bp events
	UFUNCTION(BlueprintImplementableEvent)
	void MaxTurnBpEvent(bool bRightSide);
	UFUNCTION(BlueprintImplementableEvent)
	void NotMaxTurnBpEvent();
	UFUNCTION(BlueprintImplementableEvent)
	void HitGroundBpEvent(float Speed);
	UFUNCTION(BlueprintImplementableEvent, meta = (ToolTip = "0 -> 1 max speed"))
	void BPECarSpeed(float Speed);
};
