// Fill out your copyright notice in the Description page of Project Settings.


#include "C_Opening_Level_Instance.h"
#include <Kismet/KismetSystemLibrary.h>
#include <Kismet/GameplayStatics.h>
#include "My_Game_Instance.h"
#include "C_Common.h"

// Sets default values
AC_Opening_Level_Instance::AC_Opening_Level_Instance()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// *** オープニングUIサブクラス取得 ***
	ConstructorHelpers::FClassFinder<UC_Opening_UI> _openingUIFinder(TEXT("/Game/UI/WBP_Opening"));
	if (_openingUIFinder.Class) {
		openingUISubClass = _openingUIFinder.Class;
	}
	// ***
}

// Called when the game starts or when spawned
void AC_Opening_Level_Instance::BeginPlay()
{
	Super::BeginPlay();

	//UKismetSystemLibrary::PrintString(this, "level instance", true, true, FColor::Red, 10.0f, TEXT("None"));

	// *** ウィジェット表示 ***
	openingUI = CreateWidget<UC_Opening_UI>(GetWorld(), openingUISubClass);
	if (openingUI)
	{
		openingUI->AddToViewport(); // Adds the widget to the screen
	}
	// ***

	SetupInput(); // 入力設定

	// *** デカール取得 ***
	// (コンストラクタ以外のマテリアルはLoadObject使用)
	managerSelectedDecalMaterial = LoadObject<UMaterial>(NULL, TEXT("/Game/Materials/M_Decal_Selected_Player.M_Decal_Selected_Player"), NULL, LOAD_None, NULL);
	// ***

	// *** 全てのプレイヤーを取得 ***
	TArray<AActor*> _actorPlayers = {}; // プレイヤーアクター配列
	UGameplayStatics::GetAllActorsOfClass(this, AC_Player::StaticClass(), _actorPlayers); // クラスで探す
	
	for (AActor* _a : _actorPlayers) {
		AC_Player* _player = Cast<AC_Player>(_a); // キャスト
		
		// HOMEとAWAYで分ける
		if (_player->ActorHasTag(FName("HOME"))) {
			// HOMEプレイヤー
			homePlayers.Add(_player);
		}
		else {
			// AWAYプレイヤー
			awayPlayers.Add(_player);
		}
		allPlayers.Add(_player); // 追加
	}
	// ***

	// *** タイルを取得 ***
	TArray<AActor*> _actorTiles = {}; // タイルアクター配列
	UGameplayStatics::GetAllActorsOfClass(this, AC_Tile::StaticClass(), _actorTiles); // クラスで探す

	for (AActor* _a : _actorTiles) {
		AC_Tile* _tile = Cast<AC_Tile>(_a); // キャスト

		tiles.Add(_tile);
	}
	// ***

	// *** ボール取得 ***
	AActor* _b = UGameplayStatics::GetActorOfClass(this, AC_Soccer_Ball::StaticClass());
	ball = Cast<AC_Soccer_Ball>(_b); // キャスト
	// ***

	// *** マネージャー取得 ***
	AActor* _m = UGameplayStatics::GetActorOfClass(this, AC_Manager::StaticClass());
	homeManager = Cast<AC_Manager>(_m); // キャスト
	// ***
}

