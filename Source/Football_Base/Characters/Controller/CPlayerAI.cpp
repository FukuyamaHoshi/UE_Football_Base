// Fill out your copyright notice in the Description page of Project Settings.


#include "CPlayerAI.h"
#include <Kismet/KismetSystemLibrary.h>
#include <Kismet/GameplayStatics.h>
#include <Football_Base/Systems/GameStateManager.h>
#include <Football_Base/C_Common.h>
#include <Football_Base/Actors/C_Tile.h>
#include <Football_Base/Systems/My_Game_Instance.h>

//UKismetSystemLibrary::PrintString(this, "level instance", true, true, FColor::Red, 10.0f, TEXT("None"));
void ACPlayerAI::BeginPlay()
{
	// StateSubsystem を取得
	UGameStateManager*  state = GetGameInstance()->GetSubsystem<UGameStateManager>();

	// subscribe C++ delegate
	state->OnFreeHolder.AddUObject(this, &ACPlayerAI::HandleFreeHolder);
	state->OnDuelStart.AddUObject(this, &ACPlayerAI::HandleDuelStart);
	state->OnDuelContinue.AddUObject(this, &ACPlayerAI::HandleDuelContinue); // working on duel
	state->OnLineBreak.AddUObject(this, &ACPlayerAI::HandleLineBreak);
	state->OnCross.AddUObject(this, &ACPlayerAI::HandleCross);
	state->OnShoot.AddUObject(this, &ACPlayerAI::HandleShoot);
	state->OnGetBehind.AddUObject(this, &ACPlayerAI::HandleGetBehind);
	state->OnDribbleBreakThrough.AddUObject(this, &ACPlayerAI::HandleDribbleBreakThrough);
	state->OnGoal.AddUObject(this, &ACPlayerAI::HandleGoal);
	state->OnMatchEnd.AddUObject(this, &ACPlayerAI::HandleMatchEnd);
	state->OnTurnCompletePhase.AddUObject(this, &ACPlayerAI::HandleTurnCompletePhase);
	state->OnStateTurnComplete.AddUObject(this, &ACPlayerAI::HandleStateTurnComplete);
	state->OnActionFailed.AddUObject(this, &ACPlayerAI::HandleActionFailed); // ← ADD THIS
}

void ACPlayerAI::Tick(float DeltaSeconds)
{
}

// フリー(ボールホルダー)ハンドル
void ACPlayerAI::HandleFreeHolder()
{
	// - ball holder -
	if (isBallHolder) {
		AC_Player* _controlledPlayer = Cast<AC_Player>(GetPawn());
		if (_controlledPlayer == nullptr) return;

		// < GK >
		if (_controlledPlayer->position == C_Common::GK_POSITION) {
			bool _b = PassFromGKToDF();
			
			// - failure action -
			if (_b == false) {
				// Notify GameStateManager of failure
				UGameStateManager* _state = GetGameInstance()->GetSubsystem<UGameStateManager>();
				if (_state) {
					UKismetSystemLibrary::PrintString(this, "GK pass failed - notifying GameStateManager", true, true, FColor::Red, 3.0f);
					_state->OnPlayerHandleFailed(EGameState::FreeHolder);
				}
				
				return;
			}
		}
		else {
			// < Field Player >
			bool _b = PassToFrontPlayer();

			// - failure action -
			if (_b == false) {
				// Notify GameStateManager of failure
				UGameStateManager* _state = GetGameInstance()->GetSubsystem<UGameStateManager>();
				if (_state) {
					UKismetSystemLibrary::PrintString(this, "Field player pass failed - notifying GameStateManager", true, true, FColor::Red, 3.0f);
					_state->OnPlayerHandleFailed(EGameState::FreeHolder);
				}
				
				return;
			}
		}

		isActionCompleted = true; // action completed!
		return;
	}
	
	// - except ball holder -
	isActionCompleted = true; // action completed!
}

// デュエル開始ハンドル
void ACPlayerAI::HandleDuelStart()
{
	// 所有プレイヤー取得
	AC_Player* _controlledPlayer = Cast<AC_Player>(GetPawn());
	if (_controlledPlayer == nullptr) return;
	
	// ボールホルダー時
	if (isBallHolder) {
		// - アタックモーション再生 -
		_controlledPlayer->StartAttackMotion();
		
		// - update my stamina -
		UpdateStamina(enemyAbilityCost);
		
		isActionCompleted = true; // action completed!
		return;
	}
	// ディフェンダー時
	if (isDefender) {
		// - ディフェンスモーション再生 -
		_controlledPlayer->StartDefenceMotion();
		
		// - update my stamina -
		UpdateStamina(enemyAbilityCost);
		
		isActionCompleted = true; // action completed!
		return;
	}

	isActionCompleted = true; // action completed!
}

// continue duel handler
void ACPlayerAI::HandleDuelContinue()
{
	// 所有プレイヤー取得
	AC_Player* _controlledPlayer = Cast<AC_Player>(GetPawn());
	if (_controlledPlayer == nullptr) return;

	// ボールホルダー時
	if (isBallHolder) {
		// - back pass until GK -
		BackPass();
		isActionCompleted = true; // action completed!
		return;
	}

	isActionCompleted = true; // action completed!
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
}

// 試合終了ハンドル
void ACPlayerAI::HandleMatchEnd()
{
	isActionCompleted = true; // reset

	// 初期配置
	//_controlledPlayer->SetActorLocation(initialLocation); // 位置
	//_controlledPlayer->LookForward(); // 向き
}

// ターン完了ハンドル
void ACPlayerAI::HandleTurnCompletePhase()
{
}

