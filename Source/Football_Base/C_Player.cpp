// Fill out your copyright notice in the Description page of Project Settings.


#include "C_Player.h"
#include <Kismet/GameplayStatics.h>
#include <Kismet/KismetMathLibrary.h>
#include "C_Player_Anim_Instance.h"
#include "C_Common.h"
#include "My_Game_Instance.h"
#include "GameFramework/CharacterMovementComponent.h" // キャラクター移動コンポーネントをインクルード

// Sets default values
AC_Player::AC_Player()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// *** マテリアル取得(ノーマルプレイヤー) ***
	static ConstructorHelpers::FObjectFinder<UMaterial> _normalPlayerMaterialAsset(TEXT("/Game/Models/Player_Origin/Material/M_Red_Player.M_Red_Player"));
	if (_normalPlayerMaterialAsset.Succeeded())
	{
		normalPlayerMaterial = _normalPlayerMaterialAsset.Object;
	}
	static ConstructorHelpers::FObjectFinder<UMaterial> _targetmanPlayerMaterialAsset(TEXT("/Game/Models/Player_Origin/Material/M_Target_Man.M_Target_Man"));
	if (_targetmanPlayerMaterialAsset.Succeeded())
	{
		targetmanPlayerMaterial = _targetmanPlayerMaterialAsset.Object;
	}
	static ConstructorHelpers::FObjectFinder<UMaterial> _runnerPlayerMaterialAsset(TEXT("/Game/Models/Player_Origin/Material/M_Runner.M_Runner"));
	if (_runnerPlayerMaterialAsset.Succeeded())
	{
		runnerPlayerMaterial = _runnerPlayerMaterialAsset.Object;
	}
	// ***

	// *** Widgetアイコン取得 ***
	// -- ランナー --
	ConstructorHelpers::FClassFinder<UUserWidget> _runnerWidgetFinder(TEXT("/Game/UI/Player_Type/WBP_Runner"));
	if (_runnerWidgetFinder.Class) {
		runnerWidgetIcon = _runnerWidgetFinder.Class;
	}
	// -- ターゲットマン --
	ConstructorHelpers::FClassFinder<UUserWidget> _targetmanWidgetFinder(TEXT("/Game/UI/Player_Type/WBP_Targetman"));
	if (_targetmanWidgetFinder.Class) {
		targetmanWidgetIcon = _targetmanWidgetFinder.Class;
	}
	// ***
}

