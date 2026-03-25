// Copyright Epic Games, Inc. All Rights Reserved.

#include "DomiCharacter.h"
#include "Util/DebugHelper.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "../InGameController.h"
#include "Util/BattleDataTypes.h"
#include "Components/PlayerControlComponent/PlayerControlComponent.h"

#include "Components/StatusComponent/StatusComponent.h"
#include "Components/ItemComponent/ItemComponent.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "NiagaraFunctionLibrary.h"

#include "Components/StatusComponent/StatusComponentInitializeData.h"
#include "Components/SkillComponent/SkillComponent.h"
#include "Components/SkillComponent/SkillComponentInitializeData.h"

#include "DomiFramework/GameMode/BaseGameMode.h"
#include "DomiFramework/GameState/BaseGameState.h"

#include "../Plugins/MissNoHit/Source/MissNoHit/Public/MnhTracerComponent.h"
#include "../Plugins/MissNoHit/Source/MissNoHit/Public/MnhComponents.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"
#include "EnumAndStruct/PhysicalSurfaceTypeData/PhysicalSurfaceTypeData.h"
#include "Util/AsyncLoadBPLib.h"


class UPoisonEffect;
//////////////////////////////////////////////////////////////////////////
// ADomiCharacter

ADomiCharacter::ADomiCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller
	CameraBoom->bEnableCameraLag = true; // Camera follow pawn smoothly
	CameraBoom->CameraLagSpeed = 5.0f;
	

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	// Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Battle Components
	ControlComponent = CreateDefaultSubobject<UPlayerControlComponent>(TEXT("ControlComponent"));
	StatusComponent = CreateDefaultSubobject<UStatusComponent>(TEXT("StatusComponent"));
	SkillComponent = CreateDefaultSubobject<USkillComponent>(TEXT("SkillComponent"));
	ItemComponent = CreateDefaultSubobject<UItemComponent>(TEXT("ItemComponent"));

	// InvincibilityTags Setting
	InvincibilityTags.AddTag(EffectTags::Death);
	InvincibilityTags.AddTag(EffectTags::UsingZoneya);
	InvincibilityTags.AddTag(EffectTags::StandingUp);
	InvincibilityTags.AddTag(EffectTags::DashInvincible);
	
	// Set PawnTag
	PawnTag = PawnTags::Player;

	PlayerName = TEXT("DefaultPlayer");

	GetMesh()->bReceivesDecals = false;

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
	UE_LOG(LogTemp, Warning, TEXT("ObjectType: %d"), (int32)GetCapsuleComponent()->GetCollisionObjectType());
}

AActor* ADomiCharacter::GetCurrentInteractableActor() const
{
	if (InteractableActor == nullptr)
	{
		Debug::Print("ADomiCharacter::GetCurrentInteractableObject : InteractableObject == nullptr");
		return nullptr;
	}
	else
	{
		return InteractableActor;
	}
}

void ADomiCharacter::AddInteractableActor(AActor* AddInteractableActor)
{
	if (!InteractableActorSet.Contains(AddInteractableActor))
	{
		InteractableActorSet.Add(AddInteractableActor);
		OnAddInteractableActor.Broadcast(InteractableActorSet);	
	}
}

void ADomiCharacter::RemoveInteractableActor(AActor* RemoveInteractableActor)
{
	if (InteractableActorSet.Contains(RemoveInteractableActor))
	{
		InteractableActorSet.Remove(RemoveInteractableActor);
		OnRemoveInteractableActor.Broadcast(InteractableActorSet);	
	}
}

void ADomiCharacter::ToggleCamareLagEnable()
{
	check(CameraBoom);

	CameraBoom->bEnableCameraLag = !CameraBoom->bEnableCameraLag;
}

//////////////////////////////////////////////////////////////////////////
// Input

void ADomiCharacter::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();

	// Add Input Mapping Context
	if (auto PlayerController = Cast<AInGameController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<
			UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			if (IsValid(PlayerController->DefaultMappingContext))
			{
				Subsystem->AddMappingContext(PlayerController->DefaultMappingContext, 0);
			}
			else
			{
				Debug::PrintError(TEXT("ADomiCharacter::NotifyControllerChanged : Invalid MappingContext."));
			}
		}
	}
}

void ADomiCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	InputComponent = PlayerInputComponent;
	BindInputFunctions();

	// if (ControlComponent->bIsComponentReady)
	// {
	// 	BindInputFunctions();
	// }
	// ControlComponent->OnComponentReady.BindUObject(this, &ADomiCharacter::BindInputFunctions);
}

