// Fill out your copyright notice in the Description page of Project Settings.


#include "C_Player.h"
#include <Kismet/GameplayStatics.h>
#include <Kismet/KismetMathLibrary.h>
#include "C_Player_Anim_Instance.h"
#include "C_Common.h"

// Sets default values
AC_Player::AC_Player()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AC_Player::BeginPlay()
{
	Super::BeginPlay();
	
	// ** メッシュ取得 **
	UActorComponent* _actorMesh = GetComponentByClass(USkeletalMeshComponent::StaticClass());
	myMesh = Cast<USkeletalMeshComponent>(_actorMesh); // キャスト
	// **
	
	myMesh->SetVisibility(false); // メッシュ非表示

	// *** ボール取得 ***
	AActor* _b = UGameplayStatics::GetActorOfClass(this, AC_Soccer_Ball::StaticClass());
	ball = Cast<AC_Soccer_Ball>(_b); // キャスト
	// ***

	// *** アニメーションモンタージュ取得 ***
	shortPassAnim = LoadObject<UAnimMontage>(NULL, TEXT("/Game/Animations/Montage/Player_Origin/AM_ShortPass.AM_ShortPass"), NULL, LOAD_None, NULL);
	longPassAnim = LoadObject<UAnimMontage>(NULL, TEXT("/Game/Animations/Montage/Player_Origin/AM_Long_Pass.AM_Long_Pass"), NULL, LOAD_None, NULL);
	trapAnim = LoadObject<UAnimMontage>(NULL, TEXT("/Game/Animations/Montage/Player_Origin/AM_Trap.AM_Trap"), NULL, LOAD_None, NULL);
	// ***

	// *** アニメーションインスタンス取得 ***
	if (myMesh)
	{
		// 2. アニメーションインスタンスを取得
		UAnimInstance* _animInstance = myMesh->GetAnimInstance();

		// 3. アニメーションインスタンスがUMyAnimInstance型であることを確認
		if (_animInstance)
		{
			playerAnimInstance = Cast<UC_Player_Anim_Instance>(_animInstance);
		}
	}
	// ***
}

// Called every frame
void AC_Player::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// *** 移動処理 ***
	if (isMoving) Move();
	// ***

	// *** トラップアニメーション ***
	if (isTrap) {
		float _dis = 0; // ボールとの距離
		if (ball) {
			_dis = GetDistanceTo(ball);
		}
		
		if (_dis < 400.0f) { // この距離以内にきたらで実行
			isTrap = false;

			// プレイヤーアニメーション
			UAnimInstance* _animInstance = myMesh->GetAnimInstance(); // アニメーションインスタンス
			if (trapAnim)
				_animInstance->Montage_Play(trapAnim);
		}
	}
	// ***

	// *** ボールホルダー時アニメーション切り替え ***
	if (isBallHolder) { // 〇ボールホルダー時
		// -- ボールキープ --
		if (isStanding) {
			if (ball) {
				if (ball->isMoving == false && isTrap == false) {
					BallKeeping();

					isStanding = false;
				}
			}
		}
	}
	else { // ●ボールホルダーでない
		// -- スタンディング --
		if (isStanding == false) {
			if (playerAnimInstance)
			{
				playerAnimInstance->isKeep = false;
			}

			isStanding = true;
		}
	}
	// ***
}

// Called to bind functionality to input
void AC_Player::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

// メッシュを表示する
void AC_Player::DisplayMesh()
{
	myMesh->SetVisibility(true);
}

// ショートパス
void AC_Player::ShotPass(AC_Player* targetPlayer)
{
	// パス方向へ回転
	FRotator _rotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), targetPlayer->GetActorLocation());
	FQuat _q = _rotation.Quaternion(); // 変換
	_q.X = 0; // *Z軸のみ回転させる
	_q.Y = 0; // *Z軸のみ回転させる
	SetActorRotation(_q);
	

	// プレイヤーアニメーション
	UAnimInstance* _animInstance = myMesh->GetAnimInstance(); // アニメーションインスタンス
	if (shortPassAnim)
	_animInstance->Montage_Play(shortPassAnim);

	// ボール移動
	if (ball) {
		FVector _targetLocation = targetPlayer->GetActorLocation();
		ball->SetShortPass(_targetLocation);
	}
}

