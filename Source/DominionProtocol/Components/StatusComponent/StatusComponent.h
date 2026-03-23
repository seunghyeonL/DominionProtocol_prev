// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnumAndStruct/EffectData/EffectUIData.h"
#include "EnumAndStruct/PlayerStatData/PlayerStatData.h"
#include "Util/GameTagList.h"
#include "Util/DebugHelper.h"
#include "StatusComponent.generated.h"

//UI Delegate
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHealthChanged, float, Health);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMaxHealthChanged, float, MaxHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStaminaChanged, float, Stamina);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMaxStaminaChanged, float, MaxStamina);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnShieldChanged, float, Shield);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttackPowerChanged, float, AttackPower);

DECLARE_MULTICAST_DELEGATE_OneParam(FOnMaxShieldChanged, float);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnBattleMonster, AActor*);
DECLARE_MULTICAST_DELEGATE(FOnStatusEffectsChanged);
DECLARE_MULTICAST_DELEGATE(FOnDeath);

DECLARE_DELEGATE(FOnSpawned);
DECLARE_DELEGATE(FOnGroggy);

class UStatusEffectBase;
struct FStatusComponentInitializeData;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DOMINIONPROTOCOL_API UStatusComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UStatusComponent();

	// Delegates
	FOnDeath OnDeath;
	FOnGroggy OnGroggy;
	FOnSpawned OnSpawned;
	FOnStatusEffectsChanged OnStatusEffectsChanged;

	UPROPERTY(BlueprintAssignable, Category = "Stats|Events")
	FOnHealthChanged OnHealthChanged;

	UPROPERTY(BlueprintAssignable, Category = "Stats|Events")
	FOnHealthChanged OnMaxHealthChanged;

	UPROPERTY(BlueprintAssignable, Category = "Stats|Events")
	FOnStaminaChanged OnStaminaChanged;

	UPROPERTY(BlueprintAssignable, Category = "Stats|Events")
	FOnStaminaChanged OnMaxStaminaChanged;

	UPROPERTY(BlueprintAssignable, Category = "Stats|Events")
	FOnShieldChanged OnShieldChanged;

	UPROPERTY(BlueprintAssignable, Category = "Stats|Events")
	FOnAttackPowerChanged OnAttackPowerChanged;

	FOnMaxShieldChanged OnMaxShieldChanged;
	FOnBattleMonster OnBattleMonster;
	
	// Combat State
	FTimerHandle CombatTimer;
	
	FORCEINLINE bool IsInCombat() const { return bIsInCombat; }
	FORCEINLINE void SetIsInCombat(const bool Flag) { bIsInCombat = Flag; }
	void StartCombat();
	void EndCombat();
	
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	FORCEINLINE FGameplayTagContainer& GetActiveStatusEffectTags() { return ActiveStatusEffectTags; }

	TArray<FEffectUIData> GetEffectUIDatas();

	UFUNCTION(BlueprintCallable)
	const TMap<FGameplayTag, float>& GetStatMap() const { return StatMap; }
	
	FORCEINLINE void SetStatMap(const TMap<FGameplayTag, float>& NewStatMap) { StatMap = NewStatMap; }
	
	UFUNCTION()
	const TMap<FGameplayTag, UStatusEffectBase*>& GetStatusEffectMap() const { return StatusEffectMap; }
	
	float GetStat(const FGameplayTag& StatTag) const;
	void SetStat(const FGameplayTag& StatTag, float Value);

	// Delegate Handler
	void OnPrimaryWeaponChanged();
	void OnSecondaryWeaponChanged();
	
	// stat upgrade with ui
	void GetPlayerStatData(TMap<FGameplayTag, float>& UIPlayerStatData);
	void UpdateStatPreviewData(TMap<FGameplayTag, float>& UIPlayerStatData);
	void DecideStatChangeFromUI(const TMap<FGameplayTag, float>& UIPlayerStatData);
	float GetLevelUpRequiredEssence(const float InLevel) const;

	float GetCalculatedBattleStat(const FGameplayTag& StatTag, const TMap<FGameplayTag, float>& InStatMap) const;
	float GetMaxVariableStat(const FGameplayTag& StatTag) const;

	// 소프트캡 적용 스케일링: SoftCap 이전은 Coefficient × √Stat, 이후는 효율 감소
	float GetSoftCappedScaling(float StatValue, float Coefficient) const;

	// Getter/Setter with Delegate
	UFUNCTION(BlueprintCallable)
	void SetHealth(const float NewHealth);

	UFUNCTION(BlueprintCallable)
	void SetGroggyGauge(const float NewGroggyGauge);

	void SetMaxHealth(const float NewMaxHealth);
	void SetShield(const float NewShield);
	void SetStamina(float NewHealth);
	void SetMaxStamina(float NewMaxStamina);
	void SetAttackPower(float NewAttackPower);
	
	bool HasEnoughStamina(float RequiredAmount) const;
	void ConsumeStamina(float Amount);
	void StartStaminaRecovery();
	void StopStaminaRecovery();
	
	void InitializeStatusComponent(const FStatusComponentInitializeData& InitializeData);
	void RemoveActiveStatusEffect(const FGameplayTag StatusEffectTag);
	
	virtual void ActivateStatusEffect(const FGameplayTag& StatusEffectTag, const float Magnitude);
	virtual void ActivateStatusEffect(const FGameplayTag& StatusEffectTag, const float Magnitude, float Duration);
	virtual void DeactivateStatusEffect(const FGameplayTag& StatusEffectTag);
	
	void AddImmuneStatusEffect(const FGameplayTag& StatusEffectTag);
	void RemoveImmuneStatusEffect(const FGameplayTag& StatusEffectTag);

	//CheatManager
	FORCEINLINE void SwitchInfiniteStaminaMode() { bIsInfiniteStaminaMode = !bIsInfiniteStaminaMode; }
	FORCEINLINE void SwitchGodMode() { bIsGodMode = !bIsGodMode; }
	FORCEINLINE bool GetIsGodMode() { return bIsGodMode; }
	
protected:
	virtual void BeginPlay() override;
	virtual void InitializeComponent() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	float CombatDuration;

	bool bIsInCombat;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stats", meta=(AllowPrivateAccess=true))
	TMap<FGameplayTag, float> StatMap;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stats", meta=(AllowPrivateAccess=true))
	TMap<FGameplayTag, float> StatMultiplierMap;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stats", meta=(AllowPrivateAccess=true))
	TMap<FGameplayTag, UStatusEffectBase*> StatusEffectMap;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Stats", meta=(AllowPrivateAccess=true))
	FGameplayTagContainer ActiveStatusEffectTags;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats", meta = (AllowPrivateAccess = true))
	FGameplayTagContainer ImmuneStatusEffectTags;
	
	UPROPERTY(EditAnywhere, Category = "Stamina")
	float StaminaRecoveryRate;

	bool bIsRecoveringStamina = false;
	
	//CheatManager
	bool bIsInfiniteStaminaMode = false;
	bool bIsGodMode = false;

};