// Called every frame
void AC_Opening_Level_Instance::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UMy_Game_Instance* _instance = Cast<UMy_Game_Instance>(UGameplayStatics::GetGameInstance(GetWorld())); // ゲームインスタンス
	if (_instance == nullptr)  return;
	
	// *** ホバー時処理 ***
	if (_instance->game_phase == C_Common::MANAGER_SELECT_BEFORE_PHASE || _instance->game_phase == C_Common::MANAGER_SELECT_AFTER_PHASE) { // マネージャー選択(前・後)フェーズのみ
		Hover();
	}
	// ***

	if (ball == nullptr) return; // ボール取得失敗
	if (_instance->game_phase != C_Common::MATCH_PHASE) return; // *(制限)試合フェーズのみ

	// *** 試合終了時 ***
	if (isGoal && ball->isMoving == false) {
		
		if (isOnceGoal) return; // 一度のみの処理
		isOnceGoal = true; // フラグON
		
		// -- ゴール・被ゴールチーム取得 --
		float _ballX = ball->GetActorLocation().X;
		bool _isHomeGoal = (_ballX > 0) ? true : false; // どのチームのゴールか (ボール位置から)
		TArray<AC_Player*> _scoreTeam = {}; // ゴールチーム
		TArray<AC_Player*> _notScoreTeam = {}; // 被ゴールチーム
		if (_isHomeGoal) {
			_scoreTeam = homePlayers; // ゴールチーム
			_notScoreTeam = awayPlayers; // 被ゴールチーム
		}
		else {
			_scoreTeam = awayPlayers; // ゴールチーム
			_notScoreTeam = homePlayers; // 被ゴールチーム
		}

		// -- ゴールアニメーション --
		// ●ゴールチーム
		for (AC_Player* _player : _scoreTeam) {
			_player->CheerMotion();
		}
		// ●被ゴールチーム
		for (AC_Player* _player : _notScoreTeam) {
			_player->SadMotion();
		}
		// ●HOMEマネージャー
		if (homeManager) homeManager->CheerAnim();

		return;
	}
	// ***
	
	// *** 試合開始時 ***
	if (isOnceMatchStart)
	{
		isOnceMatchStart = false;
		MatchStart();
	}
	// ***


	// *** コマンド変更監視 ***
	if (_instance->command != currentCommand) { // コマンド変更がされた時
		currentCommand = _instance->command; // コマンド一時保存

		// -- コマンド開始時処理 --
		if (homeManager) homeManager->ChangeAnim(currentCommand); // アニメーション切り替え(マネージャー)
	}
	// ***


	// *** ポストプレイ処理 ***
	if (postPlayCount > 0) {
		if (ball) {
			if (ball->isMoving) return; // ボール移動中
		}
		PostPlay();

		return;
	}
	// ***
	

	// *** 裏抜けプレイヤー処理 ***
	if (getBehindingPlayer) {
		if (getBehindingPlayer->isMoving) return;

		SetBallHolder(getBehindingPlayer);
		getBehindingPlayer = nullptr;
	}
	// ***
	
	// *** シュート ***
	if (ballHolder) {
		if (ballHolder->ballKeepingCount > 0.0f) { // キープしている場合のみ
			// シュートレンジ取得
			TArray<int> _shortRangeNo = {}; // シュートレンジ(タイルNo)
			if (ballHolder->ActorHasTag("HOME")) {
				_shortRangeNo.Append({ 27, 28, 29 });
			}
			else {
				_shortRangeNo.Append({ 2, 3, 4 });
			}

			// シュート
			if (_shortRangeNo.Contains(ballHolder->tileNo)) {
				Shoot();

				return;
			}
		}
	}
	// ***

	// *** クロス ***
	if (ballHolder) {
		if (ballHolder->ballKeepingCount > 0.0f) { // キープしている場合のみ
			// クロスレンジ取得
			TArray<int> _crossRangeNo = {}; // クロスレンジ(タイルNo)
			if (ballHolder->ActorHasTag("HOME")) {
				_crossRangeNo.Append({ 26, 30 });
			}
			else {
				_crossRangeNo.Append({ 1, 5 });
			}

			// クロス
			if (_crossRangeNo.Contains(ballHolder->tileNo)) {
				Cross();

				return;
			}
		}
	}
	// ***

	// *** デゥエル ***
	if (ballHolder) {
		// *once処理
		if (isDueling) {
			if (ballHolder->isMoving == false) isDueling = false; // フラグ切り替え (移動処理終了後)
			
			return;
		}
		// デゥエル処理
		if (ballHolder->position > C_Common::GK_POSITION && GetIsFree(ballHolder) == false) { // GK以外 かつ、接敵している
			if (ballHolder->ballKeepingCount > 1.0f) { // インターバル設定
				isDueling = true; // デゥエル開始
				Duel();
				
				return;
			}
		}
	}
	// ***

	// *** ラインブレイク　***
	if (ballHolder) {
		// *once処理
		if (isLineBreak) {
			if (ballHolder->isMoving == false) isLineBreak = false; // フラグ切り替え (移動処理終了後)

			return;
		}
		if (ballHolder->ballKeepingCount > 1.0f) { // インターバル設定
			int _ballHolderLine = ballHolder->tileNo / C_Common::TILE_NUM_Y; // ボールホルダーライン
			if (deffenceLine < _ballHolderLine) {
				isLineBreak = true;
				LineBreak();

				return;
			}

		}
	}

	// -------------- コマンド処理 ---------------------
	// *** 制限 ***
	if (ball->isMoving) return; // ボール移動中
	// ***********


	// *** プレス回避行動 ***
	if (currentCommand == C_Common::ESCAPE_PRESSING_COMMAND_NO) {
		if (ballHolder == nullptr) return;
		if (ballHolder->ballKeepingCount < ESCAPE_INTERVAL) return; // インターバル設定

		// -- ボール保持チーム取得 --
		TArray<AC_Player*> _myTeamPlayers = {}; // ボール保持チーム
		if (ballHolder->ActorHasTag(FName("HOME"))) {
			_myTeamPlayers = homePlayers;
		}
		else {
			_myTeamPlayers = awayPlayers;
		}
		// -- DFラインのプレイヤー取得 --
		TArray<AC_Player*> _DFPlayers = {}; // DFプレイヤー
		for (AC_Player* _p : _myTeamPlayers) {
			if (_p->position <= C_Common::RSB_POSITION) {
				_DFPlayers.Add(_p);
			}
		}
		
		// -- パス先のプレイヤー取得 --
		AC_Player* _targetPlayer = nullptr; // パス先のプレイヤー
		bool _isBreak = false; // *2重loop対応
		if (ballHolder->position == C_Common::GK_POSITION) {
			// ●ボール保持者がGK ( 左右へ展開 )
			for (int _i = C_Common::RSB_POSITION; _i > 0; _i--) { // LSB(1) ← RSB(5)
				for (AC_Player* _player : _DFPlayers) {
					
					if (GetIsFree(_player) == false) continue; // *フリーのプレイヤーのみ

					if (_i == _player->position && GKEscapeToPlayers.Contains(_player) == false) { // ポジション位置 && まだエスケープしてない
						_targetPlayer = _player;
						GKEscapeToPlayers.Add(_targetPlayer); // 一時保存
						_isBreak = true; // *2重loop対応

						break;
					}
				}

				if (_isBreak) break; // *2重loop対応
			}
		}
		else {
			// 〇ボール保持者がGK以外
			for (AC_Player* _p : _myTeamPlayers) {
				if ((_p->position == C_Common::GK_POSITION)) { // 必ずGKにパス
					_targetPlayer = _p;

					break;
				}
			}
		}
		
		if (_targetPlayer == nullptr) { // *プレイヤーが取得できなかった場合
			GKEscapeToPlayers = {}; // 配列リセット

			return;
		}

		// -- ショートパス --
		ShortPass(_targetPlayer);
	}
	// ***

	
	// *** ロングアタック行動 ***
	if (currentCommand == C_Common::LONG_ATTACK_COMMAND_NO) {
		if (ballHolder == nullptr) return;
		if (ballHolder->position >= C_Common::LH_POSITION) return; // DFのプレイヤーのみ発動

		// -- チーム取得 --
		bool _isHome = false; // Homeか
		TArray<AC_Player*> _myTeamPlayers = {}; // マイチーム
		if (ballHolder->ActorHasTag(FName("HOME"))) {
			_isHome = true;
			_myTeamPlayers = homePlayers;
		}
		else {
			_myTeamPlayers = awayPlayers;
		}

		// -- FWプレイヤー取得 --
		TArray<AC_Player*> _myFWPlayers = {}; // FWプレイヤー
		for (AC_Player* _p : _myTeamPlayers) {
			if ((_p->position >= C_Common::LWG_POSITION)) {
				_myFWPlayers.Add(_p);

				break;
			}
		}

		// -- レーンの取得 --
		int _kickerLane = 0; // キッカーのレーン(左から1～5レーン)
		if (ballHolder->position == C_Common::GK_POSITION) {
			_kickerLane = 3;
		}
		else {
			_kickerLane = ballHolder->position;
		}

		// -- 走るプレイヤー取得 --
		AC_Player* _runPlayer = nullptr;
		for (AC_Player* _p : _myFWPlayers) {
			if ((_p->position % 5 == _kickerLane)) { // キッカーと同レーンのFW
				_runPlayer = _p;

				break;
			}
		}
		
		// -- ポイントの取得 (キッカーと同じレーン) --
		FVector _targetLocation = FVector(0, 0, 0);
		if (_isHome) {
			_targetLocation = HOME_LONG_ATTACK_POINTS[_kickerLane - 1];
		}
		else {
			_targetLocation = AWAY_LONG_ATTACK_POINTS[_kickerLane - 1];
		}

		// -- 裏抜け --
		if (_runPlayer) GetBehind(_runPlayer, _targetLocation);

		// -- ロングキック --
		LongKick(_targetLocation);
	}
	// ***


	// *** レーンアタック行動 ***
	if (currentCommand == C_Common::LANE_ATTACK_COMMAND_NO) {
		if (ballHolder == nullptr) return;

		// -- チーム取得 --
		TArray<AC_Player*> _myTeamPlayers = {}; // マイチーム
		if (ballHolder->ActorHasTag(FName("HOME"))) {
			_myTeamPlayers = homePlayers;
		}
		else {
			_myTeamPlayers = awayPlayers;
		}

		// -- レーン前方の味方プレイヤー取得 --
		int _frontTileNo = (_myTeamPlayers[0]->ActorHasTag("HOME")) ? ballHolder->tileNo + C_Common::TILE_NUM_Y : ballHolder->tileNo - C_Common::TILE_NUM_Y;
		AC_Player* _frontPlayer = nullptr; // 前方のプレイヤー
		for (AC_Player* _player : _myTeamPlayers) {
			if (_player->tileNo == _frontTileNo) {
				_frontPlayer = _player;

				break;
			}
		}

		// -- アクション --
		if (_frontPlayer)
		{
			// 〇ショートパス
			ShortPass(_frontPlayer);
		}
		else {
			// ●ポストプレー
			
			// プレイヤー取得
			AC_Player* _postPlayer = nullptr; // ポストプレイヤー
			for (AC_Player* _player : _myTeamPlayers) {
				if (_player->ActorHasTag("POST")) {
					_postPlayer = _player;

					break;
				}
			}
			if (_postPlayer == nullptr) return;

			// ボールホルダーと隣以内のレーンか判定
			int _holderLane = (ballHolder->tileNo - 1) % C_Common::TILE_NUM_Y;
			int _postLane = (_postPlayer->tileNo - 1) % C_Common::TILE_NUM_Y;
			if (FMath::Abs(_holderLane - _postLane) > 1) return;

			// ボールホルダーより前にいるか判定
			int _holderLine = (ballHolder->tileNo - 1) / C_Common::TILE_NUM_Y;
			int _postLine = (_postPlayer->tileNo - 1) / C_Common::TILE_NUM_Y;
			if (_holderLine >= _postLine) return;

			// アクション
			postPlayer = _postPlayer; // 一時保存
			PostPlay();
		}
	}
	// ***
}

