// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnterExitTrigger.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTriggerEnterExit);

UCLASS()
class RACINGUNREALPROJECT_API AEnterExitTrigger : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnterExitTrigger();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	// my deseg
public:
    UPROPERTY(EditAnywhere, Category = "EnterExit")
        class UBoxComponent* TriggerBox = nullptr;

	FTriggerEnterExit EventTriggerEnterExit;

	UFUNCTION()
		void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

private:
	UPROPERTY()
		bool bCooldown = false;
};
