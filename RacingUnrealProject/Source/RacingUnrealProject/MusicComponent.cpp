// Fill out your copyright notice in the Description page of Project Settings.


#include "MusicComponent.h"

#include "AudioDevice.h"
#include "DebugLog.h"
#include "Components/AudioComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
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
		UGameplayStatics::SpawnSoundAttached(BaseEngine1, GetOwner()->GetRootComponent());
	}
	
	if (BaseEngine2){
		UGameplayStatics::SpawnSoundAttached(BaseEngine2, GetOwner()->GetRootComponent());
		BaseEngine2->VolumeMultiplier = 0.f;
	}

	CarPawn->EnterNewStateEvent.AddDynamic(this, &UMusicComponent::SharkBodyEnterNewState);

	
}


// Called every frame
void UMusicComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	float adjustedSpeed = CarPawn->GetCurrentForwardSpeed() / CarPawn->GetMaxSpeed();
	adjustedSpeed *= 0.7f;

	LerpEngineSpeed = UKismetMathLibrary::Lerp(LerpEngineSpeed, adjustedSpeed, GetWorld()->GetDeltaSeconds());

	// Base engine 1, scifi sound
	BaseEngine1->PitchMultiplier = LerpEngineSpeed;
	
	// BaseEngine1->VolumeMultiplier = (1.f-adjustedSpeed * 0.6f);

	//base engien two (real engine)
	BaseEngine2->VolumeMultiplier = LerpEngineSpeed * 0.2f;
	
}

void UMusicComponent::PlaySpeedBoostSound() const {
	if (SpeedBoostSound) {
		UGameplayStatics::SpawnSoundAttached(SpeedBoostSound, GetOwner()->GetRootComponent());
	}
}

void UMusicComponent::SharkBodyEnterNewState(EVehicleState NewState) {
	if (NewState == EVehicleState::Grappling) {
		LerpEngineSpeed = 2.f;
		PlaySpeedBoostSound();
	}
}