// ********* input *********
// 入力設定
void AC_Opening_Level_Instance::SetupInput()
{
	// 入力を有効にする
	EnableInput(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	// 左クリックのプレスをバインド
	InputComponent->BindKey(EKeys::LeftMouseButton, IE_Pressed, this, &AC_Opening_Level_Instance::PressedLeft);
	// Wキープレスをバインド
	InputComponent->BindKey(EKeys::W, IE_Pressed, this, &AC_Opening_Level_Instance::PressedW);
	// Aキープレスをバインド
	InputComponent->BindKey(EKeys::A, IE_Pressed, this, &AC_Opening_Level_Instance::PressedA);
	// Sキープレスをバインド
	InputComponent->BindKey(EKeys::S, IE_Pressed, this, &AC_Opening_Level_Instance::PressedS);
	// Dキープレスをバインド
	InputComponent->BindKey(EKeys::D, IE_Pressed, this, &AC_Opening_Level_Instance::PressedD);
}

// 左クリック(プレス)イベント
// ●(プレイヤー選択)デカール表示
// ●試合開始処理
void AC_Opening_Level_Instance::PressedLeft()
{
	UMy_Game_Instance* _instance = Cast<UMy_Game_Instance>(UGameplayStatics::GetGameInstance(GetWorld())); // ゲームインスタンス
	if (_instance) {
		if (_instance->game_phase == C_Common::MANAGER_SELECT_BEFORE_PHASE || _instance->game_phase == C_Common::MANAGER_SELECT_AFTER_PHASE) { // *(制限)マネージャー選択(前・後)フェーズのみ
			
			// マウス位置情報取得処理
			TArray<TEnumAsByte<EObjectTypeQuery>> _objectTypes = {}; // 取得するオブジェクトタイプ
			_objectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_PhysicsBody)); // プレイヤー: PhysicsBody
			FHitResult hitResult; // オブジェクト情報
			bool _isPhysicsBodyExist = GetResultFromMouseLocation(hitResult, _objectTypes);
			
			if (_isPhysicsBodyExist == false) return; // PhisicsBody(マネージャーメッシュ)以外


			// ●デカール表示
			if (managerSelectedDecalMaterial) // マテリアルが取得できている
			{
				FVector _location = hitResult.GetActor()->GetActorLocation();
				_location.Z = 0; // 高さ調整
				FRotator _rotation = GetActorRotation();
				FVector _size = FVector(0.4f, 0.4f, 0.4f);
				managerSelectedDecalActor = GetWorld()->SpawnActor<ADecalActor>( // アクタースポーン
					_location,
					_rotation
				);
				// マテリアル表示
				if (managerSelectedDecalActor)
				{
					managerSelectedDecalActor->SetDecalMaterial(managerSelectedDecalMaterial); // マテリアルセット
					managerSelectedDecalActor->SetActorScale3D(_size); // サイズセット
				}
			}
			
			// ●フェーズ変更 (マネージャー選択後フェーズへ)
			if (_instance->game_phase == C_Common::MANAGER_SELECT_BEFORE_PHASE) {
				_instance->game_phase = C_Common::MANAGER_SELECT_AFTER_PHASE; // フェーズ
				openingUI->SwitchWidgetPanal(C_Common::MANAGER_SELECT_AFTER_PHASE); // パネル
			}
		}
	}
}

