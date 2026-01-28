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
	// action completed
	bool isActionCompleted = false;

	//  -- abiritys --
	int offence = 1;
	int defence = 1;
	int stamina = 1;

private:
	FVector initialLocation = FVector::ZeroVector; // 初期位置
	TArray<AC_Player*> passedTargets = {}; // passed players (in a attack)
	int enemyAbilityCost = 0; // enemy ability cost (for duel)

private:
	// - フリー(ボールホルダー)ハンドラ -
	void HandleFreeHolder();
	// - デュエルハンドラ -
	void HandleDuelStart();
	// - continue duel handler -
	void HandleDuelContinue(); // working on it
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
	// - ステートターン完了ハンドラ -
	void HandleStateTurnComplete();

	// ショートパス
	void ShortPass(AC_Player* toPlayer);
	// action pass from GK to DF
	// | return : completed |
	bool PassFromGKToDF();
	// action pass to front player
	// | return : completed |
	bool PassToFrontPlayer();
	// back pass (step by step to GK)
	// | return : completed |
	bool BackPass();
	// キャリー (前進ドリブル)
	// | return : 実行済みか |
	bool Carry();
	// update stamina
	void UpdateStamina(int value);

public:
	// get enemy ability cost (for duel)
	void SetEnemyAbilityCost(int enemysAbility);
};