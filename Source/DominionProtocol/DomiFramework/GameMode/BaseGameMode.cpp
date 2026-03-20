// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseGameMode.h"

#include "AI/AICharacters/BaseEnemy.h"
#include "Kismet/GameplayStatics.h"
#include "DomiFramework/GameState/BaseGameState.h"
#include "DomiFramework/GameInstance/DomiGameInstance.h"
#include "DomiFramework/GameInstance/WorldInstanceSubsystem.h"
#include "DomiFramework/GameInstance/ItemInstanceSubsystem.h"
#include "Interface/StoryDependentInterface.h"
#include "Player/Characters/DomiCharacter.h"
#include "WorldObjects/Crack.h"
#include "WorldObjects/DropEssence.h"
#include "Components/StatusComponent/StatusComponent.h"
#include "Components/ItemComponent/ItemComponent.h"
#include "Components/SpotLightComponent.h"
#include "Components/RectLightComponent.h"
#include "Components/PlayerControlComponent/PlayerControlComponent.h"
#include "EnumAndStruct/FCrackData.h"
#include "EngineUtils.h"
#include "LevelSequencePlayer.h"
#include "MovieSceneSequencePlaybackSettings.h"
#include "MovieScene.h"
#include "AI/AICharacters/BossMonster/BaseBossEnemy.h"
#include "AI/AICharacters/BossMonster/Boss1Enemy.h"
#include "AI/AIControllers/BaseAIController.h"
#include "Components/AIComponent/AIStateComponent.h"
#include "Engine/Engine.h"
#include "Components/AudioComponent.h"
#include "Engine/ExponentialHeightFog.h"
#include "Sound/SoundWave.h"
#include "Components/ExponentialHeightFogComponent.h"
#include "Components/SkyAtmosphereComponent.h"
#include "DomiFramework/WorldActorManage/ActorStateComponent.h"
#include "Engine/ExponentialHeightFog.h"
#include "Engine/PostProcessVolume.h"
#include "Engine/RectLight.h"
#include "Engine/SpotLight.h"
#include "Player/InGameController.h"

#include "Util/CheatBPLib.h"
#include "Util/GameTagList.h"
#include "Util/DebugHelper.h"
#include "WorldObjects/Boss3Skull.h"
#include "WorldObjects/DyingHelper.h"
#include "WorldObjects/Teleporter.h"

ABaseGameMode::ABaseGameMode()
	: GameInstance(nullptr),
	  PlayerCharacter(nullptr),
	  RecentCrackCache(nullptr),
	  RespawnDelay(2.f),
	  Boss3Skull(nullptr),
	  PlayTime(0),
	  bIsFadeIn(true),
      FadeDuration(1.f),
      AssetLoadDelay(1.f),
	  StartFogIntensity(8208.f),
	  TargetFogIntensity(4805095.f)
{
	static ConstructorHelpers::FClassFinder<ADropEssence> DropEssenceBPClass(TEXT("/Game/WorldObjects/BP_DropEssence"));
	if (DropEssenceBPClass.Succeeded())
	{
		DropEssenceClass = DropEssenceBPClass.Class;
	}

	EnterAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("EnterAudioComponent"));
	EnterAudioComponent->SetSound(EnterSound);
	ExitAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("ExitAudioComponent"));
	ExitAudioComponent->SetSound(ExitSound);
}

void ABaseGameMode::BeginPlay()
{
	Super::BeginPlay();

	GameInstance = Cast<UDomiGameInstance>(GetGameInstance());
	checkf(GameInstance, TEXT("Get GameInstance Fail"));

	WorldInstanceSubsystem = GameInstance->GetSubsystem<UWorldInstanceSubsystem>();
	check(WorldInstanceSubsystem);

	ItemInstanceSubsystem = GameInstance->GetSubsystem<UItemInstanceSubsystem>();
	check(ItemInstanceSubsystem);

	World = GetWorld();
	check(IsValid(World));
}

