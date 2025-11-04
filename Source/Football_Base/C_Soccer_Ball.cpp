// Fill out your copyright notice in the Description page of Project Settings.


#include "C_Soccer_Ball.h"
#include "C_Common.h"
#include <Kismet/KismetMathLibrary.h>
#include <Kismet/KismetSystemLibrary.h>

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
}

// Called every frame
void AC_Soccer_Ball::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    if (isMoving) MoveTo();
}

// Called to bind functionality to input
void AC_Soccer_Ball::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

// 指定された位置へ動く
void AC_Soccer_Ball::MoveTo()
{
    // 引数セット
    FVector currentLocation = GetActorLocation(); // 現在の位置を取得
    float time = GetWorld()->GetDeltaSeconds(); // 前のフレームからの経過時間を取得

    // ** 移動終了処理 **
    if (FVector::Distance(currentLocation, targetLocation) < 3.0) {
        SetActorLocation(targetLocation); // 位置をターゲット位置へ
        targetLocation = FVector(0, 0, 0); // ターゲット位置リセット
        isMoving = false; // 移動終了
        isLongBall = false; // ロングボールOFF

        return;
    }

    // ターゲットまでの位置取得
    FVector _newLocation = UKismetMathLibrary::VInterpTo(currentLocation, targetLocation, time, C_Common::PIECE_SPEED);

    // ** ロングボール処理 **
    if (isLongBall) {
        // 現在の距離を正規化
        float _targetDistance = FVector2D::Distance(FVector2D(fromLocation), FVector2D(targetLocation)); // 目的地までの距離
        float _currentDistance = FVector2D::Distance(FVector2D(currentLocation), FVector2D(targetLocation)); // 現在の位置から目的地までの距離
        float _normal = FMath::Abs( (_currentDistance / _targetDistance) - 1.0f );
        // カーブアセット値を取得
        float _longBallHeight = longBallHeightCurve->GetFloatValue(_normal);
        
        // 高さを追加
        _newLocation.Z += _longBallHeight * 25.0f;
    }
    // **
    
    // 自身に新しい位置を適用
    SetActorLocation(_newLocation);
}

// ショートパス処理
void AC_Soccer_Ball::SetShortPass(FVector tLocation)
{
    tLocation.Z = C_Common::BALL_BASE_LOCATION_Z; // ** Zの位置を固定 **
    targetLocation = tLocation; // 目標位置セット
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

