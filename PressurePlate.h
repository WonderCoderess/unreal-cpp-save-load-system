#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PressurePlate.generated.h"

class UPrimitiveComponent;
class UStaticMeshComponent;
class UBoxComponent;
class ASlidingDoor;

struct FPressurePlateSaveData;

UCLASS()
class ASOULINTHREE_API APressurePlate : public AActor
{
	GENERATED_BODY()
	
public:	
	APressurePlate();

	FName GetSaveId() const { return SaveId; }

	bool CaptureSaveData(FPressurePlateSaveData& OutData) const;

	bool RestoreSaveData(const FPressurePlateSaveData& SaveData);

protected:
	virtual void BeginPlay() override;

private:
	// COMPONENTS - PlateMesh is the visual representation of the pressure plate, TriggerBox is the collision box that detects when the required weight is placed on the plate

	UPROPERTY(VisibleAnywhere, Category = "Pressure Plate|Components")
	TObjectPtr<UStaticMeshComponent> PlateMesh;

	UPROPERTY(VisibleAnywhere, Category = "Pressure Plate|Components")
	TObjectPtr<UBoxComponent> TriggerBox;

	//LINKS - LinkedDoor is the sliding door that this pressure plate controls, RequiredWeight is the actor that must be placed on the pressure plate to activate it
	UPROPERTY(EditInstanceOnly, Category = "Pressure Plate|Links")
	TObjectPtr<ASlidingDoor> LinkedDoor;

	UPROPERTY(EditInstanceOnly, Category = "Pressure Plate|Links")
	TObjectPtr<AActor> RequiredWeight;

	// REFRESH DOOR STATE - check if the required weight is on the pressure plate and update the door state accordingly
	void RefreshDoorState();

	// SAVE ID - unique identifier for saving the state of the pressure plate
	UPROPERTY(EditInstanceOnly, Category = "Pressure Plate|Save")
	FName SaveId = NAME_None;

	// HANDLE WEIGHT BEGIN OVERLAP - check if the required weight is placed on the pressure plate and update the door state accordingly
	UFUNCTION()
	void HandleWeightBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	// HANDLE WEIGHT END OVERLAP - check if the weight is removed from the pressure plate and update the door state accordingly
	UFUNCTION()
	void HandleWeightEndOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);
};