// Wキー(プレス)イベント
void AC_Opening_Level_Instance::PressedW()
{
	UMy_Game_Instance* _instance = Cast<UMy_Game_Instance>(UGameplayStatics::GetGameInstance(GetWorld())); // ゲームインスタンス
	if (_instance == nullptr)  return;
	if (_instance->game_phase != C_Common::MATCH_PHASE) return; // *(制限)試合フェーズのみ

	_instance->command = C_Common::LONG_ATTACK_COMMAND_NO; // ロングアタック変更
}

// Aキー(プレス)イベント
void AC_Opening_Level_Instance::PressedA()
{
	UMy_Game_Instance* _instance = Cast<UMy_Game_Instance>(UGameplayStatics::GetGameInstance(GetWorld())); // ゲームインスタンス
	if (_instance == nullptr)  return;
	if (_instance->game_phase != C_Common::MATCH_PHASE) return; // *(制限)試合フェーズのみ

	_instance->command = C_Common::ESCAPE_PRESSING_COMMAND_NO; // プレス回避変更
}

// Sキー(プレス)イベント
void AC_Opening_Level_Instance::PressedS()
{
	UMy_Game_Instance* _instance = Cast<UMy_Game_Instance>(UGameplayStatics::GetGameInstance(GetWorld())); // ゲームインスタンス
	if (_instance == nullptr)  return;
	if (_instance->game_phase != C_Common::MATCH_PHASE) return; // *(制限)試合フェーズのみ

	_instance->command = 0; // アイドル変更
}

