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
	// 入力設定
	void SetupInput();
	// 左クリック(プレス)イベント
	void PressedLeft();
	// 左クリック(リリース)イベント
	void ReleasedLeft();
	// (*デバッグ時のみ)スペースキー(プレス)イベント
	void PressedSpaceBar();
	// 自分のチームのコマを選択
	AActor* SelectHomePiece();
	// 位置からタイルＮｏ取得
	int GetTileNoFromLocation(float x, float y);
	// マウス位置のオブジェクトを取得
	// ( return: bool(取得できたか), FHitResult&(取得したオブジェクト情報)<参照渡し> )
	bool GetObjectFromMouseLocation(TArray<TEnumAsByte<EObjectTypeQuery>> objectTypes, FHitResult& outHit);
	// ボールホルダーのプレイ選択
	void SelectPlayForBallHolder();
	// ディフェンダーのプレイ選択
	void SelectPlayForDefender();
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
	void Duel(AC_Piece* dueledPlayer);
	// プレス
	void Press();
	// マーキング
	void Marking(AC_Piece* defencePlayer);
	// 2次配置フェーズ
	void SecondPlacePhase();
	// ③-⑵プレイステップフェーズを監視するタイマー設定
	void SetTimerMonitorPlayStepPhase();
	// ③-⑵プレイステップフェーズを終了していいか監視
	void MonitorFinishPlayStepPhase();
	// (フェーズ)タイマーとステップを終了
	void FinishTimerAndStep();
	// マウスホバー時処理
	void HoverMouse();
	// ③-⑴ 準備ステップフェーズ
	void PrepareStepPhase();
	// ③-⑵ プレイステップフェーズ
	void PlayStepPhase();
	// ③-⑶ リセットステップフェーズ (bool: falseでフェーズ終了)
	bool ResetStepPhase();
	// パスレンジのタイルＮｏ取得
	TArray <int> GetTileNoInPassRange();
	// ボールホルダーの向きを取得
	int GetDirectionOfBallHolder();
	// 対人範囲のタイルNo取得
	// | passRange: パス範囲から取得 |
	TArray <int> GetTileNoInDuelRange(TArray<int> passRange);
	// マークレンジ取得
	// | currentTileNo: マークレンジ元のタイルＮｏ, playerDirection: 体の向き |
	TArray <int> GetMarkRange(int currentTileNo, int playerDirection);
	// ファーストディフェンダー取得
	AC_Piece* GetFirstDefender();
	// 次に動く最短距離のタイルNo取得
	int GetShortestNextTileNo(int fromTileNo, int toTileNo);
	// プレイヤー配置レンジを設定
	void SetPlayerPlaceRange();
	// プレイヤー配置レンジを削除
	void RemovePlayerPlaceRange();

	
	AActor* selectedPlayer; // 選択されたプレイヤー
	bool isGrap = false; // Grap(プレイヤー選択中)フラグ
	int currentHoverTileNo = 0; // 現在ホバーしているタイルNo
	int selectedPlayerTileNo = 0; // 選択しているプレイヤーのタイルNo
	int overlayTileNo = 0; // 現在光っているタイルNo
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
	TArray <int> moveToTileNos = {}; // プレイヤーとボールが動く先のタイルNo (*動く先のタイルを予約し、重複を防ぐ)
};
