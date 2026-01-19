// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "C_Player.h"
#include "CPlayerAI.generated.h"

/**
 * プレイヤーAIコントローラー
 */
UCLASS()
class FOOTBALL_BASE_API ACPlayerAI : public AAIController
{
	GENERATED_BODY()

public:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;

public:
	// ボールホルダーフラグ
	bool isBallHolder = false;
	// ディフェンダーフラグ
	bool isDefender = false;
	// フリーアピールフラグ
	bool isFreeMan = false;
	// 裏抜けフラグ (走るプレイヤー)
	bool isGetBehindRunner = false;
	// 縦ずれプレイヤーフラグ
	bool isFrontMovingPlayer = false;

private:
	// - フリー(ボールホルダー)ハンドラ -
	void HandleFreeHolder();
	// - デュエルハンドラ -
	void HandleDuelStart();
	// - ラインブレイクハンドラ -
	void HandleLineBreak();
	// - クロスハンドラ -
	void HandleCross();
	// - シュートハンドラ -
	void HandleShoot();
	// - 裏抜けハンドラ -
	void HandleGetBehind();
	// - ドリブル突破ハンドラ -
	void HandleDribbleBreakThrough();
	// - ゴールハンドラ -
	void HandleGoal();

	// ショートパス
	void ShortPass(AC_Player* toPlayer);
	// ロングキック
	void LongKick(FVector toLocation);
	// フリーマンへパス
	// | return : 実行済みか |
	bool ShortPassToFreeMan();
	// キャリー (前進ドリブル)
	// | return : 実行済みか |
	bool Carry();
};
