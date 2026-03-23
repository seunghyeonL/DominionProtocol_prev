
#include "GameTagList.h"

namespace EffectTags
{
	// ControlEffectTags
	UE_DEFINE_GAMEPLAY_TAG(Interact, "Effect.Control.Interact");
	UE_DEFINE_GAMEPLAY_TAG(UsingSkill, "Effect.Control.UsingSkill");
	UE_DEFINE_GAMEPLAY_TAG(ConsumingItem, "Effect.Control.ConsumingItem");
	UE_DEFINE_GAMEPLAY_TAG(Death, "Effect.Control.Death");
	UE_DEFINE_GAMEPLAY_TAG(Dash, "Effect.Control.Dash");
	UE_DEFINE_GAMEPLAY_TAG(Parry, "Effect.Control.Parry");
	UE_DEFINE_GAMEPLAY_TAG(DashInvincible, "Effect.Control.DashInvincible");
	UE_DEFINE_GAMEPLAY_TAG(DashAttackPossible, "Effect.Control.DashAttackPossible");
	UE_DEFINE_GAMEPLAY_TAG(UsingDash, "Effect.Control.UsingSkill.Dash");
	UE_DEFINE_GAMEPLAY_TAG(UsingParry, "Effect.Control.UsingSkill.Parry");
	UE_DEFINE_GAMEPLAY_TAG(UsingZoneya, "Effect.Control.UsingSkill.Zoneya");
	UE_DEFINE_GAMEPLAY_TAG(UsingTeleport, "Effect.Control.UsingSkill.Teleport");
	UE_DEFINE_GAMEPLAY_TAG(LockOn, "Effect.Control.LockOn");
	UE_DEFINE_GAMEPLAY_TAG(Groggy, "Effect.Control.Groggy");
	UE_DEFINE_GAMEPLAY_TAG(Parried, "Effect.Control.Parried");

	UE_DEFINE_GAMEPLAY_TAG(Stiffness, "Effect.Control.Debuff.Stiffness");
	UE_DEFINE_GAMEPLAY_TAG(Flew, "Effect.Control.Flew");
	UE_DEFINE_GAMEPLAY_TAG(StandingUp, "Effect.Control.StandingUp");

	UE_DEFINE_GAMEPLAY_TAG(ControlDebuff, "Effect.Control.Debuff");
	UE_DEFINE_GAMEPLAY_TAG(ControlBuff, "Effect.Control.Buff");
	UE_DEFINE_GAMEPLAY_TAG(Stun, "Effect.Control.Debuff.Stun");
	UE_DEFINE_GAMEPLAY_TAG(PreStun, "Effect.Control.Debuff.PreStun");
	UE_DEFINE_GAMEPLAY_TAG(Silence, "Effect.Control.Debuff.Silence");
	UE_DEFINE_GAMEPLAY_TAG(Confused, "Effect.Control.Debuff.Confused");
	
	// StatusEffectTags
	UE_DEFINE_GAMEPLAY_TAG(Running, "Effect.Status.Running");
	UE_DEFINE_GAMEPLAY_TAG(Walking, "Effect.Status.Walking");

	UE_DEFINE_GAMEPLAY_TAG(StatusDebuff, "Effect.Status.Debuff");
	UE_DEFINE_GAMEPLAY_TAG(StatusBuff, "Effect.Status.Buff");
	
	UE_DEFINE_GAMEPLAY_TAG(Slow, "Effect.Status.Debuff.Slow");
	UE_DEFINE_GAMEPLAY_TAG(AttackDown, "Effect.Status.Debuff.AttackDown");
	UE_DEFINE_GAMEPLAY_TAG(Burn, "Effect.Status.Debuff.Burn");
	UE_DEFINE_GAMEPLAY_TAG(Poison, "Effect.Status.Debuff.Poison");

	// AIEffectTags
	UE_DEFINE_GAMEPLAY_TAG(Idle, "Effect.AIState.Idle");
	UE_DEFINE_GAMEPLAY_TAG(Suspicious, "Effect.AIState.Suspicious");
	UE_DEFINE_GAMEPLAY_TAG(Alert, "Effect.AIState.Alert");
	UE_DEFINE_GAMEPLAY_TAG(Combat, "Effect.AIState.Combat");
	UE_DEFINE_GAMEPLAY_TAG(Patrol, "Effect.AIState.Patrol");
	UE_DEFINE_GAMEPLAY_TAG(Return, "Effect.AIState.Return");
}

namespace StatTags
{
	// BaseStatTags
	UE_DEFINE_GAMEPLAY_TAG(BaseStat, "Stat.Base");
	UE_DEFINE_GAMEPLAY_TAG(Level, "Stat.Base.Level");
	UE_DEFINE_GAMEPLAY_TAG(LIFE, "Stat.Base.LIFE");
	UE_DEFINE_GAMEPLAY_TAG(STR, "Stat.Base.STR");
	UE_DEFINE_GAMEPLAY_TAG(SPL, "Stat.Base.SPL");
	UE_DEFINE_GAMEPLAY_TAG(END, "Stat.Base.END");
	UE_DEFINE_GAMEPLAY_TAG(BaseAttackPower, "Stat.Base.BaseAttackPower");
	UE_DEFINE_GAMEPLAY_TAG(BaseMagicPower, "Stat.Base.BaseMagicPower");
	UE_DEFINE_GAMEPLAY_TAG(BaseMaxHealth, "Stat.Base.BaseMaxHealth");
	UE_DEFINE_GAMEPLAY_TAG(BaseMaxStamina, "Stat.Base.BaseMaxStamina");

