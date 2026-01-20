// Fill out your copyright notice in the Description page of Project Settings.


#include "GameStateManager.h"
#include <Kismet/GameplayStatics.h>
#include "CPlayerAI.h"
#include "My_Game_Instance.h"

//UKismetSystemLibrary::PrintString(this, "level instance", true, true, FColor::Red, 10.0f, TEXT("None"));
void UGameStateManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	// 初期化処理を書く
	//UKismetSystemLibrary::PrintString(this, "level instance", true, true, FColor::Red, 10.0f, TEXT("None"));
}

void UGameStateManager::Deinitialize()
{
	// 終了処理を書く
	Super::Deinitialize();

}

void UGameStateManager::Tick(float DeltaTime)
{
	// 初期化処理
	if (isInitialized == false) {
		isInitialized = Initialize();
		
		return;
	}
	
	UMy_Game_Instance* _instance = Cast<UMy_Game_Instance>(UGameplayStatics::GetGameInstance(GetWorld())); // ゲームインスタンス
	if (_instance == nullptr)  return;

	// --- 試合準備フェーズ ---
	if (_instance->game_phase == C_Common::MATCH_READY_PHASE) 
	{
		// 試合開始状態更新
		UpdateMatchStart();
		
		// フェーズカウントダウン
		if (_instance->phase_count < 1.0f) {
			_instance->game_phase = C_Common::MATCH_PHASE; // 試合フェーズへ
			_instance->phase_count = C_Common::MATCH_TIME; // カウンターセット
			OnCompleteMoving(); // 移動終了時処理 (*例外的に、試合開始時に実行)
		}

		return;
	}
	
	// --- 試合フェーズ処理 ---
	if (_instance->game_phase == C_Common::MATCH_PHASE)
	{
		// - 移動終了更新 -
		UpdateCompleteMoving();

		// インターバル設定 (ボールキープ時間)
		if (ballHolder == nullptr) return;
		if (ballHolder->ballKeepingCount < C_Common::PLAY_INTERVAL) return;

		// - 状態更新 -
		// 試合終了
		UpdateMatchEnd();
		// シュート
		UpdateShoot();
		// クロス
		UpdateCross();
		// ラインブレイク
		UpdateLineBreak();
		// デュエル
		UpdateDuel();
		// フリー(ボールホルダー)
		UpdateFreeHolder();
	}
}

TStatId UGameStateManager::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UGameStateManager, STATGROUP_Tickables);

}

// 初期化処理 (アクター取得など)
bool UGameStateManager::Initialize()
{
	// *** 全てのプレイヤーを取得 ***
	if (allPlayers.Num() < 22)
	{
		// - 空にする -
		homePlayers.Empty();
		awayPlayers.Empty();
		allPlayers.Empty();

		TArray<AActor*> _actorPlayers = {}; // プレイヤーアクター配列
		UGameplayStatics::GetAllActorsOfClass(this, AC_Player::StaticClass(), _actorPlayers); // クラスで探す

		for (AActor* _a : _actorPlayers) {
			AC_Player* _player = Cast<AC_Player>(_a); // キャスト

			if (_player->Tags.Num() < 1) continue; // (*制限) TAGなし

			// HOMEプレイヤー
			if (_player->ActorHasTag(FName("HOME"))) {
				homePlayers.Add(_player);
			}

			// AWAYプレイヤー
			if (_player->ActorHasTag(FName("AWAY"))) {
				awayPlayers.Add(_player);
			}
			allPlayers.Add(_player); // 追加
		}
	}
	// ***

	// *** タイルを取得 ***
	if (tiles.Num() < 30)
	{
		// - 空にする -
		tiles.Empty();

		TArray<AActor*> _actorTiles = {}; // タイルアクター配列
		UGameplayStatics::GetAllActorsOfClass(this, AC_Tile::StaticClass(), _actorTiles); // クラスで探す

		for (AActor* _a : _actorTiles) {
			AC_Tile* _tile = Cast<AC_Tile>(_a); // キャスト

			tiles.Add(_tile);
		}
	}
	// ***

	// *** ボール取得 ***
	if (ball == nullptr)
	{
		AActor* _b = UGameplayStatics::GetActorOfClass(this, AC_Soccer_Ball::StaticClass());
		ball = Cast<AC_Soccer_Ball>(_b); // キャスト
	}
	// ***

	// - 初期化完了判定 -
	if (allPlayers.Num() < 22) return false;
	if (tiles.Num() < 30) return false;
	if (ball == nullptr) return false;

	return true;
}

