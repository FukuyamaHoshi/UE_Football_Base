// Fill out your copyright notice in the Description page of Project Settings.


#include "GameStateManager.h"
#include <Kismet/GameplayStatics.h>
#include "CPlayerAI.h"
#include "My_Game_Instance.h"

//UKismetSystemLibrary::PrintString(this, "level instance", true, true, FColor::Red, 10.0f, TEXT("None"));
void UGameStateManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
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
			
			ClearAllStates(); // 全状態クリア (スタートフラグが残るため)
			TransitionToPhase(ETurnPhase::WaitingForAction);
		}

		return;
	}
	
	// --- 試合フェーズ処理 ---
	if (_instance->game_phase == C_Common::MATCH_PHASE)
	{
		UpdateMatchEnd(); // 試合終了状態更新

		switch (currentTurnPhase)
		{
			case ETurnPhase::WaitingForAction:
				HandleWaitingForActionPhase(DeltaTime); // ← Pass DeltaTime
				break;

			case ETurnPhase::ActionPlaying:
				HandleActionPlayingPhase();
				break;

			case ETurnPhase::StateDetection:
				HandleStateDetectionPhase();
				break;

			case ETurnPhase::TurnComplete:
				HandleTurnCompletePhase();
				break;

			default:
				// Safety: If somehow in None state, start waiting
				TransitionToPhase(ETurnPhase::WaitingForAction);
				break;
		}
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
			_playerAI->isFreeMan = true; // *フラグセット
			freeMans.Add(_p); // *プレイヤーセット
		}
	}
	return true;
}

// - フリー(ボールホルダー)状態更新 -
void UGameStateManager::UpdateFreeHolder()
{
	bool detected = DedectFreeHolder();
	if (detected)
	{
		AddState(EGameState::FreeHolder);
		OnFreeHolder.Broadcast();
	}
}

// - デュエル状態検知 -
bool UGameStateManager::DedectDuel()
{
	// *条件*
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
			_defenceAI->isDefender = true; // *フラグセット
			duelDeffender = _player; // *プレイヤーセット

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
			_playerAI->isFreeMan = true; // *フラグセット
			freeMans.Add(_p); // *プレイヤーセット
		}
	}
	
	return true;
}

