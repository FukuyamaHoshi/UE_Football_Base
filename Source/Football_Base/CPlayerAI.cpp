// Fill out your copyright notice in the Description page of Project Settings.


#include "CPlayerAI.h"
#include "GameStateManager.h"
#include <Kismet/KismetSystemLibrary.h>
#include <Kismet/GameplayStatics.h>
#include "My_Game_Instance.h"

//UKismetSystemLibrary::PrintString(this, "level instance", true, true, FColor::Red, 10.0f, TEXT("None"));
void ACPlayerAI::BeginPlay()
{
	// StateSubsystem を取得
	UGameStateManager*  state = GetGameInstance()->GetSubsystem<UGameStateManager>();

	// C++ デリゲートを購読
	state->OnFreeHolder.AddUObject(this, &ACPlayerAI::HandleFreeHolder);
	state->OnDuelStart.AddUObject(this, &ACPlayerAI::HandleDuelStart);
	state->OnLineBreak.AddUObject(this, &ACPlayerAI::HandleLineBreak);
	state->OnCross.AddUObject(this, &ACPlayerAI::HandleCross);
	state->OnShoot.AddUObject(this, &ACPlayerAI::HandleShoot);
	state->OnGetBehind.AddUObject(this, &ACPlayerAI::HandleGetBehind);
	state->OnDribbleBreakThrough.AddUObject(this, &ACPlayerAI::HandleDribbleBreakThrough);
	state->OnGoal.AddUObject(this, &ACPlayerAI::HandleGoal);
	state->OnMatchStart.AddUObject(this, &ACPlayerAI::HandleMatchStart);
	state->OnMatchEnd.AddUObject(this, &ACPlayerAI::HandleMatchEnd);
}

void ACPlayerAI::Tick(float DeltaSeconds)
{
}

// フリー(ボールホルダー)ハンドル
void ACPlayerAI::HandleFreeHolder()
{
	// ボールホルダー時
	if (isBallHolder) {
		bool _b = ShortPassToFreeMan();
		if (_b == false) {
			Carry();
		}
		
		return;
	}
	// フリー時
	if (isFreeMan) {
		AC_Player* controlledPlayer = Cast<AC_Player>(GetPawn());
		if (controlledPlayer == nullptr) return;
		// ステート取得
		UGameStateManager* _state = GetGameInstance()->GetSubsystem<UGameStateManager>();
		if (_state == nullptr) return;
		
		if (_state->ballHolder)
			controlledPlayer->FreeAppeal(_state->ballHolder);
		
		return;
	}
}

// デュエル開始ハンドル
void ACPlayerAI::HandleDuelStart()
{
	// 所有プレイヤー取得
	AC_Player* _controlledPlayer = Cast<AC_Player>(GetPawn());
	if (_controlledPlayer == nullptr) return;
	
	// ボールホルダー時
	if (isBallHolder) {
		// アグレッシブスタンスに変更
		_controlledPlayer->SetDefensiveStance();

		return;
	}
	// ディフェンダー時
	if (isDefender) {
		// アグレッシブスタンスに変更
		_controlledPlayer->SetDefensiveStance();

		return;
	}
	// フリー時
	if (isFreeMan) {
		// ステート取得
		UGameStateManager* _state = GetGameInstance()->GetSubsystem<UGameStateManager>();
		if (_state == nullptr) return;

		if (_state->ballHolder)
			_controlledPlayer->FreeAppeal(_state->ballHolder);

		return;
	}
}

// ラインブレイクハンドル
void ACPlayerAI::HandleLineBreak()
{
	// ボールホルダー
	if (isBallHolder) {
		Carry();

		return;
	}

	// GK以外
	AC_Player* _controlledPlayer = Cast<AC_Player>(GetPawn());
	if (_controlledPlayer->position != C_Common::GK_POSITION) {
		// 2マス移動
		FVector _location = _controlledPlayer->GetActorLocation(); // 移動位置
		_location.X += (C_Common::TILE_SIZE * 2);
		_controlledPlayer->RunTo(_location);
	}
}

// クロスハンドル
void ACPlayerAI::HandleCross()
{
	if (isBallHolder) {
		// 自チーム取得
		AC_Player* _controlledPlayer = Cast<AC_Player>(GetPawn());
		if (_controlledPlayer == nullptr) return;
		UGameStateManager* _state = GetGameInstance()->GetSubsystem<UGameStateManager>();
		if (_state == nullptr) return;

		TArray<AC_Player*> _myTeamPlayers = {}; // マイチーム
		if (_controlledPlayer->ActorHasTag(FName("HOME"))) {
			_myTeamPlayers = _state->homePlayers;
		}
		else {
			_myTeamPlayers = _state->awayPlayers;
		}
		// ターゲット取得
		AC_Player* _targetPlayer = nullptr; // ターゲット
		for (AC_Player* _player : _myTeamPlayers) {
			// ①ターゲットマン
			if (_player->playerType == C_Common::TARGET_MAN_TYPE_NO) {
				_targetPlayer = _player;

				break;
			}
			// ②FW
			ACPlayerAI* _playerAI = Cast<ACPlayerAI>(_player->GetController());
			if (_playerAI->isBallHolder) continue;
			if (_player->position < C_Common::LWG_POSITION) continue;
			_targetPlayer = _player;

			break;
		}
		if (_targetPlayer == nullptr) return;

		// 実行
		_controlledPlayer->LongPass(_targetPlayer); // ロングパス
		_targetPlayer->Trap(_controlledPlayer); // トラップ

		return;
	}
}

