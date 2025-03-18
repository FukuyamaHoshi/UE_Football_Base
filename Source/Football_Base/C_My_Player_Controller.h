// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "C_Tile.h"
#include "C_Ball.h"
#include "C_Piece.h"
#include "GameFramework/PlayerController.h"
#include "C_My_Player_Controller.generated.h"

/**
 * 
 */
UCLASS()
class FOOTBALL_BASE_API AC_My_Player_Controller : public APlayerController
{
	GENERATED_BODY()

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	// ********* input *********
	// 入力設定
	void SetupInput();
	// 左クリック(プレス)イベント
	// | プレイヤー選択 |
	void PressedLeft();
	// 左クリック(リリース)イベント
	void ReleasedLeft();
	// (*デバッグ時のみ)スペースキー(プレス)イベント
	void PressedSpaceBar();
	// マウスホバー時,タイルをハイライトする
	void TileHighlightWhenMouseHover();
	// **********************
	


	// ********* プレイ判断 *********
	// ボールホルダーのプレイ選択
	void SelectPlayForBallHolder();
	// ディフェンダーのプレイ選択
	void SelectPlayForDefender();
	// セカンドボール時の動き
	// | 全プレイヤー対象となる |
	// | ボール周囲のプレイヤーがボールに対して移動する |
	void MovementForSecondBall();
	// *****************************
	
	
	
	// ********* プレイ *********
	// パス
	// ( 引数: targetPiece(コマ) )
	void Pass(AC_Piece* targetPiece);
	// ドリブル (前進のみ)
	void Drrible();
	// 方向転換 
	void ChangeOfDirection();
	// シュート
	void Shoot();
	// 対人
	// | dueledPlayer: 対人をされるプレイヤー (発火時,ディフェンダー) |
	// | return bool: デゥエルの勝者(ボールホルダー = ture, ディフェンダー = false) |
	bool Duel(AC_Piece* dueledPlayer);
	// 背面対人 (背負った時のプレー)
	// | dueledPlayer: 対人をされるプレイヤー (発火時,ディフェンダー) |
	// | return bool: デゥエルの勝者(ボールホルダー = ture, ディフェンダー = false) |
	bool BackDuel(AC_Piece* dueledPlayer);
	// サイド対人
	// | dueledPlayer: 対人をされるプレイヤー (発火時,ディフェンダー) |
	// | return bool: デゥエルの勝者(ボールホルダー = ture, ディフェンダー = false) |
	bool SideDuel(AC_Piece* dueledPlayer);
	// プレス
	void Press();
	// マーキング
	void Marking(AC_Piece* defencePlayer);
	// クリアリング
	// | defencePlayer: 対人をされるプレイヤー |
	void Clearing(AC_Piece* defencePlayer);
	// **************************
	


	// ********* フェーズ *********
	// 2次配置フェーズ
	void SecondPlacePhase();
	// ③-⑵プレイステップフェーズを監視するタイマー設定
	void SetTimerMonitorPlayStepPhase();
	// ③-⑵プレイステップフェーズを終了していいか監視
	void MonitorFinishPlayStepPhase();
	// (フェーズ)タイマーとステップを終了
	void FinishTimerAndStep();
	// ③-⑴ 準備ステップフェーズ
	void PrepareStepPhase();
	// ③-⑵ プレイステップフェーズ
	void PlayStepPhase();
	// ③-⑶ リセットステップフェーズ (bool: falseでフェーズ終了)
	bool ResetStepPhase();
	// **************************
	