void ABaseGameMode::StartPlay()
{
	Super::StartPlay();

	PlayerCharacter = Cast<ADomiCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));
	if (IsValid(PlayerCharacter) && !PlayerCharacter->ActorHasTag("Player"))
	{
		PlayerCharacter->Tags.Add("Player");
	}

	PlayerController = Cast<APlayerController>(PlayerCharacter->GetController());
	check(IsValid(PlayerController));

	BaseGameState = Cast<ABaseGameState>(World->GetGameState());
	check(GameState);
	BaseGameState->InitializeGame();

	// Boss3Room 액터들 저장
	if (WorldInstanceSubsystem->GetCurrentLevelName().Contains("PastLevel"))
	{
		UGameplayStatics::GetAllActorsWithTag(this, "Boss1Fog", TargetPostProcessVolumes);
		UGameplayStatics::GetAllActorsWithTag(World, TEXT("NormalState"), Boss3RoomNormalState);
		UGameplayStatics::GetAllActorsWithTag(World, TEXT("BattleState"), Boss3RoomBattleState);
	}

	//최근 균열 데이터 있을 경우 그곳으로 플레이어 이동
	if (!WorldInstanceSubsystem->GetRecentCrackName().IsEmpty())
	{
		const FCrackData* CrackData = WorldInstanceSubsystem->GetCrackData(
			WorldInstanceSubsystem->GetCurrentLevelName(), WorldInstanceSubsystem->GetRecentCrackIndex());
		check(CrackData);

		ACrack* NearCrack = BaseGameState->GetCrackByIndex(CrackData->CrackIndex);
		RecentCrackCache = NearCrack;
		PlayerCharacter->SetActorLocation(CrackData->RespawnLocation);
		PlayerCharacter->SetActorRotation(CrackData->RespawnRotation);
		FRotator NewRotation = PlayerCharacter->GetActorForwardVector().Rotation();
		PlayerController->SetControlRotation(NewRotation);
	}

	CheckFogCrackAndOffFog();
	CheckSkyAtmosphereAndToggle();

	if (!WorldInstanceSubsystem->GetCurrentLevelName().Contains("Test"))
	{
		if (RecentCrackCache->GetIsBoss3Crack())
		{
			ToggleBoss3BattleRoom(true);
		}
		else
		{
			ToggleBoss3BattleRoom(false);
		}
	}

	//균열 이동으로 인한 레벨 이동 시의 플레이어 위치 변경 로직
	if (WorldInstanceSubsystem->GetIsLevelChanged())
	{
		Debug::Print(FString::Printf(TEXT("%s, %s"), *WorldInstanceSubsystem->GetMoveTargetLocation().ToString(),
		                             *WorldInstanceSubsystem->GetMoveTargetRotation().ToString()));
		PlayerCharacter->SetActorLocationAndRotation(WorldInstanceSubsystem->GetMoveTargetLocation(),
		                                             WorldInstanceSubsystem->GetMoveTargetRotation());
		WorldInstanceSubsystem->SwitchIsLevelChanged();
		WorldInstanceSubsystem->SetMoveTargetLocation(FVector::ZeroVector);
		WorldInstanceSubsystem->SetMoveTargetRotator(FRotator::ZeroRotator);
		FRotator NewRotation = PlayerCharacter->GetActorForwardVector().Rotation();
		PlayerController->SetControlRotation(NewRotation);
	}

	//=====Enemy 위치정보 캐싱=====
	TArray<AActor*> Enemies;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABaseEnemy::StaticClass(), Enemies);
	for (AActor* Enemy : Enemies)
	{
		//FGameplayTag 정해지면 그거로 바꿔야할 듯?
		if (Enemy->GetName().Contains("Boss")) continue;

		FEnemySpawnInfo Info;
		Info.EnemyClass = Enemy->GetClass();
		Info.OriginTransform = Enemy->GetActorTransform();

		CachedEnemyInfo.Add(Info);
	}
	//==========================

	// 스토리 상태 불러오기
	Debug::Print(FString::Printf(TEXT("Current Story State: %s"), *UEnum::GetValueAsString(GameInstance->GetCurrentGameStoryState())));
	for (TActorIterator<AActor> It(GetWorld()); It; ++It)
	{
		AActor* Actor = *It;
		if (Actor->GetClass()->ImplementsInterface(UStoryDependentInterface::StaticClass()))
		{
			IStoryDependentInterface::Execute_OnStoryStateUpdated(Actor, GameInstance->GetCurrentGameStoryState());
		}
	}
	
	// StoryState >= Clear_Boss1 이면 보스1 안개 삭제
	if (WorldInstanceSubsystem->GetCurrentLevelName() == "PastLevel" && GameInstance->GetCurrentGameStoryState() >= EGameStoryState::Clear_Boss1)
	{
		if (!TargetPostProcessVolumes.IsEmpty())
		{
			TargetPostProcessVolumes[0]->Destroy();
		}
	}

	World->GetTimerManager().SetTimer(
		PlayTimer,
		this,
		&ABaseGameMode::PlayTimeAdder,
		60.f,
		true);

	World->GetTimerManager().SetTimer(
		DormancyCheckTimer,
		this,
		&ABaseGameMode::CheckAIDormancy,
		0.5f,
		true);

	// AssetLoadDelay 후 페이드인
	TWeakObjectPtr<ThisClass> WeakThis = this;
	GetWorldTimerManager().SetTimer(
		AssetLoadTimer,
		[WeakThis]()
		{
			WeakThis->PlayFade(true);
		},
		AssetLoadDelay,
		false
		);
	// SetPlayerInputEnable(false);
	// PlayFade(true);
}

