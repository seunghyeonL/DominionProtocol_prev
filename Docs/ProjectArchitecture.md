# DominionProtocol - 프로젝트 아키텍처 문서

> **엔진**: Unreal Engine 5.5  
> **장르**: 소울라이크 액션 RPG  
> **모듈**: DominionProtocol (Runtime)

---

## 목차

1. [프로젝트 개요](#1-프로젝트-개요)
2. [소스 폴더 구조](#2-소스-폴더-구조)
3. [게임 프레임워크](#3-게임-프레임워크)
4. [플레이어 시스템](#4-플레이어-시스템)
5. [AI 시스템](#5-ai-시스템)
6. [전투 시스템](#6-전투-시스템)
7. [스킬 시스템](#7-스킬-시스템)
8. [아이템 및 인벤토리](#8-아이템-및-인벤토리)
9. [월드 오브젝트](#9-월드-오브젝트)
10. [UI 시스템](#10-ui-시스템)
11. [세이브 시스템](#11-세이브-시스템)
12. [Gameplay Tag 체계](#12-gameplay-tag-체계)
13. [모듈 의존성](#13-모듈-의존성)
14. [핵심 설계 패턴](#14-핵심-설계-패턴)

---

## 1. 프로젝트 개요

**DominionProtocol**은 다크소울 계열의 소울라이크 액션 RPG이다. 플레이어는 3인칭 시점으로 적과 보스를 처치하며 스토리를 진행한다.

### 핵심 게임 루프

```
탐색 → 전투 → 에센스(소울) 획득 → 균열(화톳불)에서 휴식/레벨업 → 보스 전투 → 스토리 진행
```

### 주요 특징

- **에센스 시스템**: 적 처치 시 에센스(화폐) 획득. 사망 시 사망 지점에 드롭되며 한 번 회수 가능
- **균열(Crack) 시스템**: 다크소울의 화톳불에 해당. 리스폰 포인트, 패스트 트래블, 대화 트리거
- **스토리 게이팅**: `EGameStoryState`(20단계)로 월드 전체의 콘텐츠 접근을 제어
- **5개 보스**: 각각 고유한 AI 컨트롤러, 스킬 패턴, 페이즈 시스템 보유
- **무기 교체**: 검, 도끼, 클로 등 무기별 고유 콤보와 스킬
- **마법 스킬**: 파이어볼, 레일건, 텔레포트, 존야 등 장착형 마법 시스템

---

## 2. 소스 폴더 구조

```
Source/DominionProtocol/
├── AI/                          # AI 시스템 전체
│   ├── AICharacters/            # 적 캐릭터 클래스
│   │   ├── BaseEnemy.h/cpp      # 모든 적의 베이스 클래스
│   │   ├── BossMonster/         # 보스 적 (Boss1~5)
│   │   └── NomalMonster/        # 일반 적 (Skeleton, Wood, Minion 등)
│   ├── AIControllers/           # AI 컨트롤러
│   │   ├── BaseAIController     # 베이스 AI 컨트롤러
│   │   ├── Boss1~5AIController  # 보스별 전용 컨트롤러
│   │   └── Boss4Phase2AIController # Boss4 2페이즈 전용
│   ├── AIUtil/                  # PatrolPoint 등 유틸
│   ├── BT_Decorators/           # 비헤이비어 트리 데코레이터 (11개)
│   ├── BT_Services/             # 비헤이비어 트리 서비스 (4개)
│   ├── BT_Tasks/                # 비헤이비어 트리 태스크 (11개)
│   ├── ProtoBoss/               # 프로토타입 보스
│   └── ProtoNormalEnemy/        # 프로토타입 일반 적
│
├── Components/                  # 컴포넌트 기반 아키텍처
│   ├── AIComponent/             # AI 상태 머신
│   │   ├── AIStateComponent     # AI 상태 관리 컴포넌트
│   │   └── AIState/             # AI 상태 베이스 클래스
│   ├── ItemComponent/           # 아이템 장착/소비 관리
│   ├── LockOnComponent/         # 락온 타겟팅
│   ├── PlayerControlComponent/  # 플레이어 입력 및 제어 이펙트
│   │   ├── Effects/             # 제어 이펙트 (스턴, 넉백, 패리 등 17종)
│   │   ├── Notifies/            # 애니메이션 노티파이
│   │   └── States/              # 플레이어 스테이트 머신
│   ├── SkillComponent/          # 스킬 실행 및 콤보
│   │   └── Skills/              # 스킬 구현 전체
│   │       ├── BossSkill/       # 보스별 스킬 (Boss1~5)
│   │       ├── MagicSkill/      # 마법 스킬 (8종)
│   │       ├── NomalMonsterSkill/ # 일반 적 스킬
│   │       ├── PlayerSkill/     # 플레이어 대시/패리
│   │       ├── SkillObject/     # 스킬 오브젝트 (투사체, 레이저 등)
│   │       ├── Weapon/          # 무기별 공격 스킬 (Sword, Axe, Claw)
│   │       └── Notifies/        # 스킬 애니메이션 노티파이
│   ├── StatusComponent/         # 스탯, HP, 스태미너, 상태이상
│   │   └── StatusEffects/       # 상태이상 구현 (화상, 독, 슬로우 등)
│   └── WidgetComponent/         # 커스텀 위젯 컴포넌트
│
├── DomiFramework/               # 게임 프레임워크
│   ├── GameInstance/            # 게임 인스턴스 & 서브시스템 (5개)
│   ├── GameMode/                # 게임 모드 (Base, Title, Level1, Level2)
│   ├── GameState/               # 게임 스테이트
│   ├── ObjectPooling/           # 오브젝트 풀링 (비활성화 상태)
│   └── WorldActorManage/        # 월드 액터 상태 관리
│
├── EnumAndStruct/               # 공용 열거형 및 구조체 (15개+)
├── Interface/                   # 인터페이스 정의 (8개)
├── ItemInventory/               # 아이템 시스템
│   ├── Accessory/               # 악세서리 (반지, 목걸이, 귀걸이)
│   ├── Consumable/              # 소비 아이템 (포션, 엘릭서)
│   ├── Other/                   # 기타 아이템 (열쇠)
│   ├── Skill/                   # 스킬 아이템 (장착형 마법)
│   └── Weapon/                  # 무기 (검, 도끼, 클로, 해머)
│
├── Player/                      # 플레이어 시스템
│   ├── Characters/              # DomiCharacter, DomiAnimInstance
│   ├── BasePlayerController     # 베이스 플레이어 컨트롤러
│   ├── InGameController         # 인게임 전용 컨트롤러
│   ├── TitleController          # 타이틀 화면 컨트롤러
│   ├── Damagable.h              # 피격 인터페이스
│   └── EffectReceivable.h       # 이펙트 수신 인터페이스 (디버그)
│
├── UI/                          # UI 위젯
│   ├── FadeInOut/               # 페이드 효과
│   ├── RenewalUI/               # 리뉴얼 메뉴 UI
│   ├── UI3D/                    # 3D 위젯 (캐릭터/몬스터)
│   ├── UIInGame/                # 인게임 HUD, 대화, 보스 HP 등
│   └── UITitle/                 # 타이틀 화면 UI
│
├── Util/                        # 유틸리티
│   ├── GameTagList.h            # 전체 GameplayTag 정의 (425줄)
│   ├── BattleDataTypes.h        # 전투 데이터 구조체
│   ├── DebugHelper.h            # 디버그 도우미
│   ├── CheatBPLib/DevCheatManager # 치트 매니저
│   └── AsyncLoadBPLib           # 비동기 로딩 라이브러리
│
└── WorldObjects/                # 월드 배치 오브젝트 (20개+)
    ├── Crack                    # 균열 (화톳불)
    ├── Essence/DropEssence      # 에센스 (소울)
    ├── Helper                   # NPC 헬퍼
    ├── DialogueManager          # 대화 매니저
    ├── Door/DoorLocked          # 문/잠긴 문
    ├── Elevator/ElevatorCaller  # 엘리베이터
    ├── BossSpawner/BossCloudDoor # 보스 스폰/입구
    └── ...                      # Lever, Teleporter, ViewTarget 등
```

---

## 3. 게임 프레임워크

### 3.1 전체 계층 구조

```
UDomiGameInstance (게임 인스턴스 - 전역 싱글턴)
  ├── UItemInstanceSubsystem       (인벤토리/장비 관리)
  ├── USaveManagerSubsystem        (세이브/로드 관리)
  ├── USoundInstanceSubsystem      (사운드 볼륨 관리)
  └── UWorldInstanceSubsystem      (월드 상태/균열/에센스 관리)

ABaseGameMode (레벨별 게임 모드)
  ├── AProtoLevel1GameMode         (Level1용)
  ├── AProtoLevel2GameMode         (Level2용)
  └── ATitleGameMode               (타이틀 화면용)

ABaseGameState (게임 스테이트)
```

### 3.2 UDomiGameInstance

전역 데이터를 관리하는 싱글턴 인스턴스.

| 데이터 | 타입 | 설명 |
|--------|------|------|
| `PlayerCurrentEssence` | `int32` | 보유 에센스 (화폐) |
| `CurrentGameStoryState` | `EGameStoryState` | 현재 스토리 진행도 (20단계) |
| `StatDataMap` | `TMap<FGameplayTag, float>` | 플레이어 스탯 (레벨, STR, SPL, END 등) |
| `PlayTime` | `int32` | 플레이 시간 (초) |
| `SaveSlotName/Index` | `FString/int32` | 현재 세이브 슬롯 |

**주요 Delegate**:
- `FOnStoryStateChanged` - 스토리 진행 시 브로드캐스트
- `FOnChangedCurrentEssence` - 에센스 변동 시 브로드캐스트

### 3.3 서브시스템

#### ItemInstanceSubsystem
인벤토리, 장비 슬롯, 소비 아이템 슬롯을 관리한다.

```
InventoryDataMap    : TMap<FGameplayTag, int32>  → 보유 아이템(태그 → 수량)
EquipmentSlotMap    : TMap<FName, FGameplayTag>  → 장비 슬롯(슬롯명 → 아이템 태그)
ConsumableSlotMap   : TMap<FName, FGameplayTag>  → 소비 슬롯(슬롯명 → 아이템 태그)
PotionBoostLevel    : int32                      → 포션 강화 단계
```

#### SaveManagerSubsystem
세이브/로드 및 슬롯 관리를 담당한다.

- `StartNewGame(SlotIndex)` / `LoadSaveDataAndOpenLevel(SlotIndex)` / `DeleteSaveSlot(SlotIndex)`
- `SaveGame()` / `LoadGame()` - 전체 게임 상태 직렬화
- `SaveSlotArray: TArray<FSaveSlotMetaData>` - UI용 슬롯 메타데이터 (최대 3개)

#### SoundInstanceSubsystem
카테고리별(Master, SFX, BGM, UI) 볼륨 제어.

#### WorldInstanceSubsystem
레벨별 월드 상태를 추적한다.

| 데이터 | 설명 |
|--------|------|
| `CrackDataMap` | 레벨별 균열 활성화 상태 |
| `WorldActorDataMap` | GUID 기반 월드 액터 상태 (아이템 획득 여부, 상자 오픈 여부 등) |
| `DropEssenceData` | 사망 시 드롭된 에센스 위치/양/레벨 |
| `IsNewGameDataMap` | 레벨별 첫 진입 여부 |

### 3.4 ABaseGameMode

레벨 단위 게임 흐름을 제어한다.

- 서브시스템 캐싱 (BeginPlay 시 GameInstance, 서브시스템 참조 저장)
- 전투 관리: `StartBattle()` / `EndBattle()`
- 플레이어 리스폰: 최근 활성화된 균열에서 재시작
- 페이드 인/아웃 전환
- 에센스 드롭/회수 로직

---

## 4. 플레이어 시스템

### 4.1 클래스 계층

```
ACharacter (UE5 기본)
  └── ADomiCharacter (메인 플레이어 캐릭터)
       구현 인터페이스:
         ├── IDamagable           (피격 처리)
         ├── IParryable           (패리 가능)
         ├── IPawnTagInterface    (폰 식별)
         ├── IEffectReceivable    (이펙트 수신 - 디버그)
         ├── IControlComponentUser (컨트롤 컴포넌트 접근)
         ├── IStatusComponentUser  (스테이터스 컴포넌트 접근)
         ├── ISkillComponentUser   (스킬 컴포넌트 접근)
         └── IEffectUser           (이펙트 UI 데이터)

APlayerController (UE5 기본)
  └── ABasePlayerController (페이드 UI, 인풋 서브시스템 관리)
       └── AInGameController (30+ 인풋 액션 바인딩, HUD 위젯 생성)
```

### 4.2 핵심 컴포넌트 구성

ADomiCharacter는 4개의 핵심 컴포넌트로 구성된다:

```
ADomiCharacter
  ├── UPlayerControlComponent  → 입력 처리, 제어 이펙트, 스테이트 머신
  ├── UStatusComponent         → HP, 스태미너, 스탯, 상태이상
  ├── USkillComponent          → 스킬 실행, 콤보 관리
  ├── UItemComponent           → 장비, 무기, 소비 아이템
  ├── USpringArmComponent      → 카메라 붐
  ├── UCameraComponent         → 3인칭 카메라
  └── UNavigationInvokerComponent → 동적 내비메시 생성
```

### 4.3 입력 처리 파이프라인

```
AInGameController (Enhanced Input System)
  ↓ EnhancedInputComponent가 액션 바인딩
ADomiCharacter::BindInputFunctions()
  ↓ PlayerControlComponent 메서드에 위임
UPlayerControlComponent::Move() / Dash() / BaseAttack() ...
  ↓ 현재 스테이트에 위임
UPlayerControlState (State 패턴)
  ↓ 실제 로직 실행
SkillComponent / StatusComponent / ItemComponent
```

### 4.4 플레이어 스테이트 머신

`UPlayerControlStateBase`는 15개의 가상 입력 메서드를 정의:

| 메서드 | 실행 내용 |
|--------|----------|
| `Move()` | 이동 방향 계산 → 캐릭터 이동 |
| `Look()` | Yaw/Pitch 입력 → 카메라 회전 |
| `Dash()` | `SkillGroupTags::Dash` 스킬 실행 |
| `BaseAttack()` | `SkillGroupTags::BaseAttack` 스킬 실행 |
| `WeaponSkill()` | `SkillGroupTags::WeaponSkill` 실행 |
| `MagicSkill()` | `SkillGroupTags::MagicSkill` 실행 |
| `Parry()` | `SkillGroupTags::Parry` 실행 |
| `LockOn()` | 락온 토글 (이펙트 활성화/해제) |
| `Interact()` | `IInteractableInterface::Interact()` 호출 |
| `SwapWeapon()` | `ItemComponent->SwapWeapons()` |
| `ConsumeItemAction_1/2/3()` | 소비 슬롯별 아이템 사용 |
| `SprintStart/End()` | 달리기 상태이상 활성화/해제 |

### 4.5 제어 이펙트 (Control Effects)

`PlayerControlComponent`가 관리하는 제어 이펙트 목록:

| 이펙트 | 설명 |
|--------|------|
| `Stun` | 기절 (행동 불가) |
| `PreStun` | 기절 전조 |
| `Stiffness` | 경직 (방향별 피격 리액션) |
| `Flew` | 날려감 (넉다운) |
| `StandingUp` | 일어서기 (무적) |
| `Parry` | 패리 활성 중 |
| `LockOn` | 락온 활성 중 |
| `UsingSkill` | 스킬 사용 중 |
| `ConsumingItem` | 아이템 소비 중 |
| `DashAttackPossible` | 대시 어택 가능 |
| `DashInvincible` | 대시 무적 |
| `Interact` | 상호작용 중 |
| `Death` | 사망 |
| `Silence` | 침묵 (스킬 사용 불가) |
| `Confused` | 혼란 |

---

## 5. AI 시스템

### 5.1 적 클래스 계층

```
ACharacter (UE5)
  └── ABaseEnemy (모든 적의 베이스)
       ├── ANomalEnemy (일반 적 - 패트롤, 래그돌 사망)
       │    ├── ASkeletonEnemy → ASwordSkeletonEnemy
       │    │                  → AAxeSkeletonEnemy
       │    │                  → ADualBladeSkeletonEnemy
       │    │                  → AKnifeSkeletonEnemy
       │    ├── AWoodEnemy
       │    ├── AMinionEnemy / AGunMinionEnemy
       │    └── AEliteGolemEnemy
       │
       └── ABaseBossEnemy (보스 적 - 스토리 진행, 아이템 드롭)
            ├── ABoss1Enemy (기사 - 가중치 기반 스킬 선택)
            ├── ABoss2Enemy (검사 - 다단 콤보)
            ├── ABoss3Enemy (거인 - 그랩/돌진)
            ├── ABoss4Enemy (마녀 - 2페이즈, 클론 생성)
            └── ABoss5Enemy (최종 보스)
```

### 5.2 ABaseEnemy 인터페이스

모든 적이 구현하는 인터페이스:

```cpp
class ABaseEnemy :
  public ACharacter,
  public IDamagable,           // OnAttacked(FAttackData)
  public IParryable,           // IsParryingCond(), OnParried()
  public IPawnTagInterface,    // GetPawnTag(), GetPawnName()
  public IEffectReceivable,    // 디버그용 이펙트 표시
  public IStatusComponentUser, // GetStatusComponent()
  public ISkillComponentUser,  // GetSkillComponent()
  public IEffectUser           // 이펙트 UI 데이터
```

핵심 멤버:
- `UStatusComponent` - HP, 스탯, 상태이상 관리
- `USkillComponent` - 스킬 실행
- `ULockOnComponent` - 락온 타겟 위치
- `EssenceClass` / `EssenceAmount` - 사망 시 드롭할 에센스

### 5.3 AI 컨트롤러

```
AAIController (UE5)
  └── ABaseAIController
       ├── 감지: UAIPerceptionComponent (시야 기반)
       ├── 상태: UAIStateComponent (Idle/Combat/Return)
       ├── 행동: UBehaviorTree 실행
       └── 가상: GetAttack() → 오버라이드로 스킬 선택
            ├── ABoss1AIController (가중치 기반 4종 스킬)
            ├── ABoss2AIController
            ├── ABoss4AIController (8종 스킬 + 클론 모드)
            ├── ABoss4Phase2AIController (5종 스킬)
            └── ABoss5AIController
```

### 5.4 AI 상태 관리

`UAIStateComponent`는 GameplayTag 기반 상태 머신:

```
상태 전이:
  Idle ←→ Combat  (플레이어 감지 시)
    ↓
  Return           (타겟 상실 + 스킬 미사용 시)
    ↓
  Idle             (홈 위치 도착 시)

이동 속도:
  Idle/Return: 350
  Combat: 600
```

### 5.5 보스 스킬 선택 (가중치 방식)

보스 AI 컨트롤러는 가중치 기반 랜덤 선택을 사용:

```
1. 각 스킬에 기본 가중치(Weight) + 증가분(Increment) 설정
2. 선택된 스킬의 가중치는 리셋
3. 선택되지 않은 스킬의 가중치는 증가
4. 쿨다운이 끝난 스킬만 후보에 포함
→ 반복적이지 않으면서도 예측 가능한 행동 패턴 생성
```

### 5.6 비헤이비어 트리 커스텀 노드

#### Tasks (실행)
| 노드 | 설명 |
|------|------|
| `ExecutePattern` | 스킬 실행 (Latent Task - 애니메이션 완료 콜백) |
| `DashTo` | 대시 이동 (ExecutePattern 상속) |
| `TeleportTo` | 블랙보드 위치로 텔레포트 |
| `RotateToTargetSmoothly` | 타겟 방향으로 부드러운 회전 |
| `MoveToNextPatrolPoint` | 다음 패트롤 포인트로 이동 |
| `MoveToTimeLimited` | 시간 제한 이동 |
| `ReturnToHome` | 스폰 위치 복귀 |
| `WaitUntilTagsRemoved` | 태그 제거 대기 |
| `ResetCombo` | 콤보 카운터 리셋 |

#### Services (주기적 실행)
| 노드 | 설명 |
|------|------|
| `SelectSkill` | `GetAttack()` 호출 → 블랙보드에 스킬 태그 저장 |
| `ChooseRandomPattern` | 랜덤 패턴 선택 |
| `CalcDistanceToTarget` | 타겟까지 거리 갱신 |
| `CalcDistanceToHome` | 홈까지 거리 갱신 |

#### Decorators (조건 검사)
| 노드 | 설명 |
|------|------|
| `CanActivateSkill` | 스킬 활성화 가능 여부 |
| `CheckSkillTag` | 특정 스킬 태그 일치 여부 |
| `CheckTargetDistance` | 거리 기반 조건 |
| `CheckPlayerBehind` | 플레이어가 뒤에 있는지 |
| `RandomChance` | 확률적 실행 |
| `CheckAIStateEquals` | 현재 AI 상태 검증 |
| `BlockNodeIfHasAnyTag` / `AllTag` | 태그 기반 차단 |
| `RunNodeIfHasAnyTag` / `AllTag` | 태그 기반 실행 |

### 5.7 스킬 실행 파이프라인 (AI)

```
BTService_SelectSkill
    ↓
BaseAIController::GetAttack()  ← 보스별로 오버라이드
    ↓
Blackboard: "SkillTag" = 선택된 스킬
    ↓
BTDecorator_CanActivateSkill (유효성 검증)
    ↓
ExecutePattern BT Task
    ↓
SkillComponent::ExecuteSkill(SkillGroupTag)
    ↓
BaseSkill::Execute() (애니메이션/판정/데미지)
    ↓
OnAnimationCompleted() 콜백
    ↓
FinishLatentTask() → BT 재개
```

### 5.8 일반 적 vs 보스 차이

| 항목 | 일반 적 (ANomalEnemy) | 보스 (ABaseBossEnemy) |
|------|----------------------|----------------------|
| 사망 | 래그돌 물리 → 5초 후 제거 | `EndBattle()` + 스토리 진행 |
| 패트롤 | 2개 포인트 사이 왕복 | 없음 (전투 전용) |
| 아이템 드롭 | 에센스만 | 에센스 + 특수 아이템 |
| HP 바 | 머리 위 표시 | 화면 하단 보스 HP 바 |
| AI 컨트롤러 | BaseAIController 사용 | 전용 컨트롤러 (가중치 선택) |
| 페이즈 | 없음 | Boss4는 2페이즈 (별도 AI 컨트롤러) |

---

## 6. 전투 시스템

### 6.1 공격 데이터 구조

```cpp
struct FAttackData
{
    AActor* Instigator;           // 공격자
    float Damage;                 // 데미지
    float GroggyDamage;           // 그로기 데미지 (보스 전용)
    FVector LaunchVector;         // 넉백 방향/세기
    TArray<FEffectData> Effects;  // 부가 효과 (경직, 기절, 날림 등)
};

struct FEffectData
{
    FGameplayTag EffectTag;       // 효과 태그
    float Magnitude;             // 강도
    float Duration;              // 지속 시간
};
```

### 6.2 피격 처리 흐름 (플레이어)

```
1. OnAttacked(FAttackData) 호출
   ↓
2. 무적 상태 체크 (InvincibilityTags)
   - Death, UsingZoneya, StandingUp, DashInvincible
   - GodMode 체크
   → 무적이면 리턴
   ↓
3. HP 차감: StatusComponent->SetHealth(현재HP - Damage)
   ↓
4. 전투 상태 진입: StatusComponent->StartCombat()
   ↓
5. 넉백 적용: LaunchCharacter(LaunchVector) (벡터가 있을 경우)
   ↓
6. 이펙트 분류 및 적용:
   - Control Effect (Stiffness, Flew, Stun 등)
     → PlayerControlComponent->ActivateControlEffect()
     → 경직(Stiffness)인 경우: 피격 방향 판정(전/좌/우/후) → 방향별 피격 몽타주 재생
     → 날림(Flew)인 경우: 공격자 방향으로 회전 후 넉다운
   - Status Effect (Burn, Poison 등)
     → StatusComponent->ActivateStatusEffect()
   ↓
7. 오디오/VFX: PlayHitSound(), PlayHitVoice()
```

### 6.3 무적 상태 태그

| 태그 | 조건 |
|------|------|
| `EffectTags::Death` | 사망 상태 |
| `EffectTags::DashInvincible` | 대시 중 (i-frame) |
| `EffectTags::StandingUp` | 일어서기 중 |
| `EffectTags::UsingZoneya` | 존야 스킬 사용 중 |

### 6.4 패리 시스템

```
플레이어가 Parry 실행
  → PlayerParryStart 노티파이 → 패리 판정 시작
  → 적의 공격이 히트
    → IsParryingCond() 체크 → 패리 활성 중이면:
      → 적에게 OnParried() 호출 (경직/그로기)
      → 플레이어 패리 성공 처리
  → PlayerParryEnd 노티파이 → 패리 판정 종료
```

### 6.5 StatusComponent 스탯 체계

```
BaseStat (기본 스탯):
  Level, LIFE, STR, SPL, END
  BaseAttackPower, BaseMagicPower, BaseMaxHealth, BaseMaxStamina

StatCoefficient (계수):
  AttackPowerCoefficient, MagicPowerCoefficient,
  MaxHealthCoefficient, MaxStaminaCoefficient

BattleStat (전투 스탯 - BaseStat * Coefficient로 계산):
  MaxHealth, MaxStamina, AttackPower, MagicPower,
  Defense, MaxShield, MaxGroggyGauge, SpeedCoefficient

VariableStat (가변 스탯):
  Health, Stamina, Shield, GroggyGauge
```

### 6.6 상태이상 (Status Effects)

| 상태이상 | 종류 | 설명 |
|---------|------|------|
| Burn | 디버프 | 지속 화상 데미지 |
| Poison | 디버프 | 지속 독 데미지 |
| Slow | 디버프 | 이동속도 감소 |
| AttackDown | 디버프 | 공격력 감소 |
| AttackUp | 버프 | 공격력 증가 |
| Running | 이동 | 달리기 상태 (속도/스태미너) |
| Walking | 이동 | 걷기 상태 |
| AIStiffness | AI | AI 경직 |
| AIStun | AI | AI 기절 |
| AIGroggy | AI | AI 그로기 |
| AIDeath | AI | AI 사망 |
| AIParried | AI | AI 패리당함 |

---

## 7. 스킬 시스템

### 7.1 스킬 그룹 구조

```cpp
struct FSkillGroup
{
    TArray<UBaseSkill*> Skills;  // 콤보 단계별 스킬 배열
    int32 ComboIdx;             // 현재 콤보 인덱스
};
```

`SkillComponent`는 `TMap<FGameplayTag, FSkillGroup>`으로 스킬 그룹을 관리:
- `SkillGroupTags::BaseAttack` → 기본 공격 콤보 (무기별 3~4단)
- `SkillGroupTags::WeaponSkill` → 무기 스킬
- `SkillGroupTags::MagicSkill` → 마법 스킬
- `SkillGroupTags::Dash` → 대시
- `SkillGroupTags::Parry` → 패리
- `SkillGroupTags::DashAttack` → 대시 어택

### 7.2 스킬 실행 라이프사이클

```
ExecuteSkill(Tag) 호출
  ↓
OnSkillStart 브로드캐스트 → 스태미너 회복 중지, 제어 이펙트 활성화
  ↓
BaseSkill::Execute() → 애니메이션 몽타주 재생, 판정, 데미지
  ↓
OnSkillEnd 브로드캐스트 → 제어 이펙트 해제, 스태미너 회복 재개
  ↓
ResetCombo 타이머 시작 (ComboResetDelay 후 콤보 리셋)
```

### 7.3 무기별 기본 공격

| 무기 | 콤보 수 | 특징 |
|------|---------|------|
| Sword (검) | 3단 | 기본적인 콤보, 무기 스킬 포함 |
| Axe (도끼) | 3단 | 높은 데미지, 느린 속도 |
| Claw (클로) | 4단 | 빠른 콤보, 양손 공격 |

### 7.4 마법 스킬 목록

| 스킬 | 설명 |
|------|------|
| MagicFireballSkill | 파이어볼 투사체 |
| MagicRailGunSkill | 레일건 (직선 관통) |
| MagicLaserSkill | 레이저 빔 |
| MagicTeleportSkill | 텔레포트 이동 |
| MagicZoneyaSkill | 존야 (일시적 무적) |
| MagicInfiniteStaminaSkill | 무한 스태미너 (일시적) |
| MagicItemScanningSkill | 아이템 스캔 (주변 아이템 표시) |
| MagicCurvedProjectileSkill | 유도 투사체 |

### 7.5 스킬 오브젝트

투사체 등 월드에 스폰되는 스킬 관련 액터:

| 오브젝트 | 설명 |
|---------|------|
| `CurvedProjectile` | 유도 투사체 |
| `LaserActor` | 레이저 빔 액터 |
| `Portal` | 텔레포트 포탈 |
| `ItemScanSpawner` | 아이템 스캔 효과 |
| `Boss3Rock` | Boss3 돌 투사체 |
| `Boss5Projectile` | Boss5 투사체 |
| `Boss5Wall` | Boss5 벽 생성 |

### 7.6 애니메이션 노티파이

스킬 실행 중 애니메이션에서 트리거되는 노티파이:

| 노티파이 | 설명 |
|---------|------|
| `StartTrace` | 히트 판정 시작 |
| `StopTrace` | 히트 판정 종료 |
| `SkillAttackTrace` | 단발 히트 판정 |
| `CylinderTrace` | 원통형 히트 판정 |
| `SkillAttackEnd` | 스킬 공격 종료 |
| `MoveCharacter` | 캐릭터 이동 (돌진 등) |
| `SpawnProjectile` | 투사체 스폰 |
| `UpdatePlayRate` | 애니메이션 속도 변경 |
| `FootStep` | 발소리 (물리 서피스 기반) |
| `PlayerParryStart/End` | 패리 판정 시작/종료 |
| `StartRagDoll` | 래그돌 활성화 |
| `ItemConsumingEnd` | 아이템 소비 완료 |

---

## 8. 아이템 및 인벤토리

### 8.1 아이템 클래스 계층

```
ABaseItem (AActor + IInteractableInterface)
  ├── Weapon/
  │    ├── ABasicWeapon
  │    ├── ASword
  │    ├── AAxe
  │    ├── AClaw
  │    └── AHammer
  ├── Accessory/
  │    ├── ARingOfHealth          (HP 증가)
  │    ├── ANecklaceOfStamina     (스태미너 증가)
  │    ├── AEaringOfAttackPower   (공격력 증가)
  │    └── AAccessoryOfImmunity   (상태이상 면역)
  ├── Consumable/
  │    ├── APotionConsumable      (HP 회복, 강화 단계 지원)
  │    ├── AStaminaConsumable     (스태미너 회복)
  │    ├── AElixirConsumable      (특수 효과)
  │    └── AAddMaxPotionConsumable (포션 최대치 증가)
  ├── Skill/
  │    ├── AFireBallSkillItem
  │    ├── ARailGunSkillItem
  │    ├── ATeleportSkillItem
  │    ├── AZoneyaSkillItem
  │    ├── AInfiniteStaminaSkillItem
  │    └── AItemScanningSkillItem
  ├── Other/
  │    └── ADoorKey
  ├── AItemDropped            (드롭된 아이템)
  ├── AItemContainer          (아이템 컨테이너)
  ├── AOpenableChestItem      (열 수 있는 상자)
  ├── ABreakableChestItem     (부술 수 있는 상자)
  ├── AItemSpawner            (아이템 스포너)
  └── AItemSpawnPoint         (아이템 스폰 포인트)
```

### 8.2 아이템 데이터 구조

```cpp
// 메인 아이템 데이터 (DataTable Row)
struct FItemData : public FTableRowBase
{
    FString ItemName;                          // 아이템 이름
    EItemType ItemType;                        // Equippable / Consumable / Other
    UTexture2D* ItemIcon;                      // 아이콘
    FText ItemDescription;                     // 설명
    int32 MaxItemQuantity;                     // 최대 보유 수량 (장비=1)
    FWeaponData WeaponData;                    // 무기 데이터 (무기일 경우)
    FAccessoryData AccessoryData;              // 악세서리 데이터
    float WeaponStatMultiplier;                // 무기 스탯 배율
    FSkillComponentInitializeData SkillDatas;  // 스킬 초기화 데이터
    TMap<FGameplayTag, float> StatChangeMap;   // 장착 시 스탯 변경
};

struct FWeaponData
{
    FName SocketName;        // 부착 소켓 (e.g., "RightHand")
    USkeletalMesh* WeaponMesh;
    FTransform TraceBoxTransform;  // 히트 판정 박스
};

struct FAccessoryData
{
    FName SocketName;
    USkeletalMesh* AccessoryMesh;
};
```

### 8.3 아이템 분류 (GameplayTag 기반)

```
ItemTags::
  ├── EquippableItem
  │    ├── WeaponItem (Basic, Sword, Axe, Claw, Hammer)
  │    ├── SkillItem (RailGun, FireBall, Teleport, Zoneya, InfiniteStamina, ItemScanning)
  │    └── AccessoryItem (RingOfHealth, NecklaceOfStamina, EaringOfAttackPower, Immunity variants)
  ├── ConsumableItem
  │    ├── Potion (기본 + Boosted 1~5)
  │    ├── StaminaPotion, Elixir
  │    └── Cures (PoisonCure, BurnCure, SlowCure, AttackDownCure)
  └── OtherItem
       ├── Key, EnergyCore, GolemHeart
       ├── HintNote 1~3
       ├── WitchDiary 1~4
       └── AddMaxPotion
```

---

## 9. 월드 오브젝트

### 9.1 균열 (ACrack) - 화톳불

다크소울의 화톳불에 해당하는 핵심 오브젝트.

| 기능 | 설명 |
|------|------|
| 리스폰 포인트 | 사망 시 마지막 활성화 균열에서 부활 |
| 패스트 트래블 | 활성화된 균열 간 이동 |
| 스토리 게이팅 | `RequiredRevealStoryState`로 출현 조건 제어 |
| 대화 트리거 | 균열 상호작용 시 스토리 대화 시작 |
| BGM 관리 | 지역별 배경 음악 재생 |
| 범위 감지 | 타이머 기반 플레이어 근접 감지 (활성화/상호작용 범위) |

상태 저장: `WorldInstanceSubsystem.CrackDataMap`에 균열별 활성화 여부 저장

### 9.2 에센스 (AEssence) - 소울

적 사망 시 스폰되는 화폐 아이템.

- 나이아가라 이펙트로 시각 표현
- 스폰 후 플레이어 방향으로 가속하며 추적
- 오버랩 시 자동 수집 → `GameInstance.AddPlayerCurrentEssence()`
- 수집 즉시 제거 (저장 불필요)

### 9.3 드롭 에센스 (ADropEssence) - 사망 시 드롭

플레이어 사망 시 사망 지점에 생성.

```
사망 시:
  1. 현재 에센스 → ADropEssence 생성
  2. 위치/레벨/양 → WorldInstanceSubsystem에 저장
  3. 플레이어 에센스 = 0

회수 시:
  1. 플레이어가 드롭 에센스에 접근
  2. 에센스 회수 → GameInstance에 추가
  3. WorldInstanceSubsystem에서 제거

재사망 시:
  → 이전 드롭 에센스는 영구 소실
  → 새 드롭 에센스 생성
```

### 9.4 기타 월드 오브젝트

| 오브젝트 | 설명 |
|---------|------|
| `Helper` | NPC 캐릭터 (대화/컷씬용, 등장/퇴장 애니메이션) |
| `DialogueManager` | DataTable 기반 대화 시스템 관리 |
| `Door` / `DoorLocked` | 문 / 잠긴 문 (열쇠 필요) |
| `Elevator` / `ElevatorCaller` | 엘리베이터 / 엘리베이터 호출 장치 |
| `Lever` | 레버 (문/경로 개방) |
| `BossSpawner` | 보스 스폰 관리 |
| `BossCloudDoor` | 보스 전투 입구 (안개 문) |
| `Teleporter` | 텔레포터 (레벨 간 이동) |
| `BlockedPath` | 차단된 길 (스토리 진행으로 개방) |
| `SplineBlockingVolume` | 스플라인 기반 차단 볼륨 |
| `StoryTrigger` | 스토리 진행 트리거 |
| `ViewTarget` | 카메라 뷰 타겟 (연출용) |

---

## 10. UI 시스템

### 10.1 UI 폴더 구조

```
UI/
├── BaseHUDWidget               # HUD 베이스 클래스
├── FadeInOut/                  # 화면 전환 페이드 효과
│    └── FadeWidget
├── RenewalUI/                  # 리뉴얼 메뉴 UI
│    ├── NewMainMenuWidget      # 메인 메뉴
│    ├── NewMenuWidget          # 기본 메뉴
│    ├── NewInventoryWidget     # 인벤토리
│    ├── NewEquipmentWidget     # 장비
│    ├── NewEquippedSlotsWidget # 장착 슬롯
│    ├── NewItemWidget          # 아이템 상세
│    ├── NewStatusWidget        # 스탯 표시
│    ├── NewSettingWidget       # 설정
│    ├── NewSettingButton       # 설정 버튼
│    └── NewShotcutButton       # 단축키 버튼
├── UI3D/                       # 3D 위젯
│    ├── DomiCharacter3DWidget  # 캐릭터 3D 프리뷰
│    └── DomiMonster3DWidget    # 몬스터 3D 프리뷰
├── UIInGame/                   # 인게임 HUD
│    ├── DomiInGameHUDWidget    # 메인 HUD
│    ├── DomiInGameMenuWidget   # 인게임 메뉴
│    ├── DomiStatusBarWidget    # HP/스태미너 바
│    ├── DomiItemSlotsWidget    # 아이템 슬롯 표시
│    ├── BossMonsterHPBarWidget # 보스 HP 바
│    ├── AlertWidget            # 알림 위젯
│    ├── DomiInteractionWidget  # 상호작용 프롬프트
│    ├── DomiInventoryWidget    # 인게임 인벤토리
│    ├── NewDialogueWidget      # 대화 UI
│    ├── NewEventBoxWidget      # 이벤트 알림 박스
│    ├── NewTargetLockOnWidget  # 락온 대상 표시
│    ├── NewCrackMenuWidget     # 균열 메뉴
│    ├── DomiCrackWrapWidget    # 균열 메뉴 래퍼
│    └── StatModifyWidget/      # 스탯 레벨업 UI
└── UITitle/                    # 타이틀 화면
     ├── NewTitleHUDWidget      # 타이틀 HUD
     ├── NewTitleMenuUI         # 타이틀 메뉴
     ├── NewTitleMenuButton     # 메뉴 버튼
     ├── NewSaveSlot            # 세이브 슬롯 위젯
     └── NewSaveSlotBoxUI       # 세이브 슬롯 박스
```

### 10.2 UI 바인딩 방식

Delegate 기반 데이터 바인딩:
- `StatusComponent.OnHealthChanged` → HP 바 갱신
- `StatusComponent.OnStaminaChanged` → 스태미너 바 갱신
- `StatusComponent.OnShieldChanged` → 쉴드 바 갱신
- `GameInstance.OnChangedCurrentEssence` → 에센스 표시 갱신
- `DialogueManager.OnUpdateDialogueText` → 대화 텍스트 갱신
- `PlayerControlComponent.OnActiveLockOn` → 락온 UI 토글
- `Crack.OnActiveCrack` → 균열 활성화 알림

---

## 11. 세이브 시스템

### 11.1 세이브 데이터 구조

```
UDomiSaveGame (직렬화 대상)
  │
  ├─ FInstanceData (게임 인스턴스 데이터)
  │   ├── SaveSlotName / SaveSlotIndex
  │   ├── PlayerCurrentEssence
  │   ├── CurrentGameStoryState
  │   ├── StatDataMap (레벨, STR, SPL, END 등)
  │   └── PlayTime
  │
  ├─ FItemSubsystemData (인벤토리 데이터)
  │   ├── InventoryMap: 보유 아이템
  │   ├── EquipmentSlots: 장착 장비
  │   └── ConsumableSlots: 소비 슬롯
  │
  └─ FWorldInstanceSubsystemData (월드 상태)
      ├── IsNewGameDataMap: 레벨별 첫 진입 여부
      ├── CurrentLevelName / DisplayName
      ├── CrackDataMap: 균열 활성화 상태
      ├── WorldActorDataMap: 개별 액터 상태 (GUID 기반)
      │    └── FWorldActorData
      │         ├── UniqueActorID (GUID)
      │         ├── bIsItemCollected
      │         ├── bIsChestOpened
      │         └── bIsDead
      └── DropEssence 상태
           ├── bIsDropEssenceExist
           ├── DropEssenceAmount / Location / Level
```

### 11.2 세이브/로드 흐름

```
세이브:
  BaseGameMode.Save()
    → SaveManagerSubsystem.SaveGame()
      → UDomiSaveGame 인스턴스 생성
      → GameInstance.GetSaveData()       → FInstanceData
      → ItemSubsystem.GetSaveData()      → FItemSubsystemData
      → WorldSubsystem.GetSaveData()     → FWorldInstanceSubsystemData
      → UGameplayStatics::SaveGameToSlot()
      → FSaveSlotMetaData 갱신

로드:
  SaveManagerSubsystem.LoadGame()
    → UGameplayStatics::LoadGameFromSlot()
    → GameInstance.LoadSaveData(instanceData)
    → ItemSubsystem.LoadSaveData(itemData)
    → WorldSubsystem.LoadSaveData(worldData)
    → 타겟 레벨 오픈
    → 최근 균열에서 리스폰
```

---

## 12. Gameplay Tag 체계

프로젝트 전체에서 GameplayTag를 사용하여 식별, 분류, 조건 검사를 수행한다.

### 12.1 네임스페이스 구조

| 네임스페이스 | 용도 | 주요 태그 |
|-------------|------|----------|
| `EffectTags` | 제어/상태/AI 이펙트 | Stun, Parry, Dash, Burn, Poison, Idle, Combat |
| `StatTags` | 스탯 (기본/전투/가변) | Level, STR, MaxHealth, AttackPower, Health |
| `SkillGroupTags` | 스킬 그룹 | BaseAttack, WeaponSkill, MagicSkill, Boss별 패턴 |
| `SkillTags` | 개별 스킬 | BaseAttackSword1~3, MagicFireBallSkill, Boss1~5 스킬 |
| `PawnTags` | 폰 식별 | Player, Boss1~5, AxeSkeleton, Minion 등 |
| `ItemTags` | 아이템 분류 | SwordWeapon, Potion, Key, HintNote 등 |
| `TraceTags` | 히트 판정 트레이스 | HandRightSword, HandLeftClaw, Body |
| `WorldActorTags` | 월드 액터 분류 | ItemDropped, OpenableChestItem, DyingHelper |

### 12.2 태그 활용 패턴

```
스탯 관리:     TMap<FGameplayTag, float> StatMap → 태그로 스탯 접근
이펙트 관리:   FGameplayTagContainer ActiveTags → 태그 추가/제거로 상태 변경
무적 판정:     InvincibilityTags.HasTag(EffectTag) → 무적 여부 검사
스킬 실행:     ExecuteSkill(SkillGroupTag) → 태그로 스킬 그룹 선택
아이템 분류:   ItemTags 계층 구조로 아이템 종류 판별
AI 상태:       AIStateComponent에서 태그로 상태 전환
```

---

## 13. 모듈 의존성

### Build.cs 의존 모듈

| 모듈 | 용도 |
|------|------|
| Core, CoreUObject, Engine | UE5 기본 |
| InputCore, EnhancedInput | 입력 시스템 |
| GameplayTags | 태그 기반 시스템 |
| AIModule, NavigationSystem, GameplayTasks | AI 및 내비게이션 |
| Niagara | 이펙트 (에센스, 스킬 VFX) |
| LevelSequence, MovieScene, MovieSceneTracks | 컷씬/시퀀스 |
| PhysicsCore, GeometryCollectionEngine | 물리/파괴 |
| MissNoHit | 커스텀 플러그인 (히트 판정) |

에디터 전용: `Slate`, `SlateCore`, `MovieSceneTools`

### 플러그인

| 플러그인 | 설명 |
|---------|------|
| MissNoHit | 히트 판정 관련 커스텀 플러그인 |
| UEGitPlugin | Git 소스 컨트롤 |
| ModelingToolsEditorMode | 모델링 도구 (에디터) |
| Landmass | 지형 생성 |
| VirtualHeightfieldMesh | 가상 높이필드 |
| MovieRenderPipeline | 영상 렌더링 |

---

## 14. 핵심 설계 패턴

### 14.1 컴포넌트 기반 아키텍처

플레이어와 적 모두 `StatusComponent`, `SkillComponent`를 공유하며, 인터페이스(`IStatusComponentUser`, `ISkillComponentUser`)를 통해 접근한다. 캐릭터 클래스는 통합 허브 역할만 수행.

### 14.2 인터페이스 기반 디커플링

8개 인터페이스로 시스템 간 의존성을 최소화:
- `IDamagable` → 누구든 피격 가능
- `IInteractableInterface` → 누구든 상호작용 가능
- `ISkillComponentUser` → 스킬 시스템에 독립적으로 접근

### 14.3 GameplayTag 중심 데이터 드리븐 설계

매직 넘버/문자열 대신 계층적 태그 사용. 스탯, 이펙트, 스킬, 아이템 모두 태그로 식별하여 확장성 확보.

### 14.4 Delegate 기반 이벤트 시스템

직접 호출 대신 Delegate 브로드캐스트로 느슨한 결합 달성:
- UI는 StatusComponent의 Delegate를 구독
- AI는 AIStateComponent의 Delegate를 구독
- 스토리 시스템은 GameInstance의 Delegate를 구독

### 14.5 서브시스템 패턴

`UGameInstanceSubsystem`을 활용하여 전역 서비스(인벤토리, 세이브, 사운드, 월드 상태)를 관리. GameMode에서 캐싱하여 반복 조회 방지.

### 14.6 State 패턴 (플레이어 입력)

`UPlayerControlStateBase`의 가상 메서드를 통해 입력 처리를 상태별로 다르게 동작시킬 수 있는 구조. 스턴/사망/스킬 사용 중 등의 상태에서 입력 차단 가능.

### 14.7 가중치 기반 AI 스킬 선택 (보스)

단순 랜덤이 아닌 가중치+증가분+쿨다운 시스템으로 보스의 행동 다양성과 예측 가능성의 균형을 유지.

### 14.8 Latent Task 패턴 (BT)

`ExecutePattern` BT Task가 `InProgress`를 반환하고, 애니메이션 완료 콜백으로 종료하는 패턴. 비헤이비어 트리와 애니메이션 시스템의 깔끔한 분리.

---

## 부록: 스토리 진행 단계 (EGameStoryState)

| 값 | 상태 | 설명 |
|----|------|------|
| 0 | Tutorial | 튜토리얼 |
| 1 | Game_Start | 게임 시작 |
| 2 | Find_Boss1 | Boss1 탐색 |
| 3 | Battle_Boss1 | Boss1 전투 |
| 4 | Clear_Boss1 | Boss1 클리어 |
| 5 | UnlockPath_Boss1 | Boss1 경로 해제 |
| 6 | Find_PathKey_Boss2 | Boss2 열쇠 탐색 |
| 7 | Find_Boss2 | Boss2 탐색 |
| 8 | Battle_Boss2 | Boss2 전투 |
| 9 | Clear_Boss2 | Boss2 클리어 |
| 10 | UnlockPath_Boss2 | Boss2 경로 해제 |
| 11 | Unlock_Lever | 레버 해제 |
| 12 | Find_Boss3 | Boss3 탐색 |
| 13 | Battle_Boss3 | Boss3 전투 |
| 14 | Clear_Boss3 | Boss3 클리어 |
| 15 | Find_Boss4 | Boss4 탐색 |
| 16 | Clear_Boss4 | Boss4 클리어 |
| 17 | Find_Boss5 | Boss5 탐색 |
| 18 | Clear_Boss5 | Boss5 클리어 |
| 19 | Game_End | 게임 엔딩 |
