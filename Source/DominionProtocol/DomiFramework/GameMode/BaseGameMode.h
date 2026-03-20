// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LevelSequence.h"
#include "GameFramework/GameMode.h"
#include "DomiFramework/GameInstance/DomiGameInstance.h"
#include "DomiFramework/GameInstance/SaveManagerSubsystem.h"
#include "BaseGameMode.generated.h"

class ABoss3Skull;
class ATeleporter;
class UItemComponent;
class ADropEssence;
class ALevelSequenceActor;
class ULevelSequencePlayer;
class UStatusComponent;
class UItemInstanceSubsystem;
class UWorldInstanceSubsystem;
class ABaseGameState;
class ADomiCharacter;
class ACrack;
struct FGameplayTag;

USTRUCT(BlueprintType)
struct FEnemySpawnInfo
{
	GENERATED_BODY()

	UPROPERTY()
	TSubclassOf<AActor> EnemyClass;

	UPROPERTY()
	FTransform OriginTransform;
};

DECLARE_MULTICAST_DELEGATE(FOnPlayerSpawn);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnStartBattle, AActor*);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnEndBattle, AActor*);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnSpawnDropEssence, ADropEssence*);

UCLASS()
class DOMINIONPROTOCOL_API ABaseGameMode : public AGameMode
{
	GENERATED_BODY()

#pragma region Share
	
public:
	ABaseGameMode();

	virtual void BeginPlay() override;
	virtual void StartPlay() override;
	
	virtual void StartBattle(AActor* SpawnedBoss);
	virtual void EndBattle(AActor* DeadMonster = nullptr);

	virtual void OnPlayerDeath();

	UFUNCTION(BlueprintCallable)
	void Save();

	//긴급탈출 : UI에서 호출
	UFUNCTION(BlueprintCallable)
	void MoveToRecentCrack();
	
	UFUNCTION()
	void SaveItemDataToInstance();
	
	void RestorePlayer();
	
	void RespawnPlayerCharacter();

	//Boss3Skull에서 호출
	void ToggleBoss3BattleRoom(bool bIsInBattleRoom);
	
	//UI 쪽에서 레벨과 균열인덱스를 정하면 해당 함수를 호출하도록 하면 됩니다
	UFUNCTION(BlueprintCallable)
	void MoveToTargetCrack(FString InOwningCrackLevelName, int32 InCrackIndex);
	
	//균열 쪽에서 호출할 함수
	void DestroyAllNormalEnemy();	// 기존 적들 제거
	void RespawnEnemies();	// 적 기존 위치에 리스폰

	//Teleporter에서 호출
	void CheckSkyAtmosphereAndToggle(ATeleporter* Teleporter = nullptr);

	//Getter
	FORCEINLINE int32 GetPlayTime() { return PlayTime; }
	
	//Setter
	FORCEINLINE void SetRecentCrackCache(ACrack* NewCrack) { RecentCrackCache = NewCrack ; }

	void CheckAIDormancy();

protected:
	void PlayTimeAdder();

	void CheckFogCrackAndOffFog();

	void DisappearBoss1Fog();

	void FadeBoss1FogOut();

	void UpdateInstanceData();
	
public:
	// Delegate
	FOnPlayerSpawn OnPlayerSpawn;
	FOnStartBattle OnStartBattle;
	FOnEndBattle OnEndBattle;
	FOnSpawnDropEssence OnSpawnDropEssence;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Spawning")
	TSubclassOf<ADropEssence> DropEssenceClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UAudioComponent> EnterAudioComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UAudioComponent> ExitAudioComponent;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TObjectPtr<USoundWave> EnterSound;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TObjectPtr<USoundWave> ExitSound;
	
	UPROPERTY()
	TObjectPtr<UDomiGameInstance> GameInstance;

	UPROPERTY()
	TObjectPtr<UWorldInstanceSubsystem> WorldInstanceSubsystem;

	UPROPERTY()
	TObjectPtr<UItemInstanceSubsystem> ItemInstanceSubsystem;

	UPROPERTY()
	TObjectPtr<ABaseGameState> BaseGameState;
	
	UPROPERTY()
	TObjectPtr<UWorld> World;

	UPROPERTY()
	TObjectPtr<APlayerController> PlayerController;

	UPROPERTY()
	TObjectPtr<ADomiCharacter> PlayerCharacter;

	UPROPERTY()
	TObjectPtr<UStatusComponent> StatusComponent;

	UPROPERTY()
	TObjectPtr<UItemComponent> ItemComponent;
	
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<ACrack> RecentCrackCache;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float RespawnDelay;

	UPROPERTY()
	TObjectPtr<ABoss3Skull> Boss3Skull;
	
	UPROPERTY()
	TArray<FEnemySpawnInfo> CachedEnemyInfo;

	UPROPERTY()
	TArray<AActor*> Boss3RoomNormalState;

	UPROPERTY()
	TArray<AActor*> Boss3RoomBattleState;

	UPROPERTY()
	TArray<AActor*> TargetPostProcessVolumes;

	UPROPERTY()
	TObjectPtr<APostProcessVolume> Boss1FogProcessVolume;

	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> MaterialInstanceDynamic;

	UPROPERTY()
	TObjectPtr<UMaterialInterface> MaterialInterface;

	bool bIsSameLevelMove;

	FName MoveTargetLevelName;
	
	FVector PendingMoveLocation;
	
	FRotator PendingMoveRotation;

	int32 PlayTime;

	FTimerHandle PlayTimer;

	FTimerHandle DormancyCheckTimer;

	FTimerHandle Boss1FogDisappearTimerHandle;

	float FogFadeStartTime;

	float FogFadeDuration;

	float StartFogIntensity;
	
	float TargetFogIntensity;

	FVector PlayerDeathLocation;


#pragma endregion

	
#pragma region KyuHyeok

public:
	UFUNCTION(BlueprintCallable)
	void PlayFade(bool bFadeIn);
	
	void PlayerLevelUp(FGameplayTag StatTag);
	
	void SetPlayerInputEnable(bool bEnable);
	
protected:
	UFUNCTION()
	void OnFadeSequenceFinished();

	UPROPERTY()
	ALevelSequenceActor* SequenceActor;

	UPROPERTY()
	bool bIsFadeIn;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float FadeDuration;

	FTimerHandle FadeTimer;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float AssetLoadDelay;
		
	FTimerHandle AssetLoadTimer;
	
#pragma endregion


#pragma region SeoYoung

#pragma endregion
};