	// Base -> Battle Coefficients
	UE_DEFINE_GAMEPLAY_TAG(StatCoefficient, "Stat.Coefficient");
	UE_DEFINE_GAMEPLAY_TAG(AttackPowerCoefficient, "Stat.Coefficient.AttackPower");
	UE_DEFINE_GAMEPLAY_TAG(MagicPowerCoefficient, "Stat.Coefficient.MagicPower");
	UE_DEFINE_GAMEPLAY_TAG(MaxHealthCoefficient, "Stat.Coefficient.MaxHealth");
	UE_DEFINE_GAMEPLAY_TAG(MaxStaminaCoefficient, "Stat.Coefficient.MaxStamina");
	UE_DEFINE_GAMEPLAY_TAG(SoftCapThreshold, "Stat.Coefficient.SoftCapThreshold");
	UE_DEFINE_GAMEPLAY_TAG(PostSoftCapMultiplier, "Stat.Coefficient.PostSoftCapMultiplier");
	UE_DEFINE_GAMEPLAY_TAG(LevelUpCoefficientB, "Stat.Coefficient.LevelUpB");
	UE_DEFINE_GAMEPLAY_TAG(LevelUpCoefficientC, "Stat.Coefficient.LevelUpC");
	UE_DEFINE_GAMEPLAY_TAG(LevelUpCoefficientD, "Stat.Coefficient.LevelUpD");
	
	// BattleStatTags
	UE_DEFINE_GAMEPLAY_TAG(BattleStat, "Stat.Battle");
	UE_DEFINE_GAMEPLAY_TAG(MaxHealth, "Stat.Battle.MaxHealth");
	UE_DEFINE_GAMEPLAY_TAG(MaxStamina, "Stat.Battle.MaxStamina");
	UE_DEFINE_GAMEPLAY_TAG(AttackPower, "Stat.Battle.AttackPower");
	
	UE_DEFINE_GAMEPLAY_TAG(SubAttackPower, "Stat.Battle.SubAttackPower");
	UE_DEFINE_GAMEPLAY_TAG(MagicPower, "Stat.Battle.MagicPower");
	UE_DEFINE_GAMEPLAY_TAG(StaminaRecoveryRate, "Stat.Battle.StaminaRecoveryRate");
	UE_DEFINE_GAMEPLAY_TAG(SpeedCoefficient, "Stat.Battle.SpeedCoefficient");

	UE_DEFINE_GAMEPLAY_TAG(Defense, "Stat.Battle.Defense");
	UE_DEFINE_GAMEPLAY_TAG(MaxShield, "Stat.Battle.MaxShield");
	UE_DEFINE_GAMEPLAY_TAG(MaxGroggyGauge, "Stat.Battle.MaxGroggyGauge");
	
	// VariableStatTags
	UE_DEFINE_GAMEPLAY_TAG(VariableStat, "Stat.Variable");
	UE_DEFINE_GAMEPLAY_TAG(Health, "Stat.Variable.Health");
	UE_DEFINE_GAMEPLAY_TAG(Stamina, "Stat.Variable.Stamina");
	UE_DEFINE_GAMEPLAY_TAG(Shield, "Stat.Variable.Shield");
	UE_DEFINE_GAMEPLAY_TAG(GroggyGauge, "Stat.Variable.GroggyGauge");
}

namespace SkillGroupTags
{
	UE_DEFINE_GAMEPLAY_TAG(BaseAttack, "Skill.BaseAttack");
	UE_DEFINE_GAMEPLAY_TAG(WeaponSkill, "Skill.WeaponSkill");
	UE_DEFINE_GAMEPLAY_TAG(MagicSkill, "Skill.MagicSkill");
	UE_DEFINE_GAMEPLAY_TAG(DashAttack, "Skill.DashAttack");
	UE_DEFINE_GAMEPLAY_TAG(Dash, "Skill.Dash");
	UE_DEFINE_GAMEPLAY_TAG(Parry, "Skill.Parry");

	// Boss1
	UE_DEFINE_GAMEPLAY_TAG(SuperAttack, "Skill.Boss1.SuperAttack");
	UE_DEFINE_GAMEPLAY_TAG(SpecialAttack, "Skill.Boss1.SpecialAttack");
	UE_DEFINE_GAMEPLAY_TAG(EvadeAttack, "Skill.Boss1.EvadeAttack");
	UE_DEFINE_GAMEPLAY_TAG(JumpAttack, "Skill.Boss1.JumpAttack");
	UE_DEFINE_GAMEPLAY_TAG(RangedAttack, "Skill.Boss1.RangedAttack");

	// Boss2 Skill Group Tag
	UE_DEFINE_GAMEPLAY_TAG(Boss2BaseAttack1, "Skill.Boss2.BaseAttack1");
	UE_DEFINE_GAMEPLAY_TAG(Boss2BaseAttack2, "Skill.Boss2.BaseAttack2");
	UE_DEFINE_GAMEPLAY_TAG(Boss2Circling, "Skill.Boss2.Circling");
	UE_DEFINE_GAMEPLAY_TAG(Boss2DashAttack, "Skill.Boss2.DashAttack");
	UE_DEFINE_GAMEPLAY_TAG(Boss2EvadeAttack, "Skill.Boss2.EvadeAttack");
	UE_DEFINE_GAMEPLAY_TAG(Boss2TeleportAttack, "Skill.Boss2.TeleportAttack");