// Called when the game starts or when spawned
void AC_Player::BeginPlay()
{
	Super::BeginPlay();
	
	// ** メッシュ取得 **
	UActorComponent* _actorMesh = GetComponentByClass(USkeletalMeshComponent::StaticClass());
	myMesh = Cast<USkeletalMeshComponent>(_actorMesh); // キャスト
	// **
	
	//myMesh->SetVisibility(false); // メッシュ非表示

	// *** ボール取得 ***
	AActor* _b = UGameplayStatics::GetActorOfClass(this, AC_Soccer_Ball::StaticClass());
	ball = Cast<AC_Soccer_Ball>(_b); // キャスト
	// ***

	// *** アニメーションモンタージュ取得 ***
	shortPassAnim = LoadObject<UAnimMontage>(NULL, TEXT("/Game/Animations/Montage/Player_Origin/AM_ShortPass.AM_ShortPass"), NULL, LOAD_None, NULL);
	longPassAnim = LoadObject<UAnimMontage>(NULL, TEXT("/Game/Animations/Montage/Player_Origin/AM_Long_Pass.AM_Long_Pass"), NULL, LOAD_None, NULL);
	trapAnim = LoadObject<UAnimMontage>(NULL, TEXT("/Game/Animations/Montage/Player_Origin/AM_Trap.AM_Trap"), NULL, LOAD_None, NULL);
	regateAnim = LoadObject<UAnimMontage>(NULL, TEXT("/Game/Animations/Montage/Player_Origin/AM_Regate.AM_Regate"), NULL, LOAD_None, NULL);
	tackleAnim = LoadObject<UAnimMontage>(NULL, TEXT("/Game/Animations/Montage/Player_Origin/AM_Tackle.AM_Tackle"), NULL, LOAD_None, NULL);
	sad1Anim = LoadObject<UAnimMontage>(NULL, TEXT("/Game/Animations/Montage/Player_Origin/AM_Sad_1.AM_Sad_1"), NULL, LOAD_None, NULL);
	sad2Anim = LoadObject<UAnimMontage>(NULL, TEXT("/Game/Animations/Montage/Player_Origin/AM_Sad_2.AM_Sad_2"), NULL, LOAD_None, NULL);
	cheer1Anim = LoadObject<UAnimMontage>(NULL, TEXT("/Game/Animations/Montage/Player_Origin/AM_Cheer_1.AM_Cheer_1"), NULL, LOAD_None, NULL);
	cheer2Anim = LoadObject<UAnimMontage>(NULL, TEXT("/Game/Animations/Montage/Player_Origin/AM_Cheer_2.AM_Cheer_2"), NULL, LOAD_None, NULL);
	freeAppealAnim = LoadObject<UAnimMontage>(NULL, TEXT("/Game/Animations/Montage/Player_Origin/AM_Free.AM_Free"), NULL, LOAD_None, NULL);
	targetmanAppealAnim = LoadObject<UAnimMontage>(NULL, TEXT("/Game/Animations/Montage/Player_Origin/AM_Free_Target.AM_Free_Target"), NULL, LOAD_None, NULL);
	postmanAppealAnim = LoadObject<UAnimMontage>(NULL, TEXT("/Game/Animations/Montage/Player_Origin/AM_Free_Post.AM_Free_Post"), NULL, LOAD_None, NULL);
	runnerAppealAnim = LoadObject<UAnimMontage>(NULL, TEXT("/Game/Animations/Montage/Player_Origin/AM_Free_Runner.AM_Free_Runner"), NULL, LOAD_None, NULL);
	poketmanAppealAnim = LoadObject<UAnimMontage>(NULL, TEXT("/Game/Animations/Montage/Player_Origin/AM_Free_Poket.AM_Free_Poket"), NULL, LOAD_None, NULL);
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

	// *** Widgetコンポーネント取得 ***
	myWidget = FindComponentByClass<UWidgetComponent>();
	// ***

	// *** 全てのタイルを取得 ***
	TArray<AActor*> _actorTiles = {}; // タイルアクター配列
	UGameplayStatics::GetAllActorsOfClass(this, AC_Tile::StaticClass(), _actorTiles); // クラスで探す

	for (AActor* _a : _actorTiles) {
		AC_Tile* _tile = Cast<AC_Tile>(_a); // キャスト
		
		tiles.Add(_tile);
	}
	// ***
	
	// *** タイルNoセット ***
	tileNo = GetTileNoFromLocation();
	// ***

	// *** ポジションセット ***
	SetPosition();
	// ***
}

// Called every frame
void AC_Player::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	// *** 移動処理 ***
	if (isMoving) {
		Move(DeltaTime);

		return;
	}
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


	// *** アニメーションBP 調整 ***
	if (isBallHolder) { // 〇ボールホルダー時
		// -- ボールキープ --
		if (isBallKeeping == false) {
			if (ball) {
				if (ball->isMoving == false && isTrap == false) {
					BallKeeping();

					isBallKeeping = true; // フラグON ←これは中に
				}
			}
		}
	}
	else { // ●ボールホルダーでない
		// -- スタンディング --
		if (isBallKeeping) {
			if (playerAnimInstance)
			{
				playerAnimInstance->isKeep = false;
			}
			isBallKeeping = false; // フラグOFF
		}
	}

	// -- ボールキープ時間計測 --
	if (isBallKeeping) {
		ballKeepingCount += DeltaTime;
	}
	else {
		ballKeepingCount = 0.0f; // リセット
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

// セットポジション (位置からポジション取得)
void AC_Player::SetPosition() 
{	
	// -- タイルNo取得 --
	int _tileNo = GetTileNoFromLocation();

	// -- ポジションセット --
	if (_tileNo == 0) {
		position = C_Common::GK_POSITION;
	}
	else if (_tileNo == 1 || _tileNo == 30) {
		position = C_Common::LSB_POSITION;
	}
	else if (_tileNo == 2 || _tileNo == 29) {
		position = C_Common::LCB_POSITION;
	}
	else if (_tileNo == 3 || _tileNo == 28) {
		position = C_Common::CB_POSITION;
	}
	else if (_tileNo == 4 || _tileNo == 27) {
		position = C_Common::RCB_POSITION;
	}
	else if (_tileNo == 5 || _tileNo == 26) {
		position = C_Common::RSB_POSITION;
	}
	else if (_tileNo == 6 || _tileNo == 25) {
		position = C_Common::LH_POSITION;
	}
	else if (_tileNo == 7 || _tileNo == 24) {
		position = C_Common::LIH_POSITION;
	}
	else if (_tileNo == 8 || _tileNo == 23) {
		position = C_Common::CH_POSITION;
	}
	else if (_tileNo == 9 || _tileNo == 22) {
		position = C_Common::RIH_POSITION;
	}
	else if (_tileNo == 10 || _tileNo == 21) {
		position = C_Common::RH_POSITION;
	}
	else if (_tileNo == 11 || _tileNo == 20) {
		position = C_Common::LWG_POSITION;
	}
	else if (_tileNo == 12 || _tileNo == 19) {
		position = C_Common::LST_POSITION;
	}
	else if (_tileNo == 13 || _tileNo == 18) {
		position = C_Common::CF_POSITION;
	}
	else if (_tileNo == 14 || _tileNo == 17) {
		position = C_Common::RST_POSITION;
	}
	else if (_tileNo == 15 || _tileNo == 16) {
		position = C_Common::RWG_POSITION;
	}
	else {
		UKismetSystemLibrary::PrintString(this, "NO POSITION PLAYER EXIST", true, true, FColor::Red, 10.0f, TEXT("None"));
	}
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

// スペースパス (ショートパス)
void AC_Player::SpacePass(FVector toLocation)
{
	// パス方向へ回転
	FRotator _rotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), toLocation);
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
		ball->SetShortPass(toLocation);
	}
}