// - フリー(ボールホルダー)状態検知 -
bool UGameStateManager::DedectFreeHolder()
{
	// *条件*
	// ボール停止中
	if (ball->isMoving) return false;
	// すべての状態フラグ確認
	if (CheckStateFlags()) return false;

	// *プレイヤーフラグ更新*
	// - フリーマン -
	// ボール保持チーム取得
	TArray<AC_Player*> _myTeamPlayers = {}; // ボール保持チーム
	if (ballHolder->ActorHasTag(FName("HOME"))) {
		_myTeamPlayers = homePlayers;
	}
	else {
		_myTeamPlayers = awayPlayers;
	}
	// フラグ更新
	for (AC_Player* _p : _myTeamPlayers) {
		if (_p->isMoving) continue; // 移動中はなし
		if (_p == ballHolder) continue; // ボールホルダーなし

		if (GetIsFree(_p)) {
			ACPlayerAI* _playerAI = Cast<ACPlayerAI>(_p->GetController());
			_playerAI->isFreeMan = true; // フリーマンフラグセット
		}
	}
	return true;
}

// - フリー(ボールホルダー)状態更新 -
void UGameStateManager::UpdateFreeHolder()
{
	if (isFreeHolder) return; // do once

	isFreeHolder = DedectFreeHolder();
	// フリー通知
	if (isFreeHolder) OnFreeHolder.Broadcast();
}

// - デュエル状態検知 -
bool UGameStateManager::DedectDuel()
{
	// *条件*
	// ボール停止中
	if (ball->isMoving) return false;
	// ボールホルダーが存在
	if (ballHolder == nullptr) return false;
	// GK以外
	if (ballHolder->position == C_Common::GK_POSITION) return false;
	// 接敵中
	if (GetIsFree(ballHolder)) return false;
	// すべての状態フラグ確認
	if (CheckStateFlags()) return false;

	// *プレイヤーフラグ更新*
	// - ディフェンダー -
	TArray<AC_Player*> _deffencePlayers = {};
	if (ballHolder->ActorHasTag(FName("HOME"))) {
		_deffencePlayers = awayPlayers;
	}
	else {
		_deffencePlayers = homePlayers;
	}
	for (AC_Player* _player : _deffencePlayers) {
		if (_player->tileNo == ballHolder->tileNo) {
			// プレイヤーのAIコントローラー取得
			ACPlayerAI* _defenceAI = Cast<ACPlayerAI>(_player->GetController());
			_defenceAI->isDefender = true; // ディフェンダーフラグセット

			break;
		}
	}

	// - フリーマン -
	// ボール保持チーム取得
	TArray<AC_Player*> _myTeamPlayers = {}; // ボール保持チーム
	if (ballHolder->ActorHasTag(FName("HOME"))) {
		_myTeamPlayers = homePlayers;
	}
	else {
		_myTeamPlayers = awayPlayers;
	}
	// フラグ更新
	for (AC_Player* _p : _myTeamPlayers) {
		if (_p->isMoving) continue; // 移動中はなし
		if (_p == ballHolder) continue; // ボールホルダーなし

		if (GetIsFree(_p)) {
			ACPlayerAI* _playerAI = Cast<ACPlayerAI>(_p->GetController());
			_playerAI->isFreeMan = true; // フリーマンフラグセット
		}
	}
	
	return true;
}

// - デュエル状態更新 -
void UGameStateManager::UpdateDuel()
{
	if (isDuel) return; // do once処理

	isDuel = DedectDuel();
	// デュエル開始通知
	if (isDuel) {
		OnDuelStart.Broadcast();

		// -- デゥエル開始後、『デゥエル時プレイ選択』を遅延実行 --
		UWorld* _world = GetWorld();
		if (_world == nullptr) return;
		// Create a timer delegate to call the function
		FTimerDelegate _timerDel = FTimerDelegate::CreateUObject(this, &UGameStateManager::DuelPlayChoice /*, optional params */);
		// Provide a timer handle and a delay value so identifiers are defined.
		FTimerHandle _timerHandle_SomeTask;
		const float _delaySeconds = 2.0f; // ディレイ時間
		_world->GetTimerManager().SetTimer(_timerHandle_SomeTask, _timerDel, _delaySeconds, false /* bLoop */);
	}
}