	// Boss4
	UE_DEFINE_GAMEPLAY_TAG(Boss4Phase2LevelStart, "Skill.Boss4.LevelStart.Phase2");
	UE_DEFINE_GAMEPLAY_TAG(Boss4WeaponAttack1Start, "Skill.Boss4.WeaponAttack1.start");
	UE_DEFINE_GAMEPLAY_TAG(Boss4WeaponAttack1First, "Skill.Boss4.WeaponAttack1.first");
	UE_DEFINE_GAMEPLAY_TAG(Boss4WeaponAttack1Second, "Skill.Boss4.WeaponAttack1.second");
	UE_DEFINE_GAMEPLAY_TAG(Boss4WeaponAttack1Third, "Skill.Boss4.WeaponAttack1.third");
	UE_DEFINE_GAMEPLAY_TAG(Boss4WeaponAttack2, "Skill.Boss4.WeaponAttack2");
	UE_DEFINE_GAMEPLAY_TAG(Boss4WeaponAttack3, "Skill.Boss4.WeaponAttack3");
	UE_DEFINE_GAMEPLAY_TAG(Boss4WeaponAttack3First, "Skill.Boss4.WeaponAttack3.first");
	UE_DEFINE_GAMEPLAY_TAG(Boss4WeaponAttack3Second, "Skill.Boss4.WeaponAttack3.second");
	UE_DEFINE_GAMEPLAY_TAG(Boss4WeaponAttack3Third, "Skill.Boss4.WeaponAttack3.third");
	UE_DEFINE_GAMEPLAY_TAG(Boss4WeaponAttack3Fourth, "Skill.Boss4.WeaponAttack3.fourth");
	UE_DEFINE_GAMEPLAY_TAG(Boss4FlameStrike, "Skill.Boss4.FlameStrike");
	UE_DEFINE_GAMEPLAY_TAG(Boss4SpawnClone, "Skill.Boss4.SpawnClone");
	UE_DEFINE_GAMEPLAY_TAG(Boss4CloneStart, "Skill.Boss4.CloneStart");
	UE_DEFINE_GAMEPLAY_TAG(Boss4InfernoSpike, "Skill.Boss4.InfernoSpike");
	UE_DEFINE_GAMEPLAY_TAG(Boss4HellfireFall, "Skill.Boss4.HellfireFall");
	UE_DEFINE_GAMEPLAY_TAG(Boss4BlazeSphere, "Skill.Boss4.BlazeSphere");
	UE_DEFINE_GAMEPLAY_TAG(Boss4BackDash, "Skill.Boss4.BackDash");
	UE_DEFINE_GAMEPLAY_TAG(Boss4Teleport, "Skill.Boss4.Teleport");
	UE_DEFINE_GAMEPLAY_TAG(Boss4RandomMove, "Skill.Boss4.RandomMove");
	UE_DEFINE_GAMEPLAY_TAG(Boss4EvadeAttack, "Skill.Boss4.EvadeAttack");
	UE_DEFINE_GAMEPLAY_TAG(Boss4RisingWall, "Skill.Boss4.RisingWall");
	UE_DEFINE_GAMEPLAY_TAG(Boss4SlashWave, "Skill.Boss4.SlashWave");
	UE_DEFINE_GAMEPLAY_TAG(Boss4SpawnClonePhase2, "Skill.Boss4.SpawnClonePhase2");
	UE_DEFINE_GAMEPLAY_TAG(Boss4ClonePhase2_1First, "Skill.Boss4.ClonePhase2_1.first");
	UE_DEFINE_GAMEPLAY_TAG(Boss4ClonePhase2_1Second, "Skill.Boss4.ClonePhase2_1.second");
	UE_DEFINE_GAMEPLAY_TAG(Boss4ClonePhase2_1Third, "Skill.Boss4.ClonePhase2_1.third");
	UE_DEFINE_GAMEPLAY_TAG(Boss4ClonePhase2_2First, "Skill.Boss4.ClonePhase2_2.first");
	UE_DEFINE_GAMEPLAY_TAG(Boss4ClonePhase2_2Second, "Skill.Boss4.ClonePhase2_2.second");
	UE_DEFINE_GAMEPLAY_TAG(Boss4ClonePhase2_2Third, "Skill.Boss4.ClonePhase2_2.third");
}

namespace SkillTags
{
	// Player
	UE_DEFINE_GAMEPLAY_TAG(BaseAttackSword1, "Skill.BaseAttack.first.Sword");
	UE_DEFINE_GAMEPLAY_TAG(BaseAttackSword2, "Skill.BaseAttack.second.Sword");
	UE_DEFINE_GAMEPLAY_TAG(BaseAttackSword3, "Skill.BaseAttack.third.Sword");
	UE_DEFINE_GAMEPLAY_TAG(BaseAttackAxe1, "Skill.BaseAttack.first.Axe");
	UE_DEFINE_GAMEPLAY_TAG(BaseAttackAxe2, "Skill.BaseAttack.second.Axe");
	UE_DEFINE_GAMEPLAY_TAG(BaseAttackAxe3, "Skill.BaseAttack.third.Axe");
	UE_DEFINE_GAMEPLAY_TAG(BaseAttackClaw1, "Skill.BaseAttack.first.Claw");
	UE_DEFINE_GAMEPLAY_TAG(BaseAttackClaw2, "Skill.BaseAttack.second.Claw");
	UE_DEFINE_GAMEPLAY_TAG(BaseAttackClaw3, "Skill.BaseAttack.third.Claw");
	UE_DEFINE_GAMEPLAY_TAG(BaseAttackClaw4, "Skill.BaseAttack.fourth.Claw");
	
