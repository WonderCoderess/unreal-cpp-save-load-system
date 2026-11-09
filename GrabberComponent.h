#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GrabberComponent.generated.h"
class UPhysicsHandleComponent;


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ASOULINTHREE_API UGrabberComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UGrabberComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void Grab();             
	void Release();
	void UpdateHeldObject();

	UFUNCTION(BlueprintCallable, Category = "Grabber")
	void ToggleGrab();

private:
	// Physics handle component used to manipulate physics objects
	UPROPERTY()
	TObjectPtr<UPhysicsHandleComponent> PhysicsHandle;

	// Grabber properties for distance and radius of the grab area
	UPROPERTY(EditAnywhere, Category = "Grabber", meta = (ClampMin = "0.0", Units = "cm"))
	float GrabDistance = 200.f;

	UPROPERTY(EditAnywhere, Category = "Grabber", meta = (ClampMin = "0.0", Units = "cm"))
	float HoldDistance = 150.f;

	UPROPERTY(EditAnywhere, Category = "Grabber", meta = (ClampMin = "0.1", Units = "cm"))
	float GrabRadius = 10.f;
};
