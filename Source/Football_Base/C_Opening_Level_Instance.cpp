// Fill out your copyright notice in the Description page of Project Settings.


#include "C_Opening_Level_Instance.h"
#include <Kismet/KismetSystemLibrary.h>
#include <Kismet/GameplayStatics.h>
#include "My_Game_Instance.h"
#include <Kismet/KismetMathLibrary.h>

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

	// *** プレイヤーBP取得 ***
	ConstructorHelpers::FClassFinder<AC_Player> _playerBPFinder(TEXT("/Game/Blue_Print/BP_Player"));
	if (_playerBPFinder.Class) {
		playerSubClass = _playerBPFinder.Class;
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
	
	// ***** マネージャー選択フェーズ *****
	if (_instance->game_phase == C_Common::MANAGER_SELECT_BEFORE_PHASE) {
		// -- ホバー許可 --
		Hover();


		return;
	}
	// *****


	// ***** プレイヤー選択・配置フェーズ *****
	if (_instance->game_phase == C_Common::PLAYER_SELECT_PLACE_PHASE) {
		// -- マウス追従 (選択時) --
		if (isPlayerGrap) {
			FollowPlayerToMouse();
		}
		// -- スポーンプレイヤー (プールから選択された) --
		if (_instance->pool_selected_player_type >= 0) {
			SpawnPlayerInPool(_instance->pool_selected_player_type);

			_instance->pool_selected_player_type = -1; // リセット
		}

		return;
	}
	// *****


	// ***** 試合準備フェーズ *****
	if (_instance->game_phase == C_Common::MATCH_READY_PHASE) {
		// -- 試合開始 --
		if (isOnceMatchStart)
		{
			isOnceMatchStart = false;
			MatchStart();
		}

		// -- フェーズ切替 --
		if (_instance->phase_count < 1.0f) {
			_instance->game_phase = C_Common::MATCH_PHASE; // 試合フェーズへ
			_instance->phase_count = C_Common::MATCH_TIME; // カウンターセット

			openingUI->SetVisibleHasLabel(true); // ボール保持・非保持ラベル表示
		}

		return;
	}
	// *****


	// ***** 試合フェーズ *****
	if (_instance->game_phase != C_Common::MATCH_PHASE) return;
	if (ball == nullptr) return; // ボール取得確認

	
	// *** 試合終了時 (試合時間終了) ***
	if (_instance->phase_count < 1) {
		// -- プレイヤー選択・配置フェーズへ --
		_instance->game_phase = C_Common::PLAYER_SELECT_PLACE_PHASE; // フェーズ
		openingUI->SwitchButtonPanal(0); // 保持・非保持ボタン
		openingUI->SetVisibleHasLabel(false); // 保持・非保持ラベル

		// -- フラグリセット --
		isGoal = false;
		goalCount = 0;
		isOnceGoal = false;
		isOnceMatchStart = true;

		// -- プレイヤーアニメーション停止 --
		for (AC_Player* _player : allPlayers) {
			_player->StopAnim();
		}

		// -- プレイヤーを元の位置へ --
		for (int _i = 0; _i < playerInitialPlaceLocation.Num(); _i++) {
			// ●位置
			allPlayers[_i]->SetActorLocation(playerInitialPlaceLocation[_i]);
			// ●向き
			FRotator _rotation = FRotator(0, 0, 0);
			// ゴール方向へ回転
			if (allPlayers[_i]->ActorHasTag("HOME")) {
				_rotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), GetActorLocation() + FVector(100.0f, 0, 0));
			}
			else {
				_rotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), GetActorLocation() + FVector(-100.0f, 0, 0));
			}
			FQuat _q = _rotation.Quaternion(); // 変換
			_q.X = 0; // *Z軸のみ回転させる
			_q.Y = 0; // *Z軸のみ回転させる
			allPlayers[_i]->SetActorRotation(_q);
		}


		return;
	}
	// ***


	// *** ゴール時 ***
	if (isGoal && ball->isMoving == false) {
		goalCount += DeltaTime;

		// -- 試合再開処理 --
		if (goalCount > 3.0f) {
			// ●フラグリセット
			isGoal = false;
			goalCount = 0;
			isOnceGoal = false;
			// ●プレイヤーアニメーション停止
			for (AC_Player* _player : allPlayers) {
				_player->StopAnim();
			}
			// ●プレイヤーを元の位置へ
			for (int _i = 0; _i < playerInitialPlaceLocation.Num(); _i++) {
				// ●位置
				allPlayers[_i]->SetActorLocation(playerInitialPlaceLocation[_i]);
				// ●向き
				FRotator _rotation = FRotator(0,0,0);
				// ゴール方向へ回転
				if (allPlayers[_i]->ActorHasTag("HOME")) {
					_rotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), GetActorLocation() + FVector(100.0f, 0, 0));
				}
				else {
					_rotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), GetActorLocation() + FVector(-100.0f, 0, 0));
				}
				FQuat _q = _rotation.Quaternion(); // 変換
				_q.X = 0; // *Z軸のみ回転させる
				_q.Y = 0; // *Z軸のみ回転させる
				allPlayers[_i]->SetActorRotation(_q);
			}
			// ●試合開始処理
			MatchStart();
			

			return;
		}


		// -- ゴールアニメーション処理 --
		if (isOnceGoal) return; // 一度のみの処理
		isOnceGoal = true; // フラグON
		
		// ●ゴール・被ゴールチーム取得
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
		// ●アニメーション
		// - ゴールチーム -
		for (AC_Player* _player : _scoreTeam) {
			_player->CheerMotion();
		}
		// - 被ゴールチーム -
		for (AC_Player* _player : _notScoreTeam) {
			_player->SadMotion();
		}
		// - HOMEマネージャー -
		if (homeManager) homeManager->CheerAnim();


		return;
	}
	// ***

	// *** ボール保持切替(クリアボール時) ***
	if (ballHolder == nullptr && ball->isMoving == false) {

		// GK取得(AWAY)
		AC_Player* _gk = nullptr;
		for (AC_Player* _p : awayPlayers) {
			if (_p->position == C_Common::GK_POSITION) {
				_gk = _p;

				break;
			}
		}
		if (_gk == nullptr) return;
		
		// ボールへ移動
		if (ball == nullptr) return;
		FVector _ballLocation = ball->GetActorLocation();
		RunTo(_gk, _ballLocation);
		
		// ボールホルダー設定
		SetBallHolder(_gk);
		if (ballHolder == nullptr) return;

		// ボール保持・非保持ボタン・ラベル切替
		if (ballHolder->ActorHasTag("HOME")) {
			openingUI->SwitchButtonPanal(1); // ボタン
			openingUI->SwitchHasLabelPanal(0); // ラベル
		}
		else {
			openingUI->SwitchButtonPanal(2); // ボタン
			openingUI->SwitchHasLabelPanal(1); // ラベル
		}

		return;
	}
	// ***


	// *** コマンド変更監視 ***
	if (_instance->command != currentCommand) { // コマンド変更がされた時
		currentCommand = _instance->command; // コマンド一時保存

		// -- マネージャーアニメーション変更 --
		if (homeManager) homeManager->ChangeAnim(currentCommand);
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
	

	// *** ポケットマン処理 ***
	if (isPoketman) {
		if (currentPoketman->isMoving) return; // ポケットマン移動中
		if (ball) {
			if (ball->isMoving) return; // ボール移動中
		}

		// -- 実行処理 --
		poketmanPlayCount++; // インクリメント

		if (poketmanPlayCount == 1) {
			// < パスポイントへ移動 >
			FVector _location = FVector(-577.0f, 0, 100.0f); // (*仮) CBの位置
			poketmanFromLocation = currentPoketman->GetActorLocation(); // 移動前の位置
			RunTo(currentPoketman, _location);
		}
		else if (poketmanPlayCount == 2) {
			// < パスを受ける >
			ShortPass(currentPoketman);
		}
		else {
			// 終了
			isPoketman = false;
		}

		return;
	}
	// --

	// -- 元の位置に移動・リセット --
	if (poketmanPlayCount > 0) {
		if (currentPoketman != ballHolder) { // ボールホルダーでない
			if (ball->isMoving == false) { // ボール移動中以外

				// < 元の位置に移動 >
				RunTo(currentPoketman, poketmanFromLocation);

				// リセット
				poketmanPlayCount = 0;
				currentPoketman = nullptr;
				poketmanFromLocation = FVector(0, 0, 0);
			}
		}
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
		if (currentCommand != C_Common::POSSETION_COMMAND_NO) { // *ポゼッションコマンド制限 (HOME)
			if (awayCommand != C_Common::POSSETION_COMMAND_NO) { // *ポゼッションコマンド制限 (AWAY)
				// デゥエル処理
				if (ballHolder->position > C_Common::GK_POSITION && GetIsFree(ballHolder) == false) { // GK以外 かつ、接敵している
					// -- ディフェンス姿勢セット --
					TArray<AC_Player*> _deffencePlayers = {}; // ディフェンスチーム
					if (ballHolder->ActorHasTag(FName("HOME"))) {
						_deffencePlayers = awayPlayers;
					}
					else {
						_deffencePlayers = homePlayers;
					}
					for (AC_Player* _player : _deffencePlayers) {
						if (_player->tileNo == ballHolder->tileNo) {
							_player->SetDefensiveStance(); // ディフェンス姿勢動作

							break;
						}
					}
					// -- デゥエル --
					if (ballHolder->ballKeepingCount > 1.0f) { // インターバル設定
						isDueling = true; // デゥエル開始
						Duel();

						return;
					}
				}
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
			int _ballHolderLine = (ballHolder->tileNo - 1) / C_Common::TILE_NUM_Y; // ボールホルダーライン
			// ディフェンスラインを超えたか確認
			bool _isOverDeffenceLine = ballHolder->ActorHasTag("HOME") ? homeDeffenceLine < _ballHolderLine : awayDeffenceLine > _ballHolderLine;
			if (_isOverDeffenceLine) {
				isLineBreak = true;
				LineBreak();

				return;
			}

		}
	}

	// *** アピール ***
	if (ballHolder) {
		if (ballHolder != preBallHolder) { // ボールホルダーが変更した時のみ
			preBallHolder = ballHolder; // *変更時一回の処理
			// -- ボール保持チーム取得 --
			TArray<AC_Player*> _myTeamPlayers = {}; // ボール保持チーム
			if (ballHolder->ActorHasTag(FName("HOME"))) {
				_myTeamPlayers = homePlayers;
			}
			else {
				_myTeamPlayers = awayPlayers;
			}
			
			// -- アクション ---
			for (AC_Player* _p : _myTeamPlayers) {
				if (_p->isMoving) continue; // 移動中はなし
				if (_p == ballHolder) continue; // ボールホルダーなし

				if (GetIsFree(_p)) _p->FreeAppeal(ballHolder); // ●フリー
				if (_p->ActorHasTag("TARGET")) _p->TargetmanAppeal(ballHolder); // ●ターゲットマン
				if (_p->ActorHasTag("POST")) _p->PostmanAppeal(ballHolder); // ●ポストマン
				if (_p->ActorHasTag("RUNNER")) _p->RunnerAppeal(); // ●ランナー
				if (_p->ActorHasTag("POKET")) _p->PoketmanAppeal(ballHolder); // ●ポケットマン
			}
		}
	}

	
	// ***** コマンド操作 *****
	// * 制限 *
	if (ball->isMoving) return; // ボール移動中
	if (ballHolder == nullptr) return; // ボールホルダーなし
	// *

	// -- AWAY (敵AI) --
	//AwayTeamMovement();
	// ---

	// -- ボール保持がHOME --
	if (ballHolder->ActorHasTag("HOME")) {
		// ⓵ボール保持コマンド (HOME)
		if (currentCommand == C_Common::POSSETION_COMMAND_NO) PossetionCommand();
		else if (currentCommand == C_Common::LONG_ATTACK_COMMAND_NO) LongAttackCommand();
		else if (currentCommand == C_Common::TECNICAL_ATTACK_COMMAND_NO) TecnicalAttackCommand();
		// ⓶ボール非保持コマンド (AWAY)
		if (awayCommand == C_Common::LOW_BLOCK_COMMAND_NO) LowBlockCommand();
		else if (awayCommand == C_Common::SIDE_PRESS_COMMAND_NO) SidePressCommand();
		else if (awayCommand == C_Common::HIGH_PRESS_COMMAND_NO) HighPressCommand();
	}
	// -- ボール保持がAWAY --
	else {
		// ⓵ボール保持コマンド (AWAY)
		if (awayCommand == C_Common::POSSETION_COMMAND_NO) PossetionCommand();
		else if (awayCommand == C_Common::LONG_ATTACK_COMMAND_NO) LongAttackCommand();
		else if (awayCommand == C_Common::TECNICAL_ATTACK_COMMAND_NO) TecnicalAttackCommand();
		// ⓶ボール非保持コマンド (HOME)
		if (currentCommand == C_Common::LOW_BLOCK_COMMAND_NO) LowBlockCommand();
		else if (currentCommand == C_Common::SIDE_PRESS_COMMAND_NO) SidePressCommand();
		else if (currentCommand == C_Common::HIGH_PRESS_COMMAND_NO) HighPressCommand();
	}
	// *****
}

// ********* input *********
// 入力設定
void AC_Opening_Level_Instance::SetupInput()
{
	// 入力を有効にする
	EnableInput(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	// 左クリックのプレスをバインド
	InputComponent->BindKey(EKeys::LeftMouseButton, IE_Pressed, this, &AC_Opening_Level_Instance::PressedLeft);
	// 左クリックのリリースをバインド
	InputComponent->BindKey(EKeys::LeftMouseButton, IE_Released, this, &AC_Opening_Level_Instance::ReleasedLeft);
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
void AC_Opening_Level_Instance::PressedLeft()
{
	UMy_Game_Instance* _instance = Cast<UMy_Game_Instance>(UGameplayStatics::GetGameInstance(GetWorld())); // ゲームインスタンス
	if (_instance) {

		// *** マネージャー選択フェーズ ***
		if (_instance->game_phase == C_Common::MANAGER_SELECT_BEFORE_PHASE) {
			
			// マウス位置情報取得処理
			TArray<TEnumAsByte<EObjectTypeQuery>> _objectTypes = {}; // 取得するオブジェクトタイプ
			_objectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_PhysicsBody)); // プレイヤー: PhysicsBody
			FHitResult hitResult; // オブジェクト情報
			bool _isPhysicsBodyExist = GetResultFromMouseLocation(hitResult, _objectTypes);
			
			// *制限*
			if (_isPhysicsBodyExist == false) return; // PhisicsBody確認
			if (hitResult.GetActor()->ActorHasTag("MANAGER") == false) return; // マネージャー確認

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
			
			// ●フェーズ変更 (プレイヤー選択・配置フェーズへ)
			if (_instance->game_phase == C_Common::MANAGER_SELECT_BEFORE_PHASE) {
				_instance->game_phase = C_Common::PLAYER_SELECT_PLACE_PHASE; // フェーズ
				openingUI->SwitchWidgetPanal(2); // パネル (マッチパネルへ)
			}
		}
		// ***


		// *** プレイヤー選択・配置フェーズ ***
		if (_instance->game_phase == C_Common::PLAYER_SELECT_PLACE_PHASE)
		{
			// -- プレイヤー選択 --
			PressedLeftForPlayerSelect();
		}
		// ***
	}
}

// 左クリック(リリース)イベント
void AC_Opening_Level_Instance::ReleasedLeft()
{
	UMy_Game_Instance* _instance = Cast<UMy_Game_Instance>(UGameplayStatics::GetGameInstance(GetWorld())); // ゲームインスタンス
	if (_instance == nullptr) return;
	// *制限*
	if (_instance->game_phase != C_Common::PLAYER_SELECT_PLACE_PHASE) return; // プレイヤー選択フェーズ確認
	if (isPlayerGrap == false) return; // プレイヤー選択中か確認
	if (selectedPlayer == nullptr) return; // プレイヤー取得できているか確認

	// -- プレイヤーをタイルに移動 --
	// プレイヤータイルNo更新
	int _fromTileNo = selectedPlayer->tileNo; // 移動前タイルNo
	int _toTileNo = selectedPlayer->GetTileNoFromLocation(); // 移動先のタイルNo
	// タイル取得
	AC_Tile* _tile = nullptr;
	for (AC_Tile* _t : tiles) {
		if (_t->tileNo == _toTileNo) {
			_tile = _t;

			break;
		}
	}
	// 移動
	if (_tile) {
		// < タイル位置へ (選択されたプレイヤー) >
		FVector _location = _tile->GetActorLocation(); // 位置
		_location.Z = C_Common::PLAYER_BASE_LOCATION_Z;
		_location.X -= 82;
		selectedPlayer->SetActorLocation(_location);
		
		// < プレイヤー入れ替え処理 (移動先タイルにいたプレイヤー移動) >
		// 移動先タイルにプレイヤーがいるか確認
		AC_Player* _toExistPlayer = nullptr; // 移動先すでにいるプレイヤー
		for (AC_Player* _p : homePlayers) {
			if (_p->tileNo == _toTileNo) {
				_toExistPlayer = _p;

				break;
			}
		}
		// プレイヤーがいれば選択プレイヤーの元の位置へ
		if (_toExistPlayer) {
			AC_Tile* _fromTile = nullptr;
			for (AC_Tile* _t : tiles) { // 元の位置タイル取得
				if (_t->tileNo == _fromTileNo) {
					_fromTile = _t;

					break;
				}
			}
			// 移動処理
			if (_fromTile) {
				// ●タイルへ
				FVector _fromTileLocation = _fromTile->GetActorLocation(); // 位置
				_fromTileLocation.Z = C_Common::PLAYER_BASE_LOCATION_Z;
				_fromTileLocation.X -= 82;
				_toExistPlayer->SetActorLocation(_fromTileLocation);
				_toExistPlayer->tileNo = _fromTileNo; // タイルNo更新
			}
			else {
				// ●サブへ
				subPlayers.Add(_toExistPlayer); // 配列追加
				FVector _subLocation = SUB_LOCATION[subPlayers.Num() - 1]; // 位置
				_toExistPlayer->SetActorLocation(_subLocation);
			}
		}
	}
	else {
		// < サブ位置へ >
		subPlayers.Add(selectedPlayer); // 配列追加
		FVector _location = SUB_LOCATION[subPlayers.Num() - 1]; // 位置
		selectedPlayer->SetActorLocation(_location);
	}
	// 変数リセット
	isPlayerGrap = false; // フラグ切り替え
	selectedPlayer->tileNo = _toTileNo; // タイルNo更新
	selectedPlayer = nullptr;

	// -- マテリアル削除 --
	if (selectedTile) selectedTile->RemoveMainMaterial();
}

// プレイヤー選択
void AC_Opening_Level_Instance::PressedLeftForPlayerSelect()
{
	// -- マウス位置(クリック時)の情報を取得 --
	TArray<TEnumAsByte<EObjectTypeQuery>> _objectTypes = {}; // 取得するオブジェクトタイプ
	_objectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_PhysicsBody)); // プレイヤー: PhysicsBody
	FHitResult hitResult; // オブジェクト情報
	// マウス位置情報取得処理
	bool _isPhysicsBodyExist = GetResultFromMouseLocation(hitResult, _objectTypes);

	// * 制限 *
	if (_isPhysicsBodyExist == false) return;
	if (hitResult.GetActor()->ActorHasTag("HOME") == false) return; // HOMEプレイヤーのみ

	// -- プレイヤーを取得 --
	selectedPlayer = Cast<AC_Player>(hitResult.GetActor()); // プレイヤー取得 (キャスト)
	if (subPlayers.Contains(selectedPlayer)) subPlayers.Remove(selectedPlayer); // 配列削除 (*サブ対応)
	
	isPlayerGrap = true; // フラグON
}

