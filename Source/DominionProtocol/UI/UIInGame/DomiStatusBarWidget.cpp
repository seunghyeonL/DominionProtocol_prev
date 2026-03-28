// Fill out your copyright notice in the Description page of Project Settings.


#include "DomiStatusBarWidget.h"

#include "Components/StatusComponent/StatusComponent.h"
#include "Player/Characters/DomiCharacter.h"

void UDomiStatusBarWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Check Widget Binding 
	ensure(PlayerHPBar);
	ensure(PlayerStaminaBar);

	// Initialize & DelegateBinding
	AActor* OwningActor = GetOwningPlayerPawn();
	if (OwningActor)
	{
		SetupStatusBarWidget(OwningActor);
	}
}

void UDomiStatusBarWidget::UpdatePlayerHPBar(const float NewHP)
{
	AlphaForHPAnim = 0.0f;
	PreHP = CurrentHP;
	CurrentHP = NewHP;
}

void UDomiStatusBarWidget::UpdatePlayerMaxHPBar(const float NewMaxHP)
{
	AlphaForHPAnim = 0.0f;
	PreMaxHP = MaxHP;
	MaxHP = NewMaxHP;
}

void UDomiStatusBarWidget::UpdatePlayerStaminaBar(const float NewStamina)
{
	AlphaForStaminaAnim = 0.0f;
	PreStamina = CurrentStamina;
	CurrentStamina = NewStamina;
}

void UDomiStatusBarWidget::UpdatePlayerMaxStaminaBar(const float NewMaxStamina)
{
	AlphaForStaminaAnim = 0.0f;
	PreMaxStamina = MaxStamina;
	MaxStamina = NewMaxStamina;
}

void UDomiStatusBarWidget::OnUpdateEffectUIDataArray(TArray<FEffectUIData> NewEffectUIDataArray)
{
	TArray<FEffectUIData> TempBuffArray;
	TArray<FEffectUIData> TempDebuffArray;
	
	for (auto EffectData:NewEffectUIDataArray)
	{
		if (EffectData.EffectTag.MatchesTag(EffectTags::ControlBuff) || EffectData.EffectTag.MatchesTag(EffectTags::StatusBuff))
		{
			TempBuffArray.Add(EffectData);		
		}

		if (EffectData.EffectTag.MatchesTag(EffectTags::ControlDebuff) || EffectData.EffectTag.MatchesTag(EffectTags::StatusDebuff))
		{
			TempDebuffArray.Add(EffectData);
		}
	}

	BuffEffectUIDataArray = TempBuffArray;
	DebuffEffectUIDataArray = TempDebuffArray;

	UpdateEffectUIDataArray();
}

void UDomiStatusBarWidget::SetupStatusBarWidget(AActor* OwningActor)
{
	ensure(IsValid(OwningActor));
	
	auto* StatusComp = OwningActor->GetComponentByClass<UStatusComponent>();
	if (StatusComp)
	{
		// Initialize
		MaxHP = StatusComp->GetStat(StatTags::MaxHealth);
		MaxStamina = StatusComp->GetStat(StatTags::MaxStamina);
		CurrentHP = StatusComp->GetStat(StatTags::Health);
		CurrentStamina = StatusComp->GetStat(StatTags::Stamina);
		PreHP = CurrentHP;
		PreMaxHP = MaxHP;
		PreStamina = CurrentStamina;
		PreMaxStamina = MaxStamina;

		// Delegate Binding
		StatusComp->OnHealthChanged.AddDynamic(this, &UDomiStatusBarWidget::UpdatePlayerHPBar);
		StatusComp->OnStaminaChanged.AddDynamic(this, &UDomiStatusBarWidget::UpdatePlayerStaminaBar);
		StatusComp->OnMaxHealthChanged.AddDynamic(this, &UDomiStatusBarWidget::UpdatePlayerMaxHPBar);
		StatusComp->OnMaxStaminaChanged.AddDynamic(this, &UDomiStatusBarWidget::UpdatePlayerMaxStaminaBar);
	}
	
	auto* PlayerCharacter = Cast<ADomiCharacter>(OwningActor);
	if (PlayerCharacter)
	{
		PlayerCharacter->OnUpdateEffectUIDataArray.AddUObject(this, &UDomiStatusBarWidget::OnUpdateEffectUIDataArray);
	}
}
