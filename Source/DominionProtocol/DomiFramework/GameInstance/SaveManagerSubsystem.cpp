// Fill out your copyright notice in the Description page of Project Settings.


#include "SaveManagerSubsystem.h"

#include "DomiGameInstance.h"
#include "ItemInstanceSubsystem.h"
#include "SoundInstanceSubsystem.h"
#include "WorldInstanceSubsystem.h"
#include "DomiFramework/DomiSaveGame.h"
#include "DomiFramework/DomiSaveSettings.h"
#include "Kismet/GameplayStatics.h"

#include "Util/DebugHelper.h"

void USaveManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	SaveSlotArray.AddDefaulted(3);

	// 기본값으로 초기화
	for (int32 i = 0; i < 3; i++)
	{
		SaveSlotArray[i].SaveSlotName = FString::Printf(TEXT("SaveSlot%d"), i + 1);
		SaveSlotArray[i].SaveSlotIndex = i;
		SaveSlotArray[i].PlayingLevelName = "PresentLevel";
		SaveSlotArray[i].PlayingLevelDisplayName = FText::FromString(TEXT("2375 에어로발리스카"));
	}
	
	// 저장된 설정이 있다면 로드
	LoadSettings();
}

void USaveManagerSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void USaveManagerSubsystem::StartNewGame(int32 SlotIndex)
{
	if (SaveSlotArray.IsValidIndex(SlotIndex))
	{
		SaveSlotArray[SlotIndex].SaveSlotExist = true;
		UDomiGameInstance* GameInstance = Cast<UDomiGameInstance>(GetGameInstance());
		check(IsValid(GameInstance));
		UWorldInstanceSubsystem* WorldInstanceSubsystem = GameInstance->GetSubsystem<UWorldInstanceSubsystem>();
		check(IsValid(WorldInstanceSubsystem));
		
		GameInstance->SetSaveSlotName(SaveSlotArray[SlotIndex].SaveSlotName);
		GameInstance->SetSaveSlotIndex(SaveSlotArray[SlotIndex].SaveSlotIndex);
		WorldInstanceSubsystem->SetCurrentLevelName("PresentLevel");
		WorldInstanceSubsystem->SetCurrentLevelDisplayName(FText::FromString(TEXT("2375 에어로발리스카")));

		SaveGame(SaveSlotArray[SlotIndex].SaveSlotName, SaveSlotArray[SlotIndex].SaveSlotIndex);
		SaveSettings();
		
		Debug::Print(TEXT("Start New Game"));
		UGameplayStatics::OpenLevel(World, FName(TEXT("PresentLevel")));
	}
}

void USaveManagerSubsystem::LoadSaveDataAndOpenLevel(int32 SlotIndex)
{
	if (SaveSlotArray.IsValidIndex(SlotIndex))
	{
		if (SaveSlotArray[SlotIndex].SaveSlotExist)
		{
			const bool bLoadResult = LoadGame(SaveSlotArray[SlotIndex].SaveSlotName, SaveSlotArray[SlotIndex].SaveSlotIndex);
			UGameplayStatics::OpenLevel(World, FName(SaveSlotArray[SlotIndex].PlayingLevelName));
		}
	}
}

void USaveManagerSubsystem::DeleteSaveSlot(int32 SlotIndex)
{
	UGameplayStatics::DeleteGameInSlot(SaveSlotArray[SlotIndex].SaveSlotName, SaveSlotArray[SlotIndex].SaveSlotIndex);
	SaveSlotArray[SlotIndex] = FSaveSlotMetaData();
	SaveSlotArray[SlotIndex].SaveSlotName = FString::Printf(TEXT("SaveSlot%d"), SlotIndex + 1);
	SaveSlotArray[SlotIndex].SaveSlotIndex = SlotIndex;
	SaveSettings();
}

bool USaveManagerSubsystem::SaveGame(const FString& SlotName, int32 UserIndex)
{
	//저장 인스턴스 생성
	UDomiSaveGame* SaveGameInstance = Cast<UDomiSaveGame>(UGameplayStatics::CreateSaveGameObject(UDomiSaveGame::StaticClass()));
	check(IsValid(SaveGameInstance));

	//DomiGameInstance 저장할 데이터 구조체(FInstanceData)로 Get
	UDomiGameInstance* GameInstance = Cast<UDomiGameInstance>(GetGameInstance());
	if (IsValid(GameInstance))
	{
		SaveGameInstance->InstanceData = GameInstance->GetSaveData();
	}

	//ItemInstanceSubsystem 저장할 데이터 구조체(FItemInstanceSubsystem)로 Get
	UItemInstanceSubsystem* ItemInstanceSubsystem = GameInstance->GetSubsystem<UItemInstanceSubsystem>();
	if (IsValid(ItemInstanceSubsystem))
	{
		SaveGameInstance->ItemSubsystemData = ItemInstanceSubsystem->GetSaveData();
	}

	//WorldInstanceSubsystem 저장할 데이터 구조체(FWorldInstanceSubsystem)로 Get
	UWorldInstanceSubsystem* WorldInstanceSubsystem = GameInstance->GetSubsystem<UWorldInstanceSubsystem>();
	if (IsValid(WorldInstanceSubsystem))
	{
		SaveGameInstance->WorldInstanceSubsystemData = WorldInstanceSubsystem->GetSaveData();
	}

	//저장 실행
	return UGameplayStatics::SaveGameToSlot(SaveGameInstance, SlotName, UserIndex);
}