// プレイヤーをマウスに追従させる (プレイヤー選択時)
void AC_Opening_Level_Instance::FollowPlayerToMouse()
{
	// -- プレイヤーをマウスに追従 --
	// マウス位置取得
	TArray<TEnumAsByte<EObjectTypeQuery>> _objectTypes = {}; // 取得するオブジェクトタイプ
	_objectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic)); // Blocking Volume
	FHitResult hitResult; // オブジェクト情報
	bool _isVolumeExist = GetResultFromMouseLocation(hitResult, _objectTypes); // マウス位置情報取得処理
	// * 制限 *
	if (_isVolumeExist == false) return; // Blocking Volume確認
	if (selectedPlayer == nullptr) return; // プレイヤー確認
	// プレイヤー位置セット
	FVector _location = hitResult.Location;
	_location.Z = C_Common::PLAYER_BASE_LOCATION_Z; // ベース位置設定
	selectedPlayer->SetActorLocation(_location); // 追従処理


	// -- タイルを光らせる --
	// ホバー中のタイル取得
	int _tileNo = selectedPlayer->GetTileNoFromLocation();
	AC_Tile* _tile = nullptr;
	for (AC_Tile* _t : tiles) {
		if (_t->tileNo == _tileNo) {
			_tile = _t;

			break;
		}
	}
	// マテリアル切替
	if (_tile == nullptr) return;
	if (_tile == selectedTile) return; // *重複マテリアルセットを防ぐ
	if (selectedTile) selectedTile->RemoveMainMaterial(); // (前回の)削除
	selectedTile = _tile;
	selectedTile->SetMaterial(); // セット
}