	UE_DEFINE_GAMEPLAY_TAG(DashAttackSword, "Skill.DashAttack.Sword");
	UE_DEFINE_GAMEPLAY_TAG(DashAttackClaw, "Skill.DashAttack.Claw");
	UE_DEFINE_GAMEPLAY_TAG(DashAttackAxe, "Skill.DashAttack.Axe");

	UE_DEFINE_GAMEPLAY_TAG(WeaponSkillSword1, "Skill.WeaponSkill.first.Sword");
	UE_DEFINE_GAMEPLAY_TAG(WeaponSkillAxe1, "Skill.WeaponSkill.first.Axe");
	UE_DEFINE_GAMEPLAY_TAG(WeaponSkillClaw1, "Skill.WeaponSkill.first.Claw");

	UE_DEFINE_GAMEPLAY_TAG(BaseAttackSpear1, "Skill.BaseAttack.first.Spear");
	UE_DEFINE_GAMEPLAY_TAG(PlayerDash, "Skill.Dash.Player");
	UE_DEFINE_GAMEPLAY_TAG(PlayerParry, "Skill.Parry.Player");

	UE_DEFINE_GAMEPLAY_TAG(MagicLaserSkill, "Skill.MagicSkill.Laser");
	UE_DEFINE_GAMEPLAY_TAG(MagicRailGunSkill, "Skill.MagicSkill.RailGun");
	UE_DEFINE_GAMEPLAY_TAG(MagicFireBallSkill, "Skill.MagicSkill.FireBall");
	UE_DEFINE_GAMEPLAY_TAG(MagicZoneyaSkill, "Skill.MagicSkill.Zoneya");
	UE_DEFINE_GAMEPLAY_TAG(MagicInfiniteStaminaSkill, "Skill.MagicSkill.InfiniteStamina");
	UE_DEFINE_GAMEPLAY_TAG(MagicTeleportSkill, "Skill.MagicSkill.Teleport");
	UE_DEFINE_GAMEPLAY_TAG(MagicItemScanningSkill, "Skill.MagicSkill.ItemScanning");	

	// NomalMonster
	UE_DEFINE_GAMEPLAY_TAG(AxeSkeletonComboAttack, "Skill.NomalMonster.AxeSkeleton.ComboAttack");
	UE_DEFINE_GAMEPLAY_TAG(SwordSkeletonBaseAttack, "Skill.NomalMonster.SwordSkeleton.BaseAttack");
	UE_DEFINE_GAMEPLAY_TAG(DualBladeSkeletonBaseAttack, "Skill.NomalMonster.DualBladeSkeleton.BaseAttack");
	UE_DEFINE_GAMEPLAY_TAG(DualBladeSkeletonJumpAttack, "Skill.NomalMonster.DualBladeSkeleton.JumpAttack");
	UE_DEFINE_GAMEPLAY_TAG(KnifeThrow, "Skill.NomalMonster.KnifeSkeleton.KnifeThrow");
	UE_DEFINE_GAMEPLAY_TAG(WoodSwiping, "Skill.NomalMonster.Wood.Swiping");
	UE_DEFINE_GAMEPLAY_TAG(WoodHurricaneKick, "Skill.NomalMonster.Wood.HurricaneKick");
	UE_DEFINE_GAMEPLAY_TAG(MinionBaseAttack, "Skill.NomalMonster.Minion.BaseAttack");
	UE_DEFINE_GAMEPLAY_TAG(GunMinionFire, "Skill.NomalMonster.GunMinion.MinionFire");
	UE_DEFINE_GAMEPLAY_TAG(GolemSmash, "Skill.NomalMonster.EliteGolem.GolemSmash");


	// Boss1
	UE_DEFINE_GAMEPLAY_TAG(Boss1LevelStart, "Skill.Boss1.LevelStart");
	UE_DEFINE_GAMEPLAY_TAG(Boss1Parried, "Skill.Boss1.Parried");

	UE_DEFINE_GAMEPLAY_TAG(Boss1BaseAttack1, "Skill.Boss1.BaseAttack.first");
	UE_DEFINE_GAMEPLAY_TAG(Boss1BaseAttack2, "Skill.Boss1.BaseAttack.second");
	UE_DEFINE_GAMEPLAY_TAG(Boss1BaseAttack3, "Skill.Boss1.BaseAttack.third");

	UE_DEFINE_GAMEPLAY_TAG(Boss1SuperAttack1, "Skill.Boss1.SuperAttack.first");
	UE_DEFINE_GAMEPLAY_TAG(Boss1SuperAttack2, "Skill.Boss1.SuperAttack.second");

	UE_DEFINE_GAMEPLAY_TAG(Boss1SpecialAttack1, "Skill.Boss1.SpecialAttack.first");
	UE_DEFINE_GAMEPLAY_TAG(Boss1SpecialAttack2, "Skill.Boss1.SpecialAttack.second");

	UE_DEFINE_GAMEPLAY_TAG(Boss1EvadeAttack, "Skill.Boss1.EvadeAttack");