void ABaseGameMode::StartBattle(AActor* SpawnedBoss)
{
	OnStartBattle.Broadcast(SpawnedBoss);
}

void ABaseGameMode::EndBattle(AActor* DeadMonster)
{
	OnEndBattle.Broadcast(DeadMonster);
	if (IsValid(DeadMonster) && DeadMonster->IsA(ABoss1Enemy::StaticClass()))
	{
		DisappearBoss1Fog();
	}
}

void ABaseGameMode::Save()
{
	USaveManagerSubsystem* SaveManagerSubsystem = GameInstance->GetSubsystem<USaveManagerSubsystem>();
	check(IsValid(SaveManagerSubsystem));

	UpdateInstanceData();

	FSaveSlotMetaData NewSaveSlotMetaData;
	NewSaveSlotMetaData.SaveSlotExist = true;
	NewSaveSlotMetaData.SaveSlotName = GameInstance->GetSaveSlotName();
	NewSaveSlotMetaData.SaveSlotIndex = GameInstance->GetSaveSlotIndex();
	NewSaveSlotMetaData.SaveDateTime = FDateTime::Now();
	NewSaveSlotMetaData.PlayTime = SaveManagerSubsystem->GetPlayTime(GameInstance->GetSaveSlotIndex()) + PlayTime;
	NewSaveSlotMetaData.PlayingLevelName = WorldInstanceSubsystem->GetCurrentLevelName();
	NewSaveSlotMetaData.PlayingLevelDisplayName = WorldInstanceSubsystem->GetCurrentLevelDisplayName();
	NewSaveSlotMetaData.RecentCrackName = RecentCrackCache->GetCrackName();
	NewSaveSlotMetaData.PlayerLevel = static_cast<int32>(PlayerCharacter->GetStatusComponent()->GetStat(StatTags::Level));
	
	SaveManagerSubsystem->SetSaveSlotData(NewSaveSlotMetaData.SaveSlotIndex, NewSaveSlotMetaData);
	SaveManagerSubsystem->SaveSettings();
	SaveManagerSubsystem->SaveGame(GameInstance->GetSaveSlotName(), GameInstance->GetSaveSlotIndex());
}

void ABaseGameMode::MoveToRecentCrack()
{
	if (IsValid(RecentCrackCache))
	{
		MoveToTargetCrack(WorldInstanceSubsystem->GetCurrentLevelName(), RecentCrackCache->GetCrackIndex());
	}
}

void ABaseGameMode::OnPlayerDeath()
{
	checkf(RecentCrackCache, TEXT("ABaseGameMode::OnPlayerDeath : RecentCrackCache is Nullptr"));

	Debug::Print("ABaseGameMode::OnPlayerDeath : Respawn Player");

	if (UDomiGameInstance* GI = GetGameInstance<UDomiGameInstance>())
	{
		if (GI->ReturnStoryState())
		{
			EndBattle();
		}
	}

	PlayerDeathLocation = PlayerCharacter->GetActorLocation();

	FTimerHandle RespawnTimerHandle;
	GetWorldTimerManager().SetTimer(
		RespawnTimerHandle,
		this,
		&ABaseGameMode::RespawnPlayerCharacter,
		RespawnDelay,
		false);
}

void ABaseGameMode::RestorePlayer()
{
	//체력회복
	StatusComponent = PlayerCharacter->GetStatusComponent();
	if (IsValid(StatusComponent))
	{
		StatusComponent->SetHealth(UE_BIG_NUMBER);
	}

	//회복 포션 개수 -> Max 회복
	ItemComponent = PlayerCharacter->GetItemComponent();
	if (IsValid(ItemComponent))
	{
		ItemComponent->RestorePotion();
	}
}

