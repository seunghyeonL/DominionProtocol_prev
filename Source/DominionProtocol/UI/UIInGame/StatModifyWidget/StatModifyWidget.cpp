// Fill out your copyright notice in the Description page of Project Settings.


#include "StatModifyWidget.h"

#include "Components/Button.h"
#include "Util/GameTagList.h"
#include "Components/TextBlock.h"
#include "Components/StatusComponent/StatusComponent.h"
#include "DomiFramework/GameInstance/DomiGameInstance.h"

void UStatModifyWidget::UpdatePreAttackAbilityStat()
{
	// 위젯이 Visible 일 때 실행 할 것
	ensure(PlayerStatPreviewData[StatTags::AttackPower]);
	ensure(PlayerStatPreviewData[StatTags::SubAttackPower]);
	ensure(PlayerStatPreviewData[StatTags::MagicPower]);
	ensure(PlayerStatPreviewData[StatTags::MaxHealth]);
	ensure(PlayerStatPreviewData[StatTags::MaxStamina]);	
	
	PreAttackPower = PlayerStatPreviewData[StatTags::AttackPower];
	PreSubAttackPower = PlayerStatPreviewData[StatTags::SubAttackPower];
	PreMagicPower = PlayerStatPreviewData[StatTags::MagicPower];
	PreMaxHealth = PlayerStatPreviewData[StatTags::MaxHealth];
	PreMaxStamina = PlayerStatPreviewData[StatTags::MaxStamina];	
}

void UStatModifyWidget::NativeConstruct()
{
	Super::NativeConstruct();

	AActor* OwningActor = GetOwningPlayerPawn();
	PlayerStatusComponent = OwningActor->GetComponentByClass<UStatusComponent>();
	check(PlayerStatusComponent);

	PlayerStatTags.Add(StatTags::STR);
	PlayerStatTags.Add(StatTags::LIFE);
	PlayerStatTags.Add(StatTags::SPL);
	PlayerStatTags.Add(StatTags::END);
	PlayerStatTags.Add(StatTags::AttackPower);
	PlayerStatTags.Add(StatTags::SubAttackPower);
	PlayerStatTags.Add(StatTags::MagicPower);
	PlayerStatTags.Add(StatTags::MaxHealth);
	PlayerStatTags.Add(StatTags::MaxStamina);
	PlayerStatTags.Add(StatTags::Level);

	ActivatedStatModifyMap.Add(StatTags::STR, FSlateColor(FLinearColor::White));
	ActivatedStatModifyMap.Add(StatTags::LIFE, FSlateColor(FLinearColor::White));
	ActivatedStatModifyMap.Add(StatTags::SPL, FSlateColor(FLinearColor::White));
	ActivatedStatModifyMap.Add(StatTags::END, FSlateColor(FLinearColor::White));
	ActivatedStatModifyMap.Add(StatTags::AttackPower, FSlateColor(FLinearColor::White));
	ActivatedStatModifyMap.Add(StatTags::SubAttackPower, FSlateColor(FLinearColor::White));
	ActivatedStatModifyMap.Add(StatTags::MagicPower, FSlateColor(FLinearColor::White));
	ActivatedStatModifyMap.Add(StatTags::MaxHealth, FSlateColor(FLinearColor::White));
	ActivatedStatModifyMap.Add(StatTags::MaxStamina, FSlateColor(FLinearColor::White));
	ActivatedStatModifyMap.Add(StatTags::Level, FSlateColor(FLinearColor::White));

	StatModifiedNumMap.Add(StatTags::STR, 0);
	StatModifiedNumMap.Add(StatTags::LIFE, 0);
	StatModifiedNumMap.Add(StatTags::SPL, 0);
	StatModifiedNumMap.Add(StatTags::END, 0);
	StatModifiedNumMap.Add(StatTags::AttackPower, 0);
	StatModifiedNumMap.Add(StatTags::SubAttackPower, 0);
	StatModifiedNumMap.Add(StatTags::MagicPower, 0);
	StatModifiedNumMap.Add(StatTags::MaxHealth, 0);
	StatModifiedNumMap.Add(StatTags::MaxStamina, 0);
	StatModifiedNumMap.Add(StatTags::Level, 0);
	
	StatModifiedStringMap.Add(StatTags::STR, TEXT(""));
	StatModifiedStringMap.Add(StatTags::LIFE, TEXT(""));
	StatModifiedStringMap.Add(StatTags::SPL, TEXT(""));
	StatModifiedStringMap.Add(StatTags::END, TEXT(""));
	StatModifiedStringMap.Add(StatTags::AttackPower, TEXT(""));
	StatModifiedStringMap.Add(StatTags::SubAttackPower, TEXT(""));
	StatModifiedStringMap.Add(StatTags::MagicPower, TEXT(""));
	StatModifiedStringMap.Add(StatTags::MaxHealth, TEXT(""));
	StatModifiedStringMap.Add(StatTags::MaxStamina, TEXT(""));
	StatModifiedStringMap.Add(StatTags::Level, TEXT(""));

	for (auto PlayerStatTag : PlayerStatTags)
	{
		PlayerStatPreviewData.Add(PlayerStatTag, 0.f);
	}
	
	SetVisibility(ESlateVisibility::Collapsed);

	InitializeWidgetDatas();

	UpdatePreviewStat();
	
}

