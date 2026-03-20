// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/LockOnComponent/LockOnComponent.h"
#include "Components/SkillComponent/SkillComponentUser.h"
#include "Components/StatusComponent/StatusComponentUser.h"
#include "GameFramework/Character.h"
#include "Interface/EffectUser.h"
#include "Interface/Parryable.h"
#include "Util/GameTagList.h"
#include "Player/Damagable.h"
#include "Interface/PawnTagInterface.h"
#include "Player/EffectReceivable.h"
#include "BaseEnemy.generated.h"

class AEssence;
class USkillComponent;
class UStatusComponent;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnEffectUIDataArray, TArray<FEffectUIData>&);

UCLASS()
class DOMINIONPROTOCOL_API ABaseEnemy :
public ACharacter,
public IDamagable,
public IParryable,
public IPawnTagInterface,
public IEffectReceivable,
public IStatusComponentUser,
public ISkillComponentUser,
public IEffectUser
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseEnemy();

	FOnEffectUIDataArray OnEffectUIDataArray;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI|State")
	bool bIsAttacking = true;

	// Death Handler
	UFUNCTION(BlueprintNativeEvent)
	void OnDeath();
	
	virtual void OnDeath_Implementation();

	// Groggy Handler
	virtual void OnGroggy();
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	// StatusComponentUser
	FORCEINLINE virtual UStatusComponent* GetStatusComponent() const override { return StatusComponent; }
	virtual void InitializeStatusComponent() override;
	virtual FGameplayTagContainer& GetActiveStatusEffectTags() override;

	// SkillComponentUser
	FORCEINLINE virtual USkillComponent* GetSkillComponent() const override { return SkillComponent; }
	virtual void InitializeSkillComponent() override;
	virtual void ExecuteSkill(FGameplayTag SkillGroupTag) override;
	
	// Damagable
	virtual void OnAttacked_Implementation(const FAttackData& AttackData) override;

	// Parryable
	virtual bool IsParryingCond_Implementation() override;
	virtual void OnParried_Implementation() override;
	
	// EffectReceivable (for debug)
	virtual void ShowControlEffectTags_Implementation() override;
	virtual void ShowStatusEffectTags_Implementation() override;
	virtual void ActivateStatusEffect_Implementation(const FGameplayTag& EffectTag) override;

	// EffectUser
	virtual FGameplayTagContainer GetAllActivateEffectDatas_Implementation() const override;
	virtual void SendEffectUIDatas_Implementation() const override;
	
	// PawnTagInterface
	virtual FString GetPawnName_Implementation() override;
	virtual FGameplayTag GetPawnTag_Implementation() override;

	// Dormancy
	UFUNCTION(BlueprintCallable, Category = "AI|Dormancy")
	virtual void DormancyOn();

	UFUNCTION(BlueprintCallable, Category = "AI|Dormancy")
	virtual void DormancyOff();

	UFUNCTION(BlueprintCallable, Category = "AI|Dormancy")
	FORCEINLINE bool IsDormant() const { return bIsDormant; }

	// LockOnComponent
	FORCEINLINE FVector GetLockOnLocation() const { return LockOnComponent->GetComponentLocation(); }

	FString GetMonsterName() const { return MonsterName; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStatusComponent> StatusComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkillComponent> SkillComponent;

	// LockOnComponent: LockOn할 위치 블루프린트에서 위치 수정
	UPROPERTY(EditDefaultsOnly, BlueprintReadwrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<ULockOnComponent> LockOnComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tag", meta = (AllowPrivateAccess = "true"))
	FGameplayTag PawnTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "States", meta = (AllowPrivateAccess = "true"))
	FGameplayTagContainer InvincibilityTags;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Spawning")
	TSubclassOf<AEssence> EssenceClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Property")
	int32 EssenceAmount;
	
	// UI Section
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UDomiWidgetComponent> HPWidgetComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UDomiWidgetComponent> LockOnWidgetComponent;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FString MonsterName;

	bool bIsDormant = false;

	// Dormancy 진입 전 Tick이 활성화되어 있던 컴포넌트 목록 (복원용)
	TArray<TWeakObjectPtr<UActorComponent>> CachedTickingComponents;
};
