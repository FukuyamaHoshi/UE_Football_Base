// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "C_Soccer_Ball.h"
#include "C_Player_Anim_Instance.h"
#include "C_Tile.h"
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

	// メッシュを表示する
	void DisplayMesh();
	// セットポジション (Tagから変数にポジションを設置する)
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
	// 移動(セット)
	void MoveTo(FVector toLocation);
	// ドリブル (相手を抜く)
	void RegateDrrible();
	// タックル (Root Motion)
	void Tackle();
	// ドリブル (前進)
	void Drrible();
	// 喜びアニメーション
	void CheerMotion();
	// 悲しみアニメーション
	void SadMotion();

private:
	// 移動処理
	void Move();
	// 位置からタイルＮｏ取得
	int GetTileNoFromLocation(float x, float y);

	USkeletalMeshComponent* myMesh = nullptr; // メッシュ
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
	bool isTrap = false; // トラップアニメーションするか
	UC_Player_Anim_Instance* playerAnimInstance = nullptr; // アニメーションインスタンス
	FVector fromLocation = FVector(0, 0, 0); // 動く前のターゲット位置(一時保存)
	FVector targetLocation = FVector(0, 0, 0); // 動くターゲット位置(一時保存)
	bool isBallKeeping = false; // ボールキープ判定
	TArray <AC_Tile*> tiles = {}; // 全てのタイル
	bool isDrribling = false; // ドリブル中か

	const FVector HOME_GOAL_LOCATION = FVector(-1200.0f, 0, 0); // ゴール位置(home)
	const FVector AWAY_GOAL_LOCATION = FVector(1200.0f, 0, 0); // ゴール位置(away)
};
