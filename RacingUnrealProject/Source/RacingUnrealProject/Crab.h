// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Crab.generated.h"

UCLASS()
class RACINGUNREALPROJECT_API ACrab : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACrab();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
    //my deseg ---------------------
    UPROPERTY(EditAnywhere, Category = "Crab")
        class USplineComponent* Spline = nullptr;
	UPROPERTY(EditAnywhere, Category = "Crab")
		class USkeletalMeshComponent* CrabMesh = nullptr;

	UPROPERTY(EditAnywhere, Category = "Crab")
		float MoveSpeed = 60.f;


private:
	UPROPERTY()
	float CurrentDistance = 0.f;
};