// ロングキック
void AC_Player::LongKick(FVector toLocation)
{
	// パス方向へ回転
	FRotator _rotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), toLocation);
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
		ball->SetLongPass(toLocation);
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

// シュート
void AC_Player::Shoot()
{
	// ゴール位置取得
	FVector _goalLocation = FVector(0, 0, 0);
	if (ActorHasTag(FName("HOME"))) {
		_goalLocation = AWAY_GOAL_LOCATION; // AWAY
	}
	else {
		_goalLocation = HOME_GOAL_LOCATION; // HOME
	}

	// ゴール方向へ回転
	FRotator _rotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), _goalLocation);
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
		ball->SetLongPass(_goalLocation);
	}
}

// ボール保持
void AC_Player::BallKeeping()
{
	// アニメーション
	if (playerAnimInstance)
	{
		playerAnimInstance->isKeep = true;
	}
	
	// 前方向へ回転 (*必ず前を向く)
	FRotator _rotation = FRotator::ZeroRotator;
	if (ActorHasTag("HOME")) {
		_rotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), GetActorLocation() + FVector(1, 0, 0));
	}
	else {
		_rotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), GetActorLocation() + FVector(-1, 0, 0));
	}
	FQuat _q = _rotation.Quaternion(); // 変換
	_q.X = 0; // *Z軸のみ回転させる
	_q.Y = 0; // *Z軸のみ回転させる
	SetActorRotation(_q);

	// ボールをプレイヤーの前へ移動
	FVector _playerFront = GetActorLocation() + (GetActorForwardVector() * 30.0f); // プレイヤーの前の位置
	_playerFront.Z = C_Common::BALL_BASE_LOCATION_Z; // ** Zの位置を固定 **
	if (ball) {
		ball->SetActorLocation(_playerFront);
	}
}

// 走る (セット)
void AC_Player::RunTo(FVector toLocation)
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
		// 第1引数はブレンドアウト時間 (任意)、第2引数は停止するモンタージュ (nullptrで全て対象)
		playerAnimInstance->Montage_Stop(0.25f, nullptr); // アニメーションモンタージュ停止 (0.25秒かけてブレンドアウト)
		playerAnimInstance->isRun = true; // ABP開始
	}

	// ONタイルプレイヤー削除
	for (AC_Tile* _tile : tiles) {
		if (_tile->tileNo == tileNo) {
			
			if (_tile->onPlayerNum > 0) _tile->onPlayerNum--; // 減らす

			break;
		}
	}
}