	UE_DEFINE_GAMEPLAY_TAG(Boss1JumpAttack, "Skill.Boss1.JumpAttack");

	UE_DEFINE_GAMEPLAY_TAG(Boss1RangedAttack, "Skill.Boss1.RangedAttack");

	// Boss2
	UE_DEFINE_GAMEPLAY_TAG(Boss2LevelStart, "Skill.Boss2.LevelStart");
	UE_DEFINE_GAMEPLAY_TAG(Boss2BaseAttack1First, "Skill.Boss2.BaseAttack1.first");
	UE_DEFINE_GAMEPLAY_TAG(Boss2BaseAttack1Second, "Skill.Boss2.BaseAttack1.second");
	UE_DEFINE_GAMEPLAY_TAG(Boss2BaseAttack1Third, "Skill.Boss2.BaseAttack1.third");
	UE_DEFINE_GAMEPLAY_TAG(Boss2BaseAttack1Fourth, "Skill.Boss2.BaseAttack1.Fourth");

	UE_DEFINE_GAMEPLAY_TAG(Boss2BaseAttack2First, "Skill.Boss2.BaseAttack2.first");
	UE_DEFINE_GAMEPLAY_TAG(Boss2BaseAttack2Second, "Skill.Boss2.BaseAttack2.second");
	UE_DEFINE_GAMEPLAY_TAG(Boss2BaseAttack2Third, "Skill.Boss2.BaseAttack2.third");
	UE_DEFINE_GAMEPLAY_TAG(Boss2BaseAttack2Fourth, "Skill.Boss2.BaseAttack2.fourth");
	UE_DEFINE_GAMEPLAY_TAG(Boss2BaseAttack2Fifth, "Skill.Boss2.BaseAttack2.fifth");

	UE_DEFINE_GAMEPLAY_TAG(Boss2Circling, "Skill.Boss2.Circling");

	UE_DEFINE_GAMEPLAY_TAG(Boss2DashAttackFirst, "Skill.Boss2.DashAttack.first");
	UE_DEFINE_GAMEPLAY_TAG(Boss2DashAttackSecond, "Skill.Boss2.DashAttack.second");
	UE_DEFINE_GAMEPLAY_TAG(Boss2DashAttackThird, "Skill.Boss2.DashAttack.third");
	UE_DEFINE_GAMEPLAY_TAG(Boss2DashAttackBFirst, "Skill.Boss2.DashAttackB.first");
	UE_DEFINE_GAMEPLAY_TAG(Boss2DashAttackBSecond, "Skill.Boss2.DashAttackB.second");
	UE_DEFINE_GAMEPLAY_TAG(Boss2EvadeAttackFirst, "Skill.Boss2.EvadeAttack.first");
	UE_DEFINE_GAMEPLAY_TAG(Boss2EvadeAttackSecond, "Skill.Boss2.EvadeAttack.second");
	UE_DEFINE_GAMEPLAY_TAG(Boss2Dash, "Skill.Boss2.Dash");

	// Boss3
	UE_DEFINE_GAMEPLAY_TAG(Boss3LevelStart, "Skill.Boss3.LevelStart");
	UE_DEFINE_GAMEPLAY_TAG(Boss3BaseAttack1first, "Skill.Boss3.BaseAttack1.first");
	UE_DEFINE_GAMEPLAY_TAG(Boss3BaseAttack1second, "Skill.Boss3.BaseAttack1.second");
	UE_DEFINE_GAMEPLAY_TAG(Boss3BaseAttack1third, "Skill.Boss3.BaseAttack1.third");
	UE_DEFINE_GAMEPLAY_TAG(Boss3BaseAttack2first, "Skill.Boss3.BaseAttack2.first");
	UE_DEFINE_GAMEPLAY_TAG(Boss3BaseAttack2second, "Skill.Boss3.BaseAttack2.second");
	UE_DEFINE_GAMEPLAY_TAG(Boss3BaseAttack3first, "Skill.Boss3.BaseAttack3.first");
	UE_DEFINE_GAMEPLAY_TAG(Boss3BaseAttack3second, "Skill.Boss3.BaseAttack3.second");
	UE_DEFINE_GAMEPLAY_TAG(Boss3RockThrow, "Skill.Boss3.RockThrow.RockThrow");
	UE_DEFINE_GAMEPLAY_TAG(Boss3TurningAttack, "Skill.Boss3.TurningAttack.TurningAttack");
	UE_DEFINE_GAMEPLAY_TAG(Boss3GroundFall, "Skill.Boss3.GroundFall.GroundFall");
	UE_DEFINE_GAMEPLAY_TAG(Boss3Grab, "Skill.Boss3.Grab.Grab");
	UE_DEFINE_GAMEPLAY_TAG(Boss3GrabAttack, "Skill.Boss3.Grab.GrabAttack");
	UE_DEFINE_GAMEPLAY_TAG(Boss3RushAttack, "Skill.Boss3.RushAttack.RushAttack");
	UE_DEFINE_GAMEPLAY_TAG(Boss3JumpAttack, "Skill.Boss3.JumpAttack.JumpAttack");

