// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "C_Soccer_Ball.h"
#include "C_Player_Anim_Instance.h"
#include "C_Tile.h"
#include <Components/WidgetComponent.h>
#include "C_Player.generated.h"



UCLASS()
class FOOTBALL_BASE_API AC_Player : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AC_Player();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	bool isBallHolder = false; // ボールホルダー判定
	float ballKeepingCount = 0.0f; // ボールキープ時間カウンター
	int position = -1; // ポジション (TAGから取得)
	bool isMoving = false; // 移動中か (*** フェーズ中フラグ ***)
	int tileNo = 0; // 現在のタイルNo
	int playerType = 0; // プレイヤータイプ
	DECLARE_DELEGATE(FCompleteMoveDelegate); // 移動終了コールバッグDelegate
	FCompleteMoveDelegate completeMoving; // 移動終了時コールバック関数

	// メッシュを表示する
	void DisplayMesh();
	// セットポジション (位置からポジション取得)
	void SetPosition();
	// ショートパス
	void ShotPass(AC_Player* targetPlayer);
	// ロングパス
	void LongPass(AC_Player* targetPlayer);
	// スペースパス (ショートパス)
	void SpacePass(FVector toLocation);
	// ロングキック
	void LongKick(FVector toLocation);
	// トラップ
	void Trap(AC_Player* fromPlayer);
	// シュート
	void Shoot();
	// ボール保持
	void BallKeeping();
	// 走る(セット)
	void RunTo(FVector toLocation);
	// ドリブル (相手を抜く)
	void RegateDrrible();
	// タックル (Root Motion)
	void Tackle();
	// セットディフェンス姿勢
	void SetDefensiveStance();
	// 解除ディフェンス姿勢
	void RemoveDefensiveStance();
	// ドリブル (前進)
	void Drrible();
	// 喜びアニメーション
	void CheerMotion();
	// 悲しみアニメーション
	void SadMotion();
	// フリーアピール
	void FreeAppeal(AC_Player* ballHolder);
	// ターゲットアピール
	void TargetmanAppeal(AC_Player* ballHolder);
	// ポストマンアピール
	void PostmanAppeal(AC_Player* ballHolder);
	// ランナーアピール
	void RunnerAppeal();
	// ポケットマンアピール
	void PoketmanAppeal(AC_Player* ballHolder);
	// アニメーション停止 (アイドル状態へ)
	void StopAnim();
	// 位置からタイルＮｏ取得
	int GetTileNoFromLocation();
	// スポーンプレイヤーのマテリアルセット
	void SetSpwanPlayerMaterial(int typeNo);
	// プレイヤータイプアイコンセット
	// 1: ランナー, 2: ターゲットマン
	void SetPlayerTypeIcon(int typeNo);

private:
	// 移動処理
	void Move(float dTime);


	USkeletalMeshComponent* myMesh = nullptr; // メッシュ
	UMaterial* normalPlayerMaterial = nullptr; // マテリアル(ノーマル)
	UMaterial* targetmanPlayerMaterial = nullptr; // マテリアル(ターゲットマン)
	UMaterial* runnerPlayerMaterial = nullptr; // マテリアル(ランナー)
	AC_Soccer_Ball* ball = nullptr; // ボール
	UAnimMontage* shortPassAnim = nullptr; // アニメーション(ショートパス)
	UAnimMontage* trapAnim = nullptr; // アニメーション(トラップ)
	UAnimMontage* longPassAnim = nullptr; // アニメーション(ロングパス)
	UAnimMontage* regateAnim = nullptr; // アニメーション(レガテ)
	UAnimMontage* tackleAnim = nullptr; // アニメーション(タックル)
	UAnimMontage* sad1Anim = nullptr; // アニメーション(被ゴール1)
	UAnimMontage* sad2Anim = nullptr; // アニメーション(被ゴール2)
	UAnimMontage* cheer1Anim = nullptr; // アニメーション(ゴール1)
	UAnimMontage* cheer2Anim = nullptr; // アニメーション(ゴール2)
	UAnimMontage* freeAppealAnim = nullptr; // アニメーション(フリーアピール)
	UAnimMontage* targetmanAppealAnim = nullptr; // アニメーション(ターゲットマンアピール)
	UAnimMontage* postmanAppealAnim = nullptr; // アニメーション(ポストマンアピール)
	UAnimMontage* runnerAppealAnim = nullptr; // アニメーション(ランナーアピール)
	UAnimMontage* poketmanAppealAnim = nullptr; // アニメーション(ポケットマンアピール)
	UWidgetComponent* myWidget = nullptr; // ウィジェットコンポーネント
	TSubclassOf<UUserWidget> runnerWidgetIcon = nullptr; // ランナーWidget
	TSubclassOf<UUserWidget> targetmanWidgetIcon = nullptr; // ターゲットマンWidget

	bool isTrap = false; // トラップアニメーションするか
	UC_Player_Anim_Instance* playerAnimInstance = nullptr; // アニメーションインスタンス
	FVector fromLocation = FVector(0, 0, 0); // 動く前のターゲット位置(一時保存)
	FVector targetLocation = FVector(0, 0, 0); // 動くターゲット位置(一時保存)
	bool isBallKeeping = false; // ボールキープ判定
	TArray <AC_Tile*> tiles = {}; // 全てのタイル
	bool isDrribling = false; // ドリブル中か
	float movingCount = 0.0f; // 移動経過時間
	float moveTotalTime = 0.0f; // 移動時間(指定)

	const FVector HOME_GOAL_LOCATION = FVector(-1200.0f, 0, 0); // ゴール位置(home)
	const FVector AWAY_GOAL_LOCATION = FVector(1200.0f, 0, 0); // ゴール位置(away)
};
