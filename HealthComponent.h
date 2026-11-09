#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

// Delegate for health change events
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnHealthChanged,
	float, CurrentHealth,
	float, MaxHealth
);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ASOULINTHREE_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UHealthComponent();

	// HEALTH MANAGEMENT - functions to manage the health of the actor, including healing, applying damage, and getting/setting health values.
	UFUNCTION(BlueprintCallable, Category = "Health")
	void Heal(float HealAmount);

	UFUNCTION(BlueprintCallable, Category = "Health")
	void ApplyDamage(float DamageAmount);

	UPROPERTY(BlueprintAssignable, Category = "Health")
	FOnHealthChanged OnHealthChanged;

	// Getters and setters for current health and max health
	float GetCurrentHealth() const;
	void SetCurrentHealth(float NewHealth);
	float GetMaxHealth() const { return MaxHealth; }

protected:
	virtual void BeginPlay() override;

	// HEALTH PROPERTIES - properties to manage the health of the actor, including maximum health and current health.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Health",
		meta = (ClampMin = "1.0"))
	float MaxHealth = 100.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health")
	float CurrentHealth = 100.0f;
};