// シュートハンドル
void ACPlayerAI::HandleShoot()
{
	if (isBallHolder) {
		AC_Player* _controlledPlayer = Cast<AC_Player>(GetPawn());
		_controlledPlayer->Shoot();

		return;
	}
}

// 裏抜けハンドル
void ACPlayerAI::HandleGetBehind()
{
	// -- 移動位置取得 --
	UGameStateManager* _state = GetGameInstance()->GetSubsystem<UGameStateManager>(); // ステート
	if (_state == nullptr) return;
	
	// 相手のディフェンスライン取得
	int _deffenceBehindLine = _state->ballHolder->ActorHasTag(FName("HOME")) ? (_state->awayDeffenceLine + 1) : (_state->homeDeffenceLine - 1);
	// ボールホルダーのレーン
	int _holderLane = (_state->ballHolder->tileNo - 1) % C_Common::TILE_NUM_Y;
	// 移動タイルNo
	int _moveTileNo = (_deffenceBehindLine * C_Common::TILE_NUM_Y) + _holderLane + 1;
	// 移動位置取得
	FVector _moveLocation = FVector::ZeroVector;
	for (AC_Tile* _t : _state->tiles) {
		if (_t->tileNo != _moveTileNo) continue;

		FVector _l = _t->GetActorLocation();
		if (_state->ballHolder->ActorHasTag(FName("HOME"))) {
			_l.X -= 82;
		}
		else {
			_l.X += 82;
		}

		_moveLocation = _l;
		break;
	}

	if (isBallHolder) {
		AC_Player* _controlledPlayer = Cast<AC_Player>(GetPawn());
		_controlledPlayer->LongKick(_moveLocation);
		
		return;
	}

	if (isGetBehindRunner) {
		AC_Player* _controlledPlayer = Cast<AC_Player>(GetPawn());
		if (_controlledPlayer)
			_controlledPlayer->RunTo(_moveLocation);

		return;
	}
}

// ドリブル突破ハンドル
void ACPlayerAI::HandleDribbleBreakThrough()
{
	AC_Player* _controlledPlayer = Cast<AC_Player>(GetPawn());
	if (_controlledPlayer == nullptr) return;

	// ボールホルダー
	if (isBallHolder)
	{
		_controlledPlayer->RegateDrrible();
		return;
	}

	// ディフェンダー
	if (isDefender) 
	{
		_controlledPlayer->Tackle();
		return;
	}

	// 縦ずれプレイヤー
	if (isFrontMovingPlayer) 
	{
		FVector _l = _controlledPlayer->GetActorLocation();
		if (_controlledPlayer->ActorHasTag("HOME")) {
			_l.X += C_Common::TILE_SIZE;
		}
		else {
			_l.X -= C_Common::TILE_SIZE;
		}
		_controlledPlayer->RunTo(_l); // move

		return;
	}
}

// ゴールハンドル
void ACPlayerAI::HandleGoal()
{
	AC_Player* _controlledPlayer = Cast<AC_Player>(GetPawn());
	if (_controlledPlayer == nullptr) return;
	
	if (_controlledPlayer->ActorHasTag("HOME")) {
		_controlledPlayer->CheerMotion();
	}
	else {
		_controlledPlayer->SadMotion();
	}
}

// 試合開始ハンドル
void ACPlayerAI::HandleMatchStart()
{
	AC_Player* _controlledPlayer = Cast<AC_Player>(GetPawn());
	if (_controlledPlayer == nullptr) return;
	
	// アニメーション停止 (*再試合対応)
	_controlledPlayer->StopAnim();

	// 初期配置 (*再試合対応)
	UMy_Game_Instance* _instance = Cast<UMy_Game_Instance>(UGameplayStatics::GetGameInstance(GetWorld())); // ゲームインスタンス
	if (_instance == nullptr)  return;
	if (_instance->game_phase == C_Common::MATCH_READY_PHASE) 
	{
		// <試合開始>
		initialLocation = _controlledPlayer->GetActorLocation();
	}else 
	{
		// <再試合>
		_controlledPlayer->SetActorLocation(initialLocation); // 位置
		_controlledPlayer->LookForward(); // 向き
	}

	// GK以外
	if (_controlledPlayer->position != C_Common::GK_POSITION) {
		// ミドルラインへ移動 (走る)
		if (_controlledPlayer->ActorHasTag("HOME")) {
			// HOME
			FVector _targetLocation = _controlledPlayer->GetActorLocation();
			_targetLocation.X += C_Common::TILE_SIZE;
			_controlledPlayer->RunTo(_targetLocation);
		}
		else {
			// AWAY
			FVector _targetLocation = _controlledPlayer->GetActorLocation();
			_targetLocation.X -= C_Common::TILE_SIZE * 2;
			_controlledPlayer->RunTo(_targetLocation);
		}
	}

}

