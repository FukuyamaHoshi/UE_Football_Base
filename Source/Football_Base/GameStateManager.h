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

    TArray <AC_Tile*> tiles = {}; // 全てのタイル
    AC_Player* ballHolder = nullptr; // ボールホルダー
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
    
	// - 状態フラグ -
	// フリー (ボールホルダーが接敵していない)
	bool isFreeHolder = false;
	// デュエル
    bool isDuel = false;
	// ラインブレイク中
	bool isLineBreak = false;
	// クロス中
	bool isCross = false;
	// シュート中
	bool isShoot = false;
	// 裏抜け中
	bool isGetBehind = false;
	// ドリブル突破中
	bool isDribbleBreakThrough = false;
	// 移動中
	bool isMoving = false;
	// ゴール中
	bool isGoal = false;
	// 試合開始済み
	bool isMatchStarted = false;
	// 試合終了済み
	bool isMatchEnded = false;

private:
	// 初期化処理 (アクター取得など)
	bool Initialize();
	
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

	// リセット状態フラグ
	void ResetStateFlags();
	// チェック状態フラグ
	bool CheckStateFlags();
    // デゥエル時プレイ選択
	void DuelPlayChoice();
    // ディフェンスライン設定
    void SetDeffenceLine();
    // ボールホルダーセット
    void SetBallHolder();
	// 移動終了時処理
	void OnCompleteMoving();
	// 試合終了時処理
	void OnMatchEnded();
};