// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseGameState.h"

#include "Components/ItemComponent/ItemComponent.h"
#include "GameFramework/Character.h"
#include "DomiFramework/GameMode/BaseGameMode.h"
#include "DomiFramework/GameInstance/DomiGameInstance.h"
#include "DomiFramework/GameInstance/WorldInstanceSubsystem.h"
#include "DomiFramework/GameInstance/SoundInstanceSubsystem.h"
#include "DomiFramework/GameInstance/ItemInstanceSubsystem.h"
#include "Components/SkillComponent/SkillComponentInitializeData.h"
#include "Components/StatusComponent/StatusComponentInitializeData.h"
#include "Components/StatusComponent/StatusComponent.h"
#include "EnumAndStruct/EffectData/EffectInitializeData.h"
#include "EnumAndStruct/PhysicalSurfaceTypeData/PhysicalSurfaceTypeData.h"
#include "EnumAndStruct/FCrackData.h"
#include "WorldObjects/Crack.h"
#include "Player/Characters/DomiCharacter.h"
#include "Kismet/GameplayStatics.h"

#include "Util/DebugHelper.h"


ABaseGameState::ABaseGameState()
	: SkillDataTable(nullptr),
	  SkillInitializeDataTable(nullptr),
	  StatusInitializeDataTable(nullptr),
	  GameInstance(nullptr),
	  SoundSubsystem(nullptr)
{
}

const TMap<int32, FCrackImageData>& ABaseGameState::GetPastCrackImageDataMap()
{
	return PastCrackImageDataMap;
}

const TMap<int32, FCrackImageData>& ABaseGameState::GetPresentCrackImageData()
{
	return PresentCrackImageDataMap;
}

void ABaseGameState::BeginPlay()
{
	Super::BeginPlay();

	World = GetWorld();
	check(World);
	InitializeGameInstance();
	InitializeSaveManagerInstanceSubsystem();
	InitializeWorldInstanceSubsystem();
	InitializeZeroIndexCrackData(WorldInstanceSubsystem->GetCurrentLevelName());
	InitializeSoundSubsystem();
	InitializeItemInstanceSubsystem();
	LoadCrackImageData();
}

void ABaseGameState::InitializeGameInstance()
{
	GameInstance = Cast<UDomiGameInstance>(GetGameInstance());
	check(IsValid(GameInstance));
	GameInstance->SetWorldCache(World);
}

void ABaseGameState::InitializeSaveManagerInstanceSubsystem()
{
	SaveManagerSubsystem = GameInstance->GetSubsystem<USaveManagerSubsystem>();
	check(IsValid(SaveManagerSubsystem));
	SaveManagerSubsystem->SetWorldCache(World);
	SaveManagerSubsystem->LoadSettings();
}

void ABaseGameState::InitializeWorldInstanceSubsystem()
{
	WorldInstanceSubsystem = GameInstance->GetSubsystem<UWorldInstanceSubsystem>();
	check(IsValid(WorldInstanceSubsystem));

	WorldInstanceSubsystem->SetCurrentLevelName(UGameplayStatics::GetCurrentLevelName(GetWorld(), true));
	if (WorldInstanceSubsystem->GetCurrentLevelName().Contains("Level1") || WorldInstanceSubsystem->GetCurrentLevelName().Contains("PresentLevel"))
	{
		WorldInstanceSubsystem->SetCurrentLevelDisplayName(FText::FromString(TEXT("2375 에어로발리스카")));
	}
	else if (WorldInstanceSubsystem->GetCurrentLevelName().Contains("Level 2") || WorldInstanceSubsystem->GetCurrentLevelName().Contains("PastLevel"))
	{
		WorldInstanceSubsystem->SetCurrentLevelDisplayName(FText::FromString(TEXT("1168 발리스카")));
	}
}

void ABaseGameState::InitializeSoundSubsystem()
{
	SoundSubsystem = GetGameInstance()->GetSubsystem<USoundInstanceSubsystem>();
	check(IsValid(SoundSubsystem));
	SoundSubsystem->SetWorldCache(World);
	SoundSubsystem->LoadSoundClass();
	SoundSubsystem->LoadVolumeSettings();
}

void ABaseGameState::InitializeItemInstanceSubsystem()
{
	ItemInstanceSubsystem = GetGameInstance()->GetSubsystem<UItemInstanceSubsystem>();
	check(IsValid(ItemInstanceSubsystem));
	ItemInstanceSubsystem->SetWorldCache(World);
}

FSkillData* ABaseGameState::GetSkillData(const FGameplayTag SkillTag) const
{
	check(SkillDataTable);

	return SkillDataTable->FindRow<FSkillData>(SkillTag.GetTagName(), TEXT(""));
}

