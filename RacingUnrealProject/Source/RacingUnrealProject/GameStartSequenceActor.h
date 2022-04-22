// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameStartSequenceActor.generated.h"

UCLASS()
class RACINGUNREALPROJECT_API AGameStartSequenceActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGameStartSequenceActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// my deseg -------------------------
private:
	UPROPERTY()
	class ACarPawn* CarPawn = nullptr;
	

	//camera stuff
	UPROPERTY(meta = (AllowPrivateAccess = "true"), EditAnywhere, Category = "Camera")
		class UCameraComponent* ShowoffCamema = nullptr;
	UPROPERTY(meta = (AllowPrivateAccess = "true"), EditAnywhere, Category = "Camera")
		class UArrowComponent* ArrowComponent = nullptr;
	UPROPERTY(meta = (AllowPrivateAccess = "true"), EditAnywhere, Category = "Camera")
		class USpringArmComponent* CameraSpringArm = nullptr;
	UPROPERTY(meta = (AllowPrivateAccess = "true"), EditAnywhere, Category = "Camera")
		class UCurveFloat* ShowoffYawCurve = nullptr;
	UPROPERTY()
		float CurrentAnimationTime = 0.f;

	UFUNCTION()
		void OnShowoffFinshed();
};


