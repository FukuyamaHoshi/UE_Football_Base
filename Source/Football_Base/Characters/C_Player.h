// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include <Components/WidgetComponent.h>
#include <Football_Base/Actors/C_Soccer_Ball.h>
#include <Football_Base/Actors/C_Tile.h>
#include "C_Player.generated.h"

DECLARE_DELEGATE(FCompleteMoveDelegate); // 移動終了コールバッグDelegate

/**
 * プレイヤーキャラクター
 */
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
	TArray<int> viewNos = {}; // 視野タイルNo
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
	// シュート
	void Shoot();
	// ボール保持
	void BallKeeping();
	// 走る(セット)
	void RunTo(FVector toLocation);
	// ドリブル (相手を抜く)
	void RegateDrrible();
	// ドリブル (前進)
	void Drrible();
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
	// 位置からタイルＮｏ取得
	int GetTileNoFromLocation();
	// スポーンプレイヤーのマテリアルセット
	void SetSpwanPlayerMaterial(int typeNo);
	// プレイヤータイプアイコンセット
	// 1: ランナー, 2: ターゲットマン
	void SetPlayerTypeIcon(int typeNo);
	// 前方を向く
	void LookForward();
	// 視野タイルNo更新
	void UpdateViewTileNos();

private:
	// 移動処理
	void Move(float dTime);

	USkeletalMeshComponent* myMesh = nullptr; // メッシュ
	UMaterial* normalPlayerMaterial = nullptr; // マテリアル(ノーマル)
	UMaterial* targetmanPlayerMaterial = nullptr; // マテリアル(ターゲットマン)
	UMaterial* runnerPlayerMaterial = nullptr; // マテリアル(ランナー)
	AC_Soccer_Ball* ball = nullptr; // ボール
	UWidgetComponent* myWidget = nullptr; // ウィジェットコンポーネント
	TSubclassOf<UUserWidget> runnerWidgetIcon = nullptr; // ランナーWidget
	TSubclassOf<UUserWidget> targetmanWidgetIcon = nullptr; // ターゲットマンWidget

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