void ABaseGameMode::RespawnPlayerCharacter()
{
	DestroyAllNormalEnemy();

	// Essence 레벨에 드랍
	ADropEssence* DropEssence = WorldInstanceSubsystem->GetDropEssenceCache();
	if (IsValid(DropEssence))
	{
		DropEssence->Destroy();
		WorldInstanceSubsystem->SetDropEssenceCache(nullptr);
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	ADropEssence* NewDropEssence = World->SpawnActor<ADropEssence>(DropEssenceClass, PlayerDeathLocation,
	                                                               FRotator::ZeroRotator, SpawnParams);
	OnSpawnDropEssence.Broadcast(NewDropEssence);
	if (IsValid(NewDropEssence))
	{
		Debug::Print(FString::Printf(TEXT("Spawned DropEssence : %s"), *NewDropEssence->GetName()));
		WorldInstanceSubsystem->SetDropEssenceCache(NewDropEssence);
		WorldInstanceSubsystem->SetIsDropEssenceExist(true);
		WorldInstanceSubsystem->SetDropEssenceAmount(GameInstance->GetPlayerCurrentEssence());
		WorldInstanceSubsystem->SetDropEssenceLocation(PlayerDeathLocation);
		WorldInstanceSubsystem->SetDropEssenceLocationLevel(WorldInstanceSubsystem->GetCurrentLevelName());
		GameInstance->SetPlayerCurrentEssence(0);
	}

	if (GameInstance->GetCurrentGameStoryState() == EGameStoryState::Find_Boss2)
	{
		UCheatBPLib::ToggleFog(World);
	}
	
	// 플레이어 리스폰
	if (IsValid(PlayerCharacter))
	{
		FVector RespawnLocation = RecentCrackCache->GetRespawnTargetPointLocation();
		FRotator RespawnRotation = RecentCrackCache->GetRespawnTargetPointRotation();

		PlayerCharacter->SetActorLocation(RespawnLocation);
		PlayerCharacter->SetActorRotation(RespawnRotation);

		//일단은 체력회복하고 EffectTags::Death 상태 해제
		if (!IsValid(StatusComponent))
		{
			StatusComponent = PlayerCharacter->GetStatusComponent();
		}
		StatusComponent->SetHealth(UE_BIG_NUMBER);
		TObjectPtr<UPlayerControlComponent> PlayerControlComponent = PlayerCharacter->GetPlayerControlComponent();
		PlayerControlComponent->DeactivateControlEffect(EffectTags::Death);
		RestorePlayer();

		ExitAudioComponent->Play();
		// if (auto* InGameController = Cast<AInGameController>(PlayerController))
		// {
		// 	InGameController->FadeIn(FadeDuration);
		// }

		// Using InGameHUD
		OnPlayerSpawn.Broadcast();

		UpdateInstanceData();
		Save();
		RespawnEnemies();
		ToggleBoss3BattleRoom(false);
	}
}

void ABaseGameMode::MoveToTargetCrack(FString InOwningCrackLevelName, int32 InCrackIndex)
{
	if (!WorldInstanceSubsystem->GetIsActivateCrackIndex(InOwningCrackLevelName, InCrackIndex))
	{
		Debug::Print(TEXT("Crack is not Activate"));
		return;
	}

	const FString& CurrentLevelName = WorldInstanceSubsystem->GetCurrentLevelName();
	const FCrackData* TargetCrackData = WorldInstanceSubsystem->GetCrackData(InOwningCrackLevelName, InCrackIndex);

	if (!TargetCrackData)
	{
		Debug::Print(TEXT("TargetCrackData is nullptr"));
		return;
	}

	WorldInstanceSubsystem->SetRecentCrackIndex(InCrackIndex);
	WorldInstanceSubsystem->SetRecentCrackName(TargetCrackData->CrackName);
	WorldInstanceSubsystem->SetMoveTargetLocation(TargetCrackData->RespawnLocation);
	WorldInstanceSubsystem->SetMoveTargetRotator(TargetCrackData->RespawnRotation);

	PlayFade(false);
	if (CurrentLevelName == InOwningCrackLevelName)
	{
		bIsSameLevelMove = true;
		PendingMoveLocation = TargetCrackData->RespawnLocation;
		PendingMoveRotation = TargetCrackData->RespawnRotation;
		RecentCrackCache = BaseGameState->GetCrackByIndex(InCrackIndex);
	}
	else if (CurrentLevelName != InOwningCrackLevelName)
	{
		bIsSameLevelMove = false;
		MoveTargetLevelName = FName(InOwningCrackLevelName);
		Debug::Print(TEXT("Move Another Level"));
		WorldInstanceSubsystem->SwitchIsLevelChanged();
	}
	Save();
}

void ABaseGameMode::DestroyAllNormalEnemy()
{
	TArray<AActor*> Enemies;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABaseEnemy::StaticClass(), Enemies);
	for (AActor* Enemy : Enemies)
	{
		Enemy->Destroy();
	}
}