// - デュエル状態更新 -
void UGameStateManager::UpdateDuel()
{
	bool detected = DedectDuel();
	if (detected)
	{
		AddState(EGameState::Duel);
		OnDuelStart.Broadcast();

		// デゥエル開始後、『デゥエル時プレイ選択』を遅延実行
		if (GetWorld()) {
			FTimerDelegate _timerDel = FTimerDelegate::CreateUObject(this, &UGameStateManager::DuelPlayChoice);
			FTimerHandle _timerHandle_SomeTask;
			const float _delaySeconds = 2.0f; // ディレイ時間
			GetWorld()->GetTimerManager().SetTimer(_timerHandle_SomeTask, _timerDel, _delaySeconds, false);
		}
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
	isMoving = DedectCompleteMoving();
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
	bool detected = DedectLineBreak();
	if (detected)
	{
		AddState(EGameState::LineBreak);
		OnLineBreak.Broadcast();
	}
}

// - クロス検知 -
bool UGameStateManager::DedectCross()
{
	// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
	// ✅ ADDED: CheckStateFlags
	// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
	if (CheckStateFlags()) return false;
	
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
	bool detected = DedectCross();
	if (detected)
	{
		AddState(EGameState::Cross);
		OnCross.Broadcast();
	}
}

// - シュート状態検知 -
bool UGameStateManager::DedectShoot()
{
	// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
	// ✅ ADDED: CheckStateFlags
	// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
	if (CheckStateFlags()) return false;
	
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
	bool detected = DedectShoot();
	if (detected)
	{
		AddState(EGameState::Shoot);
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

	ACPlayerAI* _frontPlayerAI = Cast<ACPlayerAI>(_frontPlayer->GetController());
	_frontPlayerAI->isGetBehindRunner = true; // *フラグセット (裏抜け)
	getBehindingRunner = _frontPlayer; // *プレイヤーセット
	
	return true;
}

// - 裏抜け状態更新 -
void UGameStateManager::UpdateGetBehind()
{
	bool detected = DedectGetBehind();
	if (detected)
	{
		AddState(EGameState::GetBehind);
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
		frontMovingPlayer = _frontPlayer; // *プレイヤーセット
	}

	return true;
}

// - ドリブル突破状態更新 -
void UGameStateManager::UpdateDribbleBreakThrough()
{
	bool detected = DedectDribbleBreakThrough();
	if (detected)
	{
		AddState(EGameState::DribbleBreakThrough);
		OnDribbleBreakThrough.Broadcast();
	}
}

// - ゴール状態検知 -
bool UGameStateManager::DedectGoal()
{
	// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
	// ✅ NOTE: This function checks Shoot state, no need for CheckStateFlags
	// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
	if (HasState(EGameState::Shoot)) return true;

	return false;
}

// - ゴール状態更新 -
void UGameStateManager::UpdateGoal()
{
	if (HasState(EGameState::Goal)) return; // do once

	bool detected = DedectGoal();
	if (detected)
	{
		AddState(EGameState::Goal);
		OnGoal.Broadcast();
	}
}

// - 試合開始状態更新 -
void UGameStateManager::UpdateMatchStart()
{
	if (HasState(EGameState::MatchStarted)) return; // do once

	AddState(EGameState::MatchStarted);
	OnMatchStart.Broadcast();
}

// - 試合再開処理 -
void UGameStateManager::OnMatchRestart()
{
	// リセット
	ClearAllStates(); // 状態
	ResetPlayerFlags(); // プレイヤーフラグ
	// 試合開始
	UpdateMatchStart();
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
	if (HasState(EGameState::MatchEnded)) return; // do once

	bool detected = DedectMatchEnd();
	if (detected)
	{
		AddState(EGameState::MatchEnded);
		OnMatchEnd.Broadcast();
		OnMatchEnded();
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

// 試合終了時処理
void UGameStateManager::OnMatchEnded()
{
	UMy_Game_Instance* _instance = Cast<UMy_Game_Instance>(UGameplayStatics::GetGameInstance(GetWorld())); // ゲームインスタンス
	if (_instance == nullptr)  return;

	_instance->game_phase = C_Common::PLAYER_SELECT_PLACE_PHASE; // フェーズ変更
	HandleTurnCompletePhase(); // ターン完了フェーズ処理
}

// 状態を追加
void UGameStateManager::AddState(EGameState State)
{
	if (!activeStates.Contains(State))
	{
		activeStates.Add(State);
		UE_LOG(LogTemp, Log, TEXT("State Added: %d"), static_cast<int32>(State));
	}
}

// 状態を削除
void UGameStateManager::RemoveState(EGameState State)
{
	activeStates.Remove(State);
	UE_LOG(LogTemp, Log, TEXT("State Removed: %d"), static_cast<int32>(State));
}

// 状態をチェック
bool UGameStateManager::HasState(EGameState State) const
{
	return activeStates.Contains(State);
}

// 全状態をクリア
void UGameStateManager::ClearAllStates()
{
	activeStates.Empty();
	UE_LOG(LogTemp, Log, TEXT("All states cleared"));
}

// プレイヤーフラグリセット
void UGameStateManager::ResetPlayerFlags()
{
	// デフェンダー(デュエル)
	if (duelDeffender) {
		ACPlayerAI* _defenceAI = Cast<ACPlayerAI>(duelDeffender->GetController());
		_defenceAI->isDefender = false; // フラグリセット
		duelDeffender = nullptr;
	}

	// フリーマン
	if (freeMans.Num() > 0) {
		for (AC_Player* _p : freeMans) {
			ACPlayerAI* _playerAI = Cast<ACPlayerAI>(_p->GetController());
			_playerAI->isFreeMan = false; // フラグリセット
		}
		freeMans.Empty();
	}

	// 裏抜けランナー
	if (getBehindingRunner) {
		ACPlayerAI* _runnerAI = Cast<ACPlayerAI>(getBehindingRunner->GetController());
		_runnerAI->isGetBehindRunner = false; // フラ
		getBehindingRunner = nullptr;
	}

	// 縦ずれプレイヤー
	if (frontMovingPlayer) {
		ACPlayerAI* _frontAI = Cast<ACPlayerAI>(frontMovingPlayer->GetController());
		_frontAI->isFrontMovingPlayer = false; // フラグ
		frontMovingPlayer = nullptr;
	}
}

// 状態フラグチェック
bool UGameStateManager::CheckStateFlags()
{
	return activeStates.Num() > 0; // いずれかの状態がアクティブか
}

// デゥエル時プレイ選択
void UGameStateManager::DuelPlayChoice()
{
	// 裏抜け or ドリブル突破
	UpdateGetBehind(); // 裏抜け状態更新
	if (HasState(EGameState::GetBehind)) return; // 裏抜け中はなし
	
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

// フェーズハンドラー (待機フェーズ)
void UGameStateManager::HandleWaitingForActionPhase(float DeltaTime)
{
	waitingPhaseTimer += DeltaTime;
	
	// Check if action interval reached
	if (waitingPhaseTimer > C_Common::PLAY_INTERVAL)
	{	
		// Reset timer
		waitingPhaseTimer = 0.0f;
		// Increment turn count
		turnCount++;
		// Update defence line and ball holder
		SetDeffenceLine();
		// set ball holder
		SetBallHolder();

		// Transition to next phase
		TransitionToPhase(ETurnPhase::ActionPlaying);
	}
}

// フェーズハンドラー (アクション実行フェーズ)
void UGameStateManager::HandleActionPlayingPhase()
{
	// 各種状態検知
    UpdateShoot();
    UpdateCross();
    UpdateLineBreak();
    UpdateDuel();
    UpdateFreeHolder();
    
    // 次のフェーズへ遷移
    TransitionToPhase(ETurnPhase::StateDetection);
}

// フェーズハンドラー (状態検知フェーズ)
void UGameStateManager::HandleStateDetectionPhase()
{
    // 移動完了チェック
    UpdateCompleteMoving();
	// デゥエル中は、その他の状態が発生するまで継続
	if (HasState(EGameState::Duel)) {
		if (activeStates.Num() == 1) return;
	}
    
    if (!isMoving)
    {
        TransitionToPhase(ETurnPhase::TurnComplete);
    }
}

// フェーズハンドラー (ターン完了フェーズ)
void UGameStateManager::HandleTurnCompletePhase()
{
    if (HasState(EGameState::Shoot))
    {
		// - 試合再開処理 (遅延実行) -
		if (HasState(EGameState::Goal)) return; // do once
		FTimerHandle TimerHandle;
		FTimerDelegate TimerDel = FTimerDelegate::CreateUObject(this, &UGameStateManager::OnMatchRestart);
		if (GetWorld())
		{
			float _delay = 2.5f;
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDel, _delay, false);
		}
		// ゴール状態更新
		UpdateGoal();

        return;
    }

	ClearAllStates();
    ResetPlayerFlags();
	OnTurnCompletePhase.Broadcast(); // ターン完了通知
    
    TransitionToPhase(ETurnPhase::WaitingForAction);
}

// フェーズ遷移
void UGameStateManager::TransitionToPhase(ETurnPhase NewPhase)
{
	// 古いフェーズを保存
	ETurnPhase OldPhase = currentTurnPhase;
	
	// 新しいフェーズに更新
	currentTurnPhase = NewPhase;
	
	// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
	// フェーズ開始時の特別処理 (オプション)
	// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
	switch (NewPhase)
	{
		case ETurnPhase::WaitingForAction:
			// 行動待ちフェーズ開始時の処理
			UKismetSystemLibrary::PrintString(this, "Waiting for action...", true, true, FColor::Yellow, 2.0f);
			break;
			
		case ETurnPhase::ActionPlaying:
			// アクション実行フェーズ開始時の処理
			UKismetSystemLibrary::PrintString(this, "Action playing...", true, true, FColor::Cyan, 2.0f);
			break;
			
		case ETurnPhase::StateDetection:
			// 状態検知フェーズ開始時の処理
			UKismetSystemLibrary::PrintString(this, "Detecting states...", true, true, FColor::Magenta, 2.0f);
			break;
			
		case ETurnPhase::TurnComplete:
			// ターン完了フェーズ開始時の処理
			UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("Turn %d complete!"), turnCount), true, true, FColor::Green, 2.0f);
			break;
	}
}