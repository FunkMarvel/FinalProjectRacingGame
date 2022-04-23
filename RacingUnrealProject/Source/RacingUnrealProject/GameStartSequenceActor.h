// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameStartSequenceActor.generated.h"
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FStartSequenceFinishedEvent);

UCLASS()
class RACINGUNREALPROJECT_API AGameStartSequenceActor : public APawn
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGameStartSequenceActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// my deseg -------------------------
private:
	UPROPERTY()
	class ACarPawn* CarPawn = nullptr;
	UPROPERTY()
	class APlayerController* PlayerController = nullptr;
	
	
public:
	/*enum StartSequenceState
	{
		Inactive,
		Showoff,
		BlendToPlayer,
		Delete
	};
	
	void ChangeState(StartSequenceState NewState);*/
private:
	// StartSequenceState CurrentState = Showoff;
	/*UPROPERTY()
		bool bEnterState = false;
	UPROPERTY()
	float CurrentStateTime = 0.f;*/
	

	//camera stuff
	UPROPERTY(meta = (AllowPrivateAccess = "true"), EditAnywhere, Category = "Camera")
		class UCameraComponent* ShowoffCamema = nullptr;
	UPROPERTY(meta = (AllowPrivateAccess = "true"), EditAnywhere, Category = "Camera")
		class UArrowComponent* ArrowComponent = nullptr;
	UPROPERTY(meta = (AllowPrivateAccess = "true"), EditAnywhere, Category = "Camera")
		class USpringArmComponent* CameraSpringArm = nullptr;
	
	UPROPERTY(meta = (AllowPrivateAccess = "true"), EditAnywhere, Category = "Camera")
		class UCurveFloat* ShowoffYawCurve = nullptr;
	UPROPERTY(meta = (AllowPrivateAccess = "true"), EditAnywhere, Category = "Camera")
		class UGrappableWidget* CountDownWidget = nullptr;
	
	UPROPERTY()
	float CurrentShowOffTime = 0.f;
	UPROPERTY(meta = (AllowPrivateAccess = "true"), EditAnywhere, Category = "Camera")
		bool bSkipSequence = false;
	

	FTimerHandle ShowOffTimerHandle;
	FTimerHandle BlendFinishedTimerHandle;
	UFUNCTION()
		void OnShowoffFinshed();
	UFUNCTION()
		void BlendFinished();
	UFUNCTION()
		void Skip();

	//events
public:
	FStartSequenceFinishedEvent StartSequenceFinishedEvent;

};