void ABaseGameMode::RespawnEnemies()
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	for (FEnemySpawnInfo EnemyInfo : CachedEnemyInfo)
	{
		ABaseEnemy* SpawnedEnemy = GetWorld()->SpawnActor<ABaseEnemy>(
			EnemyInfo.EnemyClass,
			EnemyInfo.OriginTransform,
			SpawnParams);
	}
}


#pragma region KyuHyeok

void ABaseGameMode::PlayFade(bool bFadeIn)
{
	auto* InGameController = Cast<AInGameController>(PlayerController);
	check(InGameController);
	
	if (bFadeIn)
	{
		bIsFadeIn = true;
		SetPlayerInputEnable(true);
		InGameController->FadeIn(FadeDuration);
		GetWorldTimerManager().SetTimer(
			FadeTimer,
			this,
			&ABaseGameMode::OnFadeSequenceFinished,
			FadeDuration,
			false
			);
		EnterAudioComponent->Play();
	}
	else
	{
		bIsFadeIn = false;
		SetPlayerInputEnable(false);
		InGameController->FadeOut(FadeDuration);
		GetWorldTimerManager().SetTimer(
			FadeTimer,
			this,
			&ABaseGameMode::OnFadeSequenceFinished,
			FadeDuration,
			false
			);
		ExitAudioComponent->Play();
	}
}

void ABaseGameMode::PlayerLevelUp(FGameplayTag StatTag)
{
	if (IsValid(PlayerCharacter))
	{
		if (!IsValid(StatusComponent))
		{
			StatusComponent = PlayerCharacter->GetStatusComponent();
		}
		//레벨업 로직 추후 작성
	}
}

void ABaseGameMode::OnFadeSequenceFinished()
{
	if (bIsFadeIn)
	{
		bIsFadeIn = false;
		SetPlayerInputEnable(true);
	}
	else
	{
		if (bIsSameLevelMove)
		{
			PlayerCharacter->SetActorLocationAndRotation(PendingMoveLocation, PendingMoveRotation);
			FRotator NewRotation = PlayerCharacter->GetActorForwardVector().Rotation();
			PlayerController->SetControlRotation(NewRotation);
			CheckFogCrackAndOffFog();
			CheckSkyAtmosphereAndToggle();
			if (RecentCrackCache->GetIsBoss3Crack())
			{
				ToggleBoss3BattleRoom(true);
			}
			else if (!RecentCrackCache->GetIsBoss3Crack())
			{
				ToggleBoss3BattleRoom(false);
			}
			PlayFade(true);
		}
		else
		{
			UGameplayStatics::OpenLevel(this, FName(MoveTargetLevelName));
		}
	}
}

void ABaseGameMode::SaveItemDataToInstance()
{
	if (IsValid(PlayerCharacter))
	{
		if (!IsValid(ItemComponent))
		{
			ItemComponent = PlayerCharacter->GetItemComponent();
		}
		ItemComponent->UpdateItemInstanceSubsystem();
	}
}

void ABaseGameMode::UpdateInstanceData()
{
	// 플레이어 스탯 인스턴스 전달
	GameInstance->SetStatDataMap(PlayerCharacter->GetStatusComponent()->GetStatMap());
	// 플레이어 인벤토리 인스턴스 전달
	SaveItemDataToInstance();
	// 플레이타임 인스턴스 전달
	GameInstance->AddPlayTime(GetPlayTime());
}

void ABaseGameMode::CheckAIDormancy()
{
	if (!IsValid(World)) return;

	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(World, 0);
	if (!IsValid(PlayerPawn)) return;

	const FVector PlayerLocation = PlayerPawn->GetActorLocation();

	constexpr double DormancyOffDistSq = 5000.0 * 5000.0;
	constexpr double DormancyOnDistSq = 4500.0 * 4500.0;

	TArray<AActor*> Enemies;
	UGameplayStatics::GetAllActorsOfClass(World, ABaseEnemy::StaticClass(), Enemies);

	for (AActor* Actor : Enemies)
	{
		ABaseEnemy* Enemy = Cast<ABaseEnemy>(Actor);
		if (!IsValid(Enemy)) continue;

		// 보스 제외
		if (Enemy->IsA(ABaseBossEnemy::StaticClass())) continue;

		const double DistSq = FVector::DistSquared(PlayerLocation, Enemy->GetActorLocation());

		if (!Enemy->IsDormant() && DistSq > DormancyOffDistSq)
		{
			// 가드: Idle 상태이고 스킬 미사용 중일 때만 Dormancy 진입
			if (ABaseAIController* BaseAIC = Cast<ABaseAIController>(Enemy->GetController()))
			{
				if (UAIStateComponent* StateComp = BaseAIC->GetAIStateComponent())
				{
					if (StateComp->GetCurrentStateTag() != EffectTags::Idle) continue;
				}
				if (BaseAIC->IsUsingSkill()) continue;
			}

			Enemy->DormancyOff();
		}
		else if (Enemy->IsDormant() && DistSq < DormancyOnDistSq)
		{
			Enemy->DormancyOn();
		}
	}
}

