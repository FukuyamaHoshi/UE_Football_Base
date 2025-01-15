// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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

	int currentTileNo = 0; // 現在のタイルNo
	bool isMoving = false; // 移動中か (*** フェーズ中フラグ ***)
	TArray <int> markRange; // マーク範囲 (プレイヤー各々で保持)
	int direction = 0; // 体の向き (前向き: 25, 後ろ向き: -25)
	bool isMarked = false; // マークされているか

private:
	// 指定された位置へ動く処理
	void MoveTo();

	FVector targetLocation = FVector(0, 0, 0); // 動くターゲット位置(一時保存)
};
