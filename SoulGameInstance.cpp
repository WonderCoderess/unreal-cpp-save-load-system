#include "SoulGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Pawn.h"
#include "SoulSaveGame.h"
#include "HealthComponent.h"
#include "GameFramework/Controller.h"
#include "PressurePlate.h"
#include "EngineUtils.h"

// START NEW GAME - Reset the game state and start a new game.
void USoulGameInstance::StartNewGame()
{
	CollectedArtifacts.Empty();
	PendingSaveData = nullptr;

	UGameplayStatics::SetGamePaused(this, false);
	UGameplayStatics::OpenLevel(this, FName(TEXT("L_MechanicsTest")));
}

// SAVE GAME - Save the current game state to the save slot.
bool USoulGameInstance::SaveGame(const FString& SlotName)
{
	if (SlotName.IsEmpty())
	{
		return false;
	}

	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);

	if (!IsValid(PlayerPawn))
	{
		return false;
	}

	USoulSaveGame* SaveData = Cast<USoulSaveGame>(UGameplayStatics::CreateSaveGameObject(USoulSaveGame::StaticClass()));

	if (!IsValid(SaveData))
	{
		return false;
	}

	SaveData->LevelName = FName(*UGameplayStatics::GetCurrentLevelName(this, true));

	SaveData->PlayerTransform = PlayerPawn->GetActorTransform();

	if (const AController* Controller = PlayerPawn->GetController())
	{
		SaveData->PlayerControlRotation = Controller->GetControlRotation();
		SaveData->bHasPlayerControlRotation = true;
	}

	UHealthComponent* HealthComponent = PlayerPawn->FindComponentByClass<UHealthComponent>();

	if (!IsValid(HealthComponent))
	{
		return false;
	}

	SaveData->PlayerHealth = HealthComponent->GetCurrentHealth();

	SaveData->CollectedArtifacts = CollectedArtifacts;
	
	SaveData->SavedAtUtc = FDateTime::UtcNow();

	for (TActorIterator<APressurePlate> It(GetWorld()); It; ++It)
	{
		APressurePlate* Plate = *It;
		if (!IsValid(Plate))
		{
			continue;
		}

		FName PlateId = Plate->GetSaveId();
		if (PlateId.IsNone())
		{
			continue;
		}

		FPressurePlateSaveData PlateData;
		if (Plate->CaptureSaveData(PlateData))
		{
			SaveData->PressurePlateStates.Add(PlateId, PlateData);
		}
	}

	return UGameplayStatics::SaveGameToSlot(SaveData,SlotName,0);
}

// LOAD GAME - Load the game state from the save slot and apply it to the game.
bool USoulGameInstance::LoadGame(const FString& SlotName)
{
	if (SlotName.IsEmpty())
	{
		return false;
	}

	if (!UGameplayStatics::DoesSaveGameExist(SlotName, 0))
	{
		return false;
	}

	USoulSaveGame* LoadedSaveData = Cast<USoulSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, 0));
	
	if (!IsValid(LoadedSaveData) || LoadedSaveData->LevelName.IsNone())
	{
		return false;
	}
	
	PendingSaveData = LoadedSaveData;
	CollectedArtifacts = LoadedSaveData->CollectedArtifacts;

	if (UGameplayStatics::IsGamePaused(this))
	{
		UGameplayStatics::SetGamePaused(this, false);
	}
	
	UGameplayStatics::OpenLevel(this, LoadedSaveData->LevelName);

	return true;
}

