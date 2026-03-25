// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../Damagable.h"
#include "Interface/PawnTagInterface.h"
#include "../EffectReceivable.h"
#include "Components/PlayerControlComponent/ControlComponentUser.h"
#include "Components/PlayerControlComponent/PlayerControlComponent.h"
#include "Components/StatusComponent/StatusComponentUser.h"
#include "Components/SkillComponent/SkillComponentUser.h"
#include "Interface/EffectUser.h"
#include "Interface/Parryable.h"
#include "DomiCharacter.generated.h"

class UStatusComponent;
class USkillComponent;
class UItemComponent;
class UPlayerControlComponent;
class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
struct FAttackData;

class UMnhTracerComponent;
class UMnhBoxComponent;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnAddInteractableActor, TSet<AActor*>)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnRemoveInteractableActor, TSet<AActor*>)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnInteractionWidgetScroll, float)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnUpdateEffectUIDataArray, TArray<FEffectUIData>);

UCLASS()
class DOMINIONPROTOCOL_API ADomiCharacter :
public ACharacter,
public IDamagable,
public IPawnTagInterface,
public IEffectReceivable,
public IControlComponentUser,
public IStatusComponentUser,
public ISkillComponentUser,
public IParryable,
public IEffectUser
{
	GENERATED_BODY()
	
public:
	ADomiCharacter();

	// Delegates
	FOnAddInteractableActor OnAddInteractableActor;
	FOnRemoveInteractableActor OnRemoveInteractableActor;
	FOnInteractionWidgetScroll OnInteractionWidgetScroll;
	FOnUpdateEffectUIDataArray OnUpdateEffectUIDataArray;
	
	//Getter
	UFUNCTION(BlueprintCallable)
	AActor* GetCurrentInteractableActor() const;
	FORCEINLINE const TSet<AActor*>& GetInteractableActorSet() const { return InteractableActorSet; }
	
	//Setter
	UFUNCTION(BlueprintCallable)
	FORCEINLINE void SetCurrentInteractableActor(AActor* NewActor) { InteractableActor = NewActor; }

	void AddInteractableActor(AActor* AddInteractableActor);
	void RemoveInteractableActor(AActor* RemoveInteractableActor);

	// Camera
	void ToggleCamareLagEnable();

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	virtual void Landed(const FHitResult& HitResult) override;
	
	// ControlComponentUser
	virtual FGameplayTagContainer& GetActiveControlEffectTags() override;
	FORCEINLINE virtual UPlayerControlComponent* GetPlayerControlComponent() const override { return ControlComponent; }
	virtual const FVector& GetCurrentMovementVector() const override;
	virtual void SkillStart(FGameplayTag ControlEffectTag) override;
	virtual void SkillEnd(FGameplayTag ControlEffectTag) override;

	// StatusComponentUser
	FORCEINLINE virtual UStatusComponent* GetStatusComponent() const override { return StatusComponent; }
	virtual void InitializeStatusComponent() override;
	virtual FGameplayTagContainer& GetActiveStatusEffectTags() override;
	
	// SkillComponentUser
	FORCEINLINE virtual USkillComponent* GetSkillComponent() const override { return SkillComponent; }
	virtual void InitializeSkillComponent() override;
	virtual void ExecuteSkill(FGameplayTag SkillGroupTag) override;

	// ItemComponent (Not User)
	FORCEINLINE UItemComponent* GetItemComponent() const { return ItemComponent; }
	
	// Damagable
	virtual void OnAttacked_Implementation(const FAttackData& AttackData) override;

	// PawnTagInterface
	virtual FGameplayTag GetPawnTag_Implementation() override;
	virtual FString GetPawnName_Implementation() override;

	// EffectReceivable (for debugging)
	virtual void ShowControlEffectTags_Implementation() override;
	virtual void ShowStatusEffectTags_Implementation() override;
	virtual void ActivateStatusEffect_Implementation(const FGameplayTag& EffectTag) override;

	// Parryable
	virtual bool IsParryingCond_Implementation() override;
	virtual void OnParried_Implementation() override;
	// virtual void OnParrySuccess_Implementation() override;
	virtual AActor* GetTargetEnemy_Implementation() override;

	// EffectUser
	virtual FGameplayTagContainer GetAllActivateEffectDatas_Implementation() const override;
	virtual void SendEffectUIDatas_Implementation() const override;

	// UI
	void EventInteractionWidgetScroll(const float Value);

	// Animation Montageplay
	UFUNCTION(BlueprintImplementableEvent)
	void PlayWeaponSwapAnimMontage();

	UFUNCTION(BlueprintImplementableEvent)
	void PlayHitFrontAnimMontage(float Duration);

	UFUNCTION(BlueprintImplementableEvent)
	void PlayHitRightAnimMontage(float Duration);

	UFUNCTION(BlueprintImplementableEvent)
	void PlayHitLeftAnimMontage(float Duration);

	UFUNCTION(BlueprintImplementableEvent)
	void PlayHitBackAnimMontage(float Duration);

	UFUNCTION(BlueprintImplementableEvent)
	void PlayInteractAnimMontage();

	UFUNCTION(BlueprintImplementableEvent)
	void PlayHitSound();
	
	UFUNCTION(BlueprintImplementableEvent)
	void PlayHitVoice();

	UFUNCTION(BlueprintImplementableEvent)
	void PlayDeathVoice();
	
	// WeaopnMeshChange
	UFUNCTION(BlueprintImplementableEvent)
	void ChangePrimaryWeapon(const FGameplayTag& WeaponTag);

	UFUNCTION(BlueprintImplementableEvent)
	void ShowParryWall();

	UFUNCTION(BlueprintImplementableEvent)
	void HideParryWall();

	// On Attack Detected
	UFUNCTION(BlueprintCallable)
	void PlayEffectsOnMnhAttack(const FHitResult& HitResult);

	bool IsInvincible();

protected:
	virtual void BeginPlay() override;
	
	// Bind Matched Input Functions
	void BindInputFunctions();
	virtual void NotifyControllerChanged() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void OnDeath();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UPlayerControlComponent> ControlComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStatusComponent> StatusComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkillComponent> SkillComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UItemComponent* ItemComponent;

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "States", meta = (AllowPrivateAccess = "true"))
	FGameplayTagContainer InvincibilityTags;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tag", meta = (AllowPrivateAccess = "true"))
	FGameplayTag PawnTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Teleport")
	TObjectPtr<UStaticMeshComponent> StartPoint;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Teleport")
	TObjectPtr<UStaticMeshComponent> BeginTrace;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Teleport")
	TObjectPtr<UStaticMeshComponent> DownTrace;

	FString PlayerName;

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<AActor> InteractableActor;

	UPROPERTY()
	TSet<AActor*> InteractableActorSet = {};
};

