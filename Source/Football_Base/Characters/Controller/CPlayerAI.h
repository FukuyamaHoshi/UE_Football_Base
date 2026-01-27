// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include <Football_Base/Characters/C_Player.h>
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
	// --- states ---
	// ball holder
	bool isBallHolder = false;
	// defender
	bool isDefender = false;
	// free
	bool isFreeMan = false;
	// get behind runner
	bool isGetBehindRunner = false;
	// front moving player
	bool isFrontMovingPlayer = false;

	//  -- abiritys --
	int offence = 1;
	int defence = 1;
	int stamina = 1;

private:
	FVector initialLocation = FVector::ZeroVector; // 初期位置

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
	// - 試合終了ハンドラ -
	void HandleMatchEnd();
	// - ターン完了ハンドラ -
	void HandleTurnCompletePhase();

	// ショートパス
	void ShortPass(AC_Player* toPlayer);
	// フリーマンへパス
	// | return : 実行済みか |
	bool ShortPassToFreeMan();
	// キャリー (前進ドリブル)
	// | return : 実行済みか |
	bool Carry();
};
