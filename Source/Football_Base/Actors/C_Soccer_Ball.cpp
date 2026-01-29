// Fill out your copyright notice in the Description page of Project Settings.


#include "C_Soccer_Ball.h"
#include <Kismet/KismetMathLibrary.h>
#include <Kismet/KismetSystemLibrary.h>
#include <Kismet/GameplayStatics.h>
#include <Football_Base/Systems/GameStateManager.h>

// Sets default values
AC_Soccer_Ball::AC_Soccer_Ball()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AC_Soccer_Ball::BeginPlay()
{
	Super::BeginPlay();
    
    // *** カーブアセット取得 ***
    longBallHeightCurve = LoadObject<UCurveFloat>(NULL, TEXT("/Game/Animations/Curve/C_LongBall_Height.C_LongBall_Height"), NULL, LOAD_None, NULL);
    // ***

    // StateSubsystem を取得
    UGameStateManager* state = GetGameInstance()->GetSubsystem<UGameStateManager>();

    state->OnMatchStart.AddUObject(this, &AC_Soccer_Ball::HandleMatchStart);
	state->OnMatchEnd.AddUObject(this, &AC_Soccer_Ball::HandleMatchEnd);
}

// Called every frame
void AC_Soccer_Ball::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    if (isMoving) Move(DeltaTime);
}

// Called to bind functionality to input
void AC_Soccer_Ball::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

// 指定された位置へ動く
void AC_Soccer_Ball::Move(float dTime)
{
    // *** 移動処理 ***
    if (moveTotalTime == 0.0f) moveTotalTime = 0.5f; // 移動時間指定
    movingCount += dTime; // 経過時間
    float _alpha = FMath::Clamp(movingCount / moveTotalTime, 0.0f, 1.0f); // 0から1にクランプ
    FVector _newLocation = FMath::Lerp(fromLocation, targetLocation, _alpha);

    // -- ロングボール処理 --
    if (isLongBall) {
        float _longBallHeight = longBallHeightCurve->GetFloatValue(_alpha); // ボールの高さ
        _newLocation.Z += _longBallHeight * 350.0f; // Z位置へ
    }
    // --

    SetActorLocation(_newLocation); // 移動
    // ***
    

    // ** 移動終了処理 **
    if (_alpha >= 1.0f) {
        targetLocation = FVector(0, 0, 0); // ターゲット位置リセット
        isMoving = false; // 移動終了
        isLongBall = false; // ロングボールOFF
        moveTotalTime = 0.0f; // 移動時間(指定)リセット
        movingCount = 0.0f; // 移動カウントリセット

        return;
    }
}

// - 試合開始ハンドル -
void AC_Soccer_Ball::HandleMatchStart()
{
    SetActorLocation(initialLocation); // 位置
}

// - 試合終了ハンドル -
void AC_Soccer_Ball::HandleMatchEnd()
{
    SetActorLocation(initialLocation); // 位置
}

// ショートパス処理
void AC_Soccer_Ball::SetShortPass(FVector tLocation)
{
    tLocation.Z = C_Common::BALL_BASE_LOCATION_Z; // ** Zの位置を固定 **
    targetLocation = tLocation; // 目標位置セット
    fromLocation = GetActorLocation(); // 動く前の位置セット
    isMoving = true; // 移動開始
}

// ロングパス処理
void AC_Soccer_Ball::SetLongPass(FVector tLocation)
{
    tLocation.Z = C_Common::BALL_BASE_LOCATION_Z; // ** Zの位置を固定 **
    targetLocation = tLocation; // 目標位置セット
    fromLocation = GetActorLocation(); // 動く前の位置
    isMoving = true; // 移動開始
    isLongBall = true; // ロングボールON
}

