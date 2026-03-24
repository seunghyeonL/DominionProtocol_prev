#include "Components/SkillComponent/Skills/BaseSkill.h"
#include "GameFramework/Character.h"
#include "CollisionQueryParams.h"
#include "Player/Damagable.h"
#include "Components/StatusComponent/StatusComponent.h"
#include "Util/GameTagList.h"
#include "DomiFramework/GameState/BaseGameState.h"
#include "Components/SkillComponent/Skills/SkillData.h"
#include "../Plugins/MissNoHit/Source/MissNoHit/Public/MnhTracerComponent.h"
#include "AI/AICharacters/BaseEnemy.h"
#include "Util/DebugHelper.h"
#include "Interface/Parryable.h"
#include "Engine/SkeletalMeshSocket.h"

UBaseSkill::UBaseSkill()
{
	ControlEffectTag = EffectTags::UsingSkill;
	bIsMagicSkill = false;
	bIsCoolDowning = false;
	AnimPlayRate = 1.0f;
}

void UBaseSkill::Initialize(ACharacter* InOwnerCharacter)
{
	if (IsValid(InOwnerCharacter))
	{
		OwnerCharacter = InOwnerCharacter;
	}

	UWorld* World = GetWorld();

	if (IsValid(World))
	{
		ABaseGameState* BaseGameState = World->GetGameState<ABaseGameState>();

		if (IsValid(BaseGameState))
		{
			if (FSkillData* SkillData = BaseGameState->GetSkillData(SkillTag))
			{
				AnimMontage = SkillData->AnimMontage;
				Sounds = SkillData->Sounds;
				Particles = SkillData->Particles;
				NiagaraParticles = SkillData->NiagaraParticles;
				CurvedProjectileClass = SkillData->CurvedProjectileClass;
				Stamina = SkillData->Stamina;
				AttackRadius = SkillData->AttackRadius;
				AttackForwardOffset = SkillData->AttackForwardOffset;
				DamageCoefficient = SkillData->DamageCoefficient;
				Effects = SkillData->Effects;
				LaunchGroundSpeed = SkillData->LaunchGroundSpeed;
				LaunchZSpeed = SkillData->LaunchZSpeed;
				bIsMagicSkill = SkillData->bIsMagicSkill;
				CoolDownTime = SkillData->CoolDownTime;
				GroggyDamage = SkillData->GroggyDamage;
			}
			else
			{
				Debug::PrintError(TEXT("UBaseSkill::Initialize : Invalid SkillData."));
			}
		}
		else
		{
			Debug::PrintError(TEXT("UBaseSkill::Initialize : Invalid BaseGameState."));
		}
	}
}

void UBaseSkill::Execute()
{
	check(IsValid(AnimMontage));
	check(IsValid(OwnerCharacter));

	OwnerCharacter->PlayAnimMontage(AnimMontage, AnimPlayRate);
}

// 애님 노티파이에서 실행
void UBaseSkill::AttackTrace() const
{
	check(OwnerCharacter);

	FVector ForwardVector = OwnerCharacter->GetActorForwardVector();

	FVector Start = OwnerCharacter->GetActorLocation() + ForwardVector * (AttackRadius + AttackForwardOffset);

	const USkeletalMeshSocket* TraceSocket = OwnerCharacter->GetMesh()->GetSocketByName("TraceSocket");

	// 소켓이 있다면 소켓에서 트레이스하기
	if (IsValid(TraceSocket))
	{
		Start = TraceSocket->GetSocketLocation(OwnerCharacter->GetMesh());
	}

	FVector End = Start;

	TArray<FHitResult> HitResults;

	// 트레이스 수행
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(OwnerCharacter);

	bool bHit = GetWorld()->SweepMultiByChannel(
		HitResults,
		Start,										// 시작 위치
		End,										// 끝 위치
		FQuat::Identity,
		ECollisionChannel::ECC_Pawn,    			// 충돌 채널
		FCollisionShape::MakeSphere(AttackRadius),	// 범위 설정 (구체 모양)
		QueryParams
	);

	
	// 디버그용
	Debug::DrawSphere(
		GetWorld(),
		Start,
		AttackRadius,
		12,
		FColor::Green,
		false,              // 지속 여부 (false면 일정 시간 후 사라짐)
		2.0f,               // 지속 시간 (초)
		0,
		1.0f
		);

	if (!bHit)
	{
		return;
	}

	for (const FHitResult& HitResult : HitResults)
	{
		ApplyAttackToHitActor(HitResult, 0.f);
	}
}

void UBaseSkill::AttackTrace_Cylinder(FVector Offset, float Radius, float HalfHeight) const
{
	check(OwnerCharacter);
	UWorld* World = GetWorld();
	if (!World) return;

	// 위치 계산
	FVector Start = OwnerCharacter->GetActorLocation() + OwnerCharacter->GetActorRotation().RotateVector(Offset);
	FVector End = Start;

	// 회전: Forward 방향으로 눕힌 실린더
	FVector CapsuleDirection = OwnerCharacter->GetActorForwardVector();
	FQuat CapsuleRotation = FRotationMatrix::MakeFromZ(CapsuleDirection).ToQuat();

	// 트레이스
	TArray<FHitResult> HitResults;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(OwnerCharacter);

	bool bHit = World->SweepMultiByChannel(
		HitResults,
		Start,
		End,
		CapsuleRotation,
		ECC_Pawn,
		FCollisionShape::MakeCapsule(Radius, HalfHeight),
		QueryParams
	);

	if (!bHit) return;

	for (const FHitResult& Hit : HitResults)
	{
		if (AActor* HitActor = Hit.GetActor())
		{
			ApplyAttackToHitActor(Hit, 0.f);
		}
	}
}