// Dキー(プレス)イベント
void AC_Opening_Level_Instance::PressedD()
{
	UMy_Game_Instance* _instance = Cast<UMy_Game_Instance>(UGameplayStatics::GetGameInstance(GetWorld())); // ゲームインスタンス
	if (_instance == nullptr)  return;
	if (_instance->game_phase != C_Common::MATCH_PHASE) return; // *(制限)試合フェーズのみ

	_instance->command = C_Common::LANE_ATTACK_COMMAND_NO; // レーン攻撃変更
}

// ホバー処理
// ●マネージャーアウトライン表示・非表示
void AC_Opening_Level_Instance::Hover()
{
	// -- アウトライン表示・非表示 --
	TArray<TEnumAsByte<EObjectTypeQuery>> _objectTypes = {}; // 取得するオブジェクトタイプ
	_objectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_PhysicsBody)); // プレイヤー: PhysicsBody
	FHitResult hitResult; // オブジェクト情報
	// マウス位置情報取得処理
	bool _isPhysicsBodyExist = GetResultFromMouseLocation(hitResult, _objectTypes);
	
	if (_isPhysicsBodyExist == false) { // PhisicsBody(マネージャーメッシュ)以外
		// 〇アウトライン非表示
		if (currentHoverManager) {  // *既にアウトライン表示されている
			currentHoverManager->HiddenOutline();
			currentHoverManager = nullptr;
		}

		return;
	}

	// ●アウトライン表示
	AC_Manager* _manager = Cast<AC_Manager>(hitResult.GetActor()); // マネージャー取得
	if (currentHoverManager) return; // *既にアウトライン表示されている

	currentHoverManager = _manager; // 保存
	_manager->VisibleOutline();
	// --
}