void ABaseGameMode::PlayTimeAdder()
{
	PlayTime++;
}

void ABaseGameMode::CheckFogCrackAndOffFog()
{
	if (!IsValid(RecentCrackCache))
	{
		return;
	}

	AExponentialHeightFog* Fog = Cast<AExponentialHeightFog>(
		UGameplayStatics::GetActorOfClass(World, AExponentialHeightFog::StaticClass()));
	UExponentialHeightFogComponent* FogComponent;

	if (IsValid(Fog))
	{
		FogComponent = Fog->GetComponent();
		if (!FogComponent)
		{
			Debug::Print(TEXT("FogComponent is invalid"));
			return;
		}
	}
	else
	{
		Debug::Print(TEXT("Fog is invalid"));
		return;
	}

	if (RecentCrackCache->GetIsInFogCrack())
	{
		if (FogComponent->FogMaxOpacity == 1.f)
		{
			UCheatBPLib::ToggleFog(this);
		}
	}
	else
	{
		if (FogComponent->FogMaxOpacity == 0.f)
		{
			UCheatBPLib::ToggleFog(this);
		}
	}
}

void ABaseGameMode::DisappearBoss1Fog()
{
	if (!WorldInstanceSubsystem->GetCurrentLevelName().Contains("PastLevel"))
	{
		return;
	}

	if (!TargetPostProcessVolumes.IsEmpty())
	{
		Boss1FogProcessVolume = Cast<APostProcessVolume>(TargetPostProcessVolumes[0]);
		if (IsValid(Boss1FogProcessVolume))
		{
			FWeightedBlendable& Blendable = Boss1FogProcessVolume->Settings.WeightedBlendables.Array[0];
			MaterialInstanceDynamic = Cast<UMaterialInstanceDynamic>(Blendable.Object);

			if (!IsValid(MaterialInstanceDynamic))
			{
				MaterialInterface = Cast<UMaterialInterface>(Blendable.Object);
				if (IsValid(MaterialInterface))
				{
					MaterialInstanceDynamic = UMaterialInstanceDynamic::Create(MaterialInterface, this);

					Blendable.Object = MaterialInstanceDynamic;
				}
			}
		}
	}

	FogFadeStartTime = GetWorld()->GetTimeSeconds();
	FogFadeDuration = 5.f;
	GetWorldTimerManager().SetTimer(
		Boss1FogDisappearTimerHandle,
		this,
		&ABaseGameMode::FadeBoss1FogOut,
		0.032f,
		true);
}

void ABaseGameMode::FadeBoss1FogOut()
{
	float ElapsedTime = GetWorld()->GetTimeSeconds() - FogFadeStartTime;
	float Alpha = FMath::Clamp(ElapsedTime / FogFadeDuration, 0.f, 1.f);
	float CurrentFogIntensity;

	if (ElapsedTime <= 3.f)
	{
		float Phase1Alpha = ElapsedTime / 3.f;
		CurrentFogIntensity = FMath::Lerp(StartFogIntensity, 500000.f, Phase1Alpha);
	}
	else
	{
		float Phase2Alpha = (ElapsedTime - 3.f) / 2.f;
		CurrentFogIntensity = FMath::Lerp(StartFogIntensity, TargetFogIntensity, Phase2Alpha);
	}
	
	if (IsValid(MaterialInstanceDynamic))
	{
		MaterialInstanceDynamic->SetScalarParameterValue("FogDensityDistance", CurrentFogIntensity);

		if (Alpha >= 1.f)
		{
			GetWorldTimerManager().ClearTimer(Boss1FogDisappearTimerHandle);
			Boss1FogProcessVolume->Destroy();
		}
	}
}