void UStatModifyWidget::InitializeWidgetDatas()
{
	AccumulatedRequiredEssence = 0.f;
	bCanLevelUp = false;
	
	for (auto PlayerStatTag : PlayerStatTags)
	{
		StatModifiedNumMap[PlayerStatTag] = 0.f;
	}
	
	PlayerStatusComponent->GetPlayerStatData(PlayerStatPreviewData);

	
	for (auto PlayerStatTag : PlayerStatTags)
	{
		StatModifiedStringMap[PlayerStatTag] = FString::Printf(TEXT("%d"), FMath::RoundToInt(PlayerStatPreviewData[PlayerStatTag]));
	}
	
}

void UStatModifyWidget::UpdatePreviewStat()
{
	// 레벨업 필요 재화
	auto DomiGI = Cast<UDomiGameInstance>(GetGameInstance());
	check(DomiGI);

	CurrentEssenceValue = DomiGI->GetPlayerCurrentEssence() - AccumulatedRequiredEssence;
	LevelUpRequiredEssence = PlayerStatusComponent->GetLevelUpRequiredEssence(PlayerStatPreviewData[StatTags::Level]);
	
	if (DomiGI->HasEnoughEssence(AccumulatedRequiredEssence + PlayerStatusComponent->GetLevelUpRequiredEssence(PlayerStatPreviewData[StatTags::Level])))
	{
		ActivatedLevelUpRequiredTextColor = FSlateColor(FLinearColor::Green);
		bCanLevelUp = true;
	}
	else
	{
		ActivatedLevelUpRequiredTextColor = FSlateColor(FLinearColor::Red);
		bCanLevelUp = false;
	}

	// 스탯 프리뷰 데이터 갱신
	PlayerStatusComponent->UpdateStatPreviewData(PlayerStatPreviewData);

	// 현재 스탯 받아오기
	TMap<FGameplayTag, float> CurrentStatData;

	for (auto PlayerStatTag : PlayerStatTags)
	{
		CurrentStatData.Add(PlayerStatTag, 0.f);
	}
	
	PlayerStatusComponent->GetPlayerStatData(CurrentStatData);

	// 바꿔진 스탯은 색깔 변경
	for (auto PlayerStatTag : PlayerStatTags)
	{
		if (PlayerStatPreviewData[PlayerStatTag] > CurrentStatData[PlayerStatTag])
		{
			ActivatedStatModifyMap[PlayerStatTag] = FSlateColor(FLinearColor::Green);
		}
		else
		{
			ActivatedStatModifyMap[PlayerStatTag] = FSlateColor(FLinearColor::White);
		}
	}
}

void UStatModifyWidget::OnDecideButtonClicked()
{
	auto DomiGI = Cast<UDomiGameInstance>(GetGameInstance());
	check(DomiGI);
	
	DomiGI->SubtractPlayerCurrentEssence(AccumulatedRequiredEssence);
	PlayerStatusComponent->DecideStatChangeFromUI(PlayerStatPreviewData);

}