// マウス位置に目的のオブジェクトがあるか判定して情報取得
// | retrun: 目的のオブジェクトか判定, hitRusult: 取得するオブジェクト情報, objectTypes: 目的のオブジェクトの種類(コリジョン) |
bool AC_Opening_Level_Instance::GetResultFromMouseLocation(FHitResult& hitResult, TArray<TEnumAsByte<EObjectTypeQuery>> objectTypes)
{
	// マウスの位置を取得
	FVector _mouseLocation; // *マウス位置
	FVector _mouseDirection; // マウス方向
	APlayerController* _myPlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0); // プレイヤーコントローラー
	_myPlayerController->DeprojectMousePositionToWorld(_mouseLocation, _mouseDirection); // マウス位置を３D座標で取得

	// マウス方向を変換
	const int _distance = 10000;
	_mouseDirection = (_mouseDirection * _distance) + _mouseLocation;

	// マウス位置に目的のオブジェクトタイプがあるか判定
	TArray<AActor*> _ignoreActors = {}; // 無視するアクター
	EDrawDebugTrace::Type _drawDebugType = EDrawDebugTrace::Type::None; // デバッグモード

	// オブジェクト有無の判定
	bool _isExist = UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(), _mouseLocation, _mouseDirection, objectTypes, false, _ignoreActors, _drawDebugType, hitResult, true);


	return _isExist;
}

// 試合開始時処理
void AC_Opening_Level_Instance::MatchStart()
{
	UMy_Game_Instance* _instance = Cast<UMy_Game_Instance>(UGameplayStatics::GetGameInstance(GetWorld())); // ゲームインスタンス
	if (_instance == nullptr) return;

	// -- デカール・アウトライン削除 --
	if (managerSelectedDecalActor) managerSelectedDecalActor->Destroy();
	if (currentHoverManager) currentHoverManager->HiddenOutline();
	// --


	// -- プレイヤーを表示 --
	for (AC_Player* _p : allPlayers) {
		_p->DisplayMesh();
	}
	// --


	// -- ボールホルダー決定 --
	for (AC_Player* _p : allPlayers) {
		if (_p->ActorHasTag(FName("GK")) && _p->ActorHasTag(FName("HOME"))) { // GK かつ　HOMWのプレイヤー
			SetBallHolder(_p);

			break;
		}
	}
	// --


	// -- 非保持チームをミドルラインへ移動 --
	// (*AWAYチーム)
	for (AC_Player* _p : awayPlayers) { // AWAYチーム
		if (_p->position == C_Common::GK_POSITION) continue;

		FVector _targetLocation = _p->GetActorLocation();
		_targetLocation.X -= C_Common::TILE_SIZE * 2;
		_p->RunTo(_targetLocation);
	}
	// --

	// -- 保持チームを相手のミドルラインへ移動 --
	// (*HOME)
	for (AC_Player* _p : homePlayers) { // HOMEチーム
		if (_p->position == C_Common::GK_POSITION) continue;

		FVector _targetLocation = _p->GetActorLocation();
		_targetLocation.X += C_Common::TILE_SIZE;
		_p->RunTo(_targetLocation);
	}
	// --
}

