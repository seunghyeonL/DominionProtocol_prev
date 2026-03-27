// Fill out your copyright notice in the Description page of Project Settings.


#include "DevCheatManager.h"

#include "MnhHelpers.h"
#include "Components/ExponentialHeightFogComponent.h"
#include "Player/Characters/DomiCharacter.h"
#include "Components/StatusComponent/StatusComponent.h"
#include "DomiFramework/GameMode/BaseGameMode.h"
#include "Engine/ExponentialHeightFog.h"
#include "Kismet/GameplayStatics.h"

#include "Util/GameTagList.h"
#include "Util/CheatBPLib.h"
#include "Util/DebugHelper.h"

void UDevCheatManager::Save(int32 SlotIndex)
{
	UWorld* World = GetWorld();
	if (IsValid(World))
	{
		UCheatBPLib::Save(World, SlotIndex);
	}
}

void UDevCheatManager::Load(int32 SlotIndex)
{
	UWorld* World = GetWorld();
	if (IsValid(World))
	{
		UCheatBPLib::Load(World, SlotIndex);
	}
}

void UDevCheatManager::StartGameNewSlot(int32 SlotIndex)
{
	UWorld* World = GetWorld();
	if (IsValid(World))
	{
		UCheatBPLib::StartGameNewSlot(World, SlotIndex);
	}
}

void UDevCheatManager::SaveSlotLoadAndStartGame(int32 SlotIndex)
{
	UWorld* World = GetWorld();
	if (IsValid(World))
	{
		UCheatBPLib::SaveSlotLoadAndStartGame(World, SlotIndex);
	}
}

void UDevCheatManager::ToggleDebugLines()
{
	Debug::ToggleDebugLines();
}

void UDevCheatManager::ToggleMnhDebug()
{
	//MissNoHit 플러그인 디버그 토글
	IConsoleVariable* CVar = IConsoleManager::Get().FindConsoleVariable(TEXT("Mnh.ShowLines"));
	if (CVar)
	{
		int32 CurrentValue = CVar->GetInt();
		int32 NewValue = (CurrentValue == 0) ? 1 : 0;
		CVar->Set(NewValue);
		Debug::Print(FString::Printf(TEXT("MissNoHit Debug : %s"), NewValue ? TEXT("On") : TEXT("Off")));
	}
}

void UDevCheatManager::ToggleAllDebug()
{
	// 엔진 디버그 + MissNoHit 동시 토글
	int32 CurrentValue = Debug::CVarShowDebugLines.GetValueOnAnyThread();
	int32 NewValue = (CurrentValue == 0) ? 1 : 0;
    
	// 두 콘솔 변수 함께 설정
	IConsoleManager::Get().FindConsoleVariable(TEXT("Debug.ShowLines"))->Set(NewValue);
	IConsoleManager::Get().FindConsoleVariable(TEXT("Mnh.ShowLines"))->Set(NewValue);

	Debug::Print(FString::Printf(TEXT("All Debug : %s"), NewValue ? TEXT("On") : TEXT("Off")));
}

void UDevCheatManager::God()
{
	ADomiCharacter* PlayerCharacter = Cast<ADomiCharacter>(GetOuterAPlayerController()->GetPawn());
	TObjectPtr<UStatusComponent> StatusComponent = PlayerCharacter->GetStatusComponent();
	if (IsValid(StatusComponent))
	{
		StatusComponent->SwitchGodMode();
	}
}

void UDevCheatManager::InfiniteStamina()
{
	ADomiCharacter* PlayerCharacter = Cast<ADomiCharacter>(GetOuterAPlayerController()->GetPawn());
	TObjectPtr<UStatusComponent> StatusComponent = PlayerCharacter->GetStatusComponent();
	if (IsValid(StatusComponent))
	{
		UCheatBPLib::InfiniteStamina(StatusComponent);
	}
}

void UDevCheatManager::MoveToCrack(FString TargetLevelName, int32 TargetCrackIndex)
{
	ABaseGameMode* BaseGameMode = Cast<ABaseGameMode>(GetWorld()->GetAuthGameMode());
	UCheatBPLib::MoveToCrack(BaseGameMode, TargetLevelName, TargetCrackIndex);
}

void UDevCheatManager::ForceMoveToCrack(int32 TargetCrackIndex)
{
	UCheatBPLib::ForceMoveToCrack(GetWorld(), TargetCrackIndex, Cast<ADomiCharacter>(GetOuterAPlayerController()->GetPawn()));
}

void UDevCheatManager::GetStoryState()
{
	UWorld* World = GetOuter()->GetWorld();
	if (!IsValid(World)) return;

	UDomiGameInstance* GI = Cast<UDomiGameInstance>(World->GetGameInstance());
	if (!IsValid(GI)) return;
	
	Debug::Print(FString::Printf(TEXT("Current Story State : %d"), GI->GetCurrentGameStoryState()));
}

void UDevCheatManager::SetStoryState(EGameStoryState NewStoryState)
{
	UWorld* World = GetOuter()->GetWorld();
	if (!IsValid(World)) return;

	UDomiGameInstance* GI = Cast<UDomiGameInstance>(World->GetGameInstance());
	if (IsValid(GI))
	{
		GI->SetCurrentGameStoryState(NewStoryState);
	}
}

