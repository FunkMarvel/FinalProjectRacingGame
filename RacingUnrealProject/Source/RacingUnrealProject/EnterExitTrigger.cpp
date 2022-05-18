// Fill out your copyright notice in the Description page of Project Settings.


#include "EnterExitTrigger.h"

#include "CarPawn.h"
#include "DebugLog.h"
#include "Components/BoxComponent.h"

// Sets default values
AEnterExitTrigger::AEnterExitTrigger()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;


	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	SetRootComponent(TriggerBox);
	TriggerBox->SetBoxExtent(FVector(2400.f, 600.f, 1200.f));
	TriggerBox->ShapeColor = FColor::Red;
	TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	TriggerBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
}

// Called when the game starts or when spawned
void AEnterExitTrigger::BeginPlay()
{
	Super::BeginPlay();
	
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AEnterExitTrigger::OnOverlap);
	
}

// Called every frame
void AEnterExitTrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEnterExitTrigger::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bCooldown == false && OtherActor->IsA(ACarPawn::StaticClass())) {
		EventTriggerEnterExit.Broadcast();

		// cooldown lambda / callback
		bCooldown = true;
		FTimerHandle Handle;
		FTimerDelegate Callback;
		Callback.BindLambda([this]
		{
			bCooldown = false;
		});
		GetWorld()->GetTimerManager().SetTimer(Handle, Callback, 4.f, false);
	}
	
}