// ロングパス
void AC_Player::LongPass(AC_Player* targetPlayer)
{
	// パス方向へ回転
	FRotator _rotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), targetPlayer->GetActorLocation());
	FQuat _q = _rotation.Quaternion(); // 変換
	_q.X = 0; // *Z軸のみ回転させる
	_q.Y = 0; // *Z軸のみ回転させる
	SetActorRotation(_q);


	// プレイヤーアニメーション
	UAnimInstance* _animInstance = myMesh->GetAnimInstance(); // アニメーションインスタンス
	if (longPassAnim)
		_animInstance->Montage_Play(longPassAnim);

	// ボール移動
	if (ball) {
		FVector _targetLocation = targetPlayer->GetActorLocation();
		ball->SetLongPass(_targetLocation);
	}
}

// トラップ
void AC_Player::Trap(AC_Player* fromPlayer)
{
	// パス先方向へ回転
	FRotator _rotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), fromPlayer->GetActorLocation());
	FQuat _q = _rotation.Quaternion(); // 変換
	_q.X = 0; // *Z軸のみ回転させる
	_q.Y = 0; // *Z軸のみ回転させる
	SetActorRotation(_q);

	// アニメーション実行フラグ
	isTrap = true;
}

// ボール保持
void AC_Player::BallKeeping()
{
	// アニメーション
	if (playerAnimInstance)
	{
		playerAnimInstance->isKeep = true;
	}
	
	// ボールをプレイヤーの前へ移動
	FVector _playerFront = GetActorLocation() + (GetActorForwardVector() * 30.0f); // プレイヤーの前の位置
	_playerFront.Z = C_Common::BALL_BASE_LOCATION_Z; // ** Zの位置を固定 **
	if (ball) {
		ball->SetActorLocation(_playerFront);
	}
}

// 移動 (セット)
void AC_Player::MoveTo(FVector toLocation)
{
	// ターゲット方向へ回転
	FRotator _rotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), toLocation);
	FQuat _q = _rotation.Quaternion(); // 変換
	_q.X = 0; // *Z軸のみ回転させる
	_q.Y = 0; // *Z軸のみ回転させる
	SetActorRotation(_q);

	// 移動処理
	toLocation.Z = C_Common::PLAYER_BASE_LOCATION_Z; // ** Zの位置を固定 **
	targetLocation = toLocation; // 目標位置セット
	fromLocation = GetActorLocation(); // 動く前の位置セット
	isMoving = true; // 移動開始

	// アニメーション
	if (playerAnimInstance)
	{
		playerAnimInstance->isRun = true;
	}
}

// 移動処理
void AC_Player::Move() {
	// 引数セット
	FVector _currentLocation = GetActorLocation(); // 現在の位置を取得
	float time = GetWorld()->GetDeltaSeconds(); // 前のフレームからの経過時間を取得

	// ** 移動終了処理 **
	// 現在の距離を正規化
	float _targetDistance = FVector2D::Distance(FVector2D(fromLocation), FVector2D(targetLocation)); // 目的地までの距離
	float _currentDistance = FVector2D::Distance(FVector2D(_currentLocation), FVector2D(targetLocation)); // 現在の位置から目的地までの距離
	float _normal = FMath::Abs((_currentDistance / _targetDistance) - 1.0f);
	if (_normal > 0.93f) {
		SetActorLocation(targetLocation); // 位置をターゲット位置へ
		targetLocation = FVector(0, 0, 0); // ターゲット位置リセット
		isMoving = false; // 移動終了
		if (playerAnimInstance) playerAnimInstance->isRun = false; // アニメーション

		return;
	}

	// ターゲットまでの位置取得
	FVector _newLocation = UKismetMathLibrary::VInterpTo(_currentLocation, targetLocation, time, C_Common::PIECE_SPEED * 0.3);

	// 自身に新しい位置を適用
	SetActorLocation(_newLocation);
}