// ショートパス
void AC_Opening_Level_Instance::ShortPass(AC_Player* toPlayer)
{
	if (ballHolder) {
		// アニメーション
		ballHolder->ShotPass(toPlayer); // ショートパス
		toPlayer->Trap(ballHolder); // トラップ

		// ボールホルダー切り替え
		SetBallHolder(toPlayer);
	}
}

// ロングパス
void AC_Opening_Level_Instance::LongPass(AC_Player* toPlayer)
{
	if (ballHolder) {
		// アニメーション
		ballHolder->LongPass(toPlayer); // ロングパス
		toPlayer->Trap(ballHolder); // トラップ

		// ボールホルダー切り替え
		SetBallHolder(toPlayer);
	}
}

// スペースパス (ショートパス)
void AC_Opening_Level_Instance::SpacePass(FVector toLocation) {
	if (ballHolder) {
		// 処理
		ballHolder->SpacePass(toLocation);

		// ボールホルダーOFF
		ballHolder->isBallHolder = false;
		ballHolder = nullptr;
	}
}

// ロングキック
void AC_Opening_Level_Instance::LongKick(FVector toLocation)
{
	if (ballHolder) {
		// 処理
		ballHolder->LongKick(toLocation);

		// ボールホルダーOFF
		ballHolder->isBallHolder = false;
		ballHolder = nullptr;
	}
}

// シュート
void AC_Opening_Level_Instance::Shoot()
{
	if (ballHolder) {
		// 処理
		ballHolder->Shoot();

		// ボールホルダーOFF
		ballHolder->isBallHolder = false;
		ballHolder = nullptr;
		
		// ゴールフラグ
		isGoal = true;
	}
}

// 裏抜け (走る)
void AC_Opening_Level_Instance::GetBehind(AC_Player* runPlayer, FVector toLocation)
{
	runPlayer->RunTo(toLocation);
	getBehindingPlayer = runPlayer;
}

// ボールホルダー設定
void AC_Opening_Level_Instance::SetBallHolder(AC_Player* targetPlayer)
{
	// 前回のボールホルダーがいる
	if (ballHolder) {
		ballHolder->isBallHolder = false;
	}
	ballHolder = targetPlayer;
	ballHolder->isBallHolder = true;
}

// プレイヤーフリー判定
// ( true: フリー )
bool AC_Opening_Level_Instance::GetIsFree(AC_Player* targetPlayer)
{
	bool _isFree = false;

	for (AC_Tile* _tile : tiles) {
		if (_tile->tileNo == targetPlayer->tileNo) {
			if (_tile->onPlayerNum < 2) _isFree = true;

			break;
		}
	}
	return _isFree;
}

// デゥエル
void AC_Opening_Level_Instance::Duel()
{
	// オフェンス
	ballHolder->RegateDrrible(); // *レガテドリブル

	// ディフェンス
	TArray<AC_Player*> _deffencePlayers = {}; // ディフェンスチーム
	if (ballHolder->ActorHasTag(FName("HOME"))) {
		_deffencePlayers = awayPlayers;
	}
	else {
		_deffencePlayers = homePlayers;
	}
	for (AC_Player* _player : _deffencePlayers) {
		if (_player->tileNo == ballHolder->tileNo) {
			_player->Tackle(); // *タックル

			break;
		}
	}
	
}