// APPLY PENDING SAVE - Apply the pending save data to the player pawn after level load.
bool USoulGameInstance::ApplyPendingSave(APawn* PlayerPawn)
{
	if (!IsValid(PendingSaveData) || !IsValid(PlayerPawn))
	{
		return false;
	}

	UHealthComponent* HealthComponent = PlayerPawn->FindComponentByClass<UHealthComponent>();

	if (!IsValid(HealthComponent))
	{
		return false;
	}

	PlayerPawn->SetActorTransform(PendingSaveData->PlayerTransform);

	HealthComponent->SetCurrentHealth(PendingSaveData->PlayerHealth);

	if (PendingSaveData->bHasPlayerControlRotation)
	{
		if (AController* Controller = PlayerPawn->GetController())
		{
			Controller->SetControlRotation(PendingSaveData->PlayerControlRotation);
		}
	}

	for (TActorIterator<APressurePlate> It(GetWorld()); It; ++It)
	{
		APressurePlate* Plate = *It;

		if (!IsValid(Plate) || Plate->GetSaveId().IsNone())
		{
			continue;
		}

		const FPressurePlateSaveData* PlateData =
			PendingSaveData->PressurePlateStates.Find(Plate->GetSaveId());

		if (PlateData)
		{
			Plate->RestoreSaveData(*PlateData);
		}
	}

	PendingSaveData = nullptr;

	return true;
}

// COLLECT ARTIFACT - Add the artifact ID to the collected artifacts set if it hasn't been collected yet.
bool USoulGameInstance::CollectArtifact(FName ArtifactId)
{
	if (ArtifactId.IsNone() || HasArtifact(ArtifactId))
	{
		return false;
	}

	CollectedArtifacts.Add(ArtifactId);

	return true;
}

// HAS ARTIFACT - Check if the artifact ID is in the collected artifacts set.
bool USoulGameInstance::HasArtifact(FName ArtifactId) const
{
	return CollectedArtifacts.Contains(ArtifactId);
}

// DOES SAVE EXIST - Check if a save game exists for the given slot name.
bool USoulGameInstance::DoesSaveExist(const FString& SlotName) const
{
	if (SlotName.IsEmpty())
	{
		return false;
	}

	return UGameplayStatics::DoesSaveGameExist(SlotName, 0);
}

// GET SAVE SLOT DATE - Retrieve the date and time when the save slot was last saved.
FText USoulGameInstance::GetSaveSlotDate(const FString& SlotName) const
{
	if (SlotName.IsEmpty())
	{
		return FText::GetEmpty();
	}

	const USoulSaveGame* SaveData = Cast<USoulSaveGame>(
		UGameplayStatics::LoadGameFromSlot(SlotName, 0)
	);

	if (!IsValid(SaveData) || SaveData->SavedAtUtc.GetTicks() == 0)
	{
		return FText::GetEmpty();
	}

	return FText::AsDateTime(
		SaveData->SavedAtUtc,
		EDateTimeStyle::Short,
		EDateTimeStyle::Short
	);
}

// GET SAVE SLOT DISPLAY INFO - Retrieve display information for a save slot, including the label and whether the slot is enabled.
void USoulGameInstance::GetSaveSlotDisplayInfo(int32 SlotNumber, bool bIsSaveMode, FText& OutLabel, bool& bOutIsEnabled) const {

	const FString SlotName = FString::Printf(TEXT("Slot_%02d"), SlotNumber);
	const bool bSaveExists = DoesSaveExist(SlotName);
	
	// Determine if the slot should be enabled based on whether it's save mode or if a save exists
	bOutIsEnabled = bIsSaveMode || bSaveExists;

	if (!bSaveExists)
	{
		// No save exists for this slot, show "Empty Slot"
		OutLabel = FText::Format(NSLOCTEXT("SaveSystem", "EmptySlot", "Slot {0} - Empty Slot"), FText::AsNumber(SlotNumber));
	}
	else
	{
		FText SaveDate = GetSaveSlotDate(SlotName);
		if (SaveDate.IsEmpty())
		{	
			// empty date, show "Saved Game"
			OutLabel = FText::Format(NSLOCTEXT("SaveSystem", "SavedGame", "Slot {0} - Saved Game"), FText::AsNumber(SlotNumber)); 
		}
		else
		{
			// Show the save date in the label
			OutLabel = FText::Format(NSLOCTEXT("SaveSystem", "SavedGameWithDate", "Slot {0} - {1}"), FText::AsNumber(SlotNumber), SaveDate); 
		}
	}
}