bool USaveManagerSubsystem::LoadGame(const FString& SlotName, int32 UserIndex)
{
	if (!UGameplayStatics::DoesSaveGameExist(SlotName, UserIndex))
	{
		UE_LOG(LogTemp, Error, TEXT("[LoadGame] FAIL - Save file does not exist: %s (UserIndex=%d)"), *SlotName, UserIndex);
		return false;
	}

	UDomiSaveGame* LoadedGame = Cast<UDomiSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, UserIndex));
	if (!IsValid(LoadedGame))
	{
		UE_LOG(LogTemp, Error, TEXT("[LoadGame] FAIL - Could not deserialize: %s"), *SlotName);
		return false;
	}

	//DomiGameInstance SaveData Load(FInstanceData)
	UDomiGameInstance* GameInstance = Cast<UDomiGameInstance>(GetGameInstance());
	if (IsValid(GameInstance))
	{
		GameInstance->LoadSaveData(LoadedGame->InstanceData);
	}

	//ItemInstanceSubsystem SaveData Load
	UItemInstanceSubsystem* ItemInstanceSubsystem = GameInstance->GetSubsystem<UItemInstanceSubsystem>();
	if (IsValid(ItemInstanceSubsystem))
	{
		ItemInstanceSubsystem->LoadSaveData(LoadedGame->ItemSubsystemData);
	}

	//WorldInstanceSubsystem SaveData Load
	UWorldInstanceSubsystem* WorldInstanceSubsystem = GameInstance->GetSubsystem<UWorldInstanceSubsystem>();
	if (IsValid(WorldInstanceSubsystem))
	{
		WorldInstanceSubsystem->LoadSaveData(LoadedGame->WorldInstanceSubsystemData);
	}
	
	return true;
}

bool USaveManagerSubsystem::SaveSettings()
{
	//저장 인스턴스 생성
	UDomiSaveSettings* SaveSettingsInstance = Cast<UDomiSaveSettings>(UGameplayStatics::CreateSaveGameObject(UDomiSaveSettings::StaticClass()));
	check(IsValid(SaveSettingsInstance));

	UDomiGameInstance* GameInstance = Cast<UDomiGameInstance>(GetGameInstance());
	check(IsValid(GameInstance));

	SaveSettingsInstance->SaveSlotMetaDataArray = SaveSlotArray;
	
	//SoundInstanceSubsystem 저장할 데이터 구조체(FSoundSubsystem)로 Get
	USoundInstanceSubsystem* SoundSubsystem = GameInstance->GetSubsystem<USoundInstanceSubsystem>();
	if (IsValid(SoundSubsystem))
	{
		SaveSettingsInstance->SoundSubsystemData = SoundSubsystem->GetSaveData();
	}
	
	return UGameplayStatics::SaveGameToSlot(SaveSettingsInstance, FString::Printf(TEXT("UserSettings")), 999);
}

bool USaveManagerSubsystem::LoadSettings()
{
	if (!UGameplayStatics::DoesSaveGameExist(FString::Printf(TEXT("UserSettings")), 999))
	{
		UE_LOG(LogTemp, Warning, TEXT("[LoadSettings] No existing user settings found, using defaults."));
		return false;
	}

	UDomiSaveSettings* LoadedGame = Cast<UDomiSaveSettings>(UGameplayStatics::LoadGameFromSlot(FString::Printf(TEXT("UserSettings")), 999));
	if (!IsValid(LoadedGame))
	{
		UE_LOG(LogTemp, Warning, TEXT("[LoadSettings] Load GameData Failed."));
		return false;
	}

	UDomiGameInstance* GameInstance = Cast<UDomiGameInstance>(GetGameInstance());
	check(IsValid(GameInstance));

	SaveSlotArray = LoadedGame->SaveSlotMetaDataArray;
	
	for (int32 i = 0; i < SaveSlotArray.Num(); i++)
	{
		UE_LOG(LogTemp, Warning, TEXT("[LoadSettings] Slot[%d]: Exist=%s, Level=%s, Crack=%s"),
			i,
			SaveSlotArray[i].SaveSlotExist ? TEXT("true") : TEXT("false"),
			*SaveSlotArray[i].PlayingLevelName,
			*SaveSlotArray[i].RecentCrackName.ToString());
	}

	//SoundInstanceSubsystem SaveData Load(FSoundSubsystemData)
	USoundInstanceSubsystem* SoundSubsystem = GameInstance->GetSubsystem<USoundInstanceSubsystem>();
	if (IsValid(SoundSubsystem))
	{
		SoundSubsystem->LoadSaveData(LoadedGame->SoundSubsystemData);
	}

	Debug::Print(TEXT("Success Load All User Setting Data "));
	return true;
}