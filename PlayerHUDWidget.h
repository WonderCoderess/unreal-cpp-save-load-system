#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerHUDWidget.generated.h"

class UProgressBar;
class APawn;
class UHealthComponent;

UCLASS()
class ASOULINTHREE_API UPlayerHUDWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> PB_Health;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

private:
	// Handle the pawn change event to update the observed health component
	UFUNCTION()
	void HandlePawnChanged(APawn* OldPawn, APawn* NewPawn);

	// Handle the health change event to update the health bar
	UFUNCTION()
	void HandleHealthChanged(float CurrentHealth, float MaxHealth);

	// A weak pointer to the observed health component to avoid dangling references
	TWeakObjectPtr<UHealthComponent> ObservedHealthComponent;
};
