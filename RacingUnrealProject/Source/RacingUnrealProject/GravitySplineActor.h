// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GravitySplineAxis.h"
#include "GravitySplineActor.generated.h"

//UENUM()
//enum class EGravitySplineAxis : uint8 {
//	Axis_X,
//	Axis_Y,
//	Axis_Z,
//	Axis_W
//};

// Hjemmelekse � finne ut syntaks :))))
//#define true = (FMath::RandRange(0,100) < 99);
 
UCLASS()
class RACINGUNREALPROJECT_API AGravitySplineActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGravitySplineActor();
	/*UPROPERTY(EditAnywhere, Category = BlueprintOptions)
		uint32 bRunConstructionScriptOnDrag : 1;*/

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// my deseg --------
public:
	

	UPROPERTY(EditAnywhere)
		class UArrowComponent* ArrowComp = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class USplineComponent* SplineComp = nullptr;
	

	UPROPERTY(EditAnywhere)
		AActor* PlayerActor = nullptr;


	UFUNCTION()
		int32 GetClosestSplinePoint(class USplineComponent* splineEval, FVector Location);
	UFUNCTION(BlueprintCallable)
		virtual void VisualiseUpVectors(int Segments, USplineComponent* SplineToUse, float UpVectorLength);


	// spline up vec
	// UFUNCTION(BlueprintCallable)
	// 	FVector GetAdjustedUpVectorFromLocation(FVector Loc);
	UFUNCTION(BlueprintCallable)
		virtual FVector GetFixedUpVectorFromLocation(FVector OrgPos);
	

	/*UPROPERTY(BlueprintReadWrite, EditAnywhere)
		EGravitySplineAxis EUpVectorAxis;*/



	UPROPERTY(EditAnywhere)
		class UBoxComponent* BoxSplineOne = nullptr;
	UPROPERTY(EditAnywhere)
		class UBoxComponent* BoxSplineTwo = nullptr;

	UFUNCTION()
		float GetMaxDistance() const {return MaxDistance; }

private:
	UPROPERTY(meta = (AllowPrivateAccess = "true"), EditAnywhere, Category = "Gravity")
		float MaxDistance = 6000.f;
};