// 試合終了ハンドル
void ACPlayerAI::HandleMatchEnd()
{
	AC_Player* _controlledPlayer = Cast<AC_Player>(GetPawn());
	if (_controlledPlayer == nullptr) return;

	// アニメーション停止
	_controlledPlayer->StopAnim();

	// 初期配置
	_controlledPlayer->SetActorLocation(initialLocation); // 位置
	_controlledPlayer->LookForward(); // 向き
}

// ショートパス
void ACPlayerAI::ShortPass(AC_Player* toPlayer)
{
	AC_Player* controlledPlayer = Cast<AC_Player>(GetPawn());
	if (controlledPlayer == nullptr) return;

	// アニメーション
	controlledPlayer->ShotPass(toPlayer); // ショートパス
	toPlayer->Trap(controlledPlayer); // トラップ
}

// フリーマンへパス
bool ACPlayerAI::ShortPassToFreeMan()
{
	AC_Player* controlledPlayer = Cast<AC_Player>(GetPawn());
	if (controlledPlayer == nullptr) return false;
	UGameStateManager* _state = GetGameInstance()->GetSubsystem<UGameStateManager>();
	if (_state == nullptr) return false;

	// -- ボール保持チーム取得 --
	TArray<AC_Player*> _myTeamPlayers = {}; // ボール保持チーム
	if (controlledPlayer->ActorHasTag(FName("HOME"))) {
		_myTeamPlayers = _state->homePlayers;
	}
	else {
		_myTeamPlayers = _state->awayPlayers;
	}

	// -- ターゲット取得 --
	AC_Player* _targetPlayer = nullptr; // パス先のプレイヤー
	for (AC_Player* _p : _myTeamPlayers) {

		if (controlledPlayer->position == C_Common::GK_POSITION) {
			// < GK >
			// ①RB
			if (_p->position != C_Common::RSB_POSITION) continue;

			_targetPlayer = _p;
			break;
		}
		else {
			// < フィールドプレイヤー >
			// ①視野内
			if (controlledPlayer->viewNos.Contains(_p->tileNo) == false) continue;
			// ②フリー
			if (_state->GetIsFree(_p) == false) continue;

			_targetPlayer = _p;
			break;
		}
	}
	if (_targetPlayer == nullptr) return false;

	// -- ショートパス --
	ShortPass(_targetPlayer);
	
	return true;
}

// キャリー
bool ACPlayerAI::Carry()
{
	AC_Player* controlledPlayer = Cast<AC_Player>(GetPawn());
	if (controlledPlayer == nullptr) return false;
	UGameStateManager* _state = GetGameInstance()->GetSubsystem<UGameStateManager>();
	if (_state == nullptr) return false;

	// -- ボール保持チーム取得 --
	TArray<AC_Player*> _myTeamPlayers = {}; // ボール保持チーム
	if (controlledPlayer->ActorHasTag(FName("HOME"))) {
		_myTeamPlayers = _state->homePlayers;
	}
	else {
		_myTeamPlayers = _state->awayPlayers;
	}

	// *条件*
	// ①GKでない
	if (controlledPlayer->position == C_Common::GK_POSITION) return false;

	// -- 縦ずれ処理 --
	// ①前のプレイヤー取得
	AC_Player* _frontPlayer = nullptr; // 前のプレイヤー
	int _frontTile = controlledPlayer->tileNo + C_Common::TILE_NUM_Y;
	for (AC_Player* _p : _myTeamPlayers) {
		if (_p->tileNo == _frontTile) {
			_frontPlayer = _p;

			break;
		}
	}
	// ②移動
	if (_frontPlayer) {
		// 移動位置取得
		FVector _l = _frontPlayer->GetActorLocation();
		if (controlledPlayer->ActorHasTag("HOME")) {
			_l.X += C_Common::TILE_SIZE;
		}
		else {
			_l.X -= C_Common::TILE_SIZE;
		}
		// 移動
		_frontPlayer->RunTo(_l);
	}
	// --

	controlledPlayer->Drrible(); // ドリブル
	
	return true;
}