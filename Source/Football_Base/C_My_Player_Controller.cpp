// Fill out your copyright notice in the Description page of Project Settings.


#include "C_My_Player_Controller.h"
#include <Kismet/GameplayStatics.h>
#include "My_Game_Instance.h"

AC_My_Player_Controller::AC_My_Player_Controller()
{
	// Set this controller to call Tick() every frame
	PrimaryActorTick.bCanEverTick = true;

	// Default property values can be set here
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;

	// *** オープニングUIサブクラス取得 ***
	ConstructorHelpers::FClassFinder<UC_Opening_UI> _openingUIFinder(TEXT("/Game/UI/WBP_Opening"));
	if (_openingUIFinder.Class) {
		openingUISubClass = _openingUIFinder.Class;
	}
	// ***

	// *** プレイヤーBP取得 ***
	// - 1 -
	ConstructorHelpers::FClassFinder<AC_Player> _player1BPFinder(TEXT("/Game/Blue_Print/Players/BP_Target_Man_1"));
	if (_player1BPFinder.Class) {
		playerSubClass_1 = _player1BPFinder.Class;
	}
	// - 2 -
	ConstructorHelpers::FClassFinder<AC_Player> _player2BPFinder(TEXT("/Game/Blue_Print/Players/BP_Player_Normal_2"));
	if (_player2BPFinder.Class) {
		playerSubClass_2 = _player2BPFinder.Class;
	}
	// - 3 -
	ConstructorHelpers::FClassFinder<AC_Player> _player3BPFinder(TEXT("/Game/Blue_Print/Players/BP_Player_Runner_3"));
	if (_player3BPFinder.Class) {
		playerSubClass_3 = _player3BPFinder.Class;
	}
	// - 4 -
	ConstructorHelpers::FClassFinder<AC_Player> _player4BPFinder(TEXT("/Game/Blue_Print/Players/BP_Player_Normal_4"));
	if (_player4BPFinder.Class) {
		playerSubClass_4 = _player4BPFinder.Class;
	}
	// - 5 -
	ConstructorHelpers::FClassFinder<AC_Player> _player5BPFinder(TEXT("/Game/Blue_Print/Players/BP_Player_Normal_5"));
	if (_player5BPFinder.Class) {
		playerSubClass_5 = _player5BPFinder.Class;
	}
	// ***
}

void AC_My_Player_Controller::BeginPlay()
{
	Super::BeginPlay();

	// Initialize player controller here

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
	// ***

	// *** タイルを取得 ***
	TArray<AActor*> _actorTiles = {}; // タイルアクター配列
	UGameplayStatics::GetAllActorsOfClass(this, AC_Tile::StaticClass(), _actorTiles); // クラスで探す

	for (AActor* _a : _actorTiles) {
		AC_Tile* _tile = Cast<AC_Tile>(_a); // キャスト

		tiles.Add(_tile);
	}
	// ***

	// *** マネージャー取得 ***
	AActor* _m = UGameplayStatics::GetActorOfClass(this, AC_Manager::StaticClass());
	homeManager = Cast<AC_Manager>(_m); // キャスト
	// ***
	
}

void AC_My_Player_Controller::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Tick logic here
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
		if (_instance->pool_selected_player_no >= 0) {
			SpawnPlayerInPool(_instance->pool_selected_player_no);

			_instance->pool_selected_player_no = -1; // リセット
		}

		return;
	}
	// *****
}