void UStatModifyWidget::OnStrUpButtonClicked()
{
	if (!bCanLevelUp) return;
	if (PlayerStatPreviewData[StatTags::STR] >= MaxStatValue) return;

	PlayerStatPreviewData[StatTags::STR]++;
	StatModifiedNumMap[StatTags::STR]++;
	StatModifiedStringMap[StatTags::STR] = FString::Printf(TEXT("%d (+%d)"), FMath::RoundToInt(PlayerStatPreviewData[StatTags::STR]), StatModifiedNumMap[StatTags::STR]);
	
	AccumulatedRequiredEssence += PlayerStatusComponent->GetLevelUpRequiredEssence(PlayerStatPreviewData[StatTags::Level]);
	PlayerStatPreviewData[StatTags::Level]++;
	StatModifiedNumMap[StatTags::Level]++;
	
	StatModifiedNumMap[StatTags::AttackPower] = FMath::RoundToInt(PlayerStatusComponent->GetCalculatedBattleStat(StatTags::AttackPower, PlayerStatPreviewData) - PreAttackPower);
	StatModifiedNumMap[StatTags::SubAttackPower] = FMath::RoundToInt(PlayerStatusComponent->GetCalculatedBattleStat(StatTags::SubAttackPower, PlayerStatPreviewData) - PreSubAttackPower);
	
	StatModifiedStringMap[StatTags::AttackPower] = FString::Printf(TEXT("%d (+%d)"), FMath::RoundToInt(PlayerStatPreviewData[StatTags::AttackPower]), StatModifiedNumMap[StatTags::AttackPower]);
	StatModifiedStringMap[StatTags::SubAttackPower] = FString::Printf(TEXT("%d (+%d)"), FMath::RoundToInt(PlayerStatPreviewData[StatTags::SubAttackPower]), StatModifiedNumMap[StatTags::SubAttackPower]);
	
	UpdatePreviewStat();

	StatModifiedStringMap[StatTags::Level] = FString::Printf(TEXT("%d (+%d)"), FMath::RoundToInt(PlayerStatPreviewData[StatTags::Level]), StatModifiedNumMap[StatTags::Level]);
	
}

void UStatModifyWidget::OnStrDownButtonClicked()
{
	if (StatModifiedNumMap[StatTags::STR] == 0) return;
	
	PlayerStatPreviewData[StatTags::STR]--;
	StatModifiedNumMap[StatTags::STR]--;
	
	PlayerStatPreviewData[StatTags::Level]--;
	StatModifiedNumMap[StatTags::Level]--;

	StatModifiedNumMap[StatTags::AttackPower] = FMath::RoundToInt(PlayerStatusComponent->GetCalculatedBattleStat(StatTags::AttackPower, PlayerStatPreviewData) - PreAttackPower);
	StatModifiedNumMap[StatTags::SubAttackPower] = FMath::RoundToInt(PlayerStatusComponent->GetCalculatedBattleStat(StatTags::SubAttackPower, PlayerStatPreviewData) - PreSubAttackPower);

	AccumulatedRequiredEssence -= PlayerStatusComponent->GetLevelUpRequiredEssence(PlayerStatPreviewData[StatTags::Level]);
	
	if (StatModifiedNumMap[StatTags::STR] == 0)
	{
		StatModifiedStringMap[StatTags::STR] = FString::Printf(TEXT("%d"), FMath::RoundToInt(PlayerStatPreviewData[StatTags::STR]));
		StatModifiedStringMap[StatTags::AttackPower] = FString::Printf(TEXT("%d"), FMath::RoundToInt(PlayerStatPreviewData[StatTags::AttackPower]));
		StatModifiedStringMap[StatTags::SubAttackPower] = FString::Printf(TEXT("%d"), FMath::RoundToInt(PlayerStatPreviewData[StatTags::SubAttackPower]));
		UpdatePreviewStat();
		StatModifiedStringMap[StatTags::Level] = FString::Printf(TEXT("%d"), FMath::RoundToInt(PlayerStatPreviewData[StatTags::Level]));
	}
	else
	{
		StatModifiedStringMap[StatTags::STR] = FString::Printf(TEXT("%d (+%d)"), FMath::RoundToInt(PlayerStatPreviewData[StatTags::STR]), StatModifiedNumMap[StatTags::STR]);
		StatModifiedStringMap[StatTags::AttackPower] = FString::Printf(TEXT("%d (+%d)"), FMath::RoundToInt(PlayerStatPreviewData[StatTags::AttackPower]), StatModifiedNumMap[StatTags::AttackPower]);
		StatModifiedStringMap[StatTags::SubAttackPower] = FString::Printf(TEXT("%d (+%d)"), FMath::RoundToInt(PlayerStatPreviewData[StatTags::SubAttackPower]), StatModifiedNumMap[StatTags::SubAttackPower]);
		UpdatePreviewStat();
		StatModifiedStringMap[StatTags::Level] = FString::Printf(TEXT("%d (+%d)"), FMath::RoundToInt(PlayerStatPreviewData[StatTags::Level]), StatModifiedNumMap[StatTags::Level]);
	}
}