// Wキー(プレス)イベント
void AC_Opening_Level_Instance::PressedW()
{
	UMy_Game_Instance* _instance = Cast<UMy_Game_Instance>(UGameplayStatics::GetGameInstance(GetWorld())); // ゲームインスタンス
	if (_instance == nullptr)  return;
	if (_instance->game_phase != C_Common::MATCH_PHASE) return; // *(制限)試合フェーズのみ

	//_instance->command = C_Common::LONG_ATTACK_COMMAND_NO; // ロングアタック変更
}

// Aキー(プレス)イベント
void AC_Opening_Level_Instance::PressedA()
{
	UMy_Game_Instance* _instance = Cast<UMy_Game_Instance>(UGameplayStatics::GetGameInstance(GetWorld())); // ゲームインスタンス
	if (_instance == nullptr)  return;
	if (_instance->game_phase != C_Common::MATCH_PHASE) return; // *(制限)試合フェーズのみ

	//_instance->command = C_Common::ESCAPE_PRESSING_COMMAND_NO; // プレス回避変更
}

// Sキー(プレス)イベント
void AC_Opening_Level_Instance::PressedS()
{
	UMy_Game_Instance* _instance = Cast<UMy_Game_Instance>(UGameplayStatics::GetGameInstance(GetWorld())); // ゲームインスタンス
	if (_instance == nullptr)  return;
	if (_instance->game_phase != C_Common::MATCH_PHASE) return; // *(制限)試合フェーズのみ

	//_instance->command = 0; // アイドル変更
}