void ABaseGameMode::ToggleBoss3BattleRoom(bool bIsBattleRoom)
{
	if (WorldInstanceSubsystem->GetCurrentLevelName() != "PastLevel")
	{
		return;
	}

	TArray<AActor*> PostProcessVolumes;
	UGameplayStatics::GetAllActorsOfClass(World, APostProcessVolume::StaticClass(), PostProcessVolumes);
	for (AActor* PostProcessVolume : PostProcessVolumes)
	{
		if (PostProcessVolume->ActorHasTag("MainPostProcessVolume"))
		{
			APostProcessVolume* WitchPostProcessVolume = Cast<APostProcessVolume>(PostProcessVolume);
			WitchPostProcessVolume->bEnabled = !bIsBattleRoom;
			WitchPostProcessVolume->SetActorHiddenInGame(bIsBattleRoom);
		}
	}

	if (bIsBattleRoom)
	{
		for (AActor* Actor : Boss3RoomNormalState)
		{
			if (Actor->IsA(ABoss3Skull::StaticClass()))
			{
				if (!IsValid(Boss3Skull))
				{
					Boss3Skull = Cast<
						ABoss3Skull>(UGameplayStatics::GetActorOfClass(World, ABoss3Skull::StaticClass()));
				}
				Boss3Skull->SetIsInBattleRoom(true);
				Boss3Skull->SetIsInteractable(false);
				UStaticMeshComponent* SkullMesh = Boss3Skull->GetSkullMeshComponent();
				UStaticMeshComponent* AltarMesh = Boss3Skull->GetAltarMeshComponent();
				SkullMesh->SetVisibility(false);
				SkullMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
				AltarMesh->SetVisibility(false);
				AltarMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			}
			else if (Actor->IsA(ASpotLight::StaticClass()))
			{
				USpotLightComponent* SpotLightComponent = Cast<ASpotLight>(Actor)->GetComponentByClass<
					USpotLightComponent>();
				if (IsValid(SpotLightComponent))
				{
					SpotLightComponent->SetHiddenInGame(true);
					SpotLightComponent->SetVisibility(false);
				}
			}
		}

		for (AActor* Actor : Boss3RoomBattleState)
		{
			if (Actor->IsA(ACrack::StaticClass()))
			{
				ACrack* Crack = Cast<ACrack>(Actor);
				if (static_cast<int32>(Crack->GetRequiredRevealStoryState()) != 0)
				{
					if (GameInstance->GetCurrentGameStoryState() < Crack->GetRequiredRevealStoryState())
					{
						continue;
					}
				}
			}
			else if (Actor->IsA(ARectLight::StaticClass()))
			{
				URectLightComponent* RectLightComponent = Cast<ARectLight>(Actor)->GetComponentByClass<URectLightComponent>();
				if (IsValid(RectLightComponent))
				{
					RectLightComponent->SetHiddenInGame(false);
					RectLightComponent->SetVisibility(true);
				}
				continue;
			}
			else if (Actor->IsA(ADyingHelper::StaticClass()))
			{
				ADyingHelper* DyingHelper = Cast<ADyingHelper>(Actor);
				if (!DyingHelper->GetActorStateComponent()->GetActorData().bIsDead && GameInstance->GetCurrentGameStoryState()>=EGameStoryState::Clear_Boss3)
				{
					DyingHelper->GetActorStateComponent()->SwitchStateAndUpdateInstance(WorldActorTags::DyingHelper);
				}
			}
			Actor->SetActorHiddenInGame(false);
			Actor->SetActorEnableCollision(true);
		}

		ADropEssence* CurrentDropEssence = WorldInstanceSubsystem->GetDropEssenceCache();
		if (IsValid(CurrentDropEssence))
		{
			CurrentDropEssence->SetActorHiddenInGame(false);
			CurrentDropEssence->SetIsInteractable(true);
		}
	}
	else if (!bIsBattleRoom)
	{
		for (AActor* Actor : Boss3RoomNormalState)
		{
			if (Actor->IsA(ABoss3Skull::StaticClass()))
			{
				if (!IsValid(Boss3Skull))
				{
					Boss3Skull = Cast<
						ABoss3Skull>(UGameplayStatics::GetActorOfClass(World, ABoss3Skull::StaticClass()));
				}
				Boss3Skull->SetIsInBattleRoom(false);
				Boss3Skull->SetIsInteractable(true);
				UStaticMeshComponent* SkullMesh = Boss3Skull->GetSkullMeshComponent();
				UStaticMeshComponent* AltarMesh = Boss3Skull->GetAltarMeshComponent();
				SkullMesh->SetVisibility(true);
				SkullMesh->SetCollisionProfileName(TEXT("BlockAll"));
				AltarMesh->SetVisibility(true);
				AltarMesh->SetCollisionProfileName(TEXT("BlockAll"));
				Boss3Skull->CheckStoryStateAndToggleVisibility();
			}
			else if (Actor->IsA(ASpotLight::StaticClass()))
			{
				USpotLightComponent* SpotLightComponent = Cast<ASpotLight>(Actor)->GetComponentByClass<
					USpotLightComponent>();
				SpotLightComponent->SetHiddenInGame(false);
				SpotLightComponent->SetVisibility(true);
			}
		}

		for (AActor* Actor : Boss3RoomBattleState)
		{
			if (Actor->IsA(ARectLight::StaticClass()))
			{
				URectLightComponent* RectLightComponent = Cast<ARectLight>(Actor)->GetComponentByClass<URectLightComponent>();
				if (IsValid(RectLightComponent))
				{
					RectLightComponent->SetHiddenInGame(true);
					RectLightComponent->SetVisibility(false);
				}
				continue;
			}
			else if (Actor->IsA(ADyingHelper::StaticClass()))
			{
				ADyingHelper* DyingHelper = Cast<ADyingHelper>(Actor);
				if (!DyingHelper->GetActorStateComponent()->GetActorData().bIsDead && GameInstance->GetCurrentGameStoryState()>=EGameStoryState::Clear_Boss3)
				{
					DyingHelper->GetActorStateComponent()->SwitchStateAndUpdateInstance(WorldActorTags::DyingHelper);
				}
			}
			Actor->SetActorHiddenInGame(true);
			Actor->SetActorEnableCollision(false);
		}

		ADropEssence* CurrentDropEssence = WorldInstanceSubsystem->GetDropEssenceCache();
		if (IsValid(CurrentDropEssence))
		{
			CurrentDropEssence->SetActorHiddenInGame(true);
			CurrentDropEssence->SetIsInteractable(false);
		}
	}
}