// ステート：ターン完了ハンドラ
void ACPlayerAI::HandleStateTurnComplete()
{
	isActionCompleted = true; // reset
}

// action pass from GK to DF
bool ACPlayerAI::PassFromGKToDF()
{
	// < if GK player is ball holder >
	//	1. serch DF
	//	2. pass to the player from left side
	AC_Player* controlledPlayer = Cast<AC_Player>(GetPawn());
	if (controlledPlayer == nullptr) return false;
	// check GK
	if (controlledPlayer->position != C_Common::GK_POSITION) return false;

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

	// - create DF position array -
	// DF positions from left to right: LSB(1), LCB(2), CB(3), RCB(4), RSB(5)
	TArray<int> _dfPositions = {
		C_Common::LSB_POSITION,
		C_Common::LCB_POSITION,
		C_Common::CB_POSITION,
		C_Common::RCB_POSITION,
		C_Common::RSB_POSITION
	};
	// Remove positions of players already in passedTargets
	for (AC_Player* _passedPlayer : passedTargets) {
		_dfPositions.Remove(_passedPlayer->position);
	}
	// Remove positions that don't exist in _myTeamPlayers
	for (int i = _dfPositions.Num() - 1; i >= 0; i--) {
		int dfPos = _dfPositions[i];
		bool positionExists = false;
		
		for (AC_Player* _p : _myTeamPlayers) {
			if (_p->position == dfPos) {
				positionExists = true;
				break;
			}
		}
		
		if (!positionExists) {
			_dfPositions.RemoveAt(i);
		}
	}
	// check empty
	if (_dfPositions.IsEmpty()) return false;

	// -- get pass target --
	AC_Player* _targetPlayer = nullptr;
	for (AC_Player* _p : _myTeamPlayers) {
		// *conditions*
		// 1. DF position from left to right
		if (_p->position != _dfPositions[0]) continue;

		passedTargets.Add(_p); // passed players
		_targetPlayer = _p;
			
		break;
	}
	if (_targetPlayer == nullptr) return false;

	// -- action short pass --
	controlledPlayer->ShotPass(_targetPlayer);
	
	return true;
}

// action pass to front player (front player is 2 tiles ahead)
bool ACPlayerAI::PassToFrontPlayer()
{
	AC_Player* _controlledPlayer = Cast<AC_Player>(GetPawn());
	if (_controlledPlayer == nullptr) return false;

	// get my tile No
	int _myTileNo = _controlledPlayer->tileNo;
	// get front tile No
	int _frontTileNo = _myTileNo + (C_Common::TILE_NUM_Y * 2);

	// get my team players
	UGameStateManager* _state = GetGameInstance()->GetSubsystem<UGameStateManager>();
	if (_state == nullptr) return false;
	TArray<AC_Player*> _myTeamPlayers = {}; // my team players
	if (_controlledPlayer->ActorHasTag(FName("HOME"))) {
		_myTeamPlayers = _state->homePlayers;
	}
	else {
		_myTeamPlayers = _state->awayPlayers;
	}

	// search front player
	AC_Player* _frontPlayer = nullptr;
	for (AC_Player* _p : _myTeamPlayers) {
		if (_p->tileNo == _frontTileNo) {
			_frontPlayer = _p;
			
			break;
		}
	}
	if (_frontPlayer == nullptr) return false;

	// -- action short pass --
	_controlledPlayer->ShotPass(_frontPlayer);
	
	return true;
}

// back pass (step by step to GK)
bool ACPlayerAI::BackPass()
{
	AC_Player* _controlledPlayer = Cast<AC_Player>(GetPawn());
	if (_controlledPlayer == nullptr) return false;

	// get my tile No
	int _myTileNo = _controlledPlayer->tileNo;
	// - check (if it is GK tile) -
	if (_myTileNo <= 0) return false; // call finish duel turn?
	
	// get back tile No
	int _backTileNo = _myTileNo - (C_Common::TILE_NUM_Y * 2);	

	// get my team players
	UGameStateManager* _state = GetGameInstance()->GetSubsystem<UGameStateManager>();
	if (_state == nullptr) return false;
	TArray<AC_Player*> _myTeamPlayers = {}; // my team players
	if (_controlledPlayer->ActorHasTag(FName("HOME"))) {
		_myTeamPlayers = _state->homePlayers;
	}
	else {
		_myTeamPlayers = _state->awayPlayers;
	}

	// - search back player (and get GK player) -
	AC_Player* _gk = nullptr;
	AC_Player* _backPlayer = nullptr;
	for (AC_Player* _p : _myTeamPlayers) {
		if (_p->tileNo == _backTileNo) {
			_backPlayer = _p;

			break;
		}
		if (_p->position == C_Common::GK_POSITION) {
			_gk = _p;
		}
	}

	// - action -
	if (_backPlayer == nullptr) {
		// < short pass to GK >
		if (_gk == nullptr) return false;
		_controlledPlayer->ShotPass(_gk);
		
		return true;
	}

	// < short pass to back player >
	_controlledPlayer->ShotPass(_backPlayer);

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

// update stamina
void ACPlayerAI::UpdateStamina(int value)
{
	stamina += value;
	if (stamina < 1) stamina = 0; // min
}

// get enemy ability cost (for duel)
void ACPlayerAI::SetEnemyAbilityCost(int enemysAbility)
{
	enemyAbilityCost = enemysAbility;
}

// Add this implementation
void ACPlayerAI::HandleActionFailed()
{
    // - complete action state -
    isActionCompleted = true;
    
}