void ADomiCharacter::PlayEffectsOnMnhAttack(const FHitResult& HitResult)
{
	check(StatusComponent);
	
	const FVector& HitLocation = HitResult.ImpactPoint;
	const FRotator HitRotation = HitResult.ImpactNormal.Rotation();
	
	EPhysicalSurface SurfaceType = SurfaceType_Default;
	if (UPhysicalMaterial* PhysMat = HitResult.PhysMaterial.Get())
	{
		SurfaceType = PhysMat->SurfaceType;
	}
	
	if (auto* GS = GetWorld()->GetGameState<ABaseGameState>())
	{
		if (FPhysicalSurfaceTypeData* SurfaceTypeData = GS->GetPhysicalSurfaceTypeData(SurfaceType))
		{
			FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
			
			float PitchMultiplier = 1.f;
			auto PrimaryWeaponTag = ItemComponent->GetEquippedItem(FName("WeaponSlot_Primary"));
			if (PrimaryWeaponTag.IsValid())
			{
				if (PrimaryWeaponTag.MatchesTag(ItemTags::ClawWeapon))
				{
					PitchMultiplier = 1.2f;
				}
				else if (PrimaryWeaponTag.MatchesTag(ItemTags::AxeWeapon))
				{
					PitchMultiplier = 0.8f;
				}
			}

			// HitSound 실행
			if (IsValid(SurfaceTypeData->HitSound))
			{
				UGameplayStatics::PlaySoundAtLocation(this, SurfaceTypeData->HitSound, HitLocation, FRotator(0.f), 0.7f, PitchMultiplier);
			}
			
			// HitVfx 실행
			if (IsValid(SurfaceTypeData->HitVfx))
			{
				UNiagaraFunctionLibrary::SpawnSystemAtLocation(
					this,
					SurfaceTypeData->HitVfx,
					HitLocation,
					HitRotation,
					FVector(1.f),
					true,
					true
				);
			}
		}
	}
}

bool ADomiCharacter::IsInvincible()
{
	check(ControlComponent)
	auto& ActiveControlEffects = GetActiveControlEffectTags();
	return ActiveControlEffects.HasAny(InvincibilityTags);
}

void ADomiCharacter::BeginPlay()
{
	Super::BeginPlay();

	check(IsValid(ItemComponent));

	ItemComponent->OnPrimaryWeaponChangedForWeaponVisibility.BindUObject(this, &ADomiCharacter::ChangePrimaryWeapon);
}

