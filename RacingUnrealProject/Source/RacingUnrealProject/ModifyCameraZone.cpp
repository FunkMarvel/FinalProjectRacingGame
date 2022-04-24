// Fill out your copyright notice in the Description page of Project Settings.


#include "ModifyCameraZone.h"

#include "CarPawn.h"
#include "Components/BoxComponent.h"
#include "GameFramework/SpringArmComponent.h"

// Sets default values
AModifyCameraZone::AModifyCameraZone()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxTrigger"));
	SetRootComponent(BoxComponent);
	BoxComponent->SetBoxExtent(FVector(800.f, 200.f, 600.f));
	BoxComponent->ShapeColor = FColor::Green;

}

// Called when the game starts or when spawned
void AModifyCameraZone::BeginPlay()
{
	Super::BeginPlay();

	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AModifyCameraZone::OnOverLap);
	BoxComponent->OnComponentEndOverlap.AddDynamic(this, &AModifyCameraZone::OnEndOverlap);
	
}

// Called every frame
void AModifyCameraZone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CarPawn)
	{
		FRotator NewRot = FMath::RInterpTo(CarPawn->CameraBoom->GetRelativeRotation(),
			FRotator(PitchAngleTarget,1.f,1.f),
			GetWorld()->GetDeltaSeconds(), 50.f);
		CarPawn->CameraBoom->SetRelativeRotation(NewRot);
	}	
}

void AModifyCameraZone::OnOverLap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA(ACarPawn::StaticClass()))
	{
		CarPawn = Cast<ACarPawn>(OtherActor);
		
		if (bAddative)
		{
			CarPawn->SetTargetCameraBoomLength(CarPawn->GetStartCameraBoomLength() + CameraBoomLength);
		}
		else
		{
			CarPawn->SetTargetCameraBoomLength(CameraBoomLength);
		}

		
	}
	
}

void AModifyCameraZone::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->IsA(ACarPawn::StaticClass()))
	{
		//resets to origninal cameraBoomLength
		CarPawn->SetTargetCameraBoomLength(CarPawn->GetStartCameraBoomLength());
		CarPawn = nullptr;
	}
}

