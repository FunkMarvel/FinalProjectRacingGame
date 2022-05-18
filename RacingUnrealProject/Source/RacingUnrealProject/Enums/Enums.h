// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Enums.generated.h"

UENUM(BlueprintType)
enum class EVehicleState : uint8{
	Driving				UMETA(DisplayName = "EDriving"),
	Grappling			UMETA(DisplayName = "EGrappling"),
	AirBorne			UMETA(DisplayName = "EAirBorne"),
	Dying				UMETA(DisplayName = "EDying"),
	Finished			UMETA(DisplayName = "EFi")
	
};

UENUM(BlueprintType)
enum class EGrappleStates : uint8{
	InActive				UMETA(DisplayName = "EInActive"),
	Traveling				UMETA(DisplayName = "ETraveling"),
	Knockoff				UMETA(DisplayName = "EKnockoff"),
	Hooked					UMETA(DisplayName = "EHooked"),
	HookedEatable			UMETA(DisplayName = "EHookedEatable"),
	Returning				UMETA(DisplayName = "EReturning")
};

//enemies
UENUM(BlueprintType)
enum class ESplineWormHeadAxis : uint8{
	Right				UMETA(DisplayName = "ERight"),
	Left				UMETA(DisplayName = "ELeft"),
	Up					UMETA(DisplayName = "EUp"),
	Down				UMETA(DisplayName = "EDown")
};