void ADomiCharacter::BindInputFunctions()
{
	if (auto PlayerController = Cast<AInGameController>(Controller))
	{
		// Set up action bindings
		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
		{
			// Moving
			if (IsValid(PlayerController->MoveAction))
			{
				EnhancedInputComponent->BindAction(PlayerController->MoveAction, ETriggerEvent::Triggered,
				                                   ControlComponent.Get(),
				                                   &UPlayerControlComponent::Move);

				EnhancedInputComponent->BindAction(PlayerController->MoveAction, ETriggerEvent::Completed,
												   ControlComponent.Get(),
												   &UPlayerControlComponent::ResetLastMovementVector);
			}

			// Looking
			if (IsValid(PlayerController->LookAction))
			{
				EnhancedInputComponent->BindAction(PlayerController->LookAction, ETriggerEvent::Triggered,
				                                   ControlComponent.Get(),
				                                   &UPlayerControlComponent::Look);
			}

			// Dash
			if (IsValid(PlayerController->DashAction))
			{
				EnhancedInputComponent->BindAction(PlayerController->DashAction, ETriggerEvent::Started,
				                                   ControlComponent.Get(),
				                                   &UPlayerControlComponent::Dash);
			}

			// Dash
			if (IsValid(PlayerController->SprintAction))
			{
				EnhancedInputComponent->BindAction(PlayerController->SprintAction, ETriggerEvent::Started,
				                                   ControlComponent.Get(),
				                                   &UPlayerControlComponent::SprintStart);
				EnhancedInputComponent->BindAction(PlayerController->SprintAction, ETriggerEvent::Completed,
												   ControlComponent.Get(),
												   &UPlayerControlComponent::SprintEnd);
			}

			// Parry
			if (IsValid(PlayerController->ParryAction))
			{
				EnhancedInputComponent->BindAction(PlayerController->ParryAction, ETriggerEvent::Started,
				                                   ControlComponent.Get(),
				                                   &UPlayerControlComponent::Parry);
			}

			// BaseAttack
			if (IsValid(PlayerController->BaseAttackAction))
			{
				EnhancedInputComponent->BindAction(PlayerController->BaseAttackAction, ETriggerEvent::Started,
				                                   ControlComponent.Get(),
				                                   &UPlayerControlComponent::BaseAttack);
			}

			// WeaponSkill
			if (IsValid(PlayerController->WeaponSkillAction))
			{
				EnhancedInputComponent->BindAction(PlayerController->WeaponSkillAction, ETriggerEvent::Started,
				                                   ControlComponent.Get(),
				                                   &UPlayerControlComponent::WeaponSkill);
			}

			// MagicSkill
			if (IsValid(PlayerController->MagicSkillAction))
			{
				EnhancedInputComponent->BindAction(PlayerController->MagicSkillAction, ETriggerEvent::Started,
				                                   ControlComponent.Get(),
				                                   &UPlayerControlComponent::MagicSkill);
			}

			// Interact
			if (IsValid(PlayerController->InteractAction))
			{
				EnhancedInputComponent->BindAction(PlayerController->InteractAction, ETriggerEvent::Started,
				                                   ControlComponent.Get(),
				                                   &UPlayerControlComponent::Interact);
			}

			// LockOn
			if (IsValid(PlayerController->LockOnAction))
			{
				EnhancedInputComponent->BindAction(PlayerController->LockOnAction, ETriggerEvent::Started,
												   ControlComponent.Get(),
												   &UPlayerControlComponent::LockOn);
			}

			// Consume Item (Slot 0)
			if (IsValid(PlayerController->ConsumeItemAction_1))
			{
				EnhancedInputComponent->BindAction(PlayerController->ConsumeItemAction_1, ETriggerEvent::Started,
													ControlComponent.Get(),
													&UPlayerControlComponent::ConsumeItemAction_1);
			}

			// Consume Item (Slot 1)
			if (IsValid(PlayerController->ConsumeItemAction_2))
			{
				EnhancedInputComponent->BindAction(PlayerController->ConsumeItemAction_2, ETriggerEvent::Started,
					ControlComponent.Get(),
					&UPlayerControlComponent::ConsumeItemAction_2);
			}

			// Consume Item (Slot 2)
			if (IsValid(PlayerController->ConsumeItemAction_3))
			{
				EnhancedInputComponent->BindAction(PlayerController->ConsumeItemAction_3, ETriggerEvent::Started,
					ControlComponent.Get(),
					&UPlayerControlComponent::ConsumeItemAction_3);
			}

			// Swap Weapon
			if (IsValid(PlayerController->SwapWeapon))
			{
				EnhancedInputComponent->BindAction(PlayerController->SwapWeapon, ETriggerEvent::Started,
					ControlComponent.Get(),
					&UPlayerControlComponent::SwapWeapon);
			}

			if (IsValid(PlayerController->InteractionScroll))
			{
				EnhancedInputComponent->BindAction(PlayerController->InteractionScroll, ETriggerEvent::Triggered,
					ControlComponent.Get(),
					&UPlayerControlComponent::InteractionScroll);
			}
		}
		else
		{
			Debug::PrintError(TEXT("ADomiCharacter::BindInputFunctions : Invalid InputComponent."));
		}
	}
}

void ADomiCharacter::Landed(const FHitResult& HitResult)
{
	Super::Landed(HitResult);
	check(StatusComponent);

	auto CurrentZSpeed = FMath::Abs(GetVelocity().Z);
	float Damage = FMath::Clamp(CurrentZSpeed - 1000.f, 0.f, 500.f) * StatusComponent->GetStat(StatTags::MaxHealth) / 500.f;
	StatusComponent->SetHealth(StatusComponent->GetStat(StatTags::Health) - Damage);
}

FGameplayTagContainer& ADomiCharacter::GetActiveControlEffectTags()
{
	check(ControlComponent);

	return ControlComponent->GetActiveControlEffectTags();
}

const FVector& ADomiCharacter::GetCurrentMovementVector() const
{
	check(ControlComponent);
	return ControlComponent->GetCurrentMovementVector();
}

void ADomiCharacter::SkillStart(FGameplayTag ControlEffectTag)
{
	check(ControlComponent);
	check(StatusComponent);
	
	ControlComponent->ActivateControlEffect(ControlEffectTag);
	StatusComponent->StopStaminaRecovery();
}

void ADomiCharacter::SkillEnd(FGameplayTag ControlEffectTag)
{
	check(ControlComponent);
	check(StatusComponent);
	
	ControlComponent->DeactivateControlEffect(ControlEffectTag);
	StatusComponent->StartStaminaRecovery();
}


FGameplayTagContainer& ADomiCharacter::GetActiveStatusEffectTags()
{
	check(StatusComponent);
	
	return StatusComponent->GetActiveStatusEffectTags();
}

