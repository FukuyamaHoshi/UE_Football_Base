#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Tickable.h" // ← 必須
#include "C_Common.h"
#include "C_Tile.h"
#include "C_Player.h"
#include "GameStateManager.generated.h"

// C++ 専用デリゲート（Blueprint 連携なし）
DECLARE_MULTICAST_DELEGATE(FOnFreeHolder);
DECLARE_MULTICAST_DELEGATE(FOnDuelStart);
DECLARE_MULTICAST_DELEGATE(FOnLineBreak);
DECLARE_MULTICAST_DELEGATE(FOnCross);
DECLARE_MULTICAST_DELEGATE(FOnShoot);
DECLARE_MULTICAST_DELEGATE(FOnGetBehind);
DECLARE_MULTICAST_DELEGATE(FOnDribbleBreakThrough);
DECLARE_MULTICAST_DELEGATE(FOnGoal);
DECLARE_MULTICAST_DELEGATE(FOnMatchStart);
DECLARE_MULTICAST_DELEGATE(FOnMatchEnd);
DECLARE_MULTICAST_DELEGATE(FOnTurnCompletePhase);

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// ✅ State Flag Enum (状態フラグ列挙型)
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
UENUM(BlueprintType)
enum class EGameState : uint8
{
	None UMETA(DisplayName = "None"),
	FreeHolder UMETA(DisplayName = "Free Holder"),                 // フリーホルダー
	Duel UMETA(DisplayName = "Duel"),                              // デュエル
	LineBreak UMETA(DisplayName = "Line Break"),                   // ラインブレイク
	Cross UMETA(DisplayName = "Cross"),                            // クロス
	Shoot UMETA(DisplayName = "Shoot"),                            // シュート
	GetBehind UMETA(DisplayName = "Get Behind"),                   // 裏抜け
	DribbleBreakThrough UMETA(DisplayName = "Dribble BreakThrough"), // ドリブル突破
	Goal UMETA(DisplayName = "Goal"),                              // ゴール
	MatchStarted UMETA(DisplayName = "Match Started"),             // 試合開始
	MatchEnded UMETA(DisplayName = "Match Ended")                  // 試合終了
};

// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
// ✅ Turn Phase Enum (ターンフェーズ列挙型)
// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
UENUM(BlueprintType)
enum class ETurnPhase : uint8
{
	None UMETA(DisplayName = "None"),
	WaitingForAction UMETA(DisplayName = "Waiting For Action"),     // AI/プレイヤー行動待ち
	ActionPlaying UMETA(DisplayName = "Action Playing"),            // アクション実行中
	StateDetection UMETA(DisplayName = "State Detection"),          // 状態検知中
	TurnComplete UMETA(DisplayName = "Turn Complete")               // ターン完了
};

/**
 * 試合状態検知サブシステム
 */
