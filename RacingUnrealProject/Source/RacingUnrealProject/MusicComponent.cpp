// Fill out your copyright notice in the Description page of Project Settings.


#include "MusicComponent.h"

#include "Kismet/GameplayStatics.h"

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

	if (MusicToUse)
	{
		UGameplayStatics::SpawnSoundAttached(MusicToUse, GetOwner()->GetRootComponent());
		
	}
	
}


// Called every frame
void UMusicComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

