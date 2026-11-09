#include "PlayerHUDWidget.h"
#include "Components/ProgressBar.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "HealthComponent.h"

// NATIVECONSTRUCT called when the widget is constructed. It binds to the OnPossessedPawnChanged event of the player controller to handle pawn changes and initializes the health bar based on the currently possessed pawn.
void UPlayerHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (APlayerController* Controller = GetOwningPlayer())
	{
		Controller->OnPossessedPawnChanged.AddUniqueDynamic(
			this, &UPlayerHUDWidget::HandlePawnChanged
		);
	}

	HandlePawnChanged(nullptr, GetOwningPlayerPawn());
}

// NATIVEDESCTRUCT called when the widget is being removed from the viewport. It unbinds from the OnPossessedPawnChanged event and the OnHealthChanged event to avoid dangling references.
void UPlayerHUDWidget::NativeDestruct()
{
	if (APlayerController* Controller = GetOwningPlayer())
	{
		Controller->OnPossessedPawnChanged.RemoveDynamic(
			this, &UPlayerHUDWidget::HandlePawnChanged
		);
	}

	if (UHealthComponent* Health = ObservedHealthComponent.Get())
	{
		Health->OnHealthChanged.RemoveDynamic(
			this, &UPlayerHUDWidget::HandleHealthChanged
		);
	}

	ObservedHealthComponent.Reset();
	Super::NativeDestruct();
}

// HANDLEPAWNCHANGED called when the player controller possesses a new pawn. It updates the observed health component and binds to its OnHealthChanged event.
void UPlayerHUDWidget::HandlePawnChanged(APawn* /*OldPawn*/, APawn* NewPawn)
{
	if (UHealthComponent* PreviousHealth = ObservedHealthComponent.Get())
	{
		PreviousHealth->OnHealthChanged.RemoveDynamic(this, &UPlayerHUDWidget::HandleHealthChanged);
	}

	ObservedHealthComponent.Reset();

	if (IsValid(NewPawn))
	{
		if (UHealthComponent* Health = NewPawn->FindComponentByClass<UHealthComponent>())
		{
			ObservedHealthComponent = Health;
			Health->OnHealthChanged.AddUniqueDynamic(this, &UPlayerHUDWidget::HandleHealthChanged);

			HandleHealthChanged(Health->GetCurrentHealth(), Health->GetMaxHealth());
			return;
		}
	}

	HandleHealthChanged(0.0f, 0.0f);
}

// HANDLEHEALTHCHANGED - called when the observed health component's health changes. It updates the health bar's percentage based on the current and maximum health values.
void UPlayerHUDWidget::HandleHealthChanged(float CurrentHealth, float MaxHealth)
{
	if (PB_Health)
	{
		const float Percent = MaxHealth > 0.0f?FMath::Clamp(CurrentHealth / MaxHealth, 0.0f, 1.0f): 0.0f;

		PB_Health->SetPercent(Percent);
	}
}