	// マウス位置に目的のオブジェクトがあるか判定して情報取得
	// | retrun: 目的のオブジェクトか判定, hitRusult: 取得するオブジェクト情報, objectTypes: 目的のオブジェクトの種類(コリジョン) |
	bool GetResultFromMouseLocation(FHitResult& hitResult, TArray<TEnumAsByte<EObjectTypeQuery>> objectTypes);
	// 位置からタイルＮｏ取得
	int GetTileNoFromLocation(float x, float y);
	// パスレンジのタイルＮｏ取得
	TArray <int> GetTileNoInPassRange();
	// ボールホルダーの向きを取得
	int GetDirectionOfBallHolder();
	// 対人範囲のタイルNo取得
	TArray <int> GetTileNoInDuelRange();
	// マークレンジ取得
	// | currentTileNo: マークレンジ元のタイルＮｏ, playerDirection: 体の向き |
	TArray <int> GetMarkRange(int currentTileNo, int playerDirection);
	// ファーストディフェンダー取得
	AC_Piece* GetFirstDefender();
	// 次に動く最短距離のタイルNo取得
	int GetShortestNextTileNo(int fromTileNo, int toTileNo);
	// プレイヤー配置可能エリアを表示
	void DisplayPlayerPlaceableErea();
	// プレイヤー配置レンジを削除
	void RemovePlayerPlaceRange();
	// 全てのプレイヤーにポジションを設定
	void SetPositionToAllPlayer();
	// 全てのプレイヤーとボールにタイル位置を設定
	TArray<int> SetTileNoToAllPlayersAndBall();
	// プレイヤーとボール位置をタイル位置へ設定
	void SetPlayerAndBallToTilePosition();
	// プレイヤー間のラインを表示
	void DisplayLineBetweenPlayers();

	
	AC_Piece* selectedPlayer; // 選択されたプレイヤー
	bool isGrap = false; // Grap(プレイヤー選択中)フラグ
	TArray <AC_Tile*> allTiles; // すべてのタイル配列
	AC_Tile* overlayTile; // 光っているタイル
	UMaterial* playerSelectedDecal = nullptr; // プレイヤー選択デカール
	UDecalComponent* currentDecal = nullptr; // 現在表示しているデカール
	AC_Ball* ball; // ボール
	TArray <AC_Piece*> allPieces; // すべてのコマ配列
	TArray <AC_Piece*> allHomePieces; // すべてのHomeコマ配列
	TArray <AC_Piece*> allAwayPieces; // すべてのAwayコマ配列
	AC_Piece* ballHolder = nullptr; // ボールホルダー
	AC_Piece* preBallHolder = nullptr; // 前回のボールホルダー
	AC_Piece* passTarget = nullptr; // パスターゲット
	bool isInitialPlacePhase = true; // 初期配置フェーズフラグ
	bool isSecondPlacePhase = false; // 2次配置フェーズフラグ
	bool isPlayStepPhase = false; // プレイステップフェーズ中
	int stepCount = 0; // ステップカウント
	TArray <int> passRangeTileNos; // パスレンジタイルNo配列
	TArray <int> duelRangeTileNos; // 対人レンジタイルNo配列
	TArray< AC_Piece*> offencePlayers; // オフェンス側のプレイヤー配列
	TArray< AC_Piece*> defencePlayers; // ディフェンス側のプレイヤー配列
	bool isHomeBall = false; // Home側がオフェンスか
	AC_Piece* firstDefender = nullptr; // ファーストディフェンダー
	TArray <int> currentTileNos = {}; // 現在のプレイヤーのタイルNo
	TArray <int> moveToTileNos = {}; // プレイヤーとボールが動く先のタイルNo (*動く先のタイルを予約し、重複を防ぐ)
	TArray <int> ballCrossTileNos = {}; // ボール周囲の十字のタイルNo
	bool isGoal = false; // ゴール判定
	bool isClering = false; // クリアリング判定
	int ballHolderRightTileNo = 0; // ボールホルダーの右タイルNo
	int ballHolderLeftTileNo = 0; // ボールホルダーの左タイルNo
	int secondBallRangeTileNum = 0; // セカンドボール反応範囲のタイル個数 (step毎に拡大)
	TArray <int> placeableRrea = {}; // プレイヤー配置可能エリア
	
	// ** ポジションエリア **
	// < HOME >
	// -FW-
	TArray <int> LWG_Erea = {}; // LWG
	TArray <int> LST_Erea = {}; // LST
	TArray <int> CF_Erea = {}; // CF
	TArray <int> RST_Erea = {}; // RST
	TArray <int> RWG_Erea = {}; // RWG
	// -MF-
	TArray <int> LH_Erea = {}; // LH
	TArray <int> LIH_Erea = {}; // LIH
	TArray <int> CH_Erea = {}; // CH
	TArray <int> RIH_Erea = {}; // RIH
	TArray <int> RH_Erea = {}; // RH
	// -DF-
	TArray <int> LSB_Erea = {}; // LSB
	TArray <int> LCB_Erea = {}; // LCB
	TArray <int> CB_Erea = {}; // CB
	TArray <int> RCB_Erea = {}; // RCB
	TArray <int> RSB_Erea = {}; // RSB
	
	// < AWAY >
	// -FW-
	TArray <int> Away_LWG_Erea = {}; // LWG
	TArray <int> Away_LST_Erea = {}; // LST
	TArray <int> Away_CF_Erea = {}; // CF
	TArray <int> Away_RST_Erea = {}; // RST
	TArray <int> Away_RWG_Erea = {}; // RWG
	// -MF-
	TArray <int> Away_LH_Erea = {}; // LH
	TArray <int> Away_LIH_Erea = {}; // LIH
	TArray <int> Away_CH_Erea = {}; // CH
	TArray <int> Away_RIH_Erea = {}; // RIH
	TArray <int> Away_RH_Erea = {}; // RH
	// -DF-
	TArray <int> Away_LSB_Erea = {}; // LSB
	TArray <int> Away_LCB_Erea = {}; // LCB
	TArray <int> Away_CB_Erea = {}; // CB
	TArray <int> Away_RCB_Erea = {}; // RCB
	TArray <int> Away_RSB_Erea = {}; // RSB
	
	// ***
};
