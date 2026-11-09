#include "HealthComponent.h"

UHealthComponent::UHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	SetCurrentHealth(MaxHealth);
}

// APPLYDAMAGE - apply damage to the actor, ensuring that the current health does not drop below zero and broadcasting the OnHealthChanged event.
void UHealthComponent::ApplyDamage(float DamageAmount)
{
	if (DamageAmount <= 0.0f)
	{
		return;
	}

	// CurrentHealth = FMath::Clamp(CurrentHealth - DamageAmount, 0.0f, MaxHealth);
	SetCurrentHealth(CurrentHealth - DamageAmount);
}

// HEAL - heal the actor, ensuring that the current health does not exceed the maximum health and broadcasting the OnHealthChanged event.
void UHealthComponent::Heal(float HealAmount)
{
	if (HealAmount <= 0.0f)
	{
		return;
	}

	// CurrentHealth = FMath::Clamp(CurrentHealth + HealAmount, 0.0f, MaxHealth);
	SetCurrentHealth(CurrentHealth + HealAmount);
}

// GETTER - get the current health of the actor.
float UHealthComponent::GetCurrentHealth() const
{
	return CurrentHealth;	
}

// SETTER - set the current health and broadcast the OnHealthChanged event to notify any listeners (like the HUD) about the change in health.
void UHealthComponent::SetCurrentHealth(float NewHealth)
{
	CurrentHealth = FMath::Clamp(NewHealth, 0.0f, MaxHealth);
	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
}