void ADomiCharacter::InitializeStatusComponent()
{
	if (auto World = GetWorld())
	{
		if (auto BaseGameState = World->GetGameState<ABaseGameState>())
		{
			if (FStatusComponentInitializeData* InitializeData = BaseGameState->GetStatusComponentInitializeData(PawnTag))
			{
				StatusComponent->InitializeStatusComponent(*InitializeData);
				StatusComponent->OnDeath.AddUObject(this, &ADomiCharacter::OnDeath);
			}
		}
	}
}

void ADomiCharacter::OnDeath()
{
	check(ControlComponent);
	check(StatusComponent);
	
	auto ActiveControlEffectTagArray = ControlComponent->GetActiveControlEffectTags().GetGameplayTagArray();
	auto ActiveStatusEffectTagArray = StatusComponent->GetActiveStatusEffectTags().GetGameplayTagArray();

	// Deactivate All Effects 
	for (int32 i = 0 ; i < ActiveControlEffectTagArray.Num(); i++)
	{
		if (ActiveControlEffectTagArray.IsValidIndex(i))
		{
			ControlComponent->DeactivateControlEffect(ActiveControlEffectTagArray[i]);
		}
	}

	for (int32 i = 0 ; i < ActiveStatusEffectTagArray.Num(); i++)
	{
		if (ActiveStatusEffectTagArray.IsValidIndex(i))
		{
			StatusComponent->DeactivateStatusEffect(ActiveStatusEffectTagArray[i]);
		}
	}

	// Activate Death Effect
	ControlComponent->ActivateControlEffect(EffectTags::Death);
	PlayDeathVoice();
	
	// 델리게이트로?
	ABaseGameMode* GameMode = Cast<ABaseGameMode>(GetWorld()->GetAuthGameMode());
	if (IsValid(GameMode))
	{
		GameMode->OnPlayerDeath();
	}
}

FString ADomiCharacter::GetPawnName_Implementation()
{
	return PlayerName;
}

void ADomiCharacter::InitializeSkillComponent()
{
	if (auto World = GetWorld())
	{
		if (auto BaseGameState = World->GetGameState<ABaseGameState>())
		{
			if (FSkillComponentInitializeData* InitializeData = BaseGameState->GetSkillComponentInitializeData(PawnTag))
			{
				SkillComponent->SetSkills(*InitializeData);
				SkillComponent->OnSkillStart.BindUObject(this, &ADomiCharacter::SkillStart);
				SkillComponent->OnSkillEnd.BindUObject(this, &ADomiCharacter::SkillEnd);
			}
		}
	}
}

void ADomiCharacter::ExecuteSkill(FGameplayTag SkillGroupTag)
{
	if (IsValid(SkillComponent))
	{
		SkillComponent->ExecuteSkill(SkillGroupTag);
	}
	else
	{
		Debug::PrintError(TEXT("ADomiCharacter::ExecuteSkill : Invalid SkillComponent."));
	}
}