void UStatModifyWidget::OnLifeUpButtonClicked()
{
	if (!bCanLevelUp) return;
	if (PlayerStatPreviewData[StatTags::LIFE] >= MaxStatValue) return;
	PlayerStatPreviewData[StatTags::LIFE]++;
	StatModifiedNumMap[StatTags::LIFE]++;
	StatModifiedStringMap[StatTags::LIFE] = FString::Printf(TEXT("%d (+%d)"), FMath::RoundToInt(PlayerStatPreviewData[StatTags::LIFE]), StatModifiedNumMap[StatTags::LIFE]);
	
	AccumulatedRequiredEssence += PlayerStatusComponent->GetLevelUpRequiredEssence(PlayerStatPreviewData[StatTags::Level]);
	PlayerStatPreviewData[StatTags::Level]++;
	StatModifiedNumMap[StatTags::Level]++;

	StatModifiedNumMap[StatTags::MaxHealth] = FMath::RoundToInt(PlayerStatusComponent->GetCalculatedBattleStat(StatTags::MaxHealth, PlayerStatPreviewData) - PreMaxHealth);
	
	StatModifiedStringMap[StatTags::MaxHealth] = FString::Printf(TEXT("%d (+%d)"), FMath::RoundToInt(PlayerStatPreviewData[StatTags::MaxHealth]), StatModifiedNumMap[StatTags::MaxHealth]);
	StatModifiedStringMap[StatTags::Level] = FString::Printf(TEXT("%d (+%d)"), FMath::RoundToInt(PlayerStatPreviewData[StatTags::Level]), StatModifiedNumMap[StatTags::Level]);
	
	UpdatePreviewStat();
}

void UStatModifyWidget::OnLifeDownButtonClicked()
{
	if (StatModifiedNumMap[StatTags::LIFE] == 0) return;
	
	PlayerStatPreviewData[StatTags::LIFE]--;
	StatModifiedNumMap[StatTags::LIFE]--;
	
	PlayerStatPreviewData[StatTags::Level]--;
	StatModifiedNumMap[StatTags::Level]--;

	StatModifiedNumMap[StatTags::MaxHealth] = FMath::RoundToInt(PlayerStatusComponent->GetCalculatedBattleStat(StatTags::MaxHealth, PlayerStatPreviewData) - PreMaxHealth);

	AccumulatedRequiredEssence -= PlayerStatusComponent->GetLevelUpRequiredEssence(PlayerStatPreviewData[StatTags::Level]);
	
	if (StatModifiedNumMap[StatTags::LIFE] == 0)
	{
		StatModifiedStringMap[StatTags::LIFE] = FString::Printf(TEXT("%d"), FMath::RoundToInt(PlayerStatPreviewData[StatTags::LIFE]));
		StatModifiedStringMap[StatTags::MaxHealth] = FString::Printf(TEXT("%d"), FMath::RoundToInt(PlayerStatPreviewData[StatTags::MaxHealth]));
		StatModifiedStringMap[StatTags::Level] = FString::Printf(TEXT("%d"), FMath::RoundToInt(PlayerStatPreviewData[StatTags::Level]));
		UpdatePreviewStat();
	}
	else
	{
		StatModifiedStringMap[StatTags::LIFE] = FString::Printf(TEXT("%d (+%d)"), FMath::RoundToInt(PlayerStatPreviewData[StatTags::LIFE]), StatModifiedNumMap[StatTags::LIFE]);
		StatModifiedStringMap[StatTags::MaxHealth] = FString::Printf(TEXT("%d (+%d)"), FMath::RoundToInt(PlayerStatPreviewData[StatTags::MaxHealth]),StatModifiedNumMap[StatTags::MaxHealth]);
		StatModifiedStringMap[StatTags::Level] = FString::Printf(TEXT("%d (+%d)"), FMath::RoundToInt(PlayerStatPreviewData[StatTags::Level]), StatModifiedNumMap[StatTags::Level]);
		UpdatePreviewStat();
	}
}