// - 移動終了検知 -
bool UGameStateManager::DedectCompleteMoving()
{
	// *条件*
	// ①ボール移動中
	if (ball->isMoving) return true;
	// ②全プレイヤー移動中
	for (AC_Player* _p : allPlayers) {
		if (_p->isMoving) {
			
			return true;
		}
	}

	return false;
}

// - 移動終了更新 -
void UGameStateManager::UpdateCompleteMoving()
{
	bool _isCurrentPlaying = DedectCompleteMoving();
	if (_isCurrentPlaying) {
		// 移動中
		isMoving = true;
	}
	else
	{
		if (isMoving)
		{
			// ターン終了処理
			turnCount++; // ターンカウントアップ
			UKismetSystemLibrary::PrintString(this, "turn end" + FString::FromInt(turnCount), true, true, FColor::Red, 10.0f, TEXT("None"));
			OnCompleteMoving(); // イベント発行
			
			isMoving = false;
		}
	}
}

// - ラインブレイク検知 -
bool UGameStateManager::DedectLineBreak()
{
	// *条件*
	// ①ボールホルダーがディフェンスラインを超えている
	int _ballHolderLine = (ballHolder->tileNo - 1) / C_Common::TILE_NUM_Y; // ボールホルダーライン
	bool _isOver = ballHolder->ActorHasTag("HOME") ? awayDeffenceLine < _ballHolderLine : homeDeffenceLine > _ballHolderLine;
	if (_isOver == false) return false;
	// ②すべての状態フラグ確認
	if (CheckStateFlags()) return false;
	
	return true;
}

// - ラインブレイク状態更新 -
void UGameStateManager::UpdateLineBreak()
{
	if (isLineBreak) return; // do once

	isLineBreak = DedectLineBreak();
	if (isLineBreak) {
		OnLineBreak.Broadcast();
	}
}

// - クロス検知 -
bool UGameStateManager::DedectCross()
{
	TArray<int> _crossRangeNo = {}; // クラスレンジ(タイルNo)
	if (ballHolder->ActorHasTag("HOME")) {
		_crossRangeNo.Append({ 26, 30 });
	}
	else {
		_crossRangeNo.Append({ 1, 5 });
	}
	if (_crossRangeNo.Contains(ballHolder->tileNo)) return true;
	
	return false;
}

// - クロス状態更新 -
void UGameStateManager::UpdateCross()
{
	if (isCross) return; // do once

	isCross = DedectCross();
	if (isCross) {
		OnCross.Broadcast();
	}
}

// - シュート状態検知 -
bool UGameStateManager::DedectShoot()
{
	
	// シュートレンジ取得
	TArray<int> _shortRangeNo = {}; // シュートレンジ(タイルNo)
	if (ballHolder->ActorHasTag("HOME")) {
		_shortRangeNo.Append({ 27, 28, 29 });
	}
	else {
		_shortRangeNo.Append({ 2, 3, 4 });
	}

	// シュート
	if (_shortRangeNo.Contains(ballHolder->tileNo)) return true;
	
	return false;
}

// - シュート状態更新 -
void UGameStateManager::UpdateShoot()
{
	if (isShoot) return; // do once

	isShoot = DedectShoot();
	if (isShoot) {
		OnShoot.Broadcast();
	}
}

// - 裏抜け状態検知 -
bool UGameStateManager::DedectGetBehind()
{
	// *条件*
	// - ①前のプレイヤーが最終ライン上にいる -
	// ボール保持チーム取得
	TArray<AC_Player*> _myTeamPlayers = {}; // ボール保持チーム
	if (ballHolder->ActorHasTag(FName("HOME"))) {
		_myTeamPlayers = homePlayers;
	}
	else {
		_myTeamPlayers = awayPlayers;
	}
	// 前のプレイヤー取得
	AC_Player* _frontPlayer = nullptr; // 前のプレイヤー
	int _frontTile = ballHolder->tileNo + C_Common::TILE_NUM_Y;
	for (AC_Player* _p : _myTeamPlayers) {
		if (_p->tileNo == _frontTile) {
			_frontPlayer = _p;

			break;
		}
	}
	if (_frontPlayer == nullptr) return false;
	// 最終ライン上にいる
	int _frontPlayerLine = (_frontPlayer->tileNo - 1) / C_Common::TILE_NUM_Y; // 前のプレイヤーライン
	bool _isOn = ballHolder->ActorHasTag("HOME") ? awayDeffenceLine == _frontPlayerLine : homeDeffenceLine == _frontPlayerLine;
	if (_isOn == false) return false;

	// *プレイヤーフラグ更新*
	ACPlayerAI* _frontPlayerAI = Cast<ACPlayerAI>(_frontPlayer->GetController());
	_frontPlayerAI->isGetBehindRunner = true;
	
	return true;
}

