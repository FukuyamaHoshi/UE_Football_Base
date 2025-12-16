// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "C_Manager.h"
#include "C_Player.h"
#include "C_Soccer_Ball.h"
#include "C_Opening_UI.h"
#include <Engine/DecalActor.h>
#include "C_Common.h"
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
	// | マネージャー選択 |
	void PressedLeft();
	// 左クリック(リリース)イベント
	void ReleasedLeft();
	// 左クリック (プレイヤー選択)
	void PressedLeftForPlayerSelect();
	// プレイヤーをマウスに追従させる (プレイヤー選択)
	void FollowPlayerToMouse();
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
	// ポゼッションコマンド
	void PossetionCommand();
	// ロングアタックコマンド
	void LongAttackCommand();
	// テクニカルアタックコマンド
	void TecnicalAttackCommand();
	// ローブロックコマンド
	void LowBlockCommand();
	// サイド圧縮コマンド
	void SidePressCommand();
	// ハイプレスコマンド
	void HighPressCommand();
	
	// 走る
	void RunTo(AC_Player* player, FVector toLocation);
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
	void PostPlay(bool isLong = false);
	// AWAYチームAI
	void AwayTeamAI();
	// プレイヤースポーン (プールから選ばれた)
	void SpawnPlayerInPool(int selectedNo);
	// 位置からタイルＮｏ取得
	int GetTileNoFromLocation(FVector location);
	// ディフェンスライン設定
	void SetDeffenceLine();


	UMaterial* managerSelectedDecalMaterial = nullptr; // マネージャー選択デカール (マテリアル)
	ADecalActor* managerSelectedDecalActor = nullptr; // マネージャー選択デカール (アクター)
	AC_Manager* currentHoverManager = nullptr; // 現在ホバー中のマネージャー
	TArray<AC_Player*> allPlayers = {}; // 全てのプレイヤー
	TArray<AC_Player*> homePlayers = {}; // HOMEプレイヤー
	TArray<AC_Player*> awayPlayers = {}; // AWAYプレイヤー
	AC_Player* ballHolder = nullptr; // ボールホルダー
	AC_Player* preBallHolder = nullptr; // 前回のボールホルダー
	AC_Soccer_Ball* ball = nullptr; // ボール
	AC_Player* selectedPlayer = nullptr; // 選択中のプレイヤー (プレイヤー選択フェーズ)
	bool isPlayerGrap = false; // 選択中か (プレイヤー選択フェーズ)
	AC_Tile* selectedTile = nullptr; // 選択中のタイル (プレイヤー選択フェーズ)
	TArray<FVector> playerInitialPlaceLocation = {}; // 全てのプレイヤーの初期配置位置
	TArray<FVector> homeTeamIntialPlaceLocation = {}; // HOMEチームの初期配置位置
	TArray<FVector> awayTeamIntialPlaceLocation = {}; // AWAYチームの初期配置位置
	TArray<int> homeTeamIntialTileNo = {}; // HOMEチームの初期配置タイルNo
	TArray<int> awayTeamIntialTileNo = {}; // AWAYチームの初期配置タイルNo
	int currentCommand = 0; // 現在の戦術コマンド (一時保存用 インスタンスのコマンドを受付け)
	int awayCommand = 0; // AWAYチームの戦術コマンド
	TArray<AC_Player*> GKEscapeToPlayers = {}; // GKがプレス回避先のプレイヤー (プレス回避時のみ)
	AC_Player* getBehindingPlayer = nullptr; // 現在裏抜け中のプレイヤー
	TArray <AC_Tile*> tiles = {}; // 全てのタイル
	bool isDueling = false; // デゥエル中か
	int homeDeffenceLine = 0; // ディフェンスライン (HOME 0-5まで)
	int awayDeffenceLine = 0; // ディフェンスライン (AWAY 0-5まで)
	bool isLineBreak = false; // ラインブレイク
	int postPlayCount = 0; // ポストプレイ手順回数カウンター
	AC_Player* passAndGoPlayer = nullptr; // 一時保存 (ポストプレイ)
	AC_Player* postPlayer = nullptr; // 一時保存 (ポストプレイ)
	AC_Manager* homeManager = nullptr; // マネージャー(HOME)
	UC_Opening_UI* openingUI = nullptr; // オープニングUI (インスタンス)
	TSubclassOf<UUserWidget> openingUISubClass = nullptr; // オープニングUI (サブクラス) *これはウィジェット表示のためのクラス
	bool isOnceMatchStart = true; // 試合開始時に一度のみ実行する判定
	bool isGoal = false; // ゴール判定
	bool isOnceGoal = false; // ゴール時に一度のみする
	float goalCount = 0; // ゴール時カウンター
	bool isPoketman = false; // ポケットマン処理開始
	AC_Player* currentPoketman = nullptr; // 現在処理中のポケットマン
	int poketmanPlayCount = 0; // ポケットマン手順回数カウンター
	FVector poketmanFromLocation = FVector(0, 0, 0); // ポケットマン移動前の位置
	// プレイヤー (サブクラス) *プレイヤーSpawn時使用
	TSubclassOf<AC_Player> playerSubClass_1 = nullptr;
	TSubclassOf<AC_Player> playerSubClass_2 = nullptr;
	TSubclassOf<AC_Player> playerSubClass_3 = nullptr;
	TSubclassOf<AC_Player> playerSubClass_4 = nullptr;
	TSubclassOf<AC_Player> playerSubClass_5 = nullptr;
	TArray <AC_Player*> subPlayers = {}; // サブプレイヤー
	TArray<AC_Player*> highPressDeffenders = {}; // ハイプレスディフェンダー

	TArray<FVector> HOME_LONG_ATTACK_POINTS = 
	{ 
		FVector(825.0f, -660.0f, C_Common::BALL_BASE_LOCATION_Z), 
		FVector(825.0f, -330.0f, C_Common::BALL_BASE_LOCATION_Z),
		FVector(825.0f, 0.0f, C_Common::BALL_BASE_LOCATION_Z),
		FVector(825.0f, 330.0f, C_Common::BALL_BASE_LOCATION_Z),
		FVector(825.0f, 660.0f, C_Common::BALL_BASE_LOCATION_Z) }; // ロングアタックのポイント (HOME)
	TArray<FVector> AWAY_LONG_ATTACK_POINTS = 
	{ 
		FVector(-825.0f, 660.0f, C_Common::BALL_BASE_LOCATION_Z),
		FVector(-825.0f, 330.0f, C_Common::BALL_BASE_LOCATION_Z),
		FVector(-825.0f, 0.0f, C_Common::BALL_BASE_LOCATION_Z),
		FVector(-825.0f, -330.0f, C_Common::BALL_BASE_LOCATION_Z),
		FVector(-825.0f, -660.0f, C_Common::BALL_BASE_LOCATION_Z) }; // ロングアタックのポイント (AWAY)
	TArray<FVector> SUB_LOCATION =
	{
		FVector(-650, 1000, C_Common::PLAYER_BASE_LOCATION_Z),
		FVector(-800, 1000, C_Common::PLAYER_BASE_LOCATION_Z),
		FVector(-950, 1000, C_Common::PLAYER_BASE_LOCATION_Z) }; // サブプレイヤー位置
		

};
