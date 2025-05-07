// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NiagaraComponent.h"
#include "GameFramework/Character.h"
#include "C_Piece.generated.h"

UCLASS()
class FOOTBALL_BASE_API AC_Piece : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AC_Piece();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// 指定された位置へ動く処理の設定
	// < 引数：tLocation(目標位置) >
	void SetMoveTo(FVector tLocation);
	// 線を表示する
	void SetDrawLineTo(FVector mainTargetLocation, FLinearColor lineColor, FVector subTargetLocation = FVector(0, 0, 0));

	int currentTileNo = 0; // 現在のタイルNo
	int currentLane = 0; // 現在のレーン
	bool isMoving = false; // 移動中か (*** フェーズ中フラグ ***)
	TArray <int> markRange = {}; // マーク範囲 (プレイヤー各々で保持)
	int direction = 0; // 体の向き (前向き: 25, 後ろ向き: -25)
	bool isMarked = false; // マークされているか
	int position = 0; // ポジション (内訳はCommon参照)
	bool isLineBreakPreAction = false; // ラインブレイク動作をしているか(*事前動作)
	
	// ** 能力値 **
	int tackle = 1; // タックル: ディフェンス時の対人能力
	int keep = 1; // キープ: オフェンス時の背面対人能力
	int drrible = 1; // ドリブル: オフェンス時の対人能力
	int heading = 1; // ヘディング: エアバトル時の勝率
	int physicalContact = 1; // フィジカルコンタクト: セカンドボール回収能力
	// *********

private:
	// 指定された位置へ動く処理
	void MoveTo();

	FVector targetLocation = FVector(0, 0, 0); // 動くターゲット位置(一時保存)
	UNiagaraComponent* mainLine = nullptr; // プレイヤー間のライン(Main)
	UNiagaraComponent* subLine = nullptr; // プレイヤー間のライン(Sub)
};
