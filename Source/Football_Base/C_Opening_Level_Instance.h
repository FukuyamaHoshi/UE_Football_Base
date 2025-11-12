// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "C_Manager.h"
#include "C_Player.h"
#include "C_Soccer_Ball.h"
#include "C_Opening_UI.h"
#include "C_Opening_Level_Instance.generated.h"

UCLASS()
class FOOTBALL_BASE_API AC_Opening_Level_Instance : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AC_Opening_Level_Instance();

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
	// Wキー(プレス)イベント
	void PressedW();
	// Aキー(プレス)イベント
	void PressedA();
	// Sキー(プレス)イベント
	void PressedS();
	// Dキー(プレス)イベント
	void PressedD();
	// ホバー処理
	void Hover();
	// マウス位置に目的のオブジェクトがあるか判定して情報取得
	// | retrun: 目的のオブジェクトか判定, hitRusult: 取得するオブジェクト情報, objectTypes: 目的のオブジェクトの種類(コリジョン) |
	bool GetResultFromMouseLocation(FHitResult& hitResult, TArray<TEnumAsByte<EObjectTypeQuery>> objectTypes);
	// 試合開始時処理
	void MatchStart();
	// ショートパス
	void ShortPass(AC_Player* toPlayer);
	// ロングパス
	void LongPass(AC_Player* toPlayer);
	// スペースパス (ショートパス)
	void SpacePass(FVector toLocation);
	// ロングキック
	void LongKick(FVector toLocation);
	// シュート
	void Shoot();
	// 裏抜け (走る)
	void GetBehind(AC_Player* runPlayer, FVector toLocation);
	// ボールホルダー設定
	void SetBallHolder(AC_Player* targetPlayer);
	// プレイヤーフリー判定
	bool GetIsFree(AC_Player* targetPlayer);
	// デゥエル
	void Duel();
	// ラインブレイク
	void LineBreak();
	// クロス
	void Cross();
	// ポストプレー
	void PostPlay();


	UMaterial* playerSelectedDecal = nullptr; // プレイヤー選択デカール
	AC_Manager* currentHoverManager = nullptr; // 現在ホバー中のマネージャー
	TArray<AC_Player*> allPlayers = {}; // 全てのプレイヤー
	TArray<AC_Player*> homePlayers = {}; // HOMEプレイヤー
	TArray<AC_Player*> awayPlayers = {}; // AWAYプレイヤー
	AC_Player* ballHolder = nullptr; // ボールホルダー
	AC_Soccer_Ball* ball = nullptr; // ボール
	int currentCommand = 0; // 現在の戦術コマンド (一時保存用 インスタンスのコマンドを受付け)
	TArray<AC_Player*> GKEscapeToPlayers = {}; // GKがプレス回避先のプレイヤー (プレス回避時のみ)
	AC_Player* getBehindingPlayer = nullptr; // 現在裏抜け中のプレイヤー
	TArray <AC_Tile*> tiles = {}; // 全てのタイル
	bool isDueling = false; // デゥエル中か
	int deffenceLine = 3; // 現在のディフェンスライン(HOME, AWAY兼用 0-5まで) *暫定処理
	bool isLineBreak = false; // ラインブレイク
	int postPlayCount = 0; // ポストプレイ手順回数カウンター
	AC_Player* passAndGoPlayer = nullptr; // 一時保存 (ポストプレイ)
	AC_Player* postPlayer = nullptr; // 一時保存 (ポストプレイ)
	AC_Manager* homeManager = nullptr; // マネージャー(HOME)
	UC_Opening_UI* openingUI = nullptr; // オープニングUI (インスタンス)
	TSubclassOf<UUserWidget> openingUISubClass = nullptr; // オープニングUI (サブクラス) *これはウィジェット表示のためのクラス

	const float ESCAPE_INTERVAL = 3.0f; // プレス回避間隔
	TArray<FVector> HOME_LONG_ATTACK_POINTS = 
	{ 
		FVector(900.0f, -610.0f, 100.0f), 
		FVector(900.0f, -350.0f, 100.0f), 
		FVector(900.0f, 0.0f, 100.0f), 
		FVector(900.0f, 350.0f, 100.0f), 
		FVector(900.0f, 610.0f, 100.0f) }; // ロングアタックのポイント (HOME)
	TArray<FVector> AWAY_LONG_ATTACK_POINTS = 
	{ 
		FVector(-900.0f, 610.0f, 100.0f), 
		FVector(-900.0f, 350.0f, 100.0f), 
		FVector(-900.0f, 0.0f, 100.0f), 
		FVector(-900.0f, -350.0f, 100.0f), 
		FVector(-900.0f, -610.0f, 100.0f) }; // ロングアタックのポイント (AWAY)
};