void UDevCheatManager::GetCurrentStoryState()
{
	UWorld* World = GetOuter()->GetWorld();
	if (!IsValid(World)) return;

	UDomiGameInstance* GI = Cast<UDomiGameInstance>(World->GetGameInstance());
	if (IsValid(GI))
	{
		EGameStoryState CurrentStoryState = GI->GetCurrentGameStoryState();
		const UEnum* EnumPtr = StaticEnum<EGameStoryState>();
		if (IsValid(EnumPtr))
		{
			Debug::Print(EnumPtr->GetNameStringByValue(static_cast<int64>(CurrentStoryState)));
		}
	}
}

void UDevCheatManager::Suicide()
{
	ADomiCharacter* PlayerCharacter = Cast<ADomiCharacter>(GetOuterAPlayerController()->GetPawn());

	if (PlayerCharacter->GetClass()->ImplementsInterface(UDamagable::StaticClass()))
	{
		IDamagable::Execute_OnAttacked(PlayerCharacter, { nullptr, UE_BIG_NUMBER, 0.f, FVector::ZeroVector, {}});
	}
}

void UDevCheatManager::AddAllItems()
{
	UWorld* World = GetWorld();
	if (IsValid(World))
	{
		UCheatBPLib::AddAllItemsToPlayerInventoryMaxQuantity(World);
	}
}

void UDevCheatManager::EquipSword()
{
	UWorld* World = GetWorld();
	if (IsValid(World))
	{
		UCheatBPLib::EquipSword(World);
	}
}

void UDevCheatManager::EquipAxe()
{
	UWorld* World = GetWorld();
	if (IsValid(World))
	{
		UCheatBPLib::EquipAxe(World);
	}
}

void UDevCheatManager::EquipClaw()
{
	UWorld* World = GetWorld();
	if (IsValid(World))
	{
		UCheatBPLib::EquipClaw(World);
	}
}

void UDevCheatManager::ActivateEffect(FName GameplayTag, float Magnitude, float Duration)
{
	ADomiCharacter* PlayerCharacter = Cast<ADomiCharacter>(GetOuterAPlayerController()->GetPawn());
	check(PlayerCharacter);
	
	FGameplayTag EffectTag = FGameplayTag::RequestGameplayTag(GameplayTag);
	
	if (EffectTag.MatchesTag(FGameplayTag::RequestGameplayTag(TEXT("Effect.Control"))))
	{
		if (auto ControlComponent = PlayerCharacter->FindComponentByClass<UPlayerControlComponent>())
		{
			if (FMath::IsNearlyZero(Duration))
			{
				ControlComponent->ActivateControlEffect(EffectTag);
			}
			else
			{
				ControlComponent->ActivateControlEffect(EffectTag, Duration);
			}
		}
	}
	else
	{
		if (auto StatusComponent = PlayerCharacter->FindComponentByClass<UStatusComponent>())
		{
			if (FMath::IsNearlyZero(Duration))
			{
				StatusComponent->ActivateStatusEffect(EffectTag, Magnitude);
			}
			else
			{
				StatusComponent->ActivateStatusEffect(EffectTag, Magnitude, Duration);
			}
			
		}
	}
}

void UDevCheatManager::ToggleFog()
{
	UWorld* World = GetWorld();
	check(IsValid(World));
	
	UCheatBPLib::ToggleFog(World);
}

void UDevCheatManager::ShowMeTheMoney()
{
	UDomiGameInstance* GameInstance = Cast<UDomiGameInstance>(GetWorld()->GetGameInstance());
	if (IsValid(GameInstance))
	{
		GameInstance->AddPlayerCurrentEssence(100000);
	}
}

void UDevCheatManager::ShowSTRStat()
{
	ADomiCharacter* PlayerCharacter = Cast<ADomiCharacter>(GetOuterAPlayerController()->GetPawn());
	if (IsValid(PlayerCharacter))
	{
		UStatusComponent* StatusComponent = PlayerCharacter->GetStatusComponent();
		float Strength = StatusComponent->GetStat(StatTags::STR);
		Debug::Print(FString::Printf(TEXT("%f"), Strength));
	}
}

void UDevCheatManager::ShowAttackPowerStat()
{
	ADomiCharacter* PlayerCharacter = Cast<ADomiCharacter>(GetOuterAPlayerController()->GetPawn());
	if (IsValid(PlayerCharacter))
	{
		UStatusComponent* StatusComponent = PlayerCharacter->GetStatusComponent();
		float AttackPower = StatusComponent->GetStat(StatTags::AttackPower);
		Debug::Print(FString::Printf(TEXT("%f"), AttackPower));
	}
}

void UDevCheatManager::ShowSubAttackPowerStat()
{
	ADomiCharacter* PlayerCharacter = Cast<ADomiCharacter>(GetOuterAPlayerController()->GetPawn());
	if (IsValid(PlayerCharacter))
	{
		UStatusComponent* StatusComponent = PlayerCharacter->GetStatusComponent();
		float SubAttackPower = StatusComponent->GetStat(StatTags::SubAttackPower);
		Debug::Print(FString::Printf(TEXT("%f"), SubAttackPower));
	}
}