void UStatModifyWidget::OnSplUpButtonClicked()
{
	if (!bCanLevelUp) return;
	if (PlayerStatPreviewData[StatTags::SPL] >= MaxStatValue) return;

	PlayerStatPreviewData[StatTags::SPL]++;
	StatModifiedNumMap[StatTags::SPL]++;
	StatModifiedStringMap[StatTags::SPL] = FString::Printf(TEXT("%d (+%d)"), FMath::RoundToInt(PlayerStatPreviewData[StatTags::SPL]), StatModifiedNumMap[StatTags::SPL]);
	
	AccumulatedRequiredEssence += PlayerStatusComponent->GetLevelUpRequiredEssence(PlayerStatPreviewData[StatTags::Level]);
	PlayerStatPreviewData[StatTags::Level]++;
	StatModifiedNumMap[StatTags::Level]++;
	StatModifiedNumMap[StatTags::MagicPower] = FMath::RoundToInt(PlayerStatusComponent->GetCalculatedBattleStat(StatTags::MagicPower, PlayerStatPreviewData) - PreMagicPower);
	
	StatModifiedStringMap[StatTags::Level] = FString::Printf(TEXT("%d (+%d)"), FMath::RoundToInt(PlayerStatPreviewData[StatTags::Level]), StatModifiedNumMap[StatTags::Level]);
	StatModifiedStringMap[StatTags::MagicPower] = FString::Printf(TEXT("%d (+%d)"), FMath::RoundToInt(PlayerStatPreviewData[StatTags::MagicPower]), StatModifiedNumMap[StatTags::MagicPower]);
	
	UpdatePreviewStat();
}

void UStatModifyWidget::OnSplDownButtonClicked()
{
	if (StatModifiedNumMap[StatTags::SPL] == 0) return;
	
	PlayerStatPreviewData[StatTags::SPL]--;
	StatModifiedNumMap[StatTags::SPL]--;
	
	PlayerStatPreviewData[StatTags::Level]--;
	StatModifiedNumMap[StatTags::Level]--;

	StatModifiedNumMap[StatTags::MagicPower] = FMath::RoundToInt(PlayerStatusComponent->GetCalculatedBattleStat(StatTags::MagicPower, PlayerStatPreviewData) - PreMagicPower);

	AccumulatedRequiredEssence -= PlayerStatusComponent->GetLevelUpRequiredEssence(PlayerStatPreviewData[StatTags::Level]);
	
	if (StatModifiedNumMap[StatTags::SPL] == 0)
	{
		StatModifiedStringMap[StatTags::SPL] = FString::Printf(TEXT("%d"), FMath::RoundToInt(PlayerStatPreviewData[StatTags::SPL]));
		StatModifiedStringMap[StatTags::MagicPower] = FString::Printf(TEXT("%d"), FMath::RoundToInt(PlayerStatPreviewData[StatTags::MagicPower]));
		StatModifiedStringMap[StatTags::Level] = FString::Printf(TEXT("%d"), FMath::RoundToInt(PlayerStatPreviewData[StatTags::Level]));
		UpdatePreviewStat();
	}
	else
	{
		StatModifiedStringMap[StatTags::SPL] = FString::Printf(TEXT("%d (+%d)"), FMath::RoundToInt(PlayerStatPreviewData[StatTags::SPL]), StatModifiedNumMap[StatTags::SPL]);
		StatModifiedStringMap[StatTags::MagicPower] = FString::Printf(TEXT("%d (+%d)"), FMath::RoundToInt(PlayerStatPreviewData[StatTags::MagicPower]), StatModifiedNumMap[StatTags::MagicPower]);
		StatModifiedStringMap[StatTags::Level] = FString::Printf(TEXT("%d (+%d)"), FMath::RoundToInt(PlayerStatPreviewData[StatTags::Level]), StatModifiedNumMap[StatTags::Level]);
		UpdatePreviewStat();
	}

}