// - 裏抜け状態更新 -
void UGameStateManager::UpdateGetBehind()
{
	if (isGetBehind) return; // do once

	isGetBehind = DedectGetBehind();
	if (isGetBehind) {
		OnGetBehind.Broadcast();
	}
}

// - ドリブル突破検知 -
bool UGameStateManager::DedectDribbleBreakThrough()
{
	// - 縦ずれプレイヤーにフラグセット -
	// ボール保持チーム取得
	TArray<AC_Player*> _myTeamPlayers = {}; // ボール保持チーム
	if (ballHolder->ActorHasTag(FName("HOME"))) {
		_myTeamPlayers = homePlayers;
	}
	else {
		_myTeamPlayers = awayPlayers;
	}
	// 前(縦ずれ)のプレイヤー取得
	AC_Player* _frontPlayer = nullptr; // 前のプレイヤー
	int _frontTile = ballHolder->tileNo + C_Common::TILE_NUM_Y;
	for (AC_Player* _p : _myTeamPlayers) {
		if (_p->tileNo == _frontTile) {
			_frontPlayer = _p;

			break;
		}
	}
	if (_frontPlayer) {
		ACPlayerAI* _frontPlayerAI = Cast<ACPlayerAI>(_frontPlayer->GetController());
		_frontPlayerAI->isFrontMovingPlayer = true; // *フラグセット (縦ずれ)
	}

	return true;
}

// - ドリブル突破状態更新 -
void UGameStateManager::UpdateDribbleBreakThrough()
{
	if (isDribbleBreakThrough) return; // do once

	isDribbleBreakThrough = DedectDribbleBreakThrough();
	if (isDribbleBreakThrough) {
		OnDribbleBreakThrough.Broadcast();
	}
}

// - ゴール状態検知 -
bool UGameStateManager::DedectGoal()
{
	if (isShoot) return true;

	return false;
}

// - ゴール状態更新 -
void UGameStateManager::UpdateGoal()
{
	if (isGoal) return; // do once

	isGoal = DedectGoal();
	if (isGoal) {
		OnGoal.Broadcast();
	}
}

// - 試合開始状態更新 -
void UGameStateManager::UpdateMatchStart()
{
	if (isMatchStarted) return; // do once
	isMatchStarted = true;

	if (isMatchStarted) {
		// 通知
		OnMatchStart.Broadcast();
	}
}

// - 試合終了検知 -
bool UGameStateManager::DedectMatchEnd()
{
	UMy_Game_Instance* _instance = Cast<UMy_Game_Instance>(UGameplayStatics::GetGameInstance(GetWorld())); // ゲームインスタンス
	if (_instance == nullptr)  return false;
	// *条件*
	// ①フェーズカウントが1.0f以下で終了
	if (_instance->phase_count > 1.0f) return false;
	
	return true;
}

// - 試合終了更新 -
void UGameStateManager::UpdateMatchEnd()
{
	if (isMatchEnded) return; // do once
	
	isMatchEnded = DedectMatchEnd();
	if (isMatchEnded) {
		// 通知
		OnMatchEnd.Broadcast();
		OnMatchEnded(); // 試合終了時処理
	}
}

// プレイヤーフリー判定
// ( true: フリー )
bool UGameStateManager::GetIsFree(AC_Player* targetPlayer)
{
	bool _isFree = false;

	// タイルNo = 0でもフリー
	if (targetPlayer->tileNo == 0 && subPlayers.Contains(targetPlayer) == false) {
		_isFree = true;

		return _isFree;
	}

	for (AC_Tile* _tile : tiles) {
		if (_tile->tileNo == targetPlayer->tileNo) {
			if (_tile->onPlayerNum < 2) _isFree = true;

			break;
		}
	}
	return _isFree;
}

