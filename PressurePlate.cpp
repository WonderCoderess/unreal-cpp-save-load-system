#include "PressurePlate.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "SlidingDoor.h"
#include "SoulSaveGame.h"
#include "Components/PrimitiveComponent.h"

APressurePlate::APressurePlate()
{
	PrimaryActorTick.bCanEverTick = false;

	PlateMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlateMesh"));
	SetRootComponent(PlateMesh);

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	TriggerBox->SetupAttachment(PlateMesh);
	TriggerBox->SetBoxExtent(FVector(50.0f, 50.0f, 20.0f));
	TriggerBox->SetRelativeLocation(FVector(0.0f, 0.0f, 20.0f));
	TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	TriggerBox->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Overlap);
	TriggerBox->SetGenerateOverlapEvents(true);
}

void APressurePlate::BeginPlay()
{
	Super::BeginPlay();

	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &APressurePlate::HandleWeightBeginOverlap);
	TriggerBox->OnComponentEndOverlap.AddDynamic(this, &APressurePlate::HandleWeightEndOverlap);

	RefreshDoorState();

}

// REFRESH DOOR STATE - check if the required weight is on the pressure plate and update the door state accordingly
void APressurePlate::RefreshDoorState()
{
	if (!IsValid(LinkedDoor) || !IsValid(RequiredWeight))
	{
		return;
	}

	const bool bWeightOnPlate =
		TriggerBox->IsOverlappingActor(RequiredWeight);

	LinkedDoor->SetDoorOpen(bWeightOnPlate);
}

// HANDEL WEIGHT BEGIN OVERLAP - check if the required weight is placed on the pressure plate and update the door state accordingly
void APressurePlate::HandleWeightBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == RequiredWeight)
	{
		RefreshDoorState();
	}
}

// HANDLE WEIGHT END OVERLAP - check if the weight is removed from the pressure plate and update the door state accordingly
void APressurePlate::HandleWeightEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor == RequiredWeight)
	{
		RefreshDoorState();
	}
}

// RESTORE SAVE DATA - restore the state of the pressure plate from the provided save data
bool APressurePlate::RestoreSaveData(const FPressurePlateSaveData& SaveData)
{
	if (!IsValid(RequiredWeight))
	{
		return false;
	}

	const bool bRestored = RequiredWeight->SetActorTransform(
		SaveData.WeightTransform,
		false,
		nullptr,
		ETeleportType::TeleportPhysics
	);

	if (!bRestored)
	{
		return false;
	}

	UPrimitiveComponent* WeightRoot =
		Cast<UPrimitiveComponent>(RequiredWeight->GetRootComponent());

	if (WeightRoot)
	{
		if (WeightRoot->IsSimulatingPhysics())
		{
			WeightRoot->SetPhysicsLinearVelocity(FVector::ZeroVector);
			WeightRoot->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
		}

		WeightRoot->UpdateOverlaps();
	}

	TriggerBox->UpdateOverlaps();

	if (IsValid(LinkedDoor))
	{
		const bool bWeightOnPlate =
			TriggerBox->IsOverlappingActor(RequiredWeight);

		LinkedDoor->SetDoorOpenInstant(bWeightOnPlate);
	}

	return true;
}

// CAPTURE SAVE DATA - capture the state of the pressure plate and store it in the provided save data
bool APressurePlate::CaptureSaveData(FPressurePlateSaveData& OutData) const
{
	if (!IsValid(RequiredWeight))
	{
		return false;
	}

	OutData.WeightTransform = RequiredWeight->GetActorTransform();
	return true;
}