	// Boss4
	UE_DEFINE_GAMEPLAY_TAG(Boss4BaseAttackFirst, "Skill.Boss4.BaseAttack.first");
	UE_DEFINE_GAMEPLAY_TAG(Boss4BaseAttackSecond, "Skill.Boss4.BaseAttack.second");
	UE_DEFINE_GAMEPLAY_TAG(Boss4WeaponAttack1Start, "Skill.Boss4.WeaponAttack1.start");
	UE_DEFINE_GAMEPLAY_TAG(Boss4WeaponAttack1First, "Skill.Boss4.WeaponAttack1.first");
	UE_DEFINE_GAMEPLAY_TAG(Boss4WeaponAttack1Second, "Skill.Boss4.WeaponAttack1.second");
	UE_DEFINE_GAMEPLAY_TAG(Boss4WeaponAttack1Third, "Skill.Boss4.WeaponAttack1.third");
	UE_DEFINE_GAMEPLAY_TAG(Boss4FlameStrike, "Skill.Boss4.FlameStrike");
	UE_DEFINE_GAMEPLAY_TAG(Boss4SpawnClone, "Skill.Boss4.SpawnClone");
	UE_DEFINE_GAMEPLAY_TAG(Boss4CloneStart, "Skill.Boss4.CloneStart");
	UE_DEFINE_GAMEPLAY_TAG(Boss4InfernoSpike, "Skill.Boss4.InfernoSpike");
	UE_DEFINE_GAMEPLAY_TAG(Boss4HellfireFall, "Skill.Boss4.HellfireFall");
	UE_DEFINE_GAMEPLAY_TAG(Boss4BlazeSphere, "Skill.Boss4.BlazeSphere");
	UE_DEFINE_GAMEPLAY_TAG(Boss4BackDash, "Skill.Boss4.BackDash");
	UE_DEFINE_GAMEPLAY_TAG(Boss4Teleport, "Skill.Boss4.Teleport");
	UE_DEFINE_GAMEPLAY_TAG(Boss4RandomMove, "Skill.Boss4.RandomMove");

	/// Boss4_2
	UE_DEFINE_GAMEPLAY_TAG(Boss4Phase2LevelStart, "Skill.Boss4.LevelStart.Phase2");
	UE_DEFINE_GAMEPLAY_TAG(Boss4WeaponAttack2First, "Skill.Boss4.WeaponAttack2.first");
	UE_DEFINE_GAMEPLAY_TAG(Boss4WeaponAttack2Second, "Skill.Boss4.WeaponAttack2.second");
	UE_DEFINE_GAMEPLAY_TAG(Boss4WeaponAttack2Third, "Skill.Boss4.WeaponAttack2.third");
	UE_DEFINE_GAMEPLAY_TAG(Boss4WeaponAttack2Fourth, "Skill.Boss4.WeaponAttack2.fourth");
	UE_DEFINE_GAMEPLAY_TAG(Boss4WeaponAttack2Fifth, "Skill.Boss4.WeaponAttack2.fifth");
	UE_DEFINE_GAMEPLAY_TAG(Boss4WeaponAttack2Sixth, "Skill.Boss4.WeaponAttack2.sixth");
	UE_DEFINE_GAMEPLAY_TAG(Boss4WeaponAttack2Seventh, "Skill.Boss4.WeaponAttack2.seventh");
	UE_DEFINE_GAMEPLAY_TAG(Boss4WeaponAttack2Eighth, "Skill.Boss4.WeaponAttack2.eighth");
	UE_DEFINE_GAMEPLAY_TAG(Boss4WeaponAttack3First, "Skill.Boss4.WeaponAttack3.first");
	UE_DEFINE_GAMEPLAY_TAG(Boss4WeaponAttack3Second, "Skill.Boss4.WeaponAttack3.second");
	UE_DEFINE_GAMEPLAY_TAG(Boss4WeaponAttack3Third, "Skill.Boss4.WeaponAttack3.third");
	UE_DEFINE_GAMEPLAY_TAG(Boss4WeaponAttack3Fourth, "Skill.Boss4.WeaponAttack3.fourth");
	UE_DEFINE_GAMEPLAY_TAG(Boss4EvadeAttack, "Skill.Boss4.EvadeAttack");
	UE_DEFINE_GAMEPLAY_TAG(Boss4RisingWall, "Skill.Boss4.RisingWall");
	UE_DEFINE_GAMEPLAY_TAG(Boss4SlashWaveFirst, "Skill.Boss4.SlashWave.first");
	UE_DEFINE_GAMEPLAY_TAG(Boss4SlashWaveSecond, "Skill.Boss4.SlashWave.second");
	UE_DEFINE_GAMEPLAY_TAG(Boss4SlashWaveThird, "Skill.Boss4.SlashWave.third");
	UE_DEFINE_GAMEPLAY_TAG(Boss4SpawnClonePhase2, "Skill.Boss4.SpawnClonePhase2");
	UE_DEFINE_GAMEPLAY_TAG(Boss4ClonePhase2_1First, "Skill.Boss4.ClonePhase2_1.first");
	UE_DEFINE_GAMEPLAY_TAG(Boss4ClonePhase2_1Second, "Skill.Boss4.ClonePhase2_1.second");
	UE_DEFINE_GAMEPLAY_TAG(Boss4ClonePhase2_1Third, "Skill.Boss4.ClonePhase2_1.third");
	UE_DEFINE_GAMEPLAY_TAG(Boss4ClonePhase2_2First, "Skill.Boss4.ClonePhase2_2.first");
	UE_DEFINE_GAMEPLAY_TAG(Boss4ClonePhase2_2Second, "Skill.Boss4.ClonePhase2_2.second");
	UE_DEFINE_GAMEPLAY_TAG(Boss4ClonePhase2_2Third, "Skill.Boss4.ClonePhase2_2.third");