void ADomiCharacter::OnAttacked_Implementation(const FAttackData& AttackData)
{
	IDamagable::OnAttacked_Implementation(AttackData);

	check(ControlComponent);
	check(SkillComponent);
	check(StatusComponent);

	//CheatManager
	if (StatusComponent->GetIsGodMode())
	{
		return;
	}
	
	auto& ActiveControlEffects = GetActiveControlEffectTags();

	if (IsInvincible())
	{
		Debug::Print(TEXT("ADomiCharacter::OnAttacked : Invincible!"));
		return;
	}

	StatusComponent->StartCombat();
	
	float CurrentHealth = StatusComponent->GetStat(StatTags::Health);
	StatusComponent->SetHealth(CurrentHealth - AttackData.Damage);

	if (!AttackData.LaunchVector.IsNearlyZero())
	{
		LaunchCharacter(AttackData.LaunchVector, true, true);
	}

	PlayHitSound();
	
	// Activate Effects
	for (FEffectData EffectData : AttackData.Effects)
	{
		auto [EffectTag, Magnitude, Duration] = EffectData;

		if (EffectTag.MatchesTag(FGameplayTag::RequestGameplayTag(TEXT("Effect.Control"))))
		{
			// 경직이 있을 경우 피격 방향 체크
			// 날아가는 중이거나 죽었을때는 안함
			if (EffectTag.MatchesTag(EffectTags::Stiffness) && !ActiveControlEffects.HasTag(EffectTags::Flew))
			{
				if (IsValid(AttackData.Instigator))
				{
					FVector ForwardVector = GetActorForwardVector();
					FVector HitDirection = (AttackData.Instigator->GetActorLocation() - GetActorLocation()).GetSafeNormal();
	
					float Dot = FVector::DotProduct(ForwardVector, HitDirection);
					float AngleDegrees = FMath::RadiansToDegrees(FMath::Acos(Dot));

					FVector Cross = FVector::CrossProduct(ForwardVector, HitDirection);

					float HitAngle = Cross.Z > 0 ? AngleDegrees : -AngleDegrees;

					if (HitAngle > -45.0f && HitAngle <= 45.0f) // 정면 피격
					{
						PlayHitFrontAnimMontage(Duration);
					}
					else if (HitAngle > 45.0f && HitAngle <= 135.0f) // 우측 피격
					{
						PlayHitRightAnimMontage(Duration);
					}
					else if (HitAngle > -135.0f && HitAngle <= -45.0f) // 좌측 피격
					{
						PlayHitLeftAnimMontage(Duration);
					}
					else
					{
						PlayHitBackAnimMontage(Duration);
					}
					
					ControlComponent->ActivateControlEffect(EffectTags::Stiffness, Duration);
					PlayHitVoice();
					return;
				}
				else
				{
					Debug::PrintError(TEXT("OnAttacked_Implementation : Invalid stiffness Instigator."));
				}
			}
			else if (EffectTag.MatchesTag(EffectTags::Flew))
			{
				if (IsValid(AttackData.Instigator))
				{
					FVector HitDirection = (AttackData.Instigator->GetActorLocation() - GetActorLocation()).GetSafeNormal();
					SetActorRotation(HitDirection.Rotation());

					ControlComponent->ActivateControlEffect(EffectTags::Flew);
					PlayHitVoice();
					return;
				}
				else
				{
					Debug::PrintError(TEXT("OnAttacked_Implementation : Invalid flew Instigator."));
				}
			}
			
			ControlComponent->ActivateControlEffect(EffectTag, Duration);
		}
		else
		{
			StatusComponent->ActivateStatusEffect(EffectTag, Magnitude, Duration);
		}
	}
}

FGameplayTag ADomiCharacter::GetPawnTag_Implementation()
{
	return PawnTag;
}

void ADomiCharacter::ShowControlEffectTags_Implementation()
{
	IEffectReceivable::ShowControlEffectTags_Implementation();

	for (auto Tag : GetActiveControlEffectTags().GetGameplayTagArray())
	{
		Debug::Print(Tag.ToString());
	}
}
void ADomiCharacter::ShowStatusEffectTags_Implementation()
{
	IEffectReceivable::ShowStatusEffectTags_Implementation();

	for (auto Tag : GetActiveStatusEffectTags().GetGameplayTagArray())
	{
		Debug::Print(Tag.ToString());
	}
}

void ADomiCharacter::ActivateStatusEffect_Implementation(const FGameplayTag& EffectTag)
{
	IEffectReceivable::ActivateStatusEffect_Implementation(EffectTag);

	check(StatusComponent);

	StatusComponent->ActivateStatusEffect(EffectTag, 0);
}

bool ADomiCharacter::IsParryingCond_Implementation()
{
	auto ActiveControlEffects = GetActiveControlEffectTags();
	if (ActiveControlEffects.HasTag(EffectTags::Parry))
	{
		return true;
	}
	return false;
}

void ADomiCharacter::OnParried_Implementation()
{
	// 일단 패리당할일 없어서 비워둠
}

AActor* ADomiCharacter::GetTargetEnemy_Implementation()
{
	check(ControlComponent);

	return ControlComponent->GetLockOnTargetActor();
}

void ADomiCharacter::EventInteractionWidgetScroll(const float Value)
{
	OnInteractionWidgetScroll.Broadcast(Value);
}

FGameplayTagContainer ADomiCharacter::GetAllActivateEffectDatas_Implementation() const
{
    check(StatusComponent);
    check(ControlComponent);

	FGameplayTagContainer ActiveEffects;
    
	ActiveEffects.AppendTags(StatusComponent->GetActiveStatusEffectTags());
	ActiveEffects.AppendTags(ControlComponent->GetActiveControlEffectTags());
	
	return ActiveEffects;
}

void ADomiCharacter::SendEffectUIDatas_Implementation() const
{
	check(StatusComponent);
	check(ControlComponent);

	TArray<FEffectUIData> EffectUIDatas;
	EffectUIDatas.Append(ControlComponent->GetEffectUIDatas());
	EffectUIDatas.Append(StatusComponent->GetEffectUIDatas());
	
	OnUpdateEffectUIDataArray.Broadcast(EffectUIDatas);
}