// ドリブル (相手を抜く)
void AC_Player::RegateDrrible()
{
	// 移動位置取得
	FVector _ToLocation = GetActorLocation();
	if (ActorHasTag("HOME")) {
		_ToLocation.X += C_Common::TILE_SIZE;
	}
	else {
		_ToLocation.X -= C_Common::TILE_SIZE;
	}
	_ToLocation.Z = C_Common::PLAYER_BASE_LOCATION_Z; // ** Zの位置を固定 **
	targetLocation = _ToLocation; // 目標位置セット
	fromLocation = GetActorLocation(); // 動く前の位置セット
	isMoving = true; // 移動開始
	isDrribling = true; // ドリブル開始

	// アニメーション時間取得
	if (regateAnim) moveTotalTime = regateAnim->GetPlayLength(); // 移動時間 (指定)

	// プレイヤーアニメーション
	if (regateAnim && playerAnimInstance)
		playerAnimInstance->Montage_Play(regateAnim);

	// ONタイルプレイヤー削除
	for (AC_Tile* _tile : tiles) {
		if (_tile->tileNo == tileNo) {

			if (_tile->onPlayerNum > 0) _tile->onPlayerNum--; // 減らす

			break;
		}
	}

	// ボールキープ時間リセット
	ballKeepingCount = 0.0f;
}

// タックル (Root Motion)
void AC_Player::Tackle()
{
	// プレイヤーアニメーション
	if (tackleAnim && playerAnimInstance)
		playerAnimInstance->Montage_Play(tackleAnim);
	
	// ディフェンス姿勢解除
	RemoveDefensiveStance();
}

// ディフェンス姿勢
void AC_Player::SetDefensiveStance()
{
	// プレイヤーアニメーション
	if (playerAnimInstance) {
		playerAnimInstance->isDefensiveStance = true;
	}
}

// 解除ディフェンス姿勢
void AC_Player::RemoveDefensiveStance()
{
	// プレイヤーアニメーション
	if (playerAnimInstance) {
		playerAnimInstance->isDefensiveStance = false;
	}
}

// 移動処理
void AC_Player::Move(float dTime) {
	
	// *** 移動処理 ***
	if (moveTotalTime == 0.0f) moveTotalTime = 1.0f; // 移動時間指定
	movingCount += dTime; // 経過時間
	float _alpha = FMath::Clamp(movingCount / moveTotalTime, 0.0f, 1.0f); // 0から1にクランプ

	FVector _newPlayerLocation = FMath::Lerp(fromLocation, targetLocation, _alpha);
	SetActorLocation(_newPlayerLocation); // 移動
	// ***
	

	// *** ドリブル時処理 ***
	if (isDrribling && ball) {
		//FVector _ballLocation = _newPlayerLocation + (GetActorForwardVector() * 20.0f); // プレイヤーの前の位置;
		ball->SetActorLocation(_newPlayerLocation);
	}
	// ***


	// ** 移動終了処理 **
	if (_alpha >= 1.0f)
	{
		// -- リセットフラグ --
		targetLocation = FVector(0, 0, 0); // ターゲット位置リセット
		moveTotalTime = 0.0f; // 移動時間(指定)リセット
		movingCount = 0.0f; // 移動カウントリセット
		isMoving = false; // 移動終了
		if (playerAnimInstance) playerAnimInstance->isRun = false; // アニメーション
		
		// -- タイルNoセット --
		tileNo = GetTileNoFromLocation(); // タイルNo更新
		
		// -- ONタイルプレイヤー追加 --
		for (AC_Tile* _tile : tiles) {
			if (_tile->tileNo == tileNo) {
				_tile->onPlayerNum++; // 増やす

				break;
			}
		}
		
		// -- 前方を向く --
		LookForward();
		
		// -- ドリブル時 --
		isDrribling = false; // *ドリブル終了
		if (playerAnimInstance) playerAnimInstance->isDrrible = false; // アニメーション終了
		
		// -- コールバック処理 (delegate) --
		if (completeMoving.IsBound()) completeMoving.Execute(); // 実行処理

		return;
	}
	// ***
}

