// Fill out your copyright notice in the Description page of Project Settings.


#include "C_Player.h"
#include <Kismet/GameplayStatics.h>
#include <Kismet/KismetMathLibrary.h>
#include "C_Player_Anim_Instance.h"
#include "C_Common.h"
#include "My_Game_Instance.h"

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

	// *** ポジションセット ***
	SetPosition();
	// ***

	// *** タイルを取得 ***
	TArray<AActor*> _actorTiles = {}; // タイルアクター配列
	UGameplayStatics::GetAllActorsOfClass(this, AC_Tile::StaticClass(), _actorTiles); // クラスで探す

	for (AActor* _a : _actorTiles) {
		AC_Tile* _tile = Cast<AC_Tile>(_a); // キャスト
		
		tiles.Add(_tile);
	}
	// ***
	
	// *** タイルNoセット ***
	if (position == C_Common::GK_POSITION) return; // *GKはレーンセットしない

	tileNo = GetTileNoFromLocation(GetActorLocation().X, GetActorLocation().Y);
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

// セットポジション (Tagから変数にポジションを設置する)
void AC_Player::SetPosition() {
	// ポジションセット
	if (ActorHasTag("GK")) {
		position = C_Common::GK_POSITION;
	}
	else if (ActorHasTag("LSB")) {
		position = C_Common::LSB_POSITION;
	}
	else if (ActorHasTag("LCB")) {
		position = C_Common::LCB_POSITION;
	}
	else if (ActorHasTag("CB")) {
		position = C_Common::CB_POSITION;
	}
	else if (ActorHasTag("RCB")) {
		position = C_Common::RCB_POSITION;
	}
	else if (ActorHasTag("RSB")) {
		position = C_Common::RSB_POSITION;
	}
	else if (ActorHasTag("LH")) {
		position = C_Common::LH_POSITION;
	}
	else if (ActorHasTag("LIH")) {
		position = C_Common::LIH_POSITION;
	}
	else if (ActorHasTag("CH")) {
		position = C_Common::CH_POSITION;
	}
	else if (ActorHasTag("RIH")) {
		position = C_Common::RIH_POSITION;
	}
	else if (ActorHasTag("RH")) {
		position = C_Common::RH_POSITION;
	}
	else if (ActorHasTag("LWG")) {
		position = C_Common::LWG_POSITION;
	}
	else if (ActorHasTag("LST")) {
		position = C_Common::LST_POSITION;
	}
	else if (ActorHasTag("CF")) {
		position = C_Common::CF_POSITION;
	}
	else if (ActorHasTag("RST")) {
		position = C_Common::RST_POSITION;
	}
	else if (ActorHasTag("RWG")) {
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
	FRotator _rotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), GetActorLocation() + FVector(1, 0, 0));
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
		targetLocation = FVector(0, 0, 0); // ターゲット位置リセット
		moveTotalTime = 0.0f; // 移動時間(指定)リセット
		movingCount = 0.0f; // 移動カウントリセット
		isMoving = false; // 移動終了
		if (playerAnimInstance) playerAnimInstance->isRun = false; // アニメーション
		tileNo = GetTileNoFromLocation(GetActorLocation().X, GetActorLocation().Y); // タイルNo更新
		// ONタイルプレイヤー追加
		for (AC_Tile* _tile : tiles) {
			if (_tile->tileNo == tileNo) {
				_tile->onPlayerNum++; // 増やす

				break;
			}
		}

		// -- ドリブル時 --
		isDrribling = false; // *ドリブル終了
		// ドリブルアニメーション
		if (playerAnimInstance)
			playerAnimInstance->isDrrible = false; // 終了
		// --

		return;
	}
	// ***
}

// 位置からタイルＮｏ取得
// ( unreal座標  x: 縦, y: 横 )
int AC_Player::GetTileNoFromLocation(float x, float y)
{
	// タイルNo
	int tileNoY = 0; // タイルNo y (横から何個目か)
	int tileNoX = 0; // タイルNo x (下から何列目か)

	// *** タイルNo y(横から何個目か)を求める ***
	// -の時の処理
	bool isPositiveY = true; // xが正の整数か
	if (y < 0) {
		y = abs(y); // 絶対値に変換
		isPositiveY = false; // フラグ変更
	}

	if (y < (C_Common::TILE_SIZE / 2) )	tileNoY = 3;
	else if (y < (C_Common::TILE_SIZE / 2) + C_Common::TILE_SIZE) tileNoY = (isPositiveY) ? 4 : 2;
	else tileNoY = (isPositiveY) ? 5 : 1;
	// ***

	// *** タイルNo x(下から何列目か)を求める ***
	// -の時の処理
	bool isPositiveX = true; // xが正の整数か
	if (x < 0) {
		x = abs(x); // 絶対値に変換
		isPositiveX = false; // フラグ変更
	}

	if (x < C_Common::TILE_SIZE)	tileNoX = (isPositiveX) ? 4 : 3;
	else if (x < C_Common::TILE_SIZE * 2) tileNoX = (isPositiveX) ? 5 : 2;
	else tileNoX = (isPositiveX) ? 6 : 1;
	// ***

	// *** タイルNo取得 ***
	// ( (タイル縦(x) - 1個) * フィールドの横のタイル個数 ) + タイル横(y)
	int _tileNo = ((tileNoX - 1) * C_Common::TILE_NUM_Y) + tileNoY;


	return _tileNo;
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

