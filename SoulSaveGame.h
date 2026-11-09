#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SoulSaveGame.generated.h"

// Structure to hold save data for pressure plates
USTRUCT()
struct FPressurePlateSaveData
{
	GENERATED_BODY()

	UPROPERTY(SaveGame)
	FTransform WeightTransform = FTransform::Identity;
};

UCLASS()
class ASOULINTHREE_API USoulSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	//SAVE DATA - properties to save the game state, including level name, player transform, health, collected artifacts, and pressure plate states.
	UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Save Data")
	FName LevelName = NAME_None;

	UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Save Data")
	FTransform PlayerTransform = FTransform::Identity;

	UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Save Data")
	float PlayerHealth = 100.0f;

	UPROPERTY(SaveGame, BlueprintReadWrite, Category = "Save Data")
	TSet<FName> CollectedArtifacts;

	// Additional properties to save the player's control rotation and the time when the game was saved.
	UPROPERTY(SaveGame)
	FRotator PlayerControlRotation = FRotator::ZeroRotator;

	UPROPERTY(SaveGame)
	bool bHasPlayerControlRotation = false;

	UPROPERTY(SaveGame)
	FDateTime SavedAtUtc = FDateTime(0);

	// Pressure plate states to save the state of each pressure plate in the level.
	UPROPERTY(SaveGame)
	TMap<FName, FPressurePlateSaveData> PressurePlateStates;

};