// プレイヤー位置のタイルＮｏ取得
int AC_Player::GetTileNoFromLocation()
{
	int _tileNo = 0;

	// プレイヤー位置
	FVector _location = GetActorLocation();
	double _x = _location.X;
	double _y = _location.Y;
	// タイルNo
	int _tileNoY = 0; // タイルNo y (横から何個目か)
	int _tileNoX = 0; // タイルNo x (下から何列目か)

	// -- タイルNo y(横から何個目か)を求める --
	// -の時の処理
	bool _isPositiveY = true; // xが正の整数か
	if (_y < 0) {
		_y = abs(_y); // 絶対値に変換
		_isPositiveY = false; // フラグ変更
	}
	if (_y < (C_Common::TILE_SIZE / 2))	_tileNoY = 3;
	else if (_y < (C_Common::TILE_SIZE / 2) + C_Common::TILE_SIZE) _tileNoY = (_isPositiveY) ? 4 : 2;
	else if (_y < (C_Common::TILE_SIZE / 2) + C_Common::TILE_SIZE * 2) _tileNoY = (_isPositiveY) ? 5 : 1;
	else return _tileNo; // 横が2.5タイル以上 (タイルNo.0)

	// -- タイルNo x(下から何列目か)を求める --
	// -の時の処理
	bool _isPositiveX = true; // xが正の整数か
	if (_x < 0) {
		_x = abs(_x); // 絶対値に変換
		_isPositiveX = false; // フラグ変更
	}
	if (_x < C_Common::TILE_SIZE)	_tileNoX = (_isPositiveX) ? 4 : 3;
	else if (_x < C_Common::TILE_SIZE * 2) _tileNoX = (_isPositiveX) ? 5 : 2;
	else if (_x < C_Common::TILE_SIZE * 3)_tileNoX = (_isPositiveX) ? 6 : 1;
	else return _tileNo; // 縦が3タイル以上 (タイルNo.0)

	// -- タイルNo取得 --
	// ( (タイル縦(x) - 1個) * フィールドの横のタイル個数 ) + タイル横(y)
	_tileNo = ((_tileNoX - 1) * C_Common::TILE_NUM_Y) + _tileNoY;


	return _tileNo;
}

// スポーンプレイヤーのマテリアルセット
void AC_Player::SetSpwanPlayerMaterial(int typeNo)
{
	if (myMesh == nullptr) return;

	if (normalPlayerMaterial && typeNo == 0)myMesh->SetMaterial(0, normalPlayerMaterial); // ノーマル
	if (runnerPlayerMaterial && typeNo == C_Common::RUNNER_TYPE_NO)myMesh->SetMaterial(0, runnerPlayerMaterial); // ランナー
	if (targetmanPlayerMaterial && typeNo == C_Common::TARGET_MAN_TYPE_NO)myMesh->SetMaterial(0, targetmanPlayerMaterial); // ターゲットマン
}

// プレイヤータイプアイコンセット
void AC_Player::SetPlayerTypeIcon(int typeNo)
{
	if (myWidget == nullptr) return;
	if (runnerWidgetIcon == nullptr) return;
	if (targetmanWidgetIcon == nullptr) return;

	if (typeNo == C_Common::RUNNER_TYPE_NO) myWidget->SetWidgetClass(runnerWidgetIcon);
	else if (typeNo == C_Common::TARGET_MAN_TYPE_NO) myWidget->SetWidgetClass(targetmanWidgetIcon);
}

// 前方を向く
void AC_Player::LookForward()
{
	FVector _front = FVector::ZeroVector;
	if (ActorHasTag("HOME")) {
		_front = FVector(1, 0, 0);
	}
	else {
		_front = FVector(-1, 0, 0);
	}
	FRotator _rotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), GetActorLocation() + _front);
	FQuat _q = _rotation.Quaternion(); // 変換
	_q.X = 0; // *Z軸のみ回転させる
	_q.Y = 0; // *Z軸のみ回転させる
	SetActorRotation(_q);
}

// ドリブル (前進)
void AC_Player::Drrible()
{
	// 移動位置取得
	FVector _ToLocation = GetActorLocation();
	if (ActorHasTag("HOME")) {
		_ToLocation.X += C_Common::TILE_SIZE;
	}
	else {
		_ToLocation.X -= C_Common::TILE_SIZE;
	}
	_ToLocation.Z = C_Common::PLAYER_BASE_LOCATION_Z; // ** Zの位置を固定 **
	targetLocation = _ToLocation; // 目標位置セット
	fromLocation = GetActorLocation(); // 動く前の位置セット
	isMoving = true; // 移動開始
	isDrribling = true; // ドリブル開始

	// プレイヤーアニメーション
	if (playerAnimInstance) {
		playerAnimInstance->isDrrible = true;
		playerAnimInstance->isKeep = false; // *キープアニメーションOFF (これしないとアニメーション切り替わらない)
	}
	isBallKeeping = false; // *キープOFF

	// ONタイルプレイヤー削除
	for (AC_Tile* _tile : tiles) {
		if (_tile->tileNo == tileNo) {

			if (_tile->onPlayerNum > 0) _tile->onPlayerNum--; // 減らす

			break;
		}
	}

	// ボールキープ時間リセット
	ballKeepingCount = 0.0f;
}

