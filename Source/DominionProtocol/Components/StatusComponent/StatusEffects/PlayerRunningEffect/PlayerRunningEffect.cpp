// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerRunningEffect.h"

#include "Components/StatusComponent/StatusComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/Characters/DomiCharacter.h"

UPlayerRunningEffect::UPlayerRunningEffect()
{
	StatusEffectTag = EffectTags::Running;
	SpeedCoefficient = 1.7f;
	StaminaPerSecondRate = 0.1f;
}

bool UPlayerRunningEffect::Activate()
{
	if (!Super::Activate())
	{
		return false;
	}
	
	if (!IsValid(OwnerCharacter))
	{
		Debug::PrintError(TEXT("UPlayerRunningEffect::Activate : Invalid OwnerCharacter."));
		return false;
	}

	auto StatusComponent = Cast<UStatusComponent>(GetOuter());
	auto ActiveStatusEffectTags = StatusComponent->GetActiveStatusEffectTags();
	if (ActiveStatusEffectTags.HasTag(EffectTags::Walking))
	{
		StatusComponent->DeactivateStatusEffect(EffectTags::Walking);
	}
	
	auto MovementComponent = Cast<UCharacterMovementComponent>(OwnerCharacter->GetMovementComponent());
	MovementComponent->MaxWalkSpeed *= SpeedCoefficient;
	return true;
}

bool UPlayerRunningEffect::Activate(float Duration)
{
	// Super::Activate(Duration);
	Activate();
	return true;
}

void UPlayerRunningEffect::Deactivate()
{
	if (!bIsActive)
	{
		return;
	}
	
	Super::Deactivate();
	
	if (!IsValid(OwnerCharacter))
	{
		Debug::PrintError(TEXT("UPlayerRunningEffect::Deactivate : Invalid OwnerCharacter."));
		return;
	}

	auto StatusComponent = Cast<UStatusComponent>(GetOuter());
	if (!StatusComponent)
	{
		Debug::PrintError(TEXT("UPlayerRunningEffect::Deactivate : Invalid StatusComponent."));
		return;
	}
	
	auto MovementComponent = Cast<UCharacterMovementComponent>(OwnerCharacter->GetMovementComponent());
	MovementComponent->MaxWalkSpeed /= SpeedCoefficient;
}

void UPlayerRunningEffect::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	auto StatusComponent = Cast<UStatusComponent>(GetOuter());
	check(StatusComponent);

	if (!StatusComponent->IsInCombat())
	{
		return;
	}

	StatusComponent->SetStamina(StatusComponent->GetStat(StatTags::Stamina) - StatusComponent->GetStat(StatTags::MaxStamina) * StaminaPerSecondRate * DeltaTime);
	
	if (FMath::IsNearlyZero(StatusComponent->GetStat(StatTags::Stamina)))
	{
		Deactivate();
	}
}