FSkillComponentInitializeData* ABaseGameState::GetSkillComponentInitializeData(const FGameplayTag PawnTag) const
{
	check(SkillInitializeDataTable);

	return SkillInitializeDataTable->FindRow<FSkillComponentInitializeData>(PawnTag.GetTagName(), TEXT(""));
}

FStatusComponentInitializeData* ABaseGameState::GetStatusComponentInitializeData(const FGameplayTag PawnTag) const
{
	check(StatusInitializeDataTable);

	return StatusInitializeDataTable->FindRow<FStatusComponentInitializeData>(PawnTag.GetTagName(), TEXT(""));
}

FEffectInitializeData* ABaseGameState::GetEffectInitializeData(const FGameplayTag EffectTag) const
{
	check(EffectInitializeDataTable);

	return EffectInitializeDataTable->FindRow<FEffectInitializeData>(EffectTag.GetTagName(), TEXT(""));
}

FName ABaseGameState::GetSurfaceNameByEnum(EPhysicalSurface PhysicalSurfaceType) const
{
	switch (PhysicalSurfaceType)
	{
		case SurfaceType_Default : return FName("Default");
		case SurfaceType1:        return FName("Metal");
		case SurfaceType2:        return FName("Flesh");
		case SurfaceType3:        return FName("CityRoads");
		case SurfaceType4:        return FName("BuildingsAndWalls");
		case SurfaceType5:        return FName("FactoryFloor");
		case SurfaceType6:        return FName("Rebar");
		case SurfaceType7:        return FName("Boss1Blood");
		default:                  return FName("Default");
	}
}

FPhysicalSurfaceTypeData* ABaseGameState::GetPhysicalSurfaceTypeData(EPhysicalSurface PhysicalSurfaceType) const
{
	check(SurfaceDataTable);
	return SurfaceDataTable->FindRow<FPhysicalSurfaceTypeData>(GetSurfaceNameByEnum(PhysicalSurfaceType), TEXT("GetPhysicalSurfaceTypeData"));
}

void ABaseGameState::InitializeGame()
{
	check(IsValid(GameInstance));
	check(IsValid(WorldInstanceSubsystem));
	check(IsValid(SoundSubsystem));
	check(IsValid(ItemInstanceSubsystem));
	check(IsValid(SaveManagerSubsystem));

	LoadItemDataFromInstance();

	// 캐릭터 스탯값 적용
	if (IsValid(World))
	{
		ADomiCharacter* PlayerCharacter = Cast<ADomiCharacter>(World->GetFirstPlayerController()->GetPawn());
		if (IsValid(PlayerCharacter))
		{
			UStatusComponent* StatusComponent = PlayerCharacter->GetStatusComponent();
			if (IsValid(StatusComponent))
			{
				if (!GameInstance->GetStatDataMap().IsEmpty())
				{
					StatusComponent->SetStatMap(GameInstance->GetStatDataMap());

					// SetStatMap은 raw 대입이라 UI 델리게이트가 발송되지 않으므로 수동 브로드캐스트
					StatusComponent->OnMaxHealthChanged.Broadcast(StatusComponent->GetStat(StatTags::MaxHealth));
					StatusComponent->OnMaxStaminaChanged.Broadcast(StatusComponent->GetStat(StatTags::MaxStamina));
					StatusComponent->OnHealthChanged.Broadcast(StatusComponent->GetStat(StatTags::Health));
					StatusComponent->OnStaminaChanged.Broadcast(StatusComponent->GetStat(StatTags::Stamina));
				}
			}
		}
	}

	// DropEssence 인스턴스 상 존재 하면 레벨에 적용
	if (WorldInstanceSubsystem->GetIsDropEssenceExist() && WorldInstanceSubsystem->GetDropEssenceLocationLevel() == WorldInstanceSubsystem->GetCurrentLevelName())
	{
		if (!IsValid(DropEssenceClass))
		{
			return;
		}
		
		ADomiCharacter* PlayerCharacter = Cast<ADomiCharacter>(World->GetFirstPlayerController()->GetPawn());
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		ADropEssence* NewDropEssence = World->SpawnActor<ADropEssence>(DropEssenceClass, WorldInstanceSubsystem->GetDropEssenceLocation(), FRotator::ZeroRotator, SpawnParams);
	}
	else
	{
		if (WorldInstanceSubsystem->GetIsDropEssenceExist())
		{
			WorldInstanceSubsystem->SetDropEssenceCache(nullptr);
			WorldInstanceSubsystem->SetIsDropEssenceExist(false);
			WorldInstanceSubsystem->SetDropEssenceAmount(0);
			WorldInstanceSubsystem->SetDropEssenceLocation(FVector::ZeroVector);
			WorldInstanceSubsystem->SetDropEssenceLocationLevel("");
		}
	}

	// 균열 정보 초기화
	CacheAllCracks();
	LoadCrackDataFromInstance();
	InitializeCrackDataMap();
	BaseGameMode = Cast<ABaseGameMode>(World->GetAuthGameMode());
	if (IsValid(BaseGameMode))
	{
		if (IsValid(WorldInstanceSubsystem))
		{
			if (WorldInstanceSubsystem->GetCurrentLevelName() != "Proto_Level1" &&
				WorldInstanceSubsystem->GetCurrentLevelName() != "Proto_Level2" &&
				WorldInstanceSubsystem->GetCurrentLevelName() != "PastLevel" &&
				WorldInstanceSubsystem->GetCurrentLevelName() != "PresentLevel")
			{
				return;
			}
		}
		
		ACrack* NearestCrack = FindNearestCrack();
		if (WorldInstanceSubsystem->GetRecentCrackName().IsEmpty())
		{
			BaseGameMode->SetRecentCrackCache(NearestCrack);
			WorldInstanceSubsystem->SetRecentCrackName(NearestCrack->GetCrackName());
			WorldInstanceSubsystem->SetRecentCrackIndex(NearestCrack->GetCrackIndex());
		}
	}
}

