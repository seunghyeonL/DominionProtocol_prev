// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseEnemy.h"
#include "AIController.h"
#include "BrainComponent.h"
#include "Components/StatusComponent/StatusComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/LockOnComponent/LockOnComponent.h"
#include "Components/SkillComponent/SkillComponent.h"
#include "Components/SkillComponent/SkillComponentInitializeData.h"
#include "Components/StatusComponent/StatusComponentInitializeData.h"
#include "Components/WidgetComponent/DomiWidgetComponent.h"
#include "DomiFramework/GameState/BaseGameState.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "WorldObjects/Essence.h"

// Sets default values
ABaseEnemy::ABaseEnemy()
	:	EssenceAmount(500)
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	// BattleComponents
	StatusComponent = CreateDefaultSubobject<UStatusComponent>(TEXT("StatusComponent"));
	SkillComponent = CreateDefaultSubobject<USkillComponent>(TEXT("SkillComponent"));

	// UI Section
	HPWidgetComponent = CreateDefaultSubobject<UDomiWidgetComponent>(TEXT("HPWidgetComponent"));
	HPWidgetComponent->SetupAttachment(GetMesh());
	HPWidgetComponent->SetRelativeLocation(FVector(0, 0, 190));
	static ConstructorHelpers::FClassFinder<UUserWidget> HPBarWidgetRef (TEXT("/Game/Blueprints/UI/WBP_DomiMonster3DWidget.WBP_DomiMonster3DWidget_C"));
	if (HPBarWidgetRef.Class)
	{
		HPWidgetComponent->SetWidgetClass(HPBarWidgetRef.Class);
		HPWidgetComponent->SetWidgetSpace(EWidgetSpace::World);
		HPWidgetComponent->SetDrawSize(FVector2D(80.f, 10.f));
		HPWidgetComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	//LockOnComponent
	LockOnComponent = CreateDefaultSubobject<ULockOnComponent>(TEXT("LockOnComponent"));
	LockOnComponent->SetupAttachment(GetMesh(), FName("spine_03"));

	LockOnWidgetComponent = CreateDefaultSubobject<UDomiWidgetComponent>(TEXT("LockOnWidgetComponent"));
	LockOnWidgetComponent->SetupAttachment(LockOnComponent);
	// LockOnWidgetComponent->SetRelativeLocation(FVector(0, 0, 120));
	static ConstructorHelpers::FClassFinder<UUserWidget> LockOnWidgetRef (TEXT("/Game/Blueprints/UI/LockOnWidget.LockOnWidget_C"));
	if (LockOnWidgetRef.Class)
	{
		LockOnWidgetComponent->SetWidgetClass(LockOnWidgetRef.Class);
		LockOnWidgetComponent->SetWidgetSpace(EWidgetSpace::World);
		LockOnWidgetComponent->SetDrawSize(FVector2D(250.f, 250.f));
		LockOnWidgetComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	
	// InvincibilityTags Setting
	InvincibilityTags.AddTag(EffectTags::Death);

	MonsterName = TEXT("DefaultMonster");

	// BP_Class Settings
	static ConstructorHelpers::FClassFinder<AEssence> EssenceBPClass(TEXT("/Game/WorldObjects/BP_Essence"));
	if (EssenceBPClass.Succeeded())
	{
		EssenceClass = EssenceBPClass.Class;
	}

	GetMesh()->bReceivesDecals = false;

}

// Called when the game starts or when spawned
void ABaseEnemy::BeginPlay()
{
	Super::BeginPlay();
}

void ABaseEnemy::OnDeath_Implementation()
{
	StatusComponent->ActivateStatusEffect(EffectTags::Death, 0);

	// 사망 시Essence 생성
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	//캡슐컴포넌트 HalfHeight로 Essence 스폰위치 조절
	UCapsuleComponent* CapsuleComp = FindComponentByClass<UCapsuleComponent>();
	if (!IsValid(CapsuleComp))
	{
		Debug::PrintError(TEXT("ABaseEnemy::OnDeath : CapsuleComponent is not founded"));
		return;
	}
	FVector SpawnLocation = GetActorLocation() + FVector(0.f, 0.f, CapsuleComp->GetScaledCapsuleHalfHeight());
	
	AEssence* NewEssence = Cast<AEssence>(GetWorld()->SpawnActor<AEssence>(EssenceClass, SpawnLocation, GetActorRotation(), SpawnParams));
	if (IsValid(NewEssence))
	{
		// 각 몬스터별 균열 정수량만큼 Essence에 세팅
		NewEssence->SetEssenceAmount(EssenceAmount);
	}
}

void ABaseEnemy::OnGroggy()
{
	// for Boss
}

// Called every frame
void ABaseEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

FGameplayTagContainer& ABaseEnemy::GetActiveStatusEffectTags()
{
	check(StatusComponent);
	return StatusComponent->GetActiveStatusEffectTags();
}

void ABaseEnemy::InitializeSkillComponent()
{
	if (auto World = GetWorld())
	{
		if (auto BaseGameState = World->GetGameState<ABaseGameState>())
		{
			if (FSkillComponentInitializeData* InitializeData = BaseGameState->GetSkillComponentInitializeData(PawnTag))
			{
				SkillComponent->SetSkills(*InitializeData);
				// SkillComponent->OnSkillStart.BindUObject(this, &ADomiCharacter::SkillStart);
				// SkillComponent->OnSkillEnd.BindUObject(this, &ADomiCharacter::SkillEnd);
			}
		}
	}
}

void ABaseEnemy::ExecuteSkill(FGameplayTag SkillGroupTag)
{
	if (IsValid(SkillComponent))
	{
		SkillComponent->ExecuteSkill(SkillGroupTag);
	}
}

void ABaseEnemy::OnAttacked_Implementation(const FAttackData& AttackData)
{
	if (Cast<ABaseEnemy>(AttackData.Instigator)) { return; }
	IDamagable::OnAttacked_Implementation(AttackData);

	check(StatusComponent);
	check(SkillComponent)
	
	auto ActiveStatusEffects = GetActiveStatusEffectTags();
	if (ActiveStatusEffects.HasAny(InvincibilityTags))
	{
		Debug::Print(TEXT("ADomiCharacter::OnAttacked : Invincible!"));
		return;
	}

	float CurrentHealth = StatusComponent->GetStat(StatTags::Health);
	StatusComponent->SetHealth(CurrentHealth - AttackData.Damage);
	float CurrentGroggyGauge = StatusComponent->GetStat(StatTags::GroggyGauge);

	if (CurrentGroggyGauge > 0.f)
	{
		StatusComponent->SetGroggyGauge(CurrentGroggyGauge - AttackData.GroggyDamage);
	}

	if (!AttackData.LaunchVector.IsNearlyZero())
	{
		LaunchCharacter(AttackData.LaunchVector, true, true);
	}
	
	// Activate Effects
	for (FEffectData EffectData : AttackData.Effects)
	{
		auto [EffectTag, Magnitude, Duration] = EffectData;
		
		StatusComponent->ActivateStatusEffect(EffectTag, Magnitude, Duration);
	}
}

FGameplayTag ABaseEnemy::GetPawnTag_Implementation()
{
	return PawnTag;
}

bool ABaseEnemy::IsParryingCond_Implementation()
{
	// auto ActiveStatusEffects = GetActiveStatusEffectTags();
	// if (ActiveStatusEffects.HasTag(EffectTags::UsingParry))
	// {
	// 	return true;
	// }
	return false;
}

void ABaseEnemy::OnParried_Implementation()
{
	StatusComponent->ActivateStatusEffect(EffectTags::Parried, 0.f, 2.f);
}

void ABaseEnemy::ShowControlEffectTags_Implementation()
{
	IEffectReceivable::ShowControlEffectTags_Implementation();

	// This Actor Doesn't Use ControlComponent
}

void ABaseEnemy::ShowStatusEffectTags_Implementation()
{
	IEffectReceivable::ShowStatusEffectTags_Implementation();

	for (auto Tag : GetActiveStatusEffectTags().GetGameplayTagArray())
	{
		Debug::Print(Tag.ToString());
	}
}

void ABaseEnemy::ActivateStatusEffect_Implementation(const FGameplayTag& EffectTag)
{
	IEffectReceivable::ActivateStatusEffect_Implementation(EffectTag);

	check(StatusComponent);

	StatusComponent->ActivateStatusEffect(EffectTag, 0);
}

FGameplayTagContainer ABaseEnemy::GetAllActivateEffectDatas_Implementation() const
{
	return StatusComponent->GetActiveStatusEffectTags();
}

void ABaseEnemy::SendEffectUIDatas_Implementation() const
{
	check(StatusComponent);
	
	auto EffectUIDataArray = StatusComponent->GetEffectUIDatas();

	// OnEffectUIDataArray.Broadcast(EffectUIDataArray);
}

FString ABaseEnemy::GetPawnName_Implementation()
{
	return MonsterName;
}

void ABaseEnemy::DormancyOn()
{
	if (!bIsDormant) return;
	bIsDormant = false;

	SetActorTickEnabled(true);

	// 캐싱된 컴포넌트만 Tick 복원
	for (const TWeakObjectPtr<UActorComponent>& WeakComp : CachedTickingComponents)
	{
		if (WeakComp.IsValid())
		{
			WeakComp->SetComponentTickEnabled(true);
		}
	}
	CachedTickingComponents.Empty();

	if (AAIController* AIC = Cast<AAIController>(GetController()))
	{
		if (UBrainComponent* BrainComp = AIC->GetBrainComponent())
		{
			BrainComp->ResumeLogic(TEXT("Dormancy"));
		}
	}
}

void ABaseEnemy::DormancyOff()
{
	if (bIsDormant) return;
	bIsDormant = true;

	SetActorTickEnabled(false);

	// Tick이 켜져있는 컴포넌트만 캐싱 후 끄기
	CachedTickingComponents.Empty();
	TArray<UActorComponent*> Components;
	GetComponents(Components);
	for (UActorComponent* Comp : Components)
	{
		if (Comp->PrimaryComponentTick.bCanEverTick && Comp->IsComponentTickEnabled())
		{
			CachedTickingComponents.Add(Comp);
			Comp->SetComponentTickEnabled(false);
		}
	}

	if (AAIController* AIC = Cast<AAIController>(GetController()))
	{
		if (UBrainComponent* BrainComp = AIC->GetBrainComponent())
		{
			BrainComp->PauseLogic(TEXT("Dormancy"));
		}
	}

	if (UCharacterMovementComponent* MovComp = GetCharacterMovement())
	{
		MovComp->StopMovementImmediately();
	}
}

void ABaseEnemy::InitializeStatusComponent()
{
	if (auto World = GetWorld())
	{
		if (auto BaseGameState = World->GetGameState<ABaseGameState>())
		{
			if (FStatusComponentInitializeData* InitializeData = BaseGameState->GetStatusComponentInitializeData(PawnTag))
			{
				StatusComponent->InitializeStatusComponent(*InitializeData);
				StatusComponent->OnDeath.AddUObject(this, &ABaseEnemy::OnDeath);
				StatusComponent->OnGroggy.BindUObject(this, &ABaseEnemy::OnGroggy);
			}
		}
	}
}


