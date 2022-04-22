// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CameraEffecttComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RACINGUNREALPROJECT_API UCameraEffecttComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCameraEffecttComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	//My deseg ---------------
private:
	UPROPERTY()
		class UCameraComponent* CameraCurrent = nullptr;
	UPROPERTY()
		float StartFOV = 0.f;
	UPROPERTY()
		class ACarPawn* CarPawn = nullptr;

	//single run cameraModifier
	UPROPERTY(meta = (AllowPrivateAccess = "true"),EditAnywhere, Category = "Camera|Boost")
		TSubclassOf<class UCameraModifier> BoostCameraModifierClass = nullptr;

	
	UPROPERTY(meta = (AllowPrivateAccess = "true"),EditAnywhere, Category = "Camera|Speed")
		TSubclassOf<class UCameraShakeBase> SpeedCameraShakeClass = nullptr;
	UPROPERTY(meta = (AllowPrivateAccess = "true", ToolTip = "Higher means less shake"),EditAnywhere, Category = "Camera|Speed")
		float SpeedShakeInverseIntensity = 10000.f;
	UPROPERTY()
		class UCameraShakeBase* SpeedCameraShake = nullptr;
public:
	UPROPERTY()
		class UCameraModifier* BoostCameraModifier = nullptr;

	
	UFUNCTION()
	void SetCameraCurrent(class UCameraComponent* NewCamera){CameraCurrent = NewCamera; }

	UPROPERTY()
		bool bFirstCamera = true; 
	UFUNCTION()
	void SpaceCamera();

private:
	UFUNCTION() // this is very bad practice, but we are short on time so this was the only solution i figured out
		void LateBeginPlay();
};
