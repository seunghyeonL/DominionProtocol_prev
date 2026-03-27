// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CheatManager.h"
#include "EnumAndStruct/EGameStoryState.h"
#include "DevCheatManager.generated.h"


UCLASS()
class DOMINIONPROTOCOL_API UDevCheatManager : public UCheatManager
{
	GENERATED_BODY()

public:
	UFUNCTION(Exec)
	void Save(int32 SlotIndex);

	UFUNCTION(Exec)
	void Load(int32 SlotIndex);

	UFUNCTION(Exec)
	void StartGameNewSlot(int32 SlotIndex);

	UFUNCTION(Exec)
	void SaveSlotLoadAndStartGame(int32 SlotIndex);

	UFUNCTION(Exec)
	void ToggleDebugLines();

	UFUNCTION(Exec)
	void ToggleMnhDebug();

	UFUNCTION(Exec)
	void ToggleAllDebug();
	
	virtual void God() override;
	
	UFUNCTION(Exec)
	void InfiniteStamina();

	UFUNCTION(Exec)
	void MoveToCrack(FString TargetLevelName, int32 TargetCrackIndex);

	UFUNCTION(Exec)
	void ForceMoveToCrack(int32 TargetCrackIndex);

	UFUNCTION(Exec)
	void GetStoryState();
	
	UFUNCTION(Exec)
	void SetStoryState(EGameStoryState NewStoryState);
	
	UFUNCTION(Exec)
	void GetCurrentStoryState();

	UFUNCTION(Exec)
	void Suicide();

	UFUNCTION(Exec)
	void AddAllItems();

	UFUNCTION(Exec)
	void EquipSword();

	UFUNCTION(Exec)
	void EquipAxe();

	UFUNCTION(Exec)
	void EquipClaw();

	UFUNCTION(Exec)
	void ActivateEffect(FName EffectTagName, float Magnitude, float Duration);

	UFUNCTION(Exec)
	void ToggleFog();

	UFUNCTION(Exec)
	void ShowMeTheMoney();

	UFUNCTION(Exec)
	void ShowSTRStat();

	UFUNCTION(Exec)
	void ShowAttackPowerStat();

	UFUNCTION(Exec)
	void ShowSubAttackPowerStat();
};
