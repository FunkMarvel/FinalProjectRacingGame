// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "GrappleSphereComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGrappleEvent, FTransform, SphereCompTransfrom);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnReachedEvent, float, AddSpeedAmount);

/**
 * 
 */
UCLASS()
class RACINGUNREALPROJECT_API UGrappleSphereComponent : public USphereComponent
{
	GENERATED_BODY()
public:
	UGrappleSphereComponent();
	//My deseg
	// how to use:
	// On Reached and OnHooked events are handled by the car pawn(Physics Grapple Component) so dont worry about them
	// link your logic to these events using .AddDynamic for optimal usage
	// is bEnabled is false, Hookshot wont home of hit
	// SetEnabled when you want the GrappleSphereComponent to be Grappleble
	// set bEatable is IT should be dragged to the player, false if the player -> to GrappleSphereComponent
	
	//these are called by the Physics grapple component
	UPROPERTY(BlueprintAssignable , Category = "Event")
		FOnGrappleEvent OnGrappleHitEvent;
	UPROPERTY(BlueprintAssignable ,Category = "Event")
		FOnReachedEvent OnReachedEvent;

	
	UFUNCTION()
		void OnGrapple();
	UFUNCTION()
		void OnReached();

	UFUNCTION()
		float GetSpeed() const {return AddSpeed; }
	
	UFUNCTION()
		bool IsOverrideReleaseVelocity() const { return bOverrideReleaseVelocity; }
	
	UFUNCTION()
		void SetIsEatable(bool _bEnable) {bEatable = _bEnable; }
	UFUNCTION()
		bool IsEatable() const {return bEatable; }

	UFUNCTION()
	/**
	 * @brief Set the GrappleSphereComponent to be grappable
	 * @param _bEnable  
	 */
	void SetIsEnabled(bool _bEnable) {bEnabled = _bEnable; }

	UFUNCTION()
	/**
	 * @brief is the grappaple ready to be grappled?
	 * @return 
	 */
	bool IsEnabled() const {return bEnabled; }

	// speed
	UFUNCTION()
		bool IsOverrideSpeed() const { return bOverrideSpeed; }
	UFUNCTION()
		void SetOverrideSpeed(bool bOverride){ bOverrideSpeed = bOverride; }
	
private:
	UPROPERTY(EditAnywhere, Category = "Grappable")
	float AddSpeed = 550.f;
	UPROPERTY(EditAnywhere, Category = "Grappable")
	bool bEatable = false;
	UPROPERTY(EditAnywhere, Category = "Grappable")
	bool bOverrideReleaseVelocity = true;
	UPROPERTY(EditAnywhere, Category = "Grappable")
	bool bOverrideSpeed = false;
	UPROPERTY(EditAnywhere, Category = "Grappable")
	bool bEnabled = false;

	//UI
	UPROPERTY(EditAnywhere, Category = "Grappable|UI")
	bool bUseWidget = true;
	
	
};