void ABaseGameState::CacheAllCracks()
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(World, ACrack::StaticClass(), FoundActors);

	if (FoundActors.IsEmpty())
	{
		return;
	}
	
	for (AActor* Actor : FoundActors)
	{
		ACrack* Crack = Cast<ACrack>(Actor);
		if (IsValid(Crack))
		{
			AllCracksCache.Add(Crack);
		}
	}

	AllCracksCache.Sort([](const ACrack& A, const ACrack& B)
	{
		return A.GetCrackIndex() < B.GetCrackIndex();
	});
}

void ABaseGameState::LoadCrackDataFromInstance()
{
	for (int32 i = 0; i < AllCracksCache.Num(); i++)
	{
		ACrack* Crack = Cast<ACrack>(AllCracksCache[i]);

		const FCrackData* CrackData = WorldInstanceSubsystem->GetCrackData(
			WorldInstanceSubsystem->GetCurrentLevelName(), i);
		if (CrackData && CrackData->bIsActivate)
		{
			Crack->SetActive();
			Crack->SetCrackName(CrackData->CrackName);
		}
	}
}

void ABaseGameState::LoadItemDataFromInstance()
{
	if (!IsValid(ItemInstanceSubsystem))
	{
		Debug::PrintError(TEXT("ABaseGameState::LoadItemDataFromInstance: Invalid ItemInstanceSubsystem"));
		return;
	}
	
	if (!ItemInstanceSubsystem->GetInventoryDataMap().IsEmpty() &&
		!ItemInstanceSubsystem->GetEquipmentSlotMap().IsEmpty() &&
		!ItemInstanceSubsystem->GetConsumableSlotMap().IsEmpty())
	{
		ADomiCharacter* PlayerCharacter = Cast<ADomiCharacter>(World->GetFirstPlayerController()->GetPawn());
		if (IsValid(PlayerCharacter))
		{
			UItemComponent* ItemComponent = PlayerCharacter->GetItemComponent();
			if (IsValid(ItemComponent))
			{
				ItemComponent->SetInventoryMap(ItemInstanceSubsystem->GetInventoryDataMap());
				ItemComponent->SetEquipmentSlots(ItemInstanceSubsystem->GetEquipmentSlotMap());
				ItemComponent->SetConsumableSlots(ItemInstanceSubsystem->GetConsumableSlotMap());
				ItemComponent->SetPotionBoostLevel(ItemInstanceSubsystem->GetPotionBoostLevel());

				ItemComponent->SetSkillFromItemWhenLevelChanged();
				ItemComponent->DelegateExecuter();
			}
		}
	}
}

