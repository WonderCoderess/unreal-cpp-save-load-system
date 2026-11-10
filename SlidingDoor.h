#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SlidingDoor.generated.h"

class USceneComponent;
class UStaticMeshComponent;
class UBoxComponent;
class USoundBase;

UCLASS()
class ASOULINTHREE_API ASlidingDoor : public AActor
{
	GENERATED_BODY()

public:
	ASlidingDoor();
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Gate")
	void SetDoorOpen(bool bOpen);

	void SetDoorOpenInstant(bool bOpen);

protected:
	virtual void BeginPlay() override;

	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void PostInitializeComponents() override;

private:
	// GATE COMPONENTS - Root, frame, sliding panels, and player detection box.
	UPROPERTY(VisibleAnywhere, Category = "Gate|Components")
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleAnywhere, Category = "Gate|Components")
	TObjectPtr<UStaticMeshComponent> Frame;

	UPROPERTY(VisibleAnywhere, Category = "Gate|Components")
	TObjectPtr<UStaticMeshComponent> LeftDoor;

	UPROPERTY(VisibleAnywhere, Category = "Gate|Components")
	TObjectPtr<UStaticMeshComponent> RightDoor;

	UPROPERTY(VisibleAnywhere, Category = "Gate|Components")
	TObjectPtr<UBoxComponent> TriggerBox;

	// GATE SETTINGS - Detection area, slide distance, movement duration, closing delay, and player-triggered opening.
	UPROPERTY(EditAnywhere, Category = "Gate|Settings", meta = (ClampMin = "0.0", Units = "cm"))
	FVector DetectionSize = FVector(200.f, 200.f, 200.f);

	UPROPERTY(EditAnywhere, Category = "Gate|Settings", meta = (ClampMin = "0.0", Units = "cm"))
	float SlideDistance = 100.f;

	UPROPERTY(EditAnywhere, Category = "Gate|Settings", meta = (ClampMin = "0.1", Units = "s"))
	float OpenDuration = 1.f;

	UPROPERTY(EditAnywhere, Category = "Gate|Settings", meta = (ClampMin = "0.0", Units = "s"))
	float CloseDelay = 2.f;

	UPROPERTY(EditAnywhere, Category = "Gate|Settings")
	bool bOpenOnPlayerOverlap = true;

	// Gate sound effects
	UPROPERTY(EditAnywhere, Category = "Gate|Sound Effects")
	TObjectPtr<USoundBase> OpenGateSound = nullptr;

	UPROPERTY(EditAnywhere, Category = "Gate|Sound Effects")
	TObjectPtr<USoundBase> CloseGateSound = nullptr;

	// Door panel positions when the gate is closed.
	FVector LeftDoorClosedLocation = FVector::ZeroVector;
	FVector RightDoorClosedLocation = FVector::ZeroVector;

	// Whether the gate should move towards the open position.
	bool bShouldOpen = false;

	// The current amount the gate is open
	float OpenAmount = 0.0f;

	// Timer handle for scheduling the gate to close after a delay.
	FTimerHandle CloseTimerHandle;

	// Open the gate and play the open sound effect if specified.
	void OpenDoor();
	void CloseDoor();
};