	//Boss5
	UE_DEFINE_GAMEPLAY_TAG(Boss5PeckingAttack, "Skill.Boss5.PeckingAttack");
	UE_DEFINE_GAMEPLAY_TAG(Boss5ZoneyaAttack, "Skill.Boss5.ZoneyaAttack");
	UE_DEFINE_GAMEPLAY_TAG(Boss5ElectricAttack, "Skill.Boss5.ElectricAttack");
	UE_DEFINE_GAMEPLAY_TAG(Boss5SpawnEnemyAttack, "Skill.Boss5.SpawnEnemyAttack");
	UE_DEFINE_GAMEPLAY_TAG(Boss5ProjectileAttack, "Skill.Boss5.ProjectileAttack");
}

namespace PawnTags
{
	UE_DEFINE_GAMEPLAY_TAG(Player, "Pawn.Player");
	UE_DEFINE_GAMEPLAY_TAG(ProtoNormal, "Pawn.Enemy.Normal.Proto");
	UE_DEFINE_GAMEPLAY_TAG(ProtoElite, "Pawn.Enemy.Elite.Proto");
	UE_DEFINE_GAMEPLAY_TAG(ProtoBoss, "Pawn.Enemy.Boss.Proto");
	UE_DEFINE_GAMEPLAY_TAG(ArrowProto, "Pawn.Enemy.Normal.ArrowProto");
	UE_DEFINE_GAMEPLAY_TAG(AxeSkeleton, "Pawn.Enemy.Normal.AxeSkeleton");
	UE_DEFINE_GAMEPLAY_TAG(SwordSkeleton, "Pawn.Enemy.Normal.SwordSkeleton");
	UE_DEFINE_GAMEPLAY_TAG(DualBladeSkeleton, "Pawn.Enemy.Normal.DualBladeSkeleton");
	UE_DEFINE_GAMEPLAY_TAG(KnifeSkeleton, "Pawn.Enemy.Normal.KnifeSkeleton");
	UE_DEFINE_GAMEPLAY_TAG(Wood, "Pawn.Enemy.Normal.Wood");
	UE_DEFINE_GAMEPLAY_TAG(Minion, "Pawn.Enemy.Normal.Minion");
	UE_DEFINE_GAMEPLAY_TAG(GunMinion, "Pawn.Enemy.Normal.GunMinion");
	UE_DEFINE_GAMEPLAY_TAG(EliteGolem, "Pawn.Enemy.Normal.EliteGolem");
	UE_DEFINE_GAMEPLAY_TAG(Boss1, "Pawn.Enemy.Boss.1");
	UE_DEFINE_GAMEPLAY_TAG(Boss2, "Pawn.Enemy.Boss.2");
	UE_DEFINE_GAMEPLAY_TAG(Boss3, "Pawn.Enemy.Boss.3");
	UE_DEFINE_GAMEPLAY_TAG(Boss4, "Pawn.Enemy.Boss.4");
	UE_DEFINE_GAMEPLAY_TAG(Boss4Clone, "Pawn.Enemy.Boss.4.Clone");
	UE_DEFINE_GAMEPLAY_TAG(Boss4Clone2Phase, "Pawn.Enemy.Boss.4.Clone.2Phase");
	UE_DEFINE_GAMEPLAY_TAG(Boss5, "Pawn.Enemy.Boss.5");

}

namespace ItemTags
{
	UE_DEFINE_GAMEPLAY_TAG(ItemBase, "Item.Base");

	//Equippable Item Tags
	UE_DEFINE_GAMEPLAY_TAG(EquippableItem, "Item.Equippable");
	UE_DEFINE_GAMEPLAY_TAG(WeaponItem, "Item.Equippable.Weapon");
	UE_DEFINE_GAMEPLAY_TAG(BasicWeapon, "Item.Equippable.Weapon.Basic");
	UE_DEFINE_GAMEPLAY_TAG(SwordWeapon, "Item.Equippable.Weapon.Sword");
	UE_DEFINE_GAMEPLAY_TAG(AxeWeapon, "Item.Equippable.Weapon.Axe");
	UE_DEFINE_GAMEPLAY_TAG(ClawWeapon, "Item.Equippable.Weapon.Claw");
	UE_DEFINE_GAMEPLAY_TAG(HammerWeapon, "Item.Equippable.Weapon.Hammer");
	UE_DEFINE_GAMEPLAY_TAG(SkillItem, "Item.Equippable.Skill");
	UE_DEFINE_GAMEPLAY_TAG(RailGunSkillItem, "Item.Equippable.Skill.RailGun");
	UE_DEFINE_GAMEPLAY_TAG(FireBallSkillItem, "Item.Equippable.Skill.FireBall");
	UE_DEFINE_GAMEPLAY_TAG(TeleportSkillItem, "Item.Equippable.Skill.Teleport");
	UE_DEFINE_GAMEPLAY_TAG(ZoneyaSkillItem, "Item.Equippable.Skill.Zoneya");
	UE_DEFINE_GAMEPLAY_TAG(InfiniteStaminaSkillItem, "Item.Equippable.Skill.InfiniteStamina");
	UE_DEFINE_GAMEPLAY_TAG(ItemScanningSkillItem, "Item.Equippable.Skill.ItemScan");
	UE_DEFINE_GAMEPLAY_TAG(AccessoryItem, "Item.Equippable.Accessory");
	UE_DEFINE_GAMEPLAY_TAG(RingOfHealthAccessoryItem, "Item.Equippable.Accessory.RingOfHealth");
	UE_DEFINE_GAMEPLAY_TAG(NecklaceOfStaminaAccessoryItem, "Item.Equippable.Accessory.NecklaceOfStamina");
	UE_DEFINE_GAMEPLAY_TAG(EaringOfAttackPowerAccessoryItem, "Item.Equippable.Accessory.EaringOfAttackPower");
	UE_DEFINE_GAMEPLAY_TAG(SlowImmuneAccessoryItem, "Item.Equippable.Accessory.SlowImmuneAc");
	UE_DEFINE_GAMEPLAY_TAG(BurnImmuneAccessoryItem, "Item.Equippable.Accessory.BurnImmuneAc");
	UE_DEFINE_GAMEPLAY_TAG(PoisonImmuneAccessoryItem, "Item.Equippable.Accessory.PoisonImmuneAc");
	UE_DEFINE_GAMEPLAY_TAG(AttackDownImmuneAccessoryItem, "Item.Equippable.Accessory.AttackDownImmuneAc");