void UStatModifyWidget::OnEndUpButtonClicked()
{
	if (!bCanLevelUp) return;
	if (PlayerStatPreviewData[StatTags::END] >= MaxStatValue) return;

	PlayerStatPreviewData[StatTags::END]++;
	StatModifiedNumMap[StatTags::END]++;
	StatModifiedStringMap[StatTags::END] = FString::Printf(TEXT("%d (+%d)"), FMath::RoundToInt(PlayerStatPreviewData[StatTags::END]), StatModifiedNumMap[StatTags::END]);
	
	AccumulatedRequiredEssence += PlayerStatusComponent->GetLevelUpRequiredEssence(PlayerStatPreviewData[StatTags::Level]);
	PlayerStatPreviewData[StatTags::Level]++;
	StatModifiedNumMap[StatTags::Level]++;

	StatModifiedNumMap[StatTags::MaxStamina] = FMath::RoundToInt(PlayerStatusComponent->GetCalculatedBattleStat(StatTags::MaxStamina, PlayerStatPreviewData) - PreMaxStamina);
	
	StatModifiedStringMap[StatTags::Level] = FString::Printf(TEXT("%d (+%d)"), FMath::RoundToInt(PlayerStatPreviewData[StatTags::Level]), StatModifiedNumMap[StatTags::Level]);
	StatModifiedStringMap[StatTags::MaxStamina] = FString::Printf(TEXT("%d (+%d)"), FMath::RoundToInt(PlayerStatPreviewData[StatTags::MaxStamina]), StatModifiedNumMap[StatTags::MaxStamina]);
	
	UpdatePreviewStat();
}

void UStatModifyWidget::OnEndDownButtonClicked()
{
	if (StatModifiedNumMap[StatTags::END] == 0) return;
	
	PlayerStatPreviewData[StatTags::END]--;
	StatModifiedNumMap[StatTags::END]--;
	
	PlayerStatPreviewData[StatTags::Level]--;
	StatModifiedNumMap[StatTags::Level]--;

	StatModifiedNumMap[StatTags::MaxStamina] = FMath::RoundToInt(PlayerStatusComponent->GetCalculatedBattleStat(StatTags::MaxStamina, PlayerStatPreviewData) - PreMaxStamina);

	AccumulatedRequiredEssence -= PlayerStatusComponent->GetLevelUpRequiredEssence(PlayerStatPreviewData[StatTags::Level]);
	
	if (StatModifiedNumMap[StatTags::END] == 0)
	{
		StatModifiedStringMap[StatTags::END] = FString::Printf(TEXT("%d"), FMath::RoundToInt(PlayerStatPreviewData[StatTags::END]));
		StatModifiedStringMap[StatTags::MaxStamina] = FString::Printf(TEXT("%d"), FMath::RoundToInt(PlayerStatPreviewData[StatTags::MaxStamina]));
		StatModifiedStringMap[StatTags::Level] = FString::Printf(TEXT("%d"), FMath::RoundToInt(PlayerStatPreviewData[StatTags::Level]));
		UpdatePreviewStat();
	}
	else
	{
		StatModifiedStringMap[StatTags::END] = FString::Printf(TEXT("%d (+%d)"), FMath::RoundToInt(PlayerStatPreviewData[StatTags::END]), StatModifiedNumMap[StatTags::END]);
		StatModifiedStringMap[StatTags::MaxStamina] = FString::Printf(TEXT("%d (+%d)"), FMath::RoundToInt(PlayerStatPreviewData[StatTags::MaxStamina]), StatModifiedNumMap[StatTags::MaxStamina]);
		StatModifiedStringMap[StatTags::Level] = FString::Printf(TEXT("%d (+%d)"), FMath::RoundToInt(PlayerStatPreviewData[StatTags::Level]), StatModifiedNumMap[StatTags::Level]);
		UpdatePreviewStat();
	}
}