UCLASS()
class FOOTBALL_BASE_API UGameStateManager
    : public UGameInstanceSubsystem
    , public FTickableGameObject   // ← これを継承する
{
    GENERATED_BODY()

public:

    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    // TickableGameObject の Tick
    virtual void Tick(float DeltaTime) override;
    virtual TStatId GetStatId() const override;

    virtual bool IsTickable() const override { return true; }

public:
    // C++ デリゲート
    FOnFreeHolder OnFreeHolder;
    FOnDuelStart OnDuelStart;
	FOnLineBreak OnLineBreak;
	FOnCross OnCross;
	FOnShoot OnShoot;
    FOnGetBehind OnGetBehind;
	FOnDribbleBreakThrough OnDribbleBreakThrough;
	FOnGoal OnGoal;
	FOnMatchStart OnMatchStart;
	FOnMatchEnd OnMatchEnd;
	FOnTurnCompletePhase OnTurnCompletePhase;

	// プレイヤー
	AC_Player* ballHolder = nullptr; // ボールホルダー
	AC_Player* duelDeffender = nullptr; // デュエル守備側
	TArray<AC_Player*> freeMans = {}; // フリーマン (複数)
	AC_Player* getBehindingRunner = nullptr; // 裏抜けプレイヤー (走る側)
	AC_Player* frontMovingPlayer = nullptr; // 縦ずれプレイヤー

    TArray <AC_Tile*> tiles = {}; // 全てのタイル
    TArray<AC_Player*> allPlayers = {}; // 全てのプレイヤー
    TArray<AC_Player*> homePlayers = {}; // HOMEプレイヤー
    TArray<AC_Player*> awayPlayers = {}; // AWAYプレイヤー
    TArray<AC_Player*> freePlayers = {}; // フリーマン
    int homeDeffenceLine = 0; // ディフェンスライン (HOME 0-5まで)
    int awayDeffenceLine = 0; // ディフェンスライン (AWAY 0-5まで)

public:
    // -- 共通関数 --
   // - プレイヤーフリー判定 -
    bool GetIsFree(AC_Player* targetPlayer);

private:
	bool isInitialized = false; // 初期化済みフラグ
    TArray <AC_Player*> subPlayers = {}; // サブプレイヤー
    AC_Soccer_Ball* ball = nullptr; // ボール
	int turnCount = 0; // ターンカウント (移動 → 停止時のみ、カウント)
	bool isMoving = false;                // 移動中か (特別フラグ)
    
	// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
	// ✅ Turn Phase State (ターンフェーズ状態)
	// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
	ETurnPhase currentTurnPhase = ETurnPhase::None; // 現在のターンフェーズ
	float waitingPhaseTimer = 0.0f;                 // 待機フェーズ経過時間
	TArray<EGameState> activeStates = {}; // アクティブな状態のリスト
	
private:
	// 初期化処理 (アクター取得など)
	bool Initialize();
	
	// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
	// ✅ Turn Phase Handling (ターンフェーズ処理)
	// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
	// フェーズ遷移
	void TransitionToPhase(ETurnPhase NewPhase);
	// フェーズハンドラー
	void HandleWaitingForActionPhase(float DeltaTime); // ← Add DeltaTime parameter
	void HandleActionPlayingPhase();
	void HandleStateDetectionPhase();
	void HandleTurnCompletePhase();

	// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
	// ✅ State management functions (状態管理関数)
	// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
	// 状態を追加
	void AddState(EGameState State);
	// 状態を削除
	void RemoveState(EGameState State);
	// 状態をチェック
	bool HasState(EGameState State) const;
	// 全状態をクリア
	void ClearAllStates();

	// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
	// Match state dedect and update (試合状態検知・更新関数)
	// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
	// - フリー(ボールホルダー)状態検知 -
	bool DedectFreeHolder();
	// - フリー(ボールホルダー)状態更新 -
	void UpdateFreeHolder();
    // - デュエル状態検知 -
	bool DedectDuel();
	// - デュエル状態更新 -
    void UpdateDuel();
	// - 移動終了検知 -
	bool DedectCompleteMoving();
	// - 移動終了更新 -
	void UpdateCompleteMoving();
	// - ラインブレイク検知 -
	bool DedectLineBreak();
	// - ラインブレイク更新 -
	void UpdateLineBreak();
	// - クロス検知 -
	bool DedectCross();
	// - クロス更新 -
	void UpdateCross();
	// - シュート状態検知 -
	bool DedectShoot();
	// - シュート状態更新 -
	void UpdateShoot();
	// - 裏抜け状態検知 -
	bool DedectGetBehind();
	// - 裏抜け状態更新 -
	void UpdateGetBehind();
	// - ドリブル突破状態検知 -
	bool DedectDribbleBreakThrough();
	// - ドリブル突破状態更新 -
	void UpdateDribbleBreakThrough();
	// - ゴール状態検知 -
	bool DedectGoal();
	// - ゴール状態更新 -
	void UpdateGoal();
	// - 試合開始更新 -
	void UpdateMatchStart();
	// - 試合終了検知 -
	bool DedectMatchEnd();
	// - 試合終了更新 -
	void UpdateMatchEnd();
	

	// リセットプレイヤーフラグ
	void ResetPlayerFlags();
	// チェック状態フラグ
	bool CheckStateFlags();
    // デゥエル時プレイ選択
	void DuelPlayChoice();
    // ディフェンスライン設定
    void SetDeffenceLine();
    // ボールホルダーセット
    void SetBallHolder();
	// 試合終了時処理
	void OnMatchEnded();
	// 試合再開処理
	void OnMatchRestart();
};