// ********* input *********
// 入力設定
void AC_My_Player_Controller::SetupInput()
{
	// 入力を有効にする
	EnableInput(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	// 左クリックのプレスをバインド
	InputComponent->BindKey(EKeys::LeftMouseButton, IE_Pressed, this, &AC_My_Player_Controller::PressedLeft);
	// 左クリックのリリースをバインド
	InputComponent->BindKey(EKeys::LeftMouseButton, IE_Released, this, &AC_My_Player_Controller::ReleasedLeft);
	// Wキープレスをバインド
	InputComponent->BindKey(EKeys::W, IE_Pressed, this, &AC_My_Player_Controller::PressedW);
	// Aキープレスをバインド
	InputComponent->BindKey(EKeys::A, IE_Pressed, this, &AC_My_Player_Controller::PressedA);
	// Sキープレスをバインド
	InputComponent->BindKey(EKeys::S, IE_Pressed, this, &AC_My_Player_Controller::PressedS);
	// Dキープレスをバインド
	InputComponent->BindKey(EKeys::D, IE_Pressed, this, &AC_My_Player_Controller::PressedD);
}

// 左クリック(プレス)イベント
void AC_My_Player_Controller::PressedLeft()
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
				FRotator _rotation = FRotator::ZeroRotator;
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
void AC_My_Player_Controller::ReleasedLeft()
{
	UMy_Game_Instance* _instance = Cast<UMy_Game_Instance>(UGameplayStatics::GetGameInstance(GetWorld())); // ゲームインスタンス
	if (_instance == nullptr) return;
	// *制限*
	if (_instance->game_phase != C_Common::PLAYER_SELECT_PLACE_PHASE) return; // プレイヤー選択フェーズ確認
	if (isPlayerGrap == false) return; // プレイヤー選択中か確認
	if (selectedPlayer == nullptr) return; // プレイヤー取得できているか確認

	// -- プレイヤーをタイルに位置変更 --
	// プレイヤータイルNo取得
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
	// 位置変更アクション
	if (_tile) {
		// タイル位置へ (選択されたプレイヤー)
		FVector _location = _tile->GetActorLocation(); // 位置
		_location.Z = C_Common::PLAYER_BASE_LOCATION_Z;
		_location.X -= 82;
		selectedPlayer->SetActorLocation(_location);

		// - プレイヤー入れ替え処理 (既に移動先タイルにいるプレイヤーとの) -
		// 移動先にプレイヤーが存在するか確認
		AC_Player* _toExistPlayer = nullptr; // 移動先すでにいるプレイヤー
		for (AC_Player* _p : homePlayers) {
			if (_p->tileNo == _toTileNo) {
				_toExistPlayer = _p;

				break;
			}
		}
		// 移動先プレイヤーのタイル取得
		if (_toExistPlayer) {
			AC_Tile* _fromTile = nullptr;
			for (AC_Tile* _t : tiles) { // 元の位置タイル取得
				if (_t->tileNo == _fromTileNo) {
					_fromTile = _t;

					break;
				}
			}
			// プレイヤー入れ替え
			if (_fromTile) {
				// ⓵タイルへ
				FVector _fromTileLocation = _fromTile->GetActorLocation(); // 位置
				_fromTileLocation.Z = C_Common::PLAYER_BASE_LOCATION_Z;
				_fromTileLocation.X -= 82;
				_toExistPlayer->SetActorLocation(_fromTileLocation);
				_toExistPlayer->tileNo = _fromTileNo; // タイルNo更新
			}
			else {
				// ⓶サブへ
				allPlayers.Remove(_toExistPlayer); // フィールドプレイヤー(ALL)から削除
				homePlayers.Remove(_toExistPlayer); // フィールドプレイヤー(HOME)から削除
				subPlayers.Add(_toExistPlayer); // サブへ追加
				FVector _subLocation = SUB_LOCATION[subPlayers.Num() - 1]; // 位置
				_toExistPlayer->SetActorLocation(_subLocation);
			}
		}
	}
	else {
		// 位置変更のみ (サブ位置へ)
		subPlayers.Add(selectedPlayer); // 配列追加
		FVector _location = SUB_LOCATION[subPlayers.Num() - 1]; // 位置
		selectedPlayer->SetActorLocation(_location);
	}
	// 変数リセット
	isPlayerGrap = false; // フラグ切り替え
	selectedPlayer->tileNo = _toTileNo; // タイルNo更新
	allPlayers.Add(selectedPlayer); // フィールドプレイヤー(ALL)へ追加
	homePlayers.Add(selectedPlayer); // フィールドプレイヤー(HOME)へ追加
	selectedPlayer = nullptr;

	// -- マテリアル削除 --
	if (selectedTile) selectedTile->RemoveMainMaterial();
}

// プレイヤー選択
void AC_My_Player_Controller::PressedLeftForPlayerSelect()
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
void AC_My_Player_Controller::FollowPlayerToMouse()
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
void AC_My_Player_Controller::PressedW()
{
	UMy_Game_Instance* _instance = Cast<UMy_Game_Instance>(UGameplayStatics::GetGameInstance(GetWorld())); // ゲームインスタンス
	if (_instance == nullptr)  return;
	if (_instance->game_phase != C_Common::MATCH_PHASE) return; // *(制限)試合フェーズのみ

	//_instance->command = C_Common::LONG_ATTACK_COMMAND_NO; // ロングアタック変更
}

// Aキー(プレス)イベント
void AC_My_Player_Controller::PressedA()
{
	UMy_Game_Instance* _instance = Cast<UMy_Game_Instance>(UGameplayStatics::GetGameInstance(GetWorld())); // ゲームインスタンス
	if (_instance == nullptr)  return;
	if (_instance->game_phase != C_Common::MATCH_PHASE) return; // *(制限)試合フェーズのみ

	//_instance->command = C_Common::ESCAPE_PRESSING_COMMAND_NO; // プレス回避変更
}

// Sキー(プレス)イベント
void AC_My_Player_Controller::PressedS()
{
	UMy_Game_Instance* _instance = Cast<UMy_Game_Instance>(UGameplayStatics::GetGameInstance(GetWorld())); // ゲームインスタンス
	if (_instance == nullptr)  return;
	if (_instance->game_phase != C_Common::MATCH_PHASE) return; // *(制限)試合フェーズのみ

	//_instance->command = 0; // アイドル変更
}

// Dキー(プレス)イベント
void AC_My_Player_Controller::PressedD()
{
	UMy_Game_Instance* _instance = Cast<UMy_Game_Instance>(UGameplayStatics::GetGameInstance(GetWorld())); // ゲームインスタンス
	if (_instance == nullptr)  return;
	if (_instance->game_phase != C_Common::MATCH_PHASE) return; // *(制限)試合フェーズのみ

	//_instance->command = C_Common::LANE_ATTACK_COMMAND_NO; // レーン攻撃変更
}

// ホバー処理
// ●マネージャーアウトライン表示・非表示
void AC_My_Player_Controller::Hover()
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
bool AC_My_Player_Controller::GetResultFromMouseLocation(FHitResult& hitResult, TArray<TEnumAsByte<EObjectTypeQuery>> objectTypes)
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

// プレイヤースポーン (プールから選ばれた)
void AC_My_Player_Controller::SpawnPlayerInPool(int selectedNo)
{
	// -- プールプレイヤー確認 --
	TArray<TSubclassOf<AC_Player>> _poolPlayers = { playerSubClass_1, playerSubClass_2, playerSubClass_3, playerSubClass_4, playerSubClass_5 };
	for (TSubclassOf<AC_Player> _p : _poolPlayers) {
		if (_p == nullptr) return;
	}
	// -- スポーン処理 --
	FVector _spawnLocation = FVector(-1300, 1000, C_Common::PLAYER_BASE_LOCATION_Z);
	FRotator _rotation = FRotator::ZeroRotator;
	AC_Player* _player = GetWorld()->SpawnActor<AC_Player>( // アクタースポーン
		_poolPlayers[selectedNo],
		_spawnLocation,
		_rotation
	);
	// -- プレイヤータイプ設定 --
	int _playerType = 0;
	if (selectedNo == 0) _playerType = C_Common::TARGET_MAN_TYPE_NO;
	else if (selectedNo == 2) _playerType = C_Common::RUNNER_TYPE_NO;

	if (_player) {
		// -- セット --
		_player->Tags.Add(FName("HOME")); // tag
		//_player->SetSpwanPlayerMaterial(playerType); // マテリアル
		_player->SetPlayerTypeIcon(_playerType); // プレイヤータイプアイコン
		_player->playerType = _playerType; // プレイヤータイプ

		// -- 出現アニメーション --
		subPlayers.Add(_player); // サブへ
		//RunTo(_player, SUB_LOCATION[subPlayers.Num() - 1]);
	}
}