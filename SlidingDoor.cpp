#include "SlidingDoor.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Engine/StaticMesh.h"

ASlidingDoor::ASlidingDoor()
{
	PrimaryActorTick.bCanEverTick = true;

	// Create the root component.
	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	// Create the frame.
	Frame = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Frame"));
	Frame->SetupAttachment(SceneRoot);

	// Create the left door panel and attach it to the root.
	LeftDoor = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeftDoor"));
	LeftDoor->SetupAttachment(SceneRoot);
	LeftDoor->SetMobility(EComponentMobility::Movable);

	// Create the right door panel and attach it to the root.
	RightDoor = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RightDoor"));
	RightDoor->SetupAttachment(SceneRoot);
	RightDoor->SetMobility(EComponentMobility::Movable);

	// Create the player detection area.
	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	TriggerBox->SetupAttachment(SceneRoot);
	TriggerBox->SetBoxExtent(DetectionSize * 0.5f);
	TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	TriggerBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	TriggerBox->SetGenerateOverlapEvents(true);

}

void ASlidingDoor::BeginPlay()
{
	Super::BeginPlay();

	// Store the initial positions of both door panels.
	LeftDoorClosedLocation = LeftDoor->GetRelativeLocation();
	RightDoorClosedLocation = RightDoor->GetRelativeLocation();
}

void ASlidingDoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const float Step = DeltaTime / FMath::Max(OpenDuration, 0.1f);


	if (bShouldOpen)
	{
		OpenAmount += Step;
	}
	else
	{
		OpenAmount -= Step;
	}

	OpenAmount = FMath::Clamp(OpenAmount, 0.f, 1.f);


	FVector LeftDoorOpenPosition = LeftDoorClosedLocation + FVector(0.f, -SlideDistance, 0.f);
	FVector RightDoorOpenPosition = RightDoorClosedLocation + FVector(0.f, SlideDistance, 0.f);

	LeftDoor->SetRelativeLocation(
		FMath::Lerp(LeftDoorClosedLocation, LeftDoorOpenPosition, OpenAmount)
	);

	RightDoor->SetRelativeLocation(
		FMath::Lerp(RightDoorClosedLocation, RightDoorOpenPosition, OpenAmount)
	);

	// Get the pawn controlled by the first local player.
	APawn* Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

	if (!bOpenOnPlayerOverlap)
	{
		return;
	}

	const bool bPlayerInside =
		Player != nullptr && TriggerBox->IsOverlappingActor(Player);

	if (bPlayerInside)
	{
		// Cancel closing when the player is inside.
		GetWorldTimerManager().ClearTimer(CloseTimerHandle);
		OpenDoor();
	}
	else if (bShouldOpen && !GetWorldTimerManager().IsTimerActive(CloseTimerHandle))
	{
		if (CloseDelay <= 0.f)
		{
			CloseDoor();
		}
		else
		{
			GetWorldTimerManager().SetTimer(
				CloseTimerHandle,
				this,
				&ASlidingDoor::CloseDoor,
				CloseDelay,
				false
			);
		}
	}
}

// OPEN DOOR - set the door to open and play the opening sound effect if specified.
void ASlidingDoor::OpenDoor()
{
	// Ignore repeated requests to open.
	if (bShouldOpen)
	{
		return;
	}

	bShouldOpen = true;

	if (OpenGateSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this, OpenGateSound, GetActorLocation()
		);
	}
}

// CLOSE DOOR - set the door to close and play the closing sound effect if specified.
void ASlidingDoor::CloseDoor()
{

	// Ignore repeated requests to close.
	if (!bShouldOpen)
	{
		return;
	}

	bShouldOpen = false;

	if (CloseGateSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this, CloseGateSound, GetActorLocation()
		);
	}
}

// ON CONSTRUCTION - set the size of the trigger box based on the DetectionSize property.
void ASlidingDoor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	TriggerBox->SetBoxExtent(DetectionSize * 0.5f);
}

// SET DOOR OPEN - set the door to open or close based on the specified boolean value.
void ASlidingDoor::SetDoorOpen(bool bOpen)
{
	if (bOpen)
	{
		OpenDoor();
	}
	else
	{
		CloseDoor();
	}
}