	//Consumable Item Tags
	UE_DEFINE_GAMEPLAY_TAG(ConsumableItem, "Item.Consumable");
	UE_DEFINE_GAMEPLAY_TAG(Potion, "Item.Consumable.Potion");
	UE_DEFINE_GAMEPLAY_TAG(PotionBoosted, "Item.Consumable.Potion.Boosted");
	UE_DEFINE_GAMEPLAY_TAG(PotionBoosted1, "Item.Consumable.Potion.Boosted.1");
	UE_DEFINE_GAMEPLAY_TAG(PotionBoosted2, "Item.Consumable.Potion.Boosted.2");
	UE_DEFINE_GAMEPLAY_TAG(PotionBoosted3, "Item.Consumable.Potion.Boosted.3");
	UE_DEFINE_GAMEPLAY_TAG(PotionBoosted4, "Item.Consumable.Potion.Boosted.4");
	UE_DEFINE_GAMEPLAY_TAG(PotionBoosted5, "Item.Consumable.Potion.Boosted.5");
	UE_DEFINE_GAMEPLAY_TAG(StaminaPotion, "Item.Consumable.StaminaPotion");
	UE_DEFINE_GAMEPLAY_TAG(Elixir, "Item.Consumable.Elixir");
	UE_DEFINE_GAMEPLAY_TAG(PoisonCure, "Item.Consumable.PoisonCure");
	UE_DEFINE_GAMEPLAY_TAG(BurnCure, "Item.Consumable.BurnCure");
	UE_DEFINE_GAMEPLAY_TAG(SlowCure, "Item.Consumable.SlowCure");
	UE_DEFINE_GAMEPLAY_TAG(AttackDownCure, "Item.Consumable.AttackDownCure");

	//Other Item Tags
	UE_DEFINE_GAMEPLAY_TAG(OtherItem, "Item.Other");
	UE_DEFINE_GAMEPLAY_TAG(Key, "Item.Other.Key");
	UE_DEFINE_GAMEPLAY_TAG(EnergyCore, "Item.Other.EnergyCore");
	UE_DEFINE_GAMEPLAY_TAG(GolemHeart, "Item.Other.GolemHeart");
	UE_DEFINE_GAMEPLAY_TAG(HintNote1, "Item.Other.HintNote1");
	UE_DEFINE_GAMEPLAY_TAG(HintNote2, "Item.Other.HintNote2");
	UE_DEFINE_GAMEPLAY_TAG(HintNote3, "Item.Other.HintNote3");
	UE_DEFINE_GAMEPLAY_TAG(WitchDiary1, "Item.Other.WitchDiary1");
	UE_DEFINE_GAMEPLAY_TAG(WitchDiary2, "Item.Other.WitchDiary2");
	UE_DEFINE_GAMEPLAY_TAG(WitchDiary3, "Item.Other.WitchDiary3");
	UE_DEFINE_GAMEPLAY_TAG(WitchDiary4, "Item.Other.WitchDiary4");
	UE_DEFINE_GAMEPLAY_TAG(AddMaxPotion, "Item.Other.AddMaxPotion");
}

namespace TraceTags
{
	// Axe
	UE_DEFINE_GAMEPLAY_TAG(HandRightAxe, "Trace.Hand.Right.Axe");

	// Claw
	UE_DEFINE_GAMEPLAY_TAG(HandLeftClaw, "Trace.Hand.Left.Claw");
	UE_DEFINE_GAMEPLAY_TAG(HandRightClaw, "Trace.Hand.Right.Claw");
	
	// Sword
	UE_DEFINE_GAMEPLAY_TAG(HandRightSword, "Trace.Hand.Right.Sword");

	//Boss3
	UE_DEFINE_GAMEPLAY_TAG(Body, "Trace.Body");
}

namespace WorldActorTags
{
	UE_DEFINE_GAMEPLAY_TAG(ItemDropped, "WorldActor.Item.ItemDropped");
	UE_DEFINE_GAMEPLAY_TAG(OpenableChestItem, "WorldActor.Item.OpenableChestItem");
	UE_DEFINE_GAMEPLAY_TAG(ItemContainer, "WorldActor.Item.ItemContainer");
	UE_DEFINE_GAMEPLAY_TAG(DyingHelper, "WorldActor.Pawn.DyingHelper");
}
