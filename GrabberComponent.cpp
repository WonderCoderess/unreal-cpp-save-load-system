#include "GrabberComponent.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "Components/PrimitiveComponent.h"

UGrabberComponent::UGrabberComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UGrabberComponent::BeginPlay()
{
	Super::BeginPlay();
	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();

}

void UGrabberComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateHeldObject();
}

//GRAB - attempt to grab an object in front of the player within the specified grab distance and radius.
void UGrabberComponent::Grab() {

	// Check if the PhysicsHandle is valid and if it is not already holding an object.
	if (!IsValid(PhysicsHandle) || PhysicsHandle->GetGrabbedComponent())
	{
		return;
	}

	AActor* Owner = GetOwner();

	if (!IsValid(Owner))
	{
		return;
	}

	FVector ViewLocation;
	FRotator ViewRotation;

	Owner->GetActorEyesViewPoint(ViewLocation, ViewRotation);

	// Convert the view rotation to a direction vector.
	const FVector ViewDirection = ViewRotation.Vector();

	// Calculate the end of the search trace.
	const FVector TraceEnd = ViewLocation + ViewDirection * GrabDistance;

	FHitResult Hit;

	// Ignore the character that owns this component.
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(Owner);

	const bool bHit = GetWorld()->SweepSingleByChannel(
		Hit,
		ViewLocation,
		TraceEnd,
		FQuat::Identity, // No rotation for the sphere.
		ECC_Visibility,
		FCollisionShape::MakeSphere(GrabRadius),
		QueryParams
	);

	if (!bHit)
	{
		return;
	}

	UPrimitiveComponent* HitComponent = Hit.GetComponent();

	if (!IsValid(HitComponent) || !HitComponent->IsSimulatingPhysics())
	{
		return;
	}

	PhysicsHandle->GrabComponentAtLocationWithRotation(
		HitComponent,
		NAME_None,
		Hit.ImpactPoint,
		HitComponent->GetComponentRotation()
	);

}

//RELEASE - release the held object if there is one.
void UGrabberComponent::Release() {
	if (!IsValid(PhysicsHandle))
	{
		return;
	}
	PhysicsHandle->ReleaseComponent();
}

//UPDATE HELD OBJECT - update the position of the held object based on the player's view direction and hold distance.
void UGrabberComponent::UpdateHeldObject()
{
	if (!IsValid(PhysicsHandle) || !PhysicsHandle->GetGrabbedComponent())
	{
		return;
	}

	AActor* Owner = GetOwner();

	if (!IsValid(Owner))
	{
		return;
	}

	FVector ViewLocation;
	FRotator ViewRotation;
	Owner->GetActorEyesViewPoint(ViewLocation, ViewRotation);

	const FVector HoldLocation =
		ViewLocation + ViewRotation.Vector() * HoldDistance;

	PhysicsHandle->SetTargetLocation(HoldLocation);
}

//TOGGLE GRAB - toggle between grabbing and releasing an object based on the current state of the PhysicsHandle.
void UGrabberComponent::ToggleGrab()
{
	if (!IsValid(PhysicsHandle))
	{
		return;
	}

	if (PhysicsHandle->GetGrabbedComponent())
	{
		Release();
	}
	else
	{
		Grab();
	}
}