// Dキー(プレス)イベント
void AC_Opening_Level_Instance::PressedD()
{
	UMy_Game_Instance* _instance = Cast<UMy_Game_Instance>(UGameplayStatics::GetGameInstance(GetWorld())); // ゲームインスタンス
	if (_instance == nullptr)  return;
	if (_instance->game_phase != C_Common::MATCH_PHASE) return; // *(制限)試合フェーズのみ

	//_instance->command = C_Common::LANE_ATTACK_COMMAND_NO; // レーン攻撃変更
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
	
	// *制限*
	if (_isPhysicsBodyExist == false) { // PhisicsBody確認
		// 〇アウトライン非表示
		if (currentHoverManager) {  // *既にアウトライン表示されている
			currentHoverManager->HiddenOutline();
			currentHoverManager = nullptr;
		}

		return;
	}
	if (hitResult.GetActor()->ActorHasTag("MANAGER") == false) return; // マネージャー確認


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
	/*for (AC_Player* _p : allPlayers) {
		_p->DisplayMesh();
	}*/
	// --


	// -- ボールホルダー決定 --
	for (AC_Player* _p : allPlayers) {
		if (_p->ActorHasTag(FName("GK")) && _p->ActorHasTag(FName("HOME"))) { // GK かつ　HOMEのプレイヤー
			SetBallHolder(_p);

			break;
		}
	}
	// --

	// -- プレイヤー初期配置位置・タイルNo取得 --
	// 配列リセット
	playerInitialPlaceLocation.Empty();
	homeTeamIntialPlaceLocation.Empty();
	awayTeamIntialPlaceLocation.Empty();
	homeTeamIntialTileNo.Empty();
	awayTeamIntialTileNo.Empty();
	
	for (AC_Player* _p : allPlayers) {
		if (_p->ActorHasTag("HOME")) {
			homeTeamIntialPlaceLocation.Add(_p->GetActorLocation());
			homeTeamIntialTileNo.Add(_p->tileNo);
		}
		else {
			awayTeamIntialPlaceLocation.Add(_p->GetActorLocation());
			awayTeamIntialTileNo.Add(_p->tileNo);
		}
		playerInitialPlaceLocation.Add(_p->GetActorLocation());
	}
	// --


	// -- 非保持チームをミドルラインへ移動 --
	// (*AWAYチーム)
	for (AC_Player* _p : awayPlayers) { // AWAYチーム
		if (_p->position == C_Common::GK_POSITION) continue;

		FVector _targetLocation = _p->GetActorLocation();
		_targetLocation.X -= C_Common::TILE_SIZE * 2;
		RunTo(_p, _targetLocation);
	}
	// --

	// -- 保持チームを相手のミドルラインへ移動 --
	// (*HOME)
	for (AC_Player* _p : homePlayers) { // HOMEチーム
		if (_p->position == C_Common::GK_POSITION) continue;

		FVector _targetLocation = _p->GetActorLocation();
		_targetLocation.X += C_Common::TILE_SIZE;
		RunTo(_p, _targetLocation);
	}
	// --
}