// ボールホルダー設定
void UGameStateManager::SetBallHolder()
{
	// - 前回のボールホルダー -
	if (ballHolder) {
		ACPlayerAI* _ballHolderAI = Cast<ACPlayerAI>(ballHolder->GetController()); // ボールホルダーAI
		_ballHolderAI->isBallHolder = false; // *プレイヤーAI更新
	}

	// - 新しいボールホルダー取得 -
	// ボールのタイルNo取得
	int _ballTileNo = C_Common::GetTileNoFromLocation(ball->GetActorLocation());
	// ボールタイルNoのプレイヤー取得
	TArray<AC_Player*> _onTilePlayers = {};
	for (AC_Player* _p : allPlayers) {
		if (_p->tileNo == _ballTileNo) {
			_onTilePlayers.Add(_p);
		}
	}
	// 前回のボールホルダーと比較
	for (AC_Player* _p : _onTilePlayers) {
		if (_p->ActorHasTag(FName("HOME"))) { // ← HOMEボールホルダー優先 (*暫定処理)
			// 更新処理
			ballHolder = _p; // *ボールホルダー更新
			ballHolder->ballKeepingCount = 0; // *インターバルリセット
			ACPlayerAI* _ballHolderAI = Cast<ACPlayerAI>(ballHolder->GetController());
			_ballHolderAI->isBallHolder = true; // *プレイヤーAI更新
			
			break;
		}
	}
}

// 移動完了時処理
void UGameStateManager::OnCompleteMoving()
{
	// - ゴール時処理 -
	if (isShoot) {
		UpdateGoal(); // ゴール状態更新
		ResetStateFlags(); // 状態フラグリセット (*ゴール時にボールホルダーをセットするとプレイが再開される)

		// - 試合再開処理 -
		// 試合再開時は、フラグをリセットしない
		FTimerHandle TimerHandle;
		FTimerDelegate TimerDel = FTimerDelegate::CreateUObject(this, &UGameStateManager::UpdateMatchStart);
		if (GetWorld())
		{
			float _delay = 2.5f;
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDel, _delay, false);
		}
		
		return;
	}

	SetDeffenceLine(); // ディフェンスライン更新
	SetBallHolder(); // ボールホルダー更新
	ResetStateFlags(); // 状態フラグリセット
}

// 試合終了時処理
void UGameStateManager::OnMatchEnded()
{
	UMy_Game_Instance* _instance = Cast<UMy_Game_Instance>(UGameplayStatics::GetGameInstance(GetWorld())); // ゲームインスタンス
	if (_instance == nullptr)  return;

	_instance->game_phase = C_Common::PLAYER_SELECT_PLACE_PHASE; // フェーズ変更
	ResetStateFlags(); // 状態フラグリセット
}

// 状態フラグリセット
void UGameStateManager::ResetStateFlags()
{
	isFreeHolder = false;
	isDuel = false;
	isLineBreak = false;
	isGetBehind = false;
	isDribbleBreakThrough = false;
	isCross = false;
	isShoot = false;
	isGoal = false;
	isMatchStarted = false;
	isMatchEnded = false;
}

// 状態フラグチェック
bool UGameStateManager::CheckStateFlags()
{
	if (isFreeHolder
		|| isDuel
		|| isLineBreak
		|| isGetBehind
		|| isDribbleBreakThrough
		|| isCross
		|| isShoot
		|| isGoal
		|| isMatchStarted
		|| isMatchEnded
		)
	return true;

	return false;
}

// デゥエル時プレイ選択
void UGameStateManager::DuelPlayChoice()
{
	// 裏抜け or ドリブル突破
	UpdateGetBehind(); // 裏抜け状態更新
	if (isGetBehind) return; // 裏抜け中はなし
	
	UpdateDribbleBreakThrough(); // ドリブル突破状態更新

}

// ディフェンスライン設定
void UGameStateManager::SetDeffenceLine()
{
	// -- HOME --
	TArray<int> _homeLines = {};
	for (AC_Player* _p : homePlayers) {
		if (_p->position == C_Common::GK_POSITION) continue; // (*制限) GK

		int _line = (_p->tileNo - 1) / C_Common::TILE_NUM_Y;
		_homeLines.Add(_line);
	}
	if (_homeLines.IsEmpty()) return;
	homeDeffenceLine = FGenericPlatformMath::Min(_homeLines); // 最小値

	// -- AWAY --
	TArray<int> _awayLines = {};
	for (AC_Player* _p : awayPlayers) {
		if (_p->position == C_Common::GK_POSITION) continue; // (*制限) GK

		int _line = (_p->tileNo - 1) / C_Common::TILE_NUM_Y;
		_awayLines.Add(_line);
	}
	if (_awayLines.IsEmpty()) return;
	awayDeffenceLine = FGenericPlatformMath::Max(_awayLines); // 最大値
}