// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CarPawn.h"
#include "Components/ActorComponent.h"
#include "MusicComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RACINGUNREALPROJECT_API UMusicComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMusicComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	//mmy desegg----
	UFUNCTION()
		void PlaySpeedBoostSound() const;

	
private:
	UPROPERTY()
		ACarPawn* CarPawn = nullptr;

	
	UPROPERTY(EditAnywhere, meta =(AllowPrivateAccess = "true"), Category = "Music")
		class USoundBase* MusicToUse = nullptr;
	UPROPERTY(EditAnywhere, meta =(AllowPrivateAccess = "true"), Category = "SoundEffects")
		class USoundCue* BaseEngine1 = nullptr;
    UPROPERTY(EditAnywhere, meta =(AllowPrivateAccess = "true"), Category = "SoundEffects")
            class USoundCue* BaseEngine2 = nullptr;
	UPROPERTY(EditAnywhere, meta =(AllowPrivateAccess = "true"), Category = "SoundEffects")
		class USoundBase* SpeedBoostSound = nullptr;


	UPROPERTY()
	float LerpEngineSpeed = 0.f;

	UFUNCTION()
		void SharkBodyEnterNewState(EVehicleState NewState);
	/*UPROPERTY()
		class UAudioComponent* BaseEngine1AudioComponent = nullptr;*/
};