void UBaseSkill::StartTrace(const FGameplayTagContainer& TagContainer)
{
	check(OwnerCharacter);

	UMnhTracerComponent* MnhTracerComponent = OwnerCharacter->FindComponentByClass<UMnhTracerComponent>();

	if (IsValid(MnhTracerComponent))
	{
		if (TagContainer.IsEmpty())
		{
			FGameplayTagContainer GameplayTagContainer;
			GameplayTagContainer.AddTag(ItemTags::BasicWeapon);
			MnhTracerComponent->StartTracers(GameplayTagContainer);

			return;
		}

		MnhTracerComponent->StartTracers(TagContainer);
	}
}

void UBaseSkill::StopTrace(const FGameplayTagContainer& TagContainer)
{
	check(OwnerCharacter);

	UMnhTracerComponent* MnhTracerComponent = OwnerCharacter->FindComponentByClass<UMnhTracerComponent>();

	if (IsValid(MnhTracerComponent))
	{
		if (TagContainer.IsEmpty())
		{
			FGameplayTagContainer GameplayTagContainer;
			GameplayTagContainer.AddTag(ItemTags::BasicWeapon);
			MnhTracerComponent->StopTracers(GameplayTagContainer);

			return;
		}

		MnhTracerComponent->StopTracers(TagContainer);
	}
}

void UBaseSkill::ApplyAttackToHitActor(const FHitResult& HitResult, const float DeltaTime) const
{
	AActor* HitActor = HitResult.GetActor();

	if (!IsValid(HitActor))
	{
		return;
	}

	check(OwnerCharacter)

	// check Parry
	if (CheckParry(HitActor))
	{
		return;
	}
	
	if (HitActor->GetClass()->ImplementsInterface(UDamagable::StaticClass()))
	{
		FAttackData AttackData;

		AttackData.Instigator = OwnerCharacter;
		AttackData.Damage = GetFinalAttackData();
		AttackData.GroggyDamage = GroggyDamage;
		AttackData.Effects = Effects;
		
		FVector GroundLaunchVector = (HitActor->GetActorLocation() - OwnerCharacter->GetActorLocation()).GetSafeNormal() * LaunchGroundSpeed;
		
		AttackData.LaunchVector = {GroundLaunchVector.X, GroundLaunchVector.Y, LaunchZSpeed};
		
		IDamagable::Execute_OnAttacked(HitActor, AttackData);
	}
}

float UBaseSkill::GetStamina() const
{
	return Stamina;
}

void UBaseSkill::Tick(float DeltaTime)
{
}

void UBaseSkill::CoolDownStart()
{
	check(OwnerCharacter);

	// CoolDownTime이 0초면 쿨타임을 주지 않음.
	if (FMath::IsNearlyZero(CoolDownTime))
	{
		return;
	}
	
	bIsCoolDowning = true;
	OwnerCharacter->GetWorldTimerManager().SetTimer(
		CoolDownTimer,
		this,
		&UBaseSkill::CoolDownEnd,
		CoolDownTime,
		false
    );
}

void UBaseSkill::CoolDownEnd()
{
	bIsCoolDowning = false;
}

float UBaseSkill::GetFinalAttackData() const
{
	UStatusComponent* StatusComponent = OwnerCharacter->FindComponentByClass<UStatusComponent>();

	if (IsValid(StatusComponent))
	{
		float DependentStat = bIsMagicSkill ? StatusComponent->GetStat(StatTags::MagicPower) : StatusComponent->GetStat(StatTags::AttackPower);
		return DependentStat * DamageCoefficient;
	}
	
	return 0.f;
}

bool UBaseSkill::CheckParry(AActor* HitActor) const
{
	FVector AttackDirectionVector = HitActor->GetActorLocation() - OwnerCharacter->GetActorLocation();
	AttackDirectionVector = AttackDirectionVector.GetSafeNormal();
	FVector TargetForwardVector = HitActor->GetActorForwardVector();

	float RadianAngle = FMath::Acos(FVector::DotProduct(TargetForwardVector, -AttackDirectionVector));
	float DegreeAngle = FMath::RadiansToDegrees(RadianAngle);
	if (DegreeAngle > 45.f)
	{
		return false;
	}

	if (HitActor->GetClass()->ImplementsInterface(UParryable::StaticClass()))
	{
		if (!IParryable::Execute_IsParryingCond(HitActor))
		{
			return false;
		}

		auto ParryableOwner = Cast<IParryable>(OwnerCharacter);
		if (!ParryableOwner)
		{
			return false;
		}

		IParryable::Execute_OnParrySuccess(HitActor);

		if (OwnerCharacter->GetClass()->ImplementsInterface(UParryable::StaticClass()))
		{
			IParryable::Execute_OnParried(OwnerCharacter);
		}

		return true;
	}
	
	return false;
}
