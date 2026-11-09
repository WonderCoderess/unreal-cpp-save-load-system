#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "SoulGameInstance.generated.h"

class APawn;
class USoulSaveGame;

UCLASS()
class ASOULINTHREE_API USoulGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	// SAVE SYSTEM - functions to manage the save system, including starting a new game, saving and loading game data, applying pending saves, and managing collected artifacts.
	UFUNCTION(BlueprintCallable, Category = "Save System")
	void StartNewGame();

	UFUNCTION(BlueprintCallable, Category = "Save System")
	bool SaveGame(const FString& SlotName);

	UFUNCTION(BlueprintCallable, Category = "Save System")
	bool LoadGame(const FString& SlotName);

	UFUNCTION(BlueprintCallable, Category = "Save System")
	bool ApplyPendingSave(APawn* PlayerPawn);

	// ARTIFACT MANAGEMENT - functions to manage collected artifacts, including collecting artifacts and checking if an artifact has been collected.
	UFUNCTION(BlueprintCallable, Category = "Artifacts")
	bool CollectArtifact(FName ArtifactId);

	UFUNCTION(BlueprintPure, Category = "Artifacts")
	bool HasArtifact(FName ArtifactId) const;

	// SAVE SYSTEM UTILITIES - functions to check if a save exists, get the date of a save slot, and retrieve display information for save slots.
	UFUNCTION(BlueprintPure, Category = "Save System")
	bool DoesSaveExist(const FString& SlotName) const;

	UFUNCTION(BlueprintCallable, Category = "Save System")
	FText GetSaveSlotDate(const FString& SlotName) const;

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Save System")
	void GetSaveSlotDisplayInfo(
		int32 SlotNumber,
		bool bIsSaveMode,
		FText& OutLabel,
		bool& bOutIsEnabled
	) const;

protected:
	// SAVE SYSTEM PROPERTIES - properties related to the save system, including the default save slot name, pending save data, and collected artifacts.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Save System")
	FString SaveSlotName = TEXT("Slot_01");

	UPROPERTY()
	TObjectPtr<USoulSaveGame> PendingSaveData = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Artifacts") // Tset of collected artifact IDs (unique), accessible in Blueprints
	TSet<FName> CollectedArtifacts;
};