// ポゼッションコマンド
void AC_Opening_Level_Instance::PossetionCommand()
{
	if (ballHolder->ballKeepingCount < ESCAPE_INTERVAL) return; // インターバル設定

	// -- ボール保持チーム取得 --
	TArray<AC_Player*> _myTeamPlayers = {}; // ボール保持チーム
	if (ballHolder->ActorHasTag(FName("HOME"))) {
		_myTeamPlayers = homePlayers;
	}
	else {
		_myTeamPlayers = awayPlayers;
	}
	// フリーのプレイヤー取得
	TArray<AC_Player*> _freeMyTeam = {};
	for (AC_Player* _p : _myTeamPlayers) {
		if (_p->position == C_Common::GK_POSITION) continue; // (*制限) GK

		if (GetIsFree(_p)) _freeMyTeam.Add(_p);
	}
	if (_freeMyTeam.IsEmpty()) { // フリープレイヤーが存在しない
		// -- チーム後退 --
		// 後退をDFラインまでに制限
		if (_myTeamPlayers[0]->ActorHasTag("HOME")) {
			if (homeDeffenceLine == 0) return;
		}
		else {
			if (awayDeffenceLine == 5) return;
		}
		// 移動アクション
		for (AC_Player* _p : _myTeamPlayers) {
			if (_p->position == C_Common::GK_POSITION) continue; // (*制限) GK
			if (_p->isMoving) continue; // (*制限) 移動中

			FVector _location = _p->GetActorLocation();
			if (_p->ActorHasTag("HOME")) {
				_location.X -= C_Common::TILE_SIZE;
			}
			else {
				_location.X += C_Common::TILE_SIZE;
			}
			RunTo(_p, _location);
		}
		return;
	}

	// -- DFラインのプレイヤー、ポケットマン取得 --
	TArray<AC_Player*> _DFPlayers = {}; // DFプレイヤー
	for (AC_Player* _p : _myTeamPlayers) {
		if (_p->position <= C_Common::RSB_POSITION) {
			_DFPlayers.Add(_p);
		}
		if (_p->ActorHasTag("POKET") && _p->position <= C_Common::RH_POSITION) { // ポケットマン (DF, MFのみ)
			_p->position = C_Common::CB_POSITION; // *CBへポジション変更
			_DFPlayers.Add(_p);
		}
	}

	// -- パス先のプレイヤー取得 --
	AC_Player* _targetPlayer = nullptr; // パス先のプレイヤー
	bool _isBreak = false; // *2重loop対応
	if (ballHolder->position == C_Common::GK_POSITION) {
		// ●ボール保持者がGK
		// < 左右へ展開 >
		for (int _i = C_Common::RSB_POSITION; _i > 0; _i--) { // LSB(1) ← RSB(5)
			for (AC_Player* _player : _DFPlayers) {
				if (GKEscapeToPlayers.Contains(_player)) continue; // *(制限)エスケープ済みのプレイヤー

				// -- ポケットマン (フリーでなくても可) --
				if ((_i == _player->position && _player->ActorHasTag("POKET"))) {
					_targetPlayer = _player;
					GKEscapeToPlayers.Add(_targetPlayer); // 一時保存
					_isBreak = true; // *2重loop対応

					break;
				}

				if (GetIsFree(_player) == false) continue; // *(制限)フリーでない

				// -- フリーのプレイヤー --
				if (_i == _player->position) { // ポジション位置
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
		// < GKへパス >
		for (AC_Player* _p : _myTeamPlayers) {
			if ((_p->position == C_Common::GK_POSITION)) {
				_targetPlayer = _p;

				break;
			}
		}
	}

	if (_targetPlayer == nullptr) { // *プレイヤーが取得できなかった場合
		GKEscapeToPlayers = {}; // 配列リセット

		return;
	}

	// -- ポケットマン処理開始 --
	if (_targetPlayer->ActorHasTag("POKET")) {
		isPoketman = true;
		currentPoketman = _targetPlayer;

		return;
	}

	// -- ショートパス --
	ShortPass(_targetPlayer);
}

// ロングアタックコマンド
void AC_Opening_Level_Instance::LongAttackCommand()
{
	if (ballHolder->position >= C_Common::LH_POSITION) return; // DFラインのプレイヤーのみ発動 (仮)
	if (ballHolder->ActorHasTag("AWAY")) return; // AWAY処理しない (仮)

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

	// -- ターゲットプレイヤー取得 --
	// ⓵ダイレクトアタック(走る)
	AC_Player* _runPlayer = nullptr;
	for (AC_Player* _p : _myFWPlayers) {
		if ((_p->position % 5 == _kickerLane)) { // キッカーと同レーンのFW
			_runPlayer = _p;

			break;
		}
		if (_p->ActorHasTag("RUNNER")) { // FWのランナー
			_runPlayer = _p;

			break;
		}
	}
	// ⓶ポストプレイヤー
	AC_Player* _postPlayer = nullptr;
	for (AC_Player* _p : _myFWPlayers) {
		if (_p->ActorHasTag("POST")) { // FWのポストプレイヤー
			_postPlayer = _p;

			break;
		}
	}

	// -- アクション処理 --
	// ⓵ダイレクトアタック
	if (_runPlayer) {
		// ポイントの取得 (キッカーと同じレーン)
		FVector _targetLocation = FVector(0, 0, 0);
		if (_isHome) {
			_targetLocation = HOME_LONG_ATTACK_POINTS[_kickerLane - 1];
		}
		else {
			_targetLocation = AWAY_LONG_ATTACK_POINTS[_kickerLane - 1];
		}
		// 裏抜け
		if (_runPlayer) GetBehind(_runPlayer, _targetLocation);
		// ロングキック
		LongKick(_targetLocation);
	}
	
	// ⓶ポストプレイ
	else if (_postPlayer) {
		postPlayer = _postPlayer; // 一時保存
		PostPlay(true);
	}

	// ③ラフボール
	else {
		// ポイントの取得 (キッカーと同じレーン)
		FVector _targetLocation = FVector(0, 0, 0);
		if (_isHome) {
			_targetLocation = HOME_LONG_ATTACK_POINTS[_kickerLane - 1];
		}
		else {
			_targetLocation = AWAY_LONG_ATTACK_POINTS[_kickerLane - 1];
		}
		// ロングキック
		LongKick(_targetLocation);
	}
}

// テクニカルアタックコマンド
void AC_Opening_Level_Instance::TecnicalAttackCommand()
{
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
		PostPlay(false);
	}
}

// ローブロックコマンド
void AC_Opening_Level_Instance::LowBlockCommand()
{
	// チーム・初期配置・初期配置タイルNo取得
	TArray<AC_Player*> _myTeam = {}; // マイチーム
	TArray<FVector> _initialPlace = {}; // 初期配置
	TArray<int> _initialTileNos = {}; // 初期配置タイルNo
	if (ballHolder->ActorHasTag("HOME")) {
		_myTeam = awayPlayers;
		_initialPlace = awayTeamIntialPlaceLocation;
		_initialTileNos = awayTeamIntialTileNo;
	}
	else {
		_myTeam = homePlayers;
		_initialPlace = homeTeamIntialPlaceLocation;
		_initialTileNos = homeTeamIntialTileNo;
	}
	// 移動 (初期配置位置へ)
	for (int _i = 0; _i < _myTeam.Num(); _i++) {
		// 移動制限
		if (_myTeam[_i]->isMoving) continue; // (*制限) プレイヤー移動中
		if (_myTeam[_i]->tileNo == _initialTileNos[_i]) continue; // (*制限) 既に初期配置にいる
		
		RunTo(_myTeam[_i], _initialPlace[_i]); // 移動処理
	}
}

// サイド圧縮コマンド
void AC_Opening_Level_Instance::SidePressCommand()
{
	if (ballHolder->position == C_Common::GK_POSITION) return; // (*制限) GK
	
	// ボールが左右のどちらにあるか
	int _ballHolderLane = ( (ballHolder->tileNo - 1) % C_Common::TILE_NUM_Y ) + 1; // ボールホルダーレーン
	if (_ballHolderLane == 3) return; // (*制限) 中央レーン
	bool _isRight = _ballHolderLane > 3 ? true : false; // 右にいる
	// 自チーム取得
	TArray<AC_Player*> _myTeam = {};
	if (ballHolder->ActorHasTag("HOME")) {
		_myTeam = awayPlayers;
	}
	else {
		_myTeam = homePlayers;
	}
	// (ボールの)反対サイドのプレイヤー取得 (*タイルで取得)
	TArray<AC_Player*> _sidePlayers = {};
	int _sideLane = _isRight ? 1 : 5; // サイドレーンNo
	for (AC_Player* _p : _myTeam) {
		int _pLane = ((_p->tileNo - 1) % C_Common::TILE_NUM_Y) + 1; // レーン
		if (_pLane == _sideLane) _sidePlayers.Add(_p);
	}
	if (_sidePlayers.IsEmpty()) return;
	// 移動
	for (AC_Player* _sidePlayer : _sidePlayers) {
		// -- サイドプレイヤー --
		// 同ラインのプレイヤー取得
		TArray<AC_Player*> _mySameLinePlayers = {};
		int _sidePlayerLine = (_sidePlayer->tileNo - 1) / C_Common::TILE_NUM_Y; // サイドプレイヤーライン (0 - 5)
		for (AC_Player* _player : _myTeam) {
			int _playerLine = (_player->tileNo - 1) / C_Common::TILE_NUM_Y; // ライン
			if (_sidePlayerLine == _playerLine) _mySameLinePlayers.Add(_player);
		}
		// 移動先位置取得
		FVector _location = _sidePlayer->GetActorLocation();
		if (_isRight) {
			// 右へ
			_location.Y += C_Common::TILE_SIZE;
		}
		else {
			// 左へ
			_location.Y -= C_Common::TILE_SIZE;
		}
		// *移動制限
		if (_sidePlayer->isMoving) continue; // 移動中
		if (_sidePlayer->position == C_Common::GK_POSITION) continue; // GK
		if (_mySameLinePlayers.Num() >= 5) return; // 同ラインに5人以上存在
		// 移動アクション
		RunTo(_sidePlayer, _location);
		

		// -- 同ラインのプレイヤー (サイドプレイヤーは移動した後) --
		for (int _i = 1; _i < 5; _i++) {
			// 隣のタイルNo取得
			int _nextToSideTileNo = 0;
			if (_isRight) {
				_nextToSideTileNo = _sidePlayer->tileNo + _i;
			}
			else {
				_nextToSideTileNo = _sidePlayer->tileNo - _i;
			}
			// 隣のプレイヤー取得
			AC_Player* _nextToSidePlayer = nullptr;
			for (AC_Player* _p : _mySameLinePlayers) {
				if (_p->tileNo == _nextToSideTileNo) {
					_nextToSidePlayer = _p;

					break;
				}
			}
			if (_nextToSidePlayer == nullptr) break; // (*制限) 隣のプレイヤーがいない
			// 移動位置取得
			FVector _nextToLocation = _nextToSidePlayer->GetActorLocation();
			if (_isRight) {
				// 右へ
				_nextToLocation.Y += C_Common::TILE_SIZE;
			}
			else {
				// 左へ
				_nextToLocation.Y -= C_Common::TILE_SIZE;
			}
			if (_nextToSidePlayer->isMoving) break; // (*制限) 移動中
			// 移動アクション
			RunTo(_nextToSidePlayer, _nextToLocation);
		}
	}
}

// ハイプレスコマンド
void AC_Opening_Level_Instance::HighPressCommand()
{
	// *(制限) ボールホルダーGK
	if (ballHolder->position == C_Common::GK_POSITION) return;
	// *(制限) ハイプレスディフェンダー移動中
	for (AC_Player* _p : highPressDeffenders) {
		if (_p->isMoving) return;
	}
	highPressDeffenders.Empty(); // リセット
	// チーム取得
	TArray<AC_Player*> _myTeam = {}; // マイチーム
	TArray<AC_Player*> _enemyTeam = {}; // 相手チーム
	if (ballHolder->ActorHasTag("HOME")) {
		_myTeam = awayPlayers;
		_enemyTeam = homePlayers;
	}
	else {
		_myTeam = homePlayers;
		_enemyTeam = awayPlayers;
	}
	// 相手のフリーマン取得 (*GK除く)
	TArray<AC_Player*> _freeEnemys = {};
	for (AC_Player* _p : _enemyTeam) {
		if (_p->position == C_Common::GK_POSITION) continue; // (*制限) GK
		
		if (GetIsFree(_p)) _freeEnemys.Add(_p);
	}
	if (_freeEnemys.IsEmpty()) return;
	// ハイプレスディフェンダー取得
	for (AC_Player* _freeEnemy : _freeEnemys) { // 相手フリーマンの中で
		// ⓵同レーンの味方
		TArray<AC_Player*> _sameLaneMyPlayers = {};
		int _freeEnemyLane = ((_freeEnemy->tileNo - 1) % C_Common::TILE_NUM_Y) + 1; // レーン (相手のフリーマン)
		for (AC_Player* _myPlayer : _myTeam) {
			int _myPlayerLane = ((_myPlayer->tileNo - 1) % C_Common::TILE_NUM_Y) + 1; // レーン (味方チーム)
			if (_freeEnemyLane == _myPlayerLane) _sameLaneMyPlayers.Add(_myPlayer); // 同レーンの場合
		}
		if (_sameLaneMyPlayers.IsEmpty()) continue;
		
		// ⓶前方にいる最も近いプレイヤー
		for (int _i = 1; _i <= 5; _i++) {
			bool _isBreak = false; // 2重break
			int _addLine = _freeEnemy->ActorHasTag("HOME") ? _i : -_i;
			int _freeEnemyFrontLine = ((_freeEnemy->tileNo) - 1 / C_Common::TILE_NUM_Y) + _addLine; // フリーマンの前のライン

			for (AC_Player* _sameLanePlayer : _sameLaneMyPlayers) { // 同レーンの味方中で
				int _samePlayerLine = (_sameLanePlayer->tileNo) - 1 / C_Common::TILE_NUM_Y; // レーン (同レーンの味方)
				if (_freeEnemyFrontLine == _samePlayerLine) {
					highPressDeffenders.Add(_sameLanePlayer); // *取得
					
					_isBreak = true;
					break;
				}
			}
			if (_isBreak) break;
		}
	}
	if (highPressDeffenders.IsEmpty()) return;
	if (_freeEnemys.Num() != highPressDeffenders.Num()) return; // (*制限) フリーマンとディフェンダー数が異なる
	// 移動
	for (int _i = 0; _i < _freeEnemys.Num(); _i++) {
		FVector _location = _freeEnemys[_i]->GetActorLocation();
		if (highPressDeffenders[_i]->ActorHasTag("HOME")) { // 位置調整X
			_location.X -= (82 * 2); // *位置を相手プレイヤーにしているため2倍
		}
		else {
			_location.X += (82 * 2); // *位置を相手プレイヤーにしているため2倍
		}
		RunTo(highPressDeffenders[_i], _location);
	}
}

// 走る
void AC_Opening_Level_Instance::RunTo(AC_Player* player, FVector toLocation)
{
	if (player == nullptr) return;

	player->completeMoving.BindUObject(this, &ThisClass::SetDeffenceLine); // セットコールバック (Delegate)
	player->RunTo(toLocation);
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
	RunTo(runPlayer, toLocation);
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
		RunTo(_player, _location);
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
		RunTo(_player, _location);
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
void AC_Opening_Level_Instance::PostPlay(bool isLong)
{
	postPlayCount++; // インクリメント

	if (postPlayCount == 1) {

		// -- ショート (パスアンドゴー) --
		if (isLong == false) {
			// 移動位置取得
			FVector _ToLocation = ballHolder->GetActorLocation();
			passAndGoPlayer = ballHolder; // *一時保存
			int _preMoveTileNo = ballHolder->tileNo; // 移動前のタイルNo (*ディフェンス姿勢解除時に使用)
			if (ballHolder->ActorHasTag("HOME")) {
				_ToLocation.X += C_Common::TILE_SIZE;
			}
			else {
				_ToLocation.X -= C_Common::TILE_SIZE;
			}

			if (postPlayer == nullptr) return;

			// オフェンスアクション
			ShortPass(postPlayer); // ショートパス
			RunTo(passAndGoPlayer, _ToLocation); // 前進

			// ディフェンス姿勢解除
			TArray<AC_Player*> _deffencePlayers = {}; // ディフェンスチーム
			if (ballHolder->ActorHasTag(FName("HOME"))) {
				_deffencePlayers = awayPlayers;
			}
			else {
				_deffencePlayers = homePlayers;
			}
			for (AC_Player* _player : _deffencePlayers) {
				if (_player->tileNo == _preMoveTileNo) {
					_player->RemoveDefensiveStance(); // ディフェンス姿勢解除

					break;
				}
			}

		}
		
		// -- ロング(ロングパス) --
		else {
			LongPass(postPlayer);
		}
	}

	else if (postPlayCount == 2)
	{
		// -- ショート (折り返し 右横) --
		if (passAndGoPlayer) {
			FVector _location = postPlayer->GetActorLocation();
			_location.Y += C_Common::TILE_SIZE;
			
			SpacePass(_location); // スペースパス
			SetBallHolder(passAndGoPlayer); // ボールホルダー
		}

		// -- ロング (折り返し 後ろ) --
		else {
			// チーム・背後タイルNo取得
			TArray<AC_Player*> _myTeamPlayers = {}; // マイチーム
			int _backTimeNo = 0; // ポストプレイヤー背後のタイルNo
			if (ballHolder->ActorHasTag(FName("HOME"))) {
				_myTeamPlayers = homePlayers;
				_backTimeNo = postPlayer->tileNo - C_Common::TILE_NUM_Y;
			}
			else {
				_myTeamPlayers = awayPlayers;
				_backTimeNo = postPlayer->tileNo + C_Common::TILE_NUM_Y;
			}
			
			// パスターゲット取得
			AC_Player* _passToPlayer = nullptr;
			for (AC_Player* _player : _myTeamPlayers) {
				if (_player->tileNo == _backTimeNo)
				{
					_passToPlayer = _player;
					break;
				}
			}
			if (_passToPlayer == nullptr) return;
			
			// ショートパス
			ShortPass(_passToPlayer);
		}
	}
	else if (postPlayCount == 3) {
		
		if (passAndGoPlayer) return; // ショート処理なし

		// -- ロング (ポストプレイヤーを移動 左へ) --
		FVector _location = postPlayer->GetActorLocation();
		_location.Y -= C_Common::TILE_SIZE;
		RunTo(postPlayer, _location);
	}
	else {
		// 終了処理
		postPlayCount = 0;
		passAndGoPlayer = nullptr;
		postPlayer = nullptr;
	}
}

// AWAYチームの動き (敵AI)
void AC_Opening_Level_Instance::AwayTeamMovement()
{
	if (ballHolder == nullptr) return;

	// *** ハイプレス ***
	// | ボールホルダー前にいるプレイヤーが前進する |
	if (awayCommand == C_Common::HIGH_PRESS_COMMAND_NO) {
		if (ballHolder->position == C_Common::GK_POSITION) return; // GKは処理なし

		// -- プレイヤー取得 --
		int _frontTile = ballHolder->tileNo + C_Common::TILE_NUM_Y; // ボールホルダー前タイル
		AC_Player* _targetPlayer = nullptr;
		
		for (AC_Player* _player : awayPlayers) {
			if (_player->tileNo == _frontTile) {
				_targetPlayer = _player;

				break;
			}
		}
		if (_targetPlayer == nullptr) return;
		
		// -- 前進する --
		if (_targetPlayer->isMoving) return; // *(制限) ターゲットが既に移動している

		FVector _frontLocation = _targetPlayer->GetActorLocation();
		_frontLocation.X -= C_Common::TILE_SIZE;
		
		RunTo(_targetPlayer, _frontLocation); // 移動
	}
}

// プレイヤースポーン (プールから選ばれた)
void AC_Opening_Level_Instance::SpawnPlayerInPool(int playerType)
{
	if (playerSubClass == nullptr) return;
	// -- スポーン処理 --
	FVector _spawnLocation = FVector(-1300, 1000, C_Common::PLAYER_BASE_LOCATION_Z);
	FRotator _rotation = FRotator::ZeroRotator;
	AC_Player* _player = GetWorld()->SpawnActor<AC_Player>( // アクタースポーン
		playerSubClass,
		_spawnLocation,
		_rotation
	);
	
	if (_player) {
		// -- セット --
		_player->Tags.Add(FName("HOME")); // tagセット
		_player->SetSpwanPlayerMaterial(playerType); // マテリアルセット
		_player->SetPlayerTypeIcon(playerType); // プレイヤータイプアイコン

		// -- 出現アニメーション --
		subPlayers.Add(_player); // サブへ
		RunTo(_player, SUB_LOCATION[subPlayers.Num() - 1]);
	}
}

// 位置からタイルＮｏ取得
int AC_Opening_Level_Instance::GetTileNoFromLocation(FVector location)
{
	int _tileNo = 0;

	// プレイヤー位置
	FVector _location = location;
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

// ディフェンスライン設定
void AC_Opening_Level_Instance::SetDeffenceLine()
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