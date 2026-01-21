// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "C_Soccer_Ball.generated.h"

UCLASS()
class FOOTBALL_BASE_API AC_Soccer_Ball : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AC_Soccer_Ball();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// ショートパス処理開始
	// < 引数：tLocation(目標位置) >
	void SetShortPass(FVector tLocation);
	// ロングパス処理開始
	// < 引数：tLocation(目標位置) >
	void SetLongPass(FVector tLocation);

	bool isMoving = false; // 移動中か (*** フェーズ中フラグ ***)
	bool isLongBall = false; // ロングボール
	

private:
	// 移動処理
	void Move(float dTime);
	// - 試合開始ハンドル -
	void HandleMatchStart();
	// - 試合終了ハンドル -
	void HandleMatchEnd();

private:
	UCurveFloat* longBallHeightCurve = nullptr; // カーブアセット (ロングボール)
	FVector targetLocation = FVector(0, 0, 0); // 動くターゲット位置(一時保存)
	FVector fromLocation = FVector(0, 0, 0); // 動く前の位置(一時保存)
	float movingCount = 0.0f; // 移動経過時間
	float moveTotalTime = 0.0f; // 移動時間(指定)
	FVector initialLocation = FVector::ZeroVector; // 初期位置

};
