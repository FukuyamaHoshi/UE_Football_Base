// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "C_Manager.h"
#include "C_Player.h"
#include "C_Soccer_Ball.h"
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
	// ロングキック
	void LongKick(FVector toLocation);
	// シュート
	void Shoot();
	// ボールホルダー設定
	void SetBallHolder(AC_Player* targetPlayer);
	// プレス回避行動
	void EscapePressing();
	// ロングアタック
	void LongAttack();


	UMaterial* playerSelectedDecal = nullptr; // プレイヤー選択デカール
	AC_Manager* currentHoverManager = nullptr; // 現在ホバー中のマネージャー
	TArray<AC_Player*> allPlayers = {}; // 全てのプレイヤー
	TArray<AC_Player*> homePlayers = {}; // HOMEプレイヤー
	TArray<AC_Player*> awayPlayers = {}; // AWAYプレイヤー
	AC_Player* ballHolder = nullptr; // ボールホルダー
	int command = 0; // 現在の戦術コマンド
	TArray<AC_Player*> GKEscapeToPlayers = {}; // GKがプレス回避先のプレイヤー (プレス回避時のみ)

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