// 喜びアニメーション
void AC_Player::CheerMotion()
{
	bool _b = FMath::RandBool(); // ランダム

	if (_b) {
		if (cheer1Anim && playerAnimInstance) playerAnimInstance->Montage_Play(cheer1Anim);
	}
	else {
		if (cheer2Anim && playerAnimInstance) playerAnimInstance->Montage_Play(cheer2Anim);
	}
}

// 悲しみアニメーション
void AC_Player::SadMotion()
{
	bool _b = FMath::RandBool(); // ランダム

	if (_b) {
		if (sad1Anim && playerAnimInstance) playerAnimInstance->Montage_Play(sad1Anim);
	}
	else {
		if (sad2Anim && playerAnimInstance) playerAnimInstance->Montage_Play(sad2Anim);
	}
}

// フリーアピール
void AC_Player::FreeAppeal(AC_Player* ballHolder)
{
	// ボールホルダー方向へ回転
	FRotator _rotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), ballHolder->GetActorLocation());
	FQuat _q = _rotation.Quaternion(); // 変換
	_q.X = 0; // *Z軸のみ回転させる
	_q.Y = 0; // *Z軸のみ回転させる
	SetActorRotation(_q);

	if (freeAppealAnim && playerAnimInstance) playerAnimInstance->Montage_Play(freeAppealAnim);
}

// ターゲットマンアピール
void AC_Player::TargetmanAppeal(AC_Player* ballHolder)
{
	// ボールホルダー方向へ回転
	FRotator _rotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), ballHolder->GetActorLocation());
	FQuat _q = _rotation.Quaternion(); // 変換
	_q.X = 0; // *Z軸のみ回転させる
	_q.Y = 0; // *Z軸のみ回転させる
	SetActorRotation(_q);

	if (targetmanAppealAnim && playerAnimInstance) playerAnimInstance->Montage_Play(targetmanAppealAnim);
}

// ポストマンアピール
void AC_Player::PostmanAppeal(AC_Player* ballHolder)
{
	// ボールホルダー方向へ回転
	FRotator _rotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), ballHolder->GetActorLocation());
	FQuat _q = _rotation.Quaternion(); // 変換
	_q.X = 0; // *Z軸のみ回転させる
	_q.Y = 0; // *Z軸のみ回転させる
	SetActorRotation(_q);

	if (postmanAppealAnim && playerAnimInstance) playerAnimInstance->Montage_Play(postmanAppealAnim);
}

// ランナーアピール
void AC_Player::RunnerAppeal()
{
	// ボールホルダー方向へ回転
	FRotator _rotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), GetActorLocation() + FVector(100.0f, 0, 0));
	FQuat _q = _rotation.Quaternion(); // 変換
	_q.X = 0; // *Z軸のみ回転させる
	_q.Y = 0; // *Z軸のみ回転させる
	SetActorRotation(_q);

	if (runnerAppealAnim && playerAnimInstance) playerAnimInstance->Montage_Play(runnerAppealAnim);
}

// ポケットマンアピール
void AC_Player::PoketmanAppeal(AC_Player* ballHolder)
{
	// ボールホルダー方向へ回転
	FRotator _rotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), ballHolder->GetActorLocation());
	FQuat _q = _rotation.Quaternion(); // 変換
	_q.X = 0; // *Z軸のみ回転させる
	_q.Y = 0; // *Z軸のみ回転させる
	SetActorRotation(_q);

	if (poketmanAppealAnim && playerAnimInstance) playerAnimInstance->Montage_Play(poketmanAppealAnim);
}

// アニメーション停止 (アイドル状態へ)
void AC_Player::StopAnim() 
{
	if (playerAnimInstance == nullptr) return;

	// -- モンタージュ --
	// 第1引数はブレンドアウト時間 (任意)、第2引数は停止するモンタージュ (nullptrで全て対象)
	playerAnimInstance->Montage_Stop(0.25f, nullptr); // アニメーションモンタージュ停止 (0.25秒かけてブレンドアウト)
	
	// -- ルートモーション --
	UCharacterMovementComponent* _movementComponent = GetCharacterMovement(); // コンポーネント
	if (_movementComponent) {
		_movementComponent->StopMovementImmediately();
	}

	// -- Anim BluePrint --
	playerAnimInstance->isKeep = false;
	playerAnimInstance->isRun = false;
	playerAnimInstance->isDrrible = false;
	playerAnimInstance->isDefensiveStance = false;
}