void ABaseGameMode::CheckSkyAtmosphereAndToggle(ATeleporter* Teleporter)
{
	if (WorldInstanceSubsystem->GetCurrentLevelName().Contains("Past"))
	{
		ASkyAtmosphere* SkyAtmosphere = Cast<ASkyAtmosphere>(
			UGameplayStatics::GetActorOfClass(World, ASkyAtmosphere::StaticClass()));
		check(IsValid(SkyAtmosphere));

		if (Teleporter != nullptr)
		{
			// 던전으로 가는 텔레포터인 경우
			if (Teleporter->GetShouldOffSkyAtmosphere())
			{
				SkyAtmosphere->SetActorHiddenInGame(true);
				SkyAtmosphere->GetComponent()->SetVisibility(false);
			}
			// 던전을 벗어나는 텔레포터인 경우
			else if (Teleporter->GetShouldOnSkyAtmosphere())
			{
				SkyAtmosphere->SetActorHiddenInGame(false);
				SkyAtmosphere->GetComponent()->SetVisibility(true);
			}
			return;
		}

		// 최근 균열이 Sky를 끄는 균열이면
		if (RecentCrackCache->GetShouldOffSkyAtmosphere())
		{
			SkyAtmosphere->SetActorHiddenInGame(true);
			SkyAtmosphere->GetComponent()->SetVisibility(false);
		}
		// SkyAtmosphere를 끄지 않는 균열일 경우
		else
		{
			SkyAtmosphere->SetActorHiddenInGame(false);
			SkyAtmosphere->GetComponent()->SetVisibility(true);
		}
	}
	else
	{
		return;
	}
}

void ABaseGameMode::SetPlayerInputEnable(bool bEnable)
{
	if (!IsValid(PlayerCharacter))
	{
		Debug::Print(TEXT("PlayerCharacter is not valid"));
		return;
	}

	if (bEnable)
	{
		PlayerController->SetIgnoreMoveInput(false);
		PlayerController->SetIgnoreLookInput(false);
		PlayerController->EnableInput(PlayerController);

		FInputModeGameOnly InputMode;
		PlayerController->SetInputMode(InputMode);
	}
	else
	{
		PlayerController->SetIgnoreMoveInput(true);
		PlayerController->SetIgnoreLookInput(true);
		PlayerController->DisableInput(PlayerController);

		FInputModeUIOnly InputMode;
		PlayerController->SetInputMode(InputMode);
	}
}

#pragma endregion


#pragma region SeoYoung



#pragma endregion
