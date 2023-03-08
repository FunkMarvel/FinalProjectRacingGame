// Fill out your copyright notice in the Description page of Project Settings.
// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseEnemyActor.h"

#include "../CarPawn.h"
#include "../GravitySplineActor.h"
#include "Components/ArrowComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"

/**
 * @brief Parent class for drones and spiky ball enemies.
 */
ABaseEnemyActor::ABaseEnemyActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// collider:
	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SetRootComponent(SphereComp);
	SphereComp->SetSimulatePhysics(false);
	SphereComp->SetEnableGravity(false);
	SphereComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	SphereComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	SphereComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Block);

	// cosmetic mesh:
	CosmeticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	CosmeticMesh->SetupAttachment(GetRootComponent());
	CosmeticMesh->SetSimulatePhysics(false);
	CosmeticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// arrow
	ArrowRayCastStart = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow RayCastStart"));
	ArrowRayCastStart->SetupAttachment(GetRootComponent());
}

/**
 * @brief Switches gravity spline if entering new area.
 */
void ABaseEnemyActor::OnBeginOverLap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                     UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA<AGravitySplineActor>())
	{
		GravitySplineActive = Cast<AGravitySplineActor>(OtherActor);
	}
}

/**
 * @brief Get vector pointing from enemy to player.
 * @param bNormalized if true returns unit vector, if false returns difference vector.
 * @return FVector pointing from enemy to player.
 */
FVector ABaseEnemyActor::GetToPlayerVector(bool bNormalized)
{
	FVector ToPlayer{PlayerPawn->GetActorLocation() - GetActorLocation()};
	if (bNormalized)
	{
		return ToPlayer.GetSafeNormal();
	}
	return ToPlayer;
}

// Called when the game starts or when spawned
void ABaseEnemyActor::BeginPlay()
{
	Super::BeginPlay();
	GravitySplineActive = GetClosestGravitySpline();
	GameModeBase = Cast<ARacingUnrealProjectGameModeBase>(GetWorld()->GetAuthGameMode());
}

/**
 * @brief Gets closest gravity spline to enemy.
 * @return pointer to AGravitySplineActor closest to enemy.
 */
AGravitySplineActor* ABaseEnemyActor::GetClosestGravitySpline()
{
	TArray<AActor*> GravitySplines;
	if (GetWorld())
	{
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGravitySplineActor::StaticClass(), GravitySplines);
	}
	float MinDistance{};
	int32 MinIndex{};
	for (int32 i{}; i < GravitySplines.Num(); i++)
	{
		float TempDistance{};
		TempDistance = static_cast<float>(FVector::Distance(GravitySplines[i]->GetActorLocation(), GetActorLocation()));
		if (i == 0)
		{
			MinDistance = TempDistance;
		}
		else if (MinDistance > TempDistance)
		{
			MinDistance = TempDistance;
			MinIndex = i;
		}
	}
	return Cast<AGravitySplineActor>(GravitySplines[MinIndex]);
}

/**
 * @brief Checks if enemy is on the ground.
 * @return true if on ground, false if not on ground.
 */
bool ABaseEnemyActor::IsGrounded()
{
		FCollisionQueryParams TraceParams(FName(TEXT("")), false, GetOwner());
		if (GravitySplineActive) LocalUpVector = GravitySplineActive->GetFixedUpVectorFromLocation(SphereComp->GetComponentLocation());

		FHitResult Hit{};
		GetWorld()->LineTraceSingleByObjectType(
			Hit,
			ArrowRayCastStart->GetComponentLocation(),
			ArrowRayCastStart->GetComponentLocation() - GravitySplineActive->GetFixedUpVectorFromLocation(GetActorLocation()) *1.25* SphereComp->GetScaledSphereRadius(),
			FCollisionObjectQueryParams(ECollisionChannel::ECC_WorldStatic),
			TraceParams
		);
		if (Hit.IsValidBlockingHit()) {
			// DL_NORMAL("Hit Ground");
			return true;
		}
			// DL_NORMAL("Not grounded");
	return false;
}

// Called every frame
void ABaseEnemyActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!PlayerPawn) PlayerPawn = Cast<ACarPawn>(GetWorld()->GetFirstPlayerController()->GetPawn());
	
}

/**
 * @brief Handles death and destruction of enemy.
 */
void ABaseEnemyActor::HandleDeath()
{
	if (EnemyDeath.IsBound()) EnemyDeath.Broadcast(EnemyArrayIndex);
	if (GameModeBase) GameModeBase->AddScore(ScoreValue);
}