// ラインブレイク
void AC_Opening_Level_Instance::LineBreak()
{
	// チーム分け
	TArray<AC_Player*> _offencePlayers = {}; // オフェンスチーム
	TArray<AC_Player*> _deffencePlayers = {}; // ディフェンスチーム
	if (ballHolder->ActorHasTag(FName("HOME"))) {
		_offencePlayers = homePlayers;
		_deffencePlayers = awayPlayers;
	}
	else {
		_offencePlayers = awayPlayers;
		_deffencePlayers = homePlayers;
	}

	// -- オフェンス (2タイル前進, ボールホルダー: コンドクシオン) --
	// ボールホルダー
	ballHolder->Drrible(); // 前進ドリブル
	
	// その他オフェンス
	for (AC_Player* _player : _offencePlayers) {
		if (_player->position == C_Common::GK_POSITION) continue; // GK(*制限)
		if (_player == ballHolder) continue; // ボールホルダー(*制限)

		FVector _location = _player->GetActorLocation(); // 移動位置
		if (_player->ActorHasTag(FName("HOME"))) {
			_location.X += (C_Common::TILE_SIZE * 2);
		}
		else {
			_location.X -= (C_Common::TILE_SIZE * 2);
		}
		_player->RunTo(_location);
	}
	// --
	

	// -- ディフェンス (2タイル後退) --
	for (AC_Player* _player : _deffencePlayers) {
		if (_player->position == C_Common::GK_POSITION) continue; // GK(*制限)

		FVector _location = _player->GetActorLocation(); // 移動位置
		if (_player->ActorHasTag(FName("HOME"))) {
			_location.X -= (C_Common::TILE_SIZE * 2);
		}
		else {
			_location.X += (C_Common::TILE_SIZE * 2);
		}
		_player->RunTo(_location);
	}
	// --
}

// クロス
void AC_Opening_Level_Instance::Cross()
{
	if (ballHolder) {
		// チーム取得
		TArray<AC_Player*> _myTeamPlayers = {}; // マイチーム
		if (ballHolder->ActorHasTag(FName("HOME"))) {
			_myTeamPlayers = homePlayers;
		}
		else {
			_myTeamPlayers = awayPlayers;
		}

		// ターゲット取得
		AC_Player* _targetPlayer = nullptr; // ターゲット
		for (AC_Player* _player : _myTeamPlayers) {
			if (_player->ActorHasTag("TARGET")) {
				_targetPlayer = _player;

				break;
			}
		}
		if (_targetPlayer == nullptr) return;

		// アニメーション
		ballHolder->LongPass(_targetPlayer); // ロングパス
		_targetPlayer->Trap(ballHolder); // トラップ

		// ボールホルダー切り替え
		SetBallHolder(_targetPlayer);
	}
}

// ポストプレー
void AC_Opening_Level_Instance::PostPlay()
{
	postPlayCount++; // インクリメント

	if (postPlayCount == 1) {
		// ⓵ポストプレイヤーにパスアンドゴー
		// 移動位置取得
		FVector _ToLocation = ballHolder->GetActorLocation();
		passAndGoPlayer = ballHolder; // *一時保存
		if (ballHolder->ActorHasTag("HOME")) {
			_ToLocation.X += C_Common::TILE_SIZE;
		}
		else {
			_ToLocation.X -= C_Common::TILE_SIZE;
		}

		if (postPlayer == nullptr) return;

		// アクション
		ShortPass(postPlayer); // ショートパス
		passAndGoPlayer->RunTo(_ToLocation); // 前進
	}
	else if (postPlayCount == 2)
	{
		// ⓶折り返しのパスを出す
		FVector _location = postPlayer->GetActorLocation();
		_location.Y += C_Common::TILE_SIZE;
		SpacePass(_location); // スペースパス

		SetBallHolder(passAndGoPlayer);
	}
	else {
		// 終了処理
		postPlayCount = 0;
		passAndGoPlayer = nullptr;
		postPlayer = nullptr;
	}
}
