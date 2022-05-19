// Fill out your copyright notice in the Description page of Project Settings.


#include "MusicComponent.h"

#include "AudioDevice.h"
#include "DebugLog.h"
#include "Components/AudioComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

// Sets default values for this component's properties
UMusicComponent::UMusicComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UMusicComponent::BeginPlay()
{
	Super::BeginPlay();

	CarPawn = Cast<ACarPawn>( GetOwner());
	
	if (MusicToUse)
	{
		UGameplayStatics::SpawnSoundAttached(MusicToUse, GetOwner()->GetRootComponent());
	}

	if (BaseEngine1) {
		/*BaseEngine1AudioComponent = */UGameplayStatics::SpawnSoundAttached(BaseEngine1, GetOwner()->GetRootComponent());
	}

	
}


// Called every frame
void UMusicComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	float adjustedSpeed = CarPawn->GetCurrentForwardSpeed() / CarPawn->GetMaxSpeed();
	adjustedSpeed *= 0.7f;
	
	BaseEngine1->PitchMultiplier = adjustedSpeed;
	
}

void UMusicComponent::PlaySpeedBoostSound() const {
	if (SpeedBoostSound) {
		UGameplayStatics::SpawnSoundAttached(SpeedBoostSound, GetOwner()->GetRootComponent());
	}
}

