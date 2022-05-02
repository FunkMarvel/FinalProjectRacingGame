// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GrappleTarget.generated.h"

UCLASS()
class RACINGUNREALPROJECT_API AGrappleTarget : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGrappleTarget();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
// my deseg
private:
	UPROPERTY(meta = (AllowPrivateAccess = "true"), EditAnywhere, Category = "GrappleTarget")
		class UArrowComponent* RootArrow = nullptr;
	UPROPERTY(meta = (AllowPrivateAccess = "true"), EditAnywhere, Category = "GrappleTarget")
		class UGrappleSphereComponent* GrappleSphereComponent = nullptr;
	UPROPERTY(meta = (AllowPrivateAccess = "true"), EditAnywhere, Category = "GrappleTarget")
		class USkeletalMeshComponent* MainMesh = nullptr;
	
	
	
public:
	// grapplesphere functions
	UFUNCTION()
	void OnReachedTarget(float AddSpeedAmount);
	UFUNCTION()
	void OnGrappleTarget(FTransform SphereCompTransfrom);

	
	//enter exit relevant
	UPROPERTY(EditAnywhere, Category = "GrappleTarget")
		class AEnterExitTrigger* EnterTrigger = nullptr;
	UPROPERTY(EditAnywhere, Category = "GrappleTarget")
		class AEnterExitTrigger* ExitTrigger = nullptr;

	UFUNCTION(BlueprintCallable)
		void VisualizeTriggers();

	UFUNCTION()
		void OnEnterTrigger();
	UFUNCTION()
		void OnExitTrigger();
		
};