void ABaseGameState::InitializeCrackDataMap()
{
	// 불러오지 않은 새 게임일 경우에만 실행
	if (!WorldInstanceSubsystem->GetIsNewGame())
	{
		return;
	}

	if (AllCracksCache.IsEmpty())
	{
		Debug::Print(TEXT("No Cracks in World"));
		return;
	}
	
	TMap<FString, FCrackDataArrayStruct>* CrackDataMap = WorldInstanceSubsystem->GetCrackDataMap();
	const FString& CurrentLevelName = WorldInstanceSubsystem->GetCurrentLevelName();
	FCrackDataArrayStruct& CrackDataArray = (*CrackDataMap)[CurrentLevelName];
	
	for (ACrack* Crack : AllCracksCache)
	{
		if (Crack->GetCrackIndex() == 0)
		{
			continue;
		}

		FCrackData NewCrackData;

		NewCrackData.OwnerLevelName = WorldInstanceSubsystem->GetCurrentLevelName();
		NewCrackData.CrackIndex = Crack->GetCrackIndex();
		NewCrackData.CrackName = Crack->GetCrackName();
		NewCrackData.bIsActivate = Crack->GetIsActivate();
		NewCrackData.RespawnLocation = Crack->GetRespawnTargetPointLocation();
		NewCrackData.RespawnRotation = Crack->GetRespawnTargetPointRotation();

		bool bAlreadyExists = false;
		for (const FCrackData& ExistData : CrackDataArray.CrackDataArray)
		{
			if (ExistData.CrackIndex == Crack->GetCrackIndex())
			{
				bAlreadyExists = true;
				break;
			}
		}
		if (!bAlreadyExists)
		{
			// 해당 인덱스 존재하지 않을 때만 데이터 추가
			CrackDataArray.CrackDataArray.Add(NewCrackData);
		}
	}

	// 크랙 초기화 완료 후 NewGame 플래그 해제 (불필요한 재초기화 방지)
	WorldInstanceSubsystem->SetIsNewGame(WorldInstanceSubsystem->GetCurrentLevelName(), false);
}

void ABaseGameState::InitializeZeroIndexCrackData(const FString CurrentLevelName)
{
	check(CrackInitializeDataTable);

	if (IsValid(WorldInstanceSubsystem))
	{
		if (WorldInstanceSubsystem->GetCurrentLevelName() != "Proto_Level1" &&
			WorldInstanceSubsystem->GetCurrentLevelName() != "Proto_Level2" &&
			WorldInstanceSubsystem->GetCurrentLevelName() != "PastLevel" &&
			WorldInstanceSubsystem->GetCurrentLevelName() != "PresentLevel")
		{
			return;
		}
	}

	if (!WorldInstanceSubsystem->GetIsNewGame())
	{
		return;
	}
	
	FCrackInitializeData* Level1Row = CrackInitializeDataTable->FindRow<FCrackInitializeData>(FName(CurrentLevelName), TEXT(""));
	FCrackInitializeData* Level2Row = CrackInitializeDataTable->FindRow<FCrackInitializeData>(FName(Level1Row->ZeroIndexCrackData.LinkedLevelName), TEXT(""));
	FCrackData Level1 = Level1Row->ZeroIndexCrackData;
	FCrackData Level2 = Level2Row->ZeroIndexCrackData;

	if (IsValid(WorldInstanceSubsystem))
	{
		WorldInstanceSubsystem->InitializeCrackAndNewGameDataMap(Level1, Level2);
	}
}

ACrack* ABaseGameState::FindNearestCrack()
{
	ACharacter* PlayerCharacter = GetWorld()->GetFirstPlayerController()->GetCharacter();
	FVector PlayerLocation = PlayerCharacter->GetActorLocation();

	ACrack* NearestCrack = nullptr;
	float NearestDistanceSquared = TNumericLimits<float>::Max();

	for (ACrack* Crack : AllCracksCache)
	{
		float DistanceSquared = FVector::DistSquared(PlayerLocation, Crack->GetActorLocation());
		if (DistanceSquared < NearestDistanceSquared)
		{
			NearestCrack = Crack;
			NearestDistanceSquared = DistanceSquared;
		}
	}

	return NearestCrack;
}

void ABaseGameState::LoadCrackImageData()
{
	if (PastCrackImageData)
	{
		TArray<FName> RowNames = PastCrackImageData->GetRowNames();
		for (const FName& RowName : RowNames)
		{
			if (FCrackImageData* Row = PastCrackImageData->FindRow<FCrackImageData>(RowName, TEXT("")))
			{
				PastCrackImageDataMap.Add(Row->CrackIndex, *Row);
			}
		}
	}

	if (PresentCrackImageData)
	{
		TArray<FName> RowNames = PresentCrackImageData->GetRowNames();
		for (const FName& RowName : RowNames)
		{
			if (FCrackImageData* Row = PresentCrackImageData->FindRow<FCrackImageData>(RowName, TEXT("")))
			{
				PresentCrackImageDataMap.Add(Row->CrackIndex, *Row);
			}
		}
	}
}
