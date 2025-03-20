// Fill out your copyright notice in the Description page of Project Settings.


#include "C_My_Player_Controller.h"
#include "Kismet/KismetSystemLibrary.h"
#include <Kismet/GameplayStatics.h>
#include "C_Tile.h"
#include <Kismet/KismetStringLibrary.h>
#include "Components/DecalComponent.h"
#include "C_Common.h"

// UKismetSystemLibrary::PrintString(this, "C++ hoge!", true, true, FColor::Cyan, 2.f, TEXT("None"));
 

// Called when the game starts or when spawned
void AC_My_Player_Controller::BeginPlay()
{
	Super::BeginPlay();

	// ** デバッグ表示 **
	if (C_Common::DEBUG_MODE) {
		UKismetSystemLibrary::PrintString(this, "CURRENT MODE IS DEBUG", true, true, FColor::Cyan, 10.0f, TEXT("None"));
	}
	// **

	SetShowMouseCursor(true); // カーソルを有効にする（ゲーム全体の）
	
	SetupInput(); // 入力設定

	// *** 全てのタイルを取得 ***
	TArray<AActor*> actorTiles; // タイルアクター配列
	UGameplayStatics::GetAllActorsOfClass(this, AC_Tile::StaticClass(), actorTiles); // クラスで探す

	for (AActor* a : actorTiles) {
		AC_Tile* tile = Cast<AC_Tile>(a); // キャスト
		allTiles.Add(tile); // 追加
	}
	 // ***

	// *** タイルNo設定 ***
	// // 全てのタイル配列をソート
	// xが小さい && yが小さい順にソート
	allTiles.StableSort([](const AC_Tile& A, const AC_Tile& B) {
		if (A.GetActorLocation().X != B.GetActorLocation().X) {
			// xが同じでない(同じ列ででない)
			return A.GetActorLocation().X < B.GetActorLocation().X; // xが小さい方
		}
		else {
			// 同じ列なら
			return A.GetActorLocation().Y < B.GetActorLocation().Y; // yが小さい方
		}
		});

	// タイルにNoを設定
	for (int i = 0; i < allTiles.Num(); ++i) {
		allTiles[i]->tileNo = i + 1;
	}
	// ***

	// *** デカール取得 ***
	// (コンストラクタ以外のマテリアルはLoadObject使用)
	playerSelectedDecal = LoadObject<UMaterial>(NULL, TEXT("/Game/Materials/M_Decal_Selected_Player.M_Decal_Selected_Player"), NULL, LOAD_None, NULL);
	// ***

	// *** ボール取得 ***
	AActor* aBall = UGameplayStatics::GetActorOfClass(this, AC_Ball::StaticClass()); // クラスで探す
	ball = Cast<AC_Ball>(aBall); // キャスト
	// ***

	// *** コマ取得 ***
	TArray<AActor*> a_allPieces; // コマアクター配列
	UGameplayStatics::GetAllActorsOfClass(this, AC_Piece::StaticClass(), a_allPieces); // クラスで探す
	// 型変換
	for (AActor* a : a_allPieces) {
		AC_Piece* piece = Cast<AC_Piece>(a); // キャスト
		// HomeとAwayのコマの配列作成
		if (piece->ActorHasTag(FName("HOME"))) {
			// Home
			allHomePieces.Add(piece);
		}
		else {
			// Away
			allAwayPieces.Add(piece);
		}
		allPieces.Add(piece); // 追加
	}
	// ***

	// ** ポジションエリア配列作成 **
	// | プレイヤー配置可能エリアも追加 |
	// *HOME*
	for (int i = C_Common::HOME_PLACE_EREA[0]; i <= C_Common::HOME_PLACE_EREA[1]; i++) {
		if (i <= 250) {
			// -DF-
			if (i % C_Common::TILE_NUM_Y >= 1 && i % C_Common::TILE_NUM_Y <= 5) {
				LSB_Erea.Add(i);
				placeableRrea.Add(i); // プレイヤー配置可能エリア
			}
			else if (i % C_Common::TILE_NUM_Y >= 1 && i % C_Common::TILE_NUM_Y <= 10) {
				LCB_Erea.Add(i);
				placeableRrea.Add(i); // プレイヤー配置可能エリア
			}
			else if (i % C_Common::TILE_NUM_Y >= 1 && i % C_Common::TILE_NUM_Y <= 15) {
				CB_Erea.Add(i);
				placeableRrea.Add(i); // プレイヤー配置可能エリア
			}
			else if (i % C_Common::TILE_NUM_Y >= 1 && i % C_Common::TILE_NUM_Y <= 20) {
				RCB_Erea.Add(i);
				placeableRrea.Add(i); // プレイヤー配置可能エリア
			}
			else {
				RSB_Erea.Add(i);
				placeableRrea.Add(i); // プレイヤー配置可能エリア
			}
		}
		else if (i <= 375) {
			// -MF-
			if (i % C_Common::TILE_NUM_Y >= 1 && i % C_Common::TILE_NUM_Y <= 5) {
				LH_Erea.Add(i);
				placeableRrea.Add(i); // プレイヤー配置可能エリア
			}
			else if (i % C_Common::TILE_NUM_Y >= 1 && i % C_Common::TILE_NUM_Y <= 10) {
				LIH_Erea.Add(i);
				placeableRrea.Add(i); // プレイヤー配置可能エリア
			}
			else if (i % C_Common::TILE_NUM_Y >= 1 && i % C_Common::TILE_NUM_Y <= 15) {
				CH_Erea.Add(i);
				placeableRrea.Add(i); // プレイヤー配置可能エリア
			}
			else if (i % C_Common::TILE_NUM_Y >= 1 && i % C_Common::TILE_NUM_Y <= 20) {
				RIH_Erea.Add(i);
				placeableRrea.Add(i); // プレイヤー配置可能エリア
			}
			else {
				RH_Erea.Add(i);
				placeableRrea.Add(i); // プレイヤー配置可能エリア
			}
		}
		else {
			// -FW-
			if (i % C_Common::TILE_NUM_Y >= 1 && i % C_Common::TILE_NUM_Y <= 5) {
				LWG_Erea.Add(i);
				placeableRrea.Add(i); // プレイヤー配置可能エリア
			}
			else if (i % C_Common::TILE_NUM_Y >= 1 && i % C_Common::TILE_NUM_Y <= 10) {
				LST_Erea.Add(i);
				placeableRrea.Add(i); // プレイヤー配置可能エリア
			}
			else if (i % C_Common::TILE_NUM_Y >= 1 && i % C_Common::TILE_NUM_Y <= 15) {
				CF_Erea.Add(i);
				placeableRrea.Add(i); // プレイヤー配置可能エリア
			}
			else if (i % C_Common::TILE_NUM_Y >= 1 && i % C_Common::TILE_NUM_Y <= 20) {
				RST_Erea.Add(i);
				placeableRrea.Add(i); // プレイヤー配置可能エリア
			}
			else {
				RWG_Erea.Add(i);
				placeableRrea.Add(i); // プレイヤー配置可能エリア
			}
		}
	}
	// *AWAY*
	for (int i = C_Common::AWAY_PLACE_EREA[0]; i <= C_Common::AWAY_PLACE_EREA[1]; i++) {
		if (i <= 625) {
			// -FW-
			if (i % C_Common::TILE_NUM_Y >= 1 && i % C_Common::TILE_NUM_Y <= 5) {
				Away_RWG_Erea.Add(i);
			}
			else if (i % C_Common::TILE_NUM_Y >= 1 && i % C_Common::TILE_NUM_Y <= 10) {
				Away_RST_Erea.Add(i);
			}
			else if (i % C_Common::TILE_NUM_Y >= 1 && i % C_Common::TILE_NUM_Y <= 15) {
				Away_CF_Erea.Add(i);
			}
			else if (i % C_Common::TILE_NUM_Y >= 1 && i % C_Common::TILE_NUM_Y <= 20) {
				Away_LST_Erea.Add(i);
			}
			else {
				Away_LWG_Erea.Add(i);
			}
		}
		else if (i <= 750) {
			// -MF-
			if (i % C_Common::TILE_NUM_Y >= 1 && i % C_Common::TILE_NUM_Y <= 5) {
				Away_RH_Erea.Add(i);
			}
			else if (i % C_Common::TILE_NUM_Y >= 1 && i % C_Common::TILE_NUM_Y <= 10) {
				Away_RIH_Erea.Add(i);
			}
			else if (i % C_Common::TILE_NUM_Y >= 1 && i % C_Common::TILE_NUM_Y <= 15) {
				Away_CH_Erea.Add(i);
			}
			else if (i % C_Common::TILE_NUM_Y >= 1 && i % C_Common::TILE_NUM_Y <= 20) {
				Away_LIH_Erea.Add(i);
			}
			else {
				Away_LH_Erea.Add(i);
			}
		}
		else {
			// -DF-
			if (i % C_Common::TILE_NUM_Y >= 1 && i % C_Common::TILE_NUM_Y <= 5) {
				Away_RSB_Erea.Add(i);
			}
			else if (i % C_Common::TILE_NUM_Y >= 1 && i % C_Common::TILE_NUM_Y <= 10) {
				Away_RCB_Erea.Add(i);
			}
			else if (i % C_Common::TILE_NUM_Y >= 1 && i % C_Common::TILE_NUM_Y <= 15) {
				Away_CB_Erea.Add(i);
			}
			else if (i % C_Common::TILE_NUM_Y >= 1 && i % C_Common::TILE_NUM_Y <= 20) {
				Away_LCB_Erea.Add(i);
			}
			else {
				Away_LSB_Erea.Add(i);
			}
		}
	}

	// ** 全てのプレイヤーとボールにタイルNoを設定 **
	SetTileNoToAllPlayersAndBall();
	// **
	
	// ** プレイヤーにポジションをセット **
	SetPositionToAllPlayer();
	// **

	// ** プレイヤーとボール位置をタイル位置へ設定 **
	SetPlayerAndBallToTilePosition();
	// **

	// ** プレイヤー間のライン表示 **
	//DisplayLineBetweenPlayers();
	// **
}

// Called every frame
void AC_My_Player_Controller::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// ゴール判定
	if (isGoal) {
		// < ゴール後 >
		if (C_Common::DEBUG_MODE) UKismetSystemLibrary::PrintString(this, "GOOL !!!!!!!", true, true, FColor::Red, 2.f, TEXT("None"));
	}
	else 
	{
		// < ゴール前 >
		
		// *************************
		// フェーズ処理
		// *************************
		
		// ** ①初期配置フェーズ処理 **
		// | 連続処理 (Tick) |
		if (isInitialPlacePhase) {
			if (isGrap) {
				// < グラップ中 >

				// ** ホバー処理(タイルをハイライトする) **
				TileHighlightWhenMouseHover();
				// **

				// ** 選択したプレイヤーにマウスを追従させる **
				// マウス位置取得
				TArray<TEnumAsByte<EObjectTypeQuery>> _objectTypes = {}; // 取得するオブジェクトタイプ
				_objectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic)); // Blocking Volume
				FHitResult hitResult; // オブジェクト情報

				bool _isVolumeExist = GetResultFromMouseLocation(hitResult, _objectTypes); // マウス位置情報取得処理

				// * 制限 *
				if (_isVolumeExist == false) return; // Volumeか
				// *

				// マウスを追従
				selectedPlayer->SetActorLocation(hitResult.Location);
				// **
			}
			// **

			return;
		}

		// ** ⓶2次配置フェーズ **
		// | ゲーム開始時に一度のみ |
		if (isSecondPlacePhase) {
			isPlayStepPhase = true; // 対戦フェーズがよばれないように
			isSecondPlacePhase = false; // *一度しか呼べれないようにする

			// ** ⓶2次配置フェーズ処理 **
			SecondPlacePhase();
			// **

			// ** 監視処理 **
			SetTimerMonitorPlayStepPhase(); // ⑶対戦フェーズを開始するか
			// **

			return;
		}
		// **

		// ** ⓷対戦フェーズ処理 **
		// | ステップ処理 (Monitor関数) |
		// ③-⑵プレイステップフェーズが終了しているか
		if (isPlayStepPhase == false) {
			isPlayStepPhase = true; // ③-⑵プレイステップフェーズ開始

			// ** ③-⑴ 準備ステップフェーズ開始 **
			PrepareStepPhase();
			// **
		}
		// **
	}
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
	
	// スペースキーのプレスのみバインド(*デバッグ時のみ)
	if (C_Common::DEBUG_MODE) InputComponent->BindKey(EKeys::SpaceBar, IE_Pressed, this, &AC_My_Player_Controller::PressedSpaceBar);
}

// 左クリック(プレス)イベント
// | プレイヤー選択 |
void AC_My_Player_Controller::PressedLeft()
{
	// * 制限 *
	if (isInitialPlacePhase == false) return; // 初期配置フェーズのみ
	// *
	
	// ** マウス位置(クリック時)の情報を取得 **
	TArray<TEnumAsByte<EObjectTypeQuery>> _objectTypes = {}; // 取得するオブジェクトタイプ
	_objectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_PhysicsBody)); // プレイヤー: PhysicsBody
	FHitResult hitResult; // オブジェクト情報

	bool _isPlayerExist = GetResultFromMouseLocation(hitResult, _objectTypes); // マウス位置情報取得処理
	// **
	
	// * 制限 *
	if (_isPlayerExist == false) return; // プレイヤー(PhisicsBody)か
	if (hitResult.GetActor()->ActorHasTag(FName(TEXT("HOME"))) == false) return; // HOMEプレイヤーか
	// *

	// ** プレイヤー配置可能エリアを表示 **
	DisplayPlayerPlaceableErea();
	// **


	// ** マウス位置情報からプレイヤーを取得 **
	selectedPlayer = Cast<AC_Piece>(hitResult.GetActor()); // プレイヤー取得 (キャスト)
	isGrap = true; // フラグON
	// **
}

// 左クリック(リリース)イベント
// | 選択したプレイヤーをタイルへ移動 |
// | 移動先がタイルでない場合,元の位置へ移動 |
void AC_My_Player_Controller::ReleasedLeft()
{
	// * 制限 *
	if (isInitialPlacePhase == false) return; // 初期配置フェーズでない
	// *

	// ** プレイヤー配置可能タイルを削除 **
	RemovePlayerPlaceRange();
	// **

	// ** 移動するプレイヤー情報変更 **
	if (overlayTile != nullptr && selectedPlayer != nullptr) {
		// <プレイヤーとタイルが存在>
		// | 移動先がタイルの場合 |
		
		if (placeableRrea.Contains(overlayTile->tileNo)) { // *プレイヤー配置制限対応
			// <プレイヤー配置可能エリア内>
			if (selectedPlayer->position != C_Common::GK_POSITION) {
				// <GK以外のプレイヤー>
				
				// *タイルNo更新
				selectedPlayer->currentTileNo = overlayTile->tileNo; // *移動先タイルへ移動するため
			}
		}
	}

	if (selectedPlayer != nullptr) {
		// <プレイヤーが存在>
		
		// *位置更新
		// | プレイヤーにセットされているタイルNoへ移動 |
		FVector _currentTileLocation = allTiles[selectedPlayer->currentTileNo - 1]->GetActorLocation();
		_currentTileLocation.Z = C_Common::BASE_LOCATION_Z;
		selectedPlayer->SetActorLocation(_currentTileLocation);
	}
	// **

	// ** 変数リセット **
	overlayTile = nullptr;
	selectedPlayer = nullptr;
	// **

	isGrap = false; // フラグ切り替え
}

// (*デバッグ時のみ)スペースキー(プレス)イベント
// | 初期配置フェーズを終了 |
// | 2次配置フェーズを開始 |
void AC_My_Player_Controller::PressedSpaceBar()
{
	if (C_Common::DEBUG_MODE == false) return;

	isSecondPlacePhase = true; // 2次配置フェーズ開始
	isInitialPlacePhase = false; // 初期配置フェーズ終了
}

// マウスホバー時,タイルをハイライトする
void AC_My_Player_Controller::TileHighlightWhenMouseHover()
{
	// * 制限 (ポジション) *
	if (selectedPlayer != nullptr) {
		if (selectedPlayer->position == C_Common::GK_POSITION) return; // GK
	}
	// *

	// ** マウス位置(クリック時)の情報を取得 **
	TArray<TEnumAsByte<EObjectTypeQuery>> _objectTypes = {}; // 取得するオブジェクトタイプ
	_objectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic)); // タイル: WorldStatic
	FHitResult _hitResult; // オブジェクト情報

	bool _isWorldStatic = GetResultFromMouseLocation(_hitResult, _objectTypes); // マウス位置情報取得処理
	// **

	// * 制限 (タイル以外Hover時) *
	if (_isWorldStatic == false || _hitResult.GetActor()->ActorHasTag((FName("TILE"))) == false) { // WorldStaticでない or タイルでない
		// <タイルでない>
		if (overlayTile != nullptr) overlayTile->RemoveMainMaterial(); // ハイライト削除
		overlayTile = nullptr; // *リリース時対応

		return;
	}
	// *

	AC_Tile* _currentnHoverTile = Cast<AC_Tile>(_hitResult.GetActor()); // 現在のホバータイル(キャスト)

	// * 制限 (タイルHover時) *
	if (overlayTile != nullptr) { // *初回のみnull入るため
		if (_currentnHoverTile->tileNo == overlayTile->tileNo) return; // 現在ハイライトしているタイルと同じ
	}

	if (placeableRrea.Contains(_currentnHoverTile->tileNo) == false) { // プレイヤー配置可能エリア外
		// <プレイヤー配置可能エリア外>
		if (overlayTile != nullptr) overlayTile->RemoveMainMaterial(); // ハイライト削除
		overlayTile = nullptr; // *リリース時対応

		return;
	}
	// *


	// ** タイルをハイライトする **
	if (overlayTile != nullptr) overlayTile->RemoveMainMaterial(); // マテリアル削除 (*ハイライトを1つにするため)

	overlayTile = _currentnHoverTile; // ハイライトタイルを格納
	overlayTile->SetMaterial(); // マテリアルセット (ハイライト処理)
	// **
}
// *************************



// ********* プレイ判断 *********
// ボールホルダーのプレイ選択
// | デフォルトはロングパス |
// | (*暫定) ショートパス(パスレンジ内)は現在削除 |
void AC_My_Player_Controller::SelectPlayForBallHolder()
{
	float f_ballHolderRow = float(ballHolder->currentTileNo) / float(C_Common::TILE_NUM_Y); // ボールホルダーが何列目にいるか (小数点含む) **右端タイル処理
	int _ballSideForward = isHomeBall ? C_Common::FORWARD_DIRECTION : C_Common::BACKWARD_DIRECTION; // ボールホルダー側の前向き
	AC_Piece* _passTarget = nullptr; // パスターゲットのコマ

	// ** 対人 **
	// | 勝者:               |
	// |  ボールホルダー: 前進 |
	// |  ディフェンダー: 前進 |
	if (duelRangeTileNos.IsEmpty() == false) { // 対人レンジが取得できているか

		for (AC_Piece* p : defencePlayers) { // ディフェンダーの中で
			if (duelRangeTileNos.Contains(p->currentTileNo)) { // 対人レンジ内にいれば
				bool _isBallHolderWin = Duel(p);
				
				if (_isBallHolderWin) { // どちかが対人に処理したか
					// <ボールホルダーが勝利>

					// ** 前進する **
					// | ディフェンダーの後ろ1列へ移動 |
					
					// 何列前に進むか
					int _ballHolderRow = ballHolder->currentTileNo / C_Common::TILE_NUM_Y; // ボールホルダーRow
					int _defenderRow = p->currentTileNo / C_Common::TILE_NUM_Y; // ディフェンダーRow
					int _advanceRows = isHomeBall ? (_defenderRow - _ballHolderRow) + 1: (_defenderRow - _ballHolderRow) - 1; // ボールホルダーの後ろの列へ
					
					// 移動するタイルNo取得
					int _moveToTileNo = ballHolder->currentTileNo + (C_Common::TILE_NUM_Y * _advanceRows); // ボールホルダーが移動するタイルNo
					int _frontBallHolder = isHomeBall ? C_Common::TILE_NUM_Y: -C_Common::TILE_NUM_Y; // ボールホルダーの前方向 (ボールが移動する)
					
					// 移動
					ballHolder->SetMoveTo(allTiles[_moveToTileNo - 1]->GetActorLocation()); // ボールホルダー
					ball->SetMoveTo(allTiles[(_moveToTileNo + _frontBallHolder) - 1]->GetActorLocation()); // ボール
					// **
				}
				else {
					// <ディフェンダーが勝利>

					// ** 前進する **
					// | ディフェンダーがボールホルダーとなる |
					// | ディフェンダーの後ろ1列へ移動 |
					
					// ボールホルダーの変更
					preBallHolder = ballHolder; // 前回のボールホルダーを取得
					ballHolder = p; // ボールホルダーをディフェンダーへ変更

					// 何列前に進むか
					int _ballHolderRow = ballHolder->currentTileNo / C_Common::TILE_NUM_Y; // ボールホルダーRow
					int _defenderRow = preBallHolder->currentTileNo / C_Common::TILE_NUM_Y; // ディフェンダーRow (前回のボールホルダー)
					int _advanceRows = isHomeBall ? (_defenderRow - _ballHolderRow) - 1: (_defenderRow - _ballHolderRow) + 1; // ボールホルダーの後ろの列へ

					// 移動するタイルNo取得
					int _moveToTileNo = ballHolder->currentTileNo + (C_Common::TILE_NUM_Y * _advanceRows); // ボールホルダーが移動するタイルNo
					int _frontBallHolder = isHomeBall ? -C_Common::TILE_NUM_Y: C_Common::TILE_NUM_Y; // ボールホルダーの前方向 (ボールが移動する)

					// 移動
					ballHolder->SetMoveTo(allTiles[_moveToTileNo - 1]->GetActorLocation()); // ボールホルダー
					ball->SetMoveTo(allTiles[(_moveToTileNo + _frontBallHolder) - 1]->GetActorLocation()); // ボール
					// **
				}

				return;
			}
		}
	}
	// **

	// ** 背面対人 **
	// | 勝者:                              |
	// |  ボールホルダー: ショートパス → キープ |
	// |  ディフェンダー: クリアリング         |
	if (ballHolder->direction == -_ballSideForward) { // ボールホルダーが後ろ向きの場合
		int _ballHolderBackTileNo = ballHolder->currentTileNo + _ballSideForward; // ボールホルダーの背後のタイルNo (後ろを向いているから前のタイル)

		for (AC_Piece* dp : defencePlayers) { // ディフェンダーの中で
			if (_ballHolderBackTileNo == dp->currentTileNo) { // ボールホルダーの後ろにいる場合
				bool _isBallHolderWin = BackDuel(dp); // 背面対人 (どちらが勝利したか取得)

				if (_isBallHolderWin) { // どちかが対人に処理したか
					// <ボールホルダーが勝利>

						// ** ポストプレー (ショートパス or キープ) **
						// | パスレンジ内でターゲットを探す |
						// | パスレンジはBoforeフェーズで取得済み |
					for (AC_Piece* op : offencePlayers) { // オフェンスプレイヤーの中で
						if (passRangeTileNos.Contains(op->currentTileNo) && op != preBallHolder) { // パスレンジ内のHomeコマ && 前回のボールホルダーでない ( *リターンパス禁止処理 )
							_passTarget = op;

							break;
						}
					}

					if (_passTarget != nullptr) { // パスターゲットがいるか
						// 〇パスターゲット存在

						// ** ショートパス (落としのプレー) **
						if (C_Common::DEBUG_MODE) UKismetSystemLibrary::PrintString(this, "POST PLAY", true, true, FColor::Cyan, 2.f, TEXT("None"));
						Pass(_passTarget);
						// **

					}
					else {
						// ●パスターゲット不在

						// ** キープ **
						if (C_Common::DEBUG_MODE) UKismetSystemLibrary::PrintString(this, "KEEP", true, true, FColor::Cyan, 2.f, TEXT("None"));

						// *********************
						// ここにキープアニメーション追加!!!!
						// *********************

						// **
					}

					// **
				}
				else {
					// <ディフェンダーが勝利>

					// ** クリアリング **
					if (C_Common::DEBUG_MODE) UKismetSystemLibrary::PrintString(this, "CLEARING", true, true, FColor::Cyan, 2.f, TEXT("None"));
					Clearing(dp);
					// **
				}

				return;
			}
		}
	}
	// **

	// ** サイド対人 **
	// | ショートパスが優先され、できない場合に対人処理に移行 |
	// | → 対人しなくてもショートパスは可能                |
	// | 勝者:                                         |
	// |  ボールホルダー: 前進                           |
	// |  ディフェンダー: 前進                           |
	for (AC_Piece* dp : defencePlayers) { // ディフェンダーの中で
		if (ballHolderRightTileNo == dp->currentTileNo || ballHolderLeftTileNo == dp->currentTileNo) { // ボールホルダーのサイドにいる場合
			// パスターゲット取得 (**対人前に取得)
			for (AC_Piece* op : offencePlayers) { // オフェンスプレイヤーの中で
				if (passRangeTileNos.Contains(op->currentTileNo) && op != preBallHolder) { // パスレンジ内のHomeコマ && 前回のボールホルダーでない ( *リターンパス禁止処理 )
					_passTarget = op;

					break;
				}
			}

			if (_passTarget == nullptr) { // パスターゲットが取得できなかった場合
				bool _isBallHolderWin = SideDuel(dp); // サイド対人 (どちらが勝利したか取得)
				
				if (_isBallHolderWin) { // どちかが対人に処理したか
					// <ボールホルダーが勝利>
					
					// ** 前進する **
					// | ディフェンダーの後ろ1列へ移動 |

					// 何列前に進むか
					int _ballHolderRow = ballHolder->currentTileNo / C_Common::TILE_NUM_Y; // ボールホルダーRow
					int _defenderRow = dp->currentTileNo / C_Common::TILE_NUM_Y; // ディフェンダーRow
					int _advanceRows = isHomeBall ? (_defenderRow - _ballHolderRow) + 1 : (_defenderRow - _ballHolderRow) - 1; // ボールホルダーの後ろの列へ

					// 移動するタイルNo取得
					int _moveToTileNo = ballHolder->currentTileNo + (C_Common::TILE_NUM_Y * _advanceRows); // ボールホルダーが移動するタイルNo
					int _frontBallHolder = isHomeBall ? C_Common::TILE_NUM_Y : -C_Common::TILE_NUM_Y; // ボールホルダーの前方向 (ボールが移動する)

					// 移動
					ballHolder->SetMoveTo(allTiles[_moveToTileNo - 1]->GetActorLocation()); // ボールホルダー
					ball->SetMoveTo(allTiles[(_moveToTileNo + _frontBallHolder) - 1]->GetActorLocation()); // ボール
					// **
				}
				else {
					// <ディフェンダーが勝利>

					// ** 前進する **
					// | ディフェンダーがボールホルダーとなる |
					// | ディフェンダーの後ろ1列へ移動 |

					// ボールホルダーの変更
					preBallHolder = ballHolder; // 前回のボールホルダーを取得
					ballHolder = dp; // ボールホルダーをディフェンダーへ変更

					// 何列前に進むか
					int _ballHolderRow = ballHolder->currentTileNo / C_Common::TILE_NUM_Y; // ボールホルダーRow
					int _defenderRow = preBallHolder->currentTileNo / C_Common::TILE_NUM_Y; // ディフェンダーRow (前回のボールホルダー)
					int _advanceRows = isHomeBall ? (_defenderRow - _ballHolderRow) - 1 : (_defenderRow - _ballHolderRow) + 1; // ボールホルダーの後ろの列へ

					// 移動するタイルNo取得
					int _moveToTileNo = ballHolder->currentTileNo + (C_Common::TILE_NUM_Y * _advanceRows); // ボールホルダーが移動するタイルNo
					int _frontBallHolder = isHomeBall ? -C_Common::TILE_NUM_Y : C_Common::TILE_NUM_Y; // ボールホルダーの前方向 (ボールが移動する)

					// 移動
					ballHolder->SetMoveTo(allTiles[_moveToTileNo - 1]->GetActorLocation()); // ボールホルダー
					ball->SetMoveTo(allTiles[(_moveToTileNo + _frontBallHolder) - 1]->GetActorLocation()); // ボール
					// **
				}

				return;
			}
		}
	}
	//}

	// ** シュート **
	if (isHomeBall) { // Homeボールか
		// Home
		if (f_ballHolderRow > 34.0) { // ペナルティーの高さにいるか (割り切れたら右端のタイル)
			Shoot();

			return;
		}
	}
	else {
		// Away
		if (f_ballHolderRow <= 6.0) { // ペナルティーの高さにいるか (割り切れたら右端のタイル)
			Shoot();

			return;
		}
	}
	// **

	// ** パスターゲット取得 (ロングパス) **
	// *条件*
	// | ⓵パスターゲットが取得できていない     |
	// | ⓶ボールホルダーが前向き             |
	// | ③ボールホルダーがFWでない           |
	// | ⓸パスターゲットがパスレンジ内にいない |
	if (_passTarget == nullptr) {
		if (ballHolder->direction == _ballSideForward && ballHolder->position < C_Common::LWG_POSITION) {
			// FWエリアのプレイヤーをターゲットに設定
			for (AC_Piece* p : offencePlayers) {
				if (passRangeTileNos.Contains(p->currentTileNo)) continue; // パスターゲットがパスレンジ内にいる場合

				if (p->position == C_Common::CF_POSITION) _passTarget = p;
				if (p->position == C_Common::LST_POSITION) _passTarget = p;
				if (p->position == C_Common::RST_POSITION) _passTarget = p;
				if (p->position == C_Common::LWG_POSITION) _passTarget = p;
				if (p->position == C_Common::RWG_POSITION) _passTarget = p;

				if (_passTarget != nullptr) break;
			}
		}
	}
	// **

	// ** ボール移動判定 (パス時) **
	// | *パス処理の前に必ず実行 |
	// *制限
	// ⓵移動先タイルにプレイヤーがいる
	// ⓶移動先タイルに動く予定のプレイヤーがいる
	if (_passTarget != nullptr) {
		int _ballMoveToTileNo = _passTarget->currentTileNo + _passTarget->direction; // ボールを動かすタイルNo

		if (currentTileNos.Contains(_ballMoveToTileNo) || moveToTileNos.Contains(_ballMoveToTileNo)) _passTarget = nullptr; // パスターゲット削除
	}
	// **

	// ** パス or ドリブル or 方向転換 **
	// * 制限 *
	// |  ①パスターゲット取得可能 |
	//if (_passTarget != nullptr && _passTarget->isMarked == false) { // ターゲットがマークされていない
	if (_passTarget != nullptr) {
		// <パスターゲットが存在>

		// ** パス **
		Pass(_passTarget);
		// **
	}
	else {
		// <パスターゲットが不在>
		if (ballHolder->direction == _ballSideForward) { // 前向きか
			// <前向き>

			// ** ドリブル **
			Drrible(); // *** 前進のみ
			// **

		}
		else {
			// <前向きでない>

			// ** 方向転換 **
			ChangeOfDirection();
			// **

		}
	}
	// **
}

// ディフェンダーのプレイ選択
// | *制限条件は暫定 |
void AC_My_Player_Controller::SelectPlayForDefender()
{
	// * 制限 *
	if (firstDefender == nullptr) return; // ファーストディフェンダー
	// *

	// ** プレス **
	// | ファーストディフェンダーのみ |
	Press();
	// **

	// ** マーキング **
	// | ファーストディフェンダー除く |
	for (AC_Piece* p : defencePlayers) {
		// * 制限 *
		if (p == firstDefender) continue; // ファーストディフェンダー
		// *

		Marking(p);
	}
	// **
}

// セカンドボール時の動き
// | 全プレイヤー対象 |
// | ボール周囲のプレイヤーがボールに対して移動する |
void AC_My_Player_Controller::MovementForSecondBall()
{
	int _ballTileNo = ball->currentTileNo; // ボールタイルNo
	AC_Tile* _ballTile = allTiles[_ballTileNo - 1]; // ボールタイル
	// 十字タイル
	int _plusX = _ballTileNo + (C_Common::TILE_NUM_Y * 1);
	int _minusX = _ballTileNo - (C_Common::TILE_NUM_Y * 1);
	int _plusY = _ballTileNo + 1;
	int _minusY = _ballTileNo - 1;

	// ** ボール周囲のタイルNoを取得 **
	TArray <int> _ballAroundTileNos = {}; // ボールの周囲タイル
	// セカンドボール範囲拡大対応
	int _outSideTileNum = secondBallRangeTileNum + 1; // セカンドボールに反応するタイル個数 + 1 (これより内側のタイルを周囲タイルとみなすため)
	float _aroundRange = C_Common::NEXT_TO_TILE_DISTANCE * _outSideTileNum; // 周囲タイル範囲 (*この値以下だと周囲タイルとみなす)
	// *制限
	// | ⓵周囲タイル範囲以上 |
	for (AC_Tile* t : allTiles) {
		float _distanceFromBallTile = _ballTile->GetDistanceTo(t); // ボールタイルからの距離
		if (_aroundRange < _distanceFromBallTile) continue; // ⓵周囲タイル範囲以上

		_ballAroundTileNos.Add(t->tileNo);
	}
	// **

	// ** タイル上のプレイヤー取得 **
	TArray< AC_Piece*> _ballAroundPlayers = {}; // ボール周囲のプレイヤー
	for (AC_Piece* p : allPieces) {
		if (_ballAroundTileNos.Contains(p->currentTileNo)) _ballAroundPlayers.Add(p);
	}
	// **

	// ** プレイヤーを移動させる **
	if (_ballAroundPlayers.IsEmpty()) return; // 周囲のプレイヤーが取得できていない場合

	for (AC_Piece* p : _ballAroundPlayers) {
		// ** 十字マスの中で最も距離が近いタイル取得 **
		TArray <int> _crossTiles = { _plusX, _minusX, _plusY, _minusY }; // 十字タイルNo配列
		TArray <int> _tileDistances = { // 十字タイルとプレイヤーの距離配列
			(int)allTiles[_plusX - 1]->GetDistanceTo(p),
			(int)allTiles[_minusX - 1]->GetDistanceTo(p),
			(int)allTiles[_plusY - 1]->GetDistanceTo(p),
			(int)allTiles[_minusY - 1]->GetDistanceTo(p)
		};

		// 小さい距離順にソート
		TArray <int> _c_tileDistances = _tileDistances; // コピー
		_c_tileDistances.StableSort([](const int& A, const int& B) {
			return A < B;
			});

		int _minDistance = _c_tileDistances[0]; // 最も小さい距離
		int _i = _tileDistances.IndexOfByKey(_minDistance); // 最も近い距離のインデックス

		if (_i < 0) return; // 取得できなかった場合
		int _shotestTileNo = _crossTiles[_i]; // 最も近いタイル
		// **

		// ** 移動するタイルNo取得 **
		int _moveToTileNo = GetShortestNextTileNo(p->currentTileNo, _shotestTileNo); // 最も近いタイルへの最短距離
		// **

		if (p->currentTileNo == _moveToTileNo) return; // すでに目的地にいる場合は移動しない

		// ** 移動 **
		p->SetMoveTo(allTiles[_moveToTileNo - 1]->GetActorLocation());
		// **
	}
}
// *****************************



// ********* プレイ *********
// パス
// | 体の方向へパスを出す |
void AC_My_Player_Controller::Pass(AC_Piece* targetPiece)
{	
	// ** ターゲットの体の向きのタイル取得 **
	int _targetDirectionTileNo = targetPiece->currentTileNo + targetPiece->direction; // 動かすタイルNo
	FVector moveToLocation = allTiles[_targetDirectionTileNo - 1]->GetActorLocation(); // ボールを動かす位置
	// **

	// ** 移動先のタイルを予約する **
	if (moveToTileNos.Contains(_targetDirectionTileNo)) { // タイルが予約されているか
		// 予約あり

		return; // 移動しない
	}
	else {
		// 予約なし

		moveToTileNos.Add(_targetDirectionTileNo); // 予約する
	}
	// **

	// ** パスターゲットを設定 **
	// | 次の3-⓵準備ステップ時にボールホルダーを変更 |
	passTarget = targetPiece;
	// **
	
	// ** ボールを移動させる **
	ball->SetMoveTo(moveToLocation);
	// **
}

// ドリブル (前進のみ)
// | 1マス前進する |
void AC_My_Player_Controller::Drrible()
{
	int ballHolderTileNo = ballHolder->currentTileNo; // ボールホルダータイルNo
	int playerMoveToTileNo; // プレイヤーの動く先のタイルNo
	int ballMoveToTileNo; // ボールのの動く先のタイルNo

	// ** 移動先のタイルNo取得 **
	if (ballHolder->ActorHasTag(FName("HOME"))) {
		// Home
		playerMoveToTileNo = ballHolderTileNo + C_Common::TILE_NUM_Y; // プレイヤー
		ballMoveToTileNo = ballHolderTileNo + (C_Common::TILE_NUM_Y * 2); // ボール
	}
	else {
		// Away
		playerMoveToTileNo = ballHolderTileNo - C_Common::TILE_NUM_Y; // プレイヤー
		ballMoveToTileNo = ballHolderTileNo - (C_Common::TILE_NUM_Y * 2); // ボール
	}
	// **

	// ** 移動先のタイルを予約する **
	if (moveToTileNos.Contains(playerMoveToTileNo) || moveToTileNos.Contains(ballMoveToTileNo)) { // タイルが予約されているか
		// 予約あり

		return; // 移動しない
	}
	else {
		// 予約なし

		// 予約する
		moveToTileNos.Add(playerMoveToTileNo); // プレイヤー
		moveToTileNos.Add(ballMoveToTileNo); // ボール
	}
	// **

	// ** 移動 **
	ballHolder->SetMoveTo(allTiles[playerMoveToTileNo - 1]->GetActorLocation()); // プレイヤー
	ball->SetMoveTo(allTiles[ballMoveToTileNo - 1]->GetActorLocation()); // ボール
	// **
}

// 方向転換
void AC_My_Player_Controller::ChangeOfDirection()
{
	int ballHolderTileNo = ballHolder->currentTileNo; // ボールホルダーのタイルNo
	int direction = GetDirectionOfBallHolder(); // ボールホルダーの向き
	int moveToTileNo = 0; // 移動先のタイルNo
	int forwardDirection = isHomeBall ? C_Common::TILE_NUM_Y : -C_Common::TILE_NUM_Y; // ボールホルダーの前の方向

	// ** 移動先のタイルNo取得 **
	if (direction == -forwardDirection) { // 現在, 後ろ向きか
		// 右タッチライン上か
		if (ballHolderTileNo % C_Common::TILE_NUM_Y == 0) {
			// 右タッチライン上
			moveToTileNo = ballHolderTileNo - 1; // 左向き
		}
		else {
			// 右タッチラインでない
			moveToTileNo = ballHolderTileNo + 1; // 右向き
		}
	}
	// 現在、右向き or 左向き
	if (direction == 1 || direction == -1) moveToTileNo = ballHolderTileNo + forwardDirection; // 前向きになる
	// **


	// ** 移動先のタイルを予約する **
	if (moveToTileNos.Contains(moveToTileNo)) { // タイルが予約されているか
		// 予約あり

		return; // 移動しない
	}
	else {
		// 予約なし

		moveToTileNos.Add(moveToTileNo); // 予約する
	}
	// **

	// ** 移動 ** 
	ball->SetMoveTo(allTiles[moveToTileNo - 1]->GetActorLocation());
	// **
}

// シュート
void AC_My_Player_Controller::Shoot()
{
	isGoal = true; // ゴール判定

	FVector goalLocation; // ゴール位置
	if (ballHolder->ActorHasTag(FName("HOME"))) { // Homeプレイヤーか
		// Home
		goalLocation = FVector(2050, 0, -40);
	}
	else {
		// Away
		goalLocation = FVector(-2050, 0, -40);
	}

	// ** ボールを移動させる **
	ball->SetMoveTo(goalLocation);
	// **
}

// 対人
// | dueledPlayer: 対人をされるプレイヤー (発火時,ディフェンダー) |
bool AC_My_Player_Controller::Duel(AC_Piece* dueledPlayer)
{
	// 能力値が同じだった場合
	if (ballHolder->drrible == dueledPlayer->tackle) {
		// ランダムに勝者がきまる
		bool r = FMath::RandBool(); // ランダムBool

		return r;
	}

	// キープとタックルの能力値を比較
	if (ballHolder->drrible > dueledPlayer->tackle) {
		// <オフェンス勝利>
		return true;
	}

	// <ディフェンス勝利>
	return false;
}

// 背面対人 (背負った時のプレー)
// | dueledPlayer: 対人をされるプレイヤー (発火時,ディフェンダー) |
// | return bool: デゥエルの勝者(ボールホルダー = true, ディフェンダー = false) |
// | キープ(オフェンス) vs タックル(ディフェンス)の能力値の比較で勝者が決定 |
// | 能力値が同じだった場合、ランダムに勝者が決定 |
bool AC_My_Player_Controller::BackDuel(AC_Piece* dueledPlayer)
{
	// 能力値が同じだった場合
	if (ballHolder->keep == dueledPlayer->tackle) {
		// ランダムに勝者がきまる
		bool r = FMath::RandBool(); // ランダムBool
		
		return r;
	}

	// キープとタックルの能力値を比較
	if (ballHolder->keep > dueledPlayer->tackle) {
		// <オフェンス勝利>
		return true;
	}
	
	// <ディフェンス勝利>
	return false;
}

// サイド対人
// (*暫定) 対人処理と同じ処理
// | dueledPlayer: 対人をされるプレイヤー (発火時,ディフェンダー) |
// | return bool: デゥエルの勝者(ボールホルダー = ture, ディフェンダー = false) |
bool AC_My_Player_Controller::SideDuel(AC_Piece* dueledPlayer)
{
	// 能力値が同じだった場合
	if (ballHolder->drrible == dueledPlayer->tackle) {
		// ランダムに勝者がきまる
		bool r = FMath::RandBool(); // ランダムBool

		return r;
	}

	// キープとタックルの能力値を比較
	if (ballHolder->drrible > dueledPlayer->tackle) {
		// <オフェンス勝利>
		return true;
	}

	// <ディフェンス勝利>
	return false;
}

// プレス
// | ファーストディフェンダーがボールホルダーへ移動する |
void AC_My_Player_Controller::Press()
{
	// * 制限 *
	// | ファーストディフェンダーが対人レンジ内にいる |
	// | 移動先にプレイヤーがいる |
	if (duelRangeTileNos.IsEmpty() == false) { // 対人レンジが取得できているか
		if (duelRangeTileNos.Contains(firstDefender->currentTileNo)) return; // ファーストディフェンダーが対人レンジ内にいるか
	}
	// *

	int nextTileNo = GetShortestNextTileNo(firstDefender->currentTileNo, ballHolder->currentTileNo); // 移動先のタイルNo

	if (currentTileNos.Contains(nextTileNo)) return; // 移動先にプレイヤーがいる

	// ** 移動先のタイルを予約する **
	if (moveToTileNos.Contains(nextTileNo)) { // タイルが予約されているか
		// 予約あり

		return; // 移動しない
	}
	else {
		// 予約なし

		moveToTileNos.Add(nextTileNo); // 予約する
	}
	// **

	// ** 移動 **
	FVector nextTileLocation = allTiles[nextTileNo - 1]->GetActorLocation(); // 動くタイル位置
	firstDefender->SetMoveTo(nextTileLocation); // 移動
	// **
}

// マーキング
// | マークレンジに相手がいるかチェックし、相手の前のマスに向かって移動する |
void AC_My_Player_Controller::Marking(AC_Piece* defencePlayer)
{
	for (AC_Piece* p : offencePlayers) { // オフェンスプレイヤーごとに
		// * 制限 *
		if (p == ballHolder) continue; // ボールホルダー
		// *

		// * マークレンジに相手がいるかチェック *
		if (defencePlayer->markRange.Contains(p->currentTileNo)) {
			// < マークレンジ内に相手がいる >
			
			// 動くタイルとマーク位置を取得
			int _markedPlayersFront = p->ActorHasTag(FName("HOME")) ? C_Common::FORWARD_DIRECTION : C_Common::BACKWARD_DIRECTION; // マークされるプレイヤーの前方
			int _markingLocationTileNo = p->currentTileNo + _markedPlayersFront; // マーク位置のタイルNo (マークしていると判断する最終的な位置)
			int _moveToTileNo = GetShortestNextTileNo(defencePlayer->currentTileNo, _markingLocationTileNo); // 動くタイルNo (マーク位置へ動く過程の位置)

			// * 制限 *
			if (defencePlayer->currentTileNo == _markingLocationTileNo) return; // マーク位置のタイルNo既ににいる
			if (defencePlayer->currentTileNo == _moveToTileNo) return; // 動くタイルNo既ににいる
			// *

			// 移動先のタイルを予約する
			if (currentTileNos.Contains(_moveToTileNo) || moveToTileNos.Contains(_moveToTileNo)) {
				// <すでにプレイヤーがいる or 予約されている>

				return; // 移動しない
			}
			else {
				// <プレイヤーがいない or 予約なし>

				moveToTileNos.Add(_moveToTileNo); // 予約する
			}
			
			// 移動
			defencePlayer->SetMoveTo(allTiles[_moveToTileNo - 1]->GetActorLocation());


			return;
		}
	}
}

// クリアリング
// // | ディフェンダーの前方向にボールを4マス移動 |
// | defencePlayer: 対人をされるプレイヤー |
void AC_My_Player_Controller::Clearing(AC_Piece* defencePlayer)
{
	// ** 移動先のタイルを決定する **
	int _moveToTileNo = 0; // 移動先のタイルNo
	int _moveDistance = 4; // 移動するタイル数
	
	for (int i = 0; i < 4; i++) { // *クリアの距離を調整
		if (defencePlayer->direction > 0) { // ディフェンダーの体の向きで移動の方向を決める
			// ホーム
			_moveToTileNo = ball->currentTileNo + (C_Common::FORWARD_DIRECTION * _moveDistance);
		}
		else {
			// アウェイ
			_moveToTileNo = ball->currentTileNo - (C_Common::FORWARD_DIRECTION * _moveDistance);
		}
		// ** クリアの距離を調整 **
		if (currentTileNos.Contains(_moveToTileNo) || moveToTileNos.Contains(_moveToTileNo)) { // ボールの移動先にプレイヤーがいれば
			_moveDistance--; // クリアの距離を短縮

			continue;
		}
		// **
	}
	// **

	ball->SetMoveTo(allTiles[_moveToTileNo - 1]->GetActorLocation()); // ボール移動

	isClering = true; // クリアリング判定
}
// **************************



// ********* フェーズ *********
// ⓶2次配置フェーズ
// | AWAYチームをゲーム開始位置(*15マス前)へ移動 (*AWAYが最初必ずディフェンスのため) |
void AC_My_Player_Controller::SecondPlacePhase()
{
	for (AC_Piece* p : allAwayPieces) {
		// *制限*
		if (p->ActorHasTag(FName("GK"))) continue; // GK
		// *

		// ** プレイヤーの15マス前のタイル取得(AWAYのみ) **
		int _currentTileNo = GetTileNoFromLocation(p->GetActorLocation().X, p->GetActorLocation().Y); // 現在位置のタイルNo
		int _targetTileNo = _currentTileNo + -(C_Common::TILE_NUM_Y * 15); // 15マス前のタイル取得
		// **
		
		if (_targetTileNo < 1 || _targetTileNo > 1000) continue; // タイルNoの範囲内か

		p->SetMoveTo(allTiles[_targetTileNo - 1]->GetActorLocation()); // 移動
	}
}

// ③-⑵プレイステップフェーズを終了していいか監視
void AC_My_Player_Controller::MonitorFinishPlayStepPhase()
{
	// ** コマとボールが移動していないか **
	for (AC_Piece* p : allPieces) {
		if (p->isMoving) return;
	}
	if (ball->isMoving) return;
	// **

	FinishTimerAndStep(); // タイマーとステップを終了
}

// (フェーズ)タイマーとステップを終了
void AC_My_Player_Controller::FinishTimerAndStep()
{
	// タイマー終了
	GetWorldTimerManager().ClearAllTimersForObject(this);

	// ** ③-⑶ リセットステップフェーズ開始 **
	bool _isFinish = ResetStepPhase();
	// **

	// ③-⑵プレイステップフェーズ終了
	isPlayStepPhase = _isFinish;
}

// ③-⑴ 準備ステップフェーズ
// | フェーズカウント |
// | プレイヤー・ボールの初期値セット |
// | *ボールホルダーの切り替えはこのフェーズでする |
void AC_My_Player_Controller::PrepareStepPhase()
{
	// ** ステップ数カウント **
	stepCount++;
	// フェーズ数をデバッグ表示する
	if (C_Common::DEBUG_MODE) UKismetSystemLibrary::PrintString(this, "STEP: " + FString::FromInt(stepCount));
	// **

	// ** 全てのプレイヤーとボールのタイルNoを設定、配列を作成する **
	currentTileNos = SetTileNoToAllPlayersAndBall();
	// **

	// ** パス時 (前回のSTEPにて) **
	if (passTarget != nullptr) { // パスターゲットが設定されているか (前回のStepでパスが実行されているか)
		//// ** 前回のボールホルダーを取得 **
	   // | リターンパスを禁止にする |
		preBallHolder = ballHolder;
		// **

		// ** ボールホルダーをパスターゲットへ切り替える **
		ballHolder = passTarget;
		// **

		passTarget = nullptr; // 空にする
	}
	// **

	// ** クリアリング時 (前回のSTEPにて) **
	if (isClering) { // 前回のSTEPにてクリアリングが実行されているか
		// ** 前回のボールホルダーを取得 **
		preBallHolder = ballHolder;
		// **

		// ** ボールホルダーを空へ **
		ballHolder = nullptr;
		// **

		isClering = false; // クリアリング判定
	}
	// **

	// ** ボールホルダー取得 (*ホールホルダーがいない場合のみ) **
	// | ボール周囲(十字)にプレイヤーがいるか |
	if (ballHolder == nullptr) { // ボールホルダーがいない場合
	   // ●ボールの十字タイルNoを作成
		ballCrossTileNos.Empty(); // 空にする
		int _ballTileNo = ball->currentTileNo; // ボールタイルNo
		// 十字マス
		int _plusX = _ballTileNo + (C_Common::TILE_NUM_Y * 1);
		int _minusX = _ballTileNo - (C_Common::TILE_NUM_Y * 1);
		int _plusY = _ballTileNo + 1;
		int _minusY = _ballTileNo - 1;
		// ボールの十字タイルNo
		ballCrossTileNos.Add(_plusX);
		ballCrossTileNos.Add(_minusX);
		ballCrossTileNos.Add(_plusY);
		ballCrossTileNos.Add(_minusY);

		// ●ボールホルダー取得
		for (AC_Piece* p : allPieces) {
			if (ballCrossTileNos.Contains(p->currentTileNo)) { // 十字タイルにプレイヤーがいる場合
				ballHolder = p;

				break;
			}
		}
	}
	// **

	if (ballHolder != nullptr) { // ボールホルダーがいるか
		// <ボールホルダーあり>
		// ** どちらがオフェンスか **
		isHomeBall = ballHolder->ActorHasTag(FName("HOME")); // Homeボールか
		// **

		// ** オフェンスとディフェンスのプレイヤーをセット **
		// Homeボールか
		if (isHomeBall) {
			// Homeボール
			offencePlayers = allHomePieces;
			defencePlayers = allAwayPieces;
		}
		else {
			// Awayボール
			offencePlayers = allAwayPieces;
			defencePlayers = allHomePieces;
		}
		// **

		// ** ファーストディフェンダーをセット **
		firstDefender = GetFirstDefender();
		// **

		// ** プレイヤー全員に体の向きをセット **
		// ボールホルダー以外
		for (AC_Piece* p : allPieces) {
			// * 制限 *
			if (p == ballHolder) continue; // ボールホルダー
			// *

			int ballRow = (ball->currentTileNo / C_Common::TILE_NUM_Y) + 1; // ボールの列数
			int playerRow = (p->currentTileNo / C_Common::TILE_NUM_Y) + 1; // プレイヤーの列数
			// プレイヤーとボールの列の位置を比較
			if (playerRow < ballRow) { // プレイヤーよりボールの方が前
				// * 前向き
				p->direction = C_Common::FORWARD_DIRECTION;
			}
			else if (playerRow == ballRow && allAwayPieces.Contains(p)) { // 同列で、アウェイプレイヤー
				// アウェイプレイヤーのみ後ろ向きへ
				p->direction = C_Common::FORWARD_DIRECTION;
			}
			else { // プレイヤーよりボールの方が後ろ (ホームプレイヤー同列も含む)
				// *後ろ向き
				p->direction = C_Common::BACKWARD_DIRECTION;
			}
		}

		// ボールホルダー
		ballHolder->direction = GetDirectionOfBallHolder();
		// **

		// ** マークの設定・解除 **
		TArray <int> _defenceTileNos = {}; // ディフェンスのタイルNo
		for (AC_Piece* _dp : defencePlayers) {
			_defenceTileNos.Add(_dp->currentTileNo);
		}

		for (AC_Piece* _op : offencePlayers) {
			int _markTileNo = isHomeBall ? _op->currentTileNo + C_Common::FORWARD_DIRECTION : _op->currentTileNo + C_Common::BACKWARD_DIRECTION; // マーク位置
			
			if (_defenceTileNos.Contains(_markTileNo)) {
				// <マーク位置にディフェンダーが存在>
				_op->isMarked = true;
			}
			else {
				// <マーク位置にディフェンダーがいない>
				_op->isMarked = false;
			}
		}
		// **

		// ** パスレンジ取得 **
		passRangeTileNos = GetTileNoInPassRange(); // パスレンジ取得
		// パスレンジが取得できていない場合
		if (passRangeTileNos.IsEmpty()) {
			// パスレンジなし
			if (C_Common::DEBUG_MODE) UKismetSystemLibrary::PrintString(this, "PASS RANGE NULL !!");
		}
		else {
			// パスレンジあり
			// マテリアル表示
			for (int i : passRangeTileNos) {
				AC_Tile* t = allTiles[i - 1];
				t->SetPassRangeMaterial();
			}
		}
		// **

		// ** 対人レンジを取得 **
		duelRangeTileNos = GetTileNoInDuelRange(); // 対人レンジ取得
		// **

		// ** マークレンジをセット **
		for (AC_Piece* p : defencePlayers) {
			// * 制限 *
			if (p == firstDefender) continue; // ファーストディフェンダーの場合,セットしない
			// *

			TArray <int> _range = GetMarkRange(p->currentTileNo, p->direction); // マークレンジ取得

			if (_range.IsEmpty() == false) { // 空でない場合
				p->markRange = _range; // セット
				
				// マテリアル表示
				for (int i : _range) {
					AC_Tile* t = allTiles[i - 1];
					t->SetMarkRangeMaterial();
				}
			}
		}
		// **

		// ** ボールホルダーの左右タイルNo取得 **
		if (FMath::Abs(ballHolder->direction) == 25) { // 体の向きが前後を向いているか
			// < 前後 >
			ballHolderRightTileNo = ballHolder->currentTileNo + 1; // 右
			ballHolderLeftTileNo = ballHolder->currentTileNo - 1; // 左
		}
		else {
			// < 左右 >
			ballHolderRightTileNo = ballHolder->currentTileNo + C_Common::TILE_NUM_Y; // 右
			ballHolderLeftTileNo = ballHolder->currentTileNo - C_Common::TILE_NUM_Y; // 左
		}
		// **

		// ** セカンドボール範囲リセット **
		secondBallRangeTileNum = 0;
		// **
	}
	else {
		// <ボールホルダーなし>
		if (C_Common::DEBUG_MODE) UKismetSystemLibrary::PrintString(this, "BALL HOLDER NULL !!");

		// ** セカンドボール **
		secondBallRangeTileNum++; // セカンドボール範囲 拡大
		// **
	}

	// ** フェーズ中の処理 (必ず最後!!!) ***
	// | パスレンジ表示のため間隔を開ける |
	FTimerHandle handle;
	GetWorldTimerManager().SetTimer(handle, this, &AC_My_Player_Controller::PlayStepPhase, C_Common::INTERVAL_WITHIN_PHASE, false); // 間隔を開けて呼び出し
	// **
}

// ③-⑵ プレイステップフェーズ
// | ボール・プレイヤー移動 |
// | プレイヤー判断 |
void AC_My_Player_Controller::PlayStepPhase()
{
	// ** パスレンジ削除 (*マテリアルのみ) **
	if (passRangeTileNos.IsEmpty() == false) { // 空でないか
		// マテリアル削除
		for (int i : passRangeTileNos) {
			AC_Tile* t = allTiles[i - 1];
			t->RemoveMainMaterial();
		}
	}
	// **

	// ** マークレンジ削除 (*マテリアルのみ) **
	for (AC_Piece* p : defencePlayers) {
		if (p->markRange.IsEmpty()) continue; // 空のチェック

			// マテリアル削除
			for (int i : p->markRange) {
				AC_Tile* t = allTiles[i - 1];
				t->RemoveMainMaterial();
		}
	}
	// **
	 
	// *** プレイ選択処理 ***
	if (ballHolder != nullptr) { // ボールホルダーがいるか
		// <存在>
		// ** ボールホルダーのプレイ選択 **
		SelectPlayForBallHolder();
		// **

		// ** ディフェンダーのプレイ選択 **
		SelectPlayForDefender();
		// **
	}
	else {
		// <不在>
		// ** セカンドボール処理 **
		MovementForSecondBall();
		// **
	}
	// ******

	// ** ③-⑵プレイステップフェーズ監視タイマーセット (*最後に呼び出す) **
	SetTimerMonitorPlayStepPhase();
	// **
}

// ③-3 リセットステップフェーズ (bool: falseでフェーズ終了)
// | 初期値リセット |
bool AC_My_Player_Controller::ResetStepPhase()
{
	// ** パスレンジ削除 (*配列のみ) **
	if (passRangeTileNos.IsEmpty() == false) { // 空でないか
		passRangeTileNos.Empty(); // 配列削除
	}
	// **

	// ** マークレンジ削除 (*配列のみ) **
	for (AC_Piece* p : defencePlayers) {
		if (p->markRange.IsEmpty()) continue; // 空チェック

		p->markRange.Empty(); // 配列削除
	}
	// **

	// ** 現在のタイル・タイル予約削除 **
	currentTileNos.Empty();
	moveToTileNos.Empty();
	// **

	return false;
}
// **************************



// マウス位置に目的のオブジェクトがあるか判定して情報取得
// | retrun: 目的のオブジェクトか判定, hitRusult: 取得するオブジェクト情報, objectTypes: 目的のオブジェクトの種類(コリジョン) |
bool AC_My_Player_Controller::GetResultFromMouseLocation(FHitResult& hitResult, TArray<TEnumAsByte<EObjectTypeQuery>> objectTypes)
{
	// マウスの位置を取得
	FVector _mouseLocation; // *マウス位置
	FVector _mouseDirection; // マウス方向
	APlayerController::DeprojectMousePositionToWorld(_mouseLocation, _mouseDirection); // マウス位置を３D座標で取得

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

// 位置からタイルＮｏ取得
// ( unreal座標  x: 縦, y: 横 )
int AC_My_Player_Controller::GetTileNoFromLocation(float x, float y)
{
	// タイルNo
	int tileNoY = 0; // タイルNo y (横)
	int tileNoX = 0; // タイルNo x (縦)
	int tileNo = 0; // タイルNo

	bool isPositiveY = true; // xが正の整数か
	const int tileSize = 100; // タイルの大きさ (100cm)
	const int tilesNumY = 25; // タイルの横(y)の個数

	// *** タイルNo y(横)を求める ***
	// -の時の処理
	if (y < 0) {
		y = abs(y); // 絶対値に変換
		isPositiveY = false; // フラグ変更
	}

	if (y < 50)	tileNoY = 13;
	else if (y < 150) tileNoY = (isPositiveY) ? 14 : 12;
	else if (y < 250) tileNoY = (isPositiveY) ? 15 : 11;
	else if (y < 350) tileNoY = (isPositiveY) ? 16 : 10;
	else if (y < 450) tileNoY = (isPositiveY) ? 17 : 9;
	else if (y < 550) tileNoY = (isPositiveY) ? 18 : 8;
	else if (y < 650) tileNoY = (isPositiveY) ? 19 : 7;
	else if (y < 750) tileNoY = (isPositiveY) ? 20 : 6;
	else if (y < 850) tileNoY = (isPositiveY) ? 21 : 5;
	else if (y < 950) tileNoY = (isPositiveY) ? 22 : 4;
	else if (y < 1050) tileNoY = (isPositiveY) ? 23 : 3;
	else if (y < 1150) tileNoY = (isPositiveY) ? 24 : 2;
	else if (y < 1250) tileNoY = (isPositiveY) ? 25 : 1;
	// ***

	// *** タイルNo x(縦)を求める ***
	int tX = x / tileSize; // 中央から何列面のタイルか
	// 負の整数の時
	if (x < 0) {
		tX = abs(tX); // 絶対値へ
	}

	switch (tX) {
	case 0:
		tileNoX = (x > 0) ? 21 : 20;
		break;
	case 1:
		tileNoX = (x > 0) ? 22 : 19;
		break;
	case 2:
		tileNoX = (x > 0) ? 23 : 18;
		break;
	case 3:
		tileNoX = (x > 0) ? 24 : 17;
		break;
	case 4:
		tileNoX = (x > 0) ? 25 : 16;
		break;
	case 5:
		tileNoX = (x > 0) ? 26 : 15;
		break;
	case 6:
		tileNoX = (x > 0) ? 27 : 14;
		break;
	case 7:
		tileNoX = (x > 0) ? 28 : 13;
		break;
	case 8:
		tileNoX = (x > 0) ? 29 : 12;
		break;
	case 9:
		tileNoX = (x > 0) ? 30 : 11;
		break;
	case 10:
		tileNoX = (x > 0) ? 31 : 10;
		break;
	case 11:
		tileNoX = (x > 0) ? 32 : 9;
		break;
	case 12:
		tileNoX = (x > 0) ? 33 : 8;
		break;
	case 13:
		tileNoX = (x > 0) ? 34 : 7;
		break;
	case 14:
		tileNoX = (x > 0) ? 35 : 6;
		break;
	case 15:
		tileNoX = (x > 0) ? 36 : 5;
		break;
	case 16:
		tileNoX = (x > 0) ? 37 : 4;
		break;
	case 17:
		tileNoX = (x > 0) ? 38 : 3;
		break;
	case 18:
		tileNoX = (x > 0) ? 39 : 2;
		break;
	case 19:
		tileNoX = (x > 0) ? 40 : 1;
		break;
	}
	// ***

	// *** タイルNo取得 ***
	// ( (タイル縦(x) - 1個) * フィールドの横のタイル個数 ) + タイル横(y)
	tileNo = ((tileNoX - 1) * tilesNumY) + tileNoY;


	return tileNo;
}

// パスレンジのタイルＮｏ取得
TArray<int> AC_My_Player_Controller::GetTileNoInPassRange()
{
	TArray<int> _passRangeNos = {}; // パスレンジ (*最終)
	if (ballHolder == nullptr) return _passRangeNos; // ボールホルダーnullチェック

	int _leftBottomTileNo = 0; // 左下のタイルNo (*コート正面に対して)
	int _ballTileNo = ball->currentTileNo; // ボールタイルNo

	// ** 左下のタイルNo取得 **
	if (ballHolder->direction == 25) {
		// <正面>
		_leftBottomTileNo = _ballTileNo - 4;
	}
	else if (ballHolder->direction == -25) {
		 // <背面>
		_leftBottomTileNo = (_ballTileNo - 4) - (C_Common::TILE_NUM_Y * 5);
	}
	else if (ballHolder->direction == 1) {
		// <右>
		_leftBottomTileNo = _ballTileNo - (C_Common::TILE_NUM_Y * 4);
	}
	else {
		// <左>
		_leftBottomTileNo = _ballTileNo - (C_Common::TILE_NUM_Y * 4) - 5;
	}
	// **
	
	// ** パスレンジ取得 **
	TArray <int> _passRangeOutOfFieldNos = {}; // パスレンジ列No配列 (*フィールド外も含む)
	int _rowRange = FMath::Abs(ballHolder->direction) == 25 ? C_Common::PASS_RANGE_TILE_NUM[1] : C_Common::PASS_RANGE_TILE_NUM[0]; // 縦(行)の数
	int _columnRange = FMath::Abs(ballHolder->direction) == 25 ? C_Common::PASS_RANGE_TILE_NUM[0] : C_Common::PASS_RANGE_TILE_NUM[1]; // 横(列)の数
	
	// 縦(x)の処理
	for (int _r = 0; _r < _rowRange; _r++) {
		// 横(y)の処理
		for (int _c = 0; _c < _columnRange; _c++) {
			int _passRangeNo = _leftBottomTileNo  + _c + (C_Common::TILE_NUM_Y * _r); // パスレンジNo
			
			_passRangeOutOfFieldNos.Add(_passRangeNo);
		}
	}
	// **

	// ** パスレンジがタイル範囲を超えないようにする **
	for (int _tileNo : _passRangeOutOfFieldNos) {
		// **タイル数の上限・下限対応
		if (_tileNo < 1 || allTiles.Num() < _tileNo) continue; // 1 < タイルNo < 1000 の範囲

		// **フィールド端対応
		float _distanceFromBallTile = allTiles[_ballTileNo - 1]->GetDistanceTo(allTiles[_tileNo - 1]); // ボールタイルからの距離
		if (_distanceFromBallTile > 700.0f) continue; // 700fは決め打ち 640.0fが一番遠い距離だったため

		_passRangeNos.Add(_tileNo);
	}
	// **

	// ***** パスレンジ制限 *****
	// | ディフェンダーの配置によって制限する |
	// Row: 横(行), Column: 縦(列)
	TArray<int> _limitPassRangeNos = {}; // 制限パスレンジ

	if (FMath::Abs(ballHolder->direction) == 25) { // 体の向きが
		// <前後>
		for (AC_Piece* df : defencePlayers) {
			// *制限
			// | パスレンジ外のディフェンダー |
			if (_passRangeNos.Contains(df->currentTileNo) == false) continue; // *パスレンジ外のディフェンダー

			// ** ディフェンダーがパスレンジの真ん中 **
			if (df->currentTileNo % C_Common::TILE_NUM_Y == ballHolder->currentTileNo % C_Common::TILE_NUM_Y) { // ディフェンダーがパスレンジ真ん中にいる (同じColumn)
				for (int i : _passRangeNos) { // パスレンジが
					int _passRangeRow = i / C_Common::TILE_NUM_Y; // パスレンジRow
					int _defenderRow = df->currentTileNo / C_Common::TILE_NUM_Y; // ディフェンダーRow
					bool _isPassRangeBackRow = ballHolder->direction > 0 ? _defenderRow <= _passRangeRow : _defenderRow >= _passRangeRow; // パスレンジがディフェンダーの背面のRow = ture
					
					// *パスレンジ制限条件
					// | ⓵ディフェンダーの背面(ディフェンダーRow含む) |
					if (_isPassRangeBackRow) _limitPassRangeNos.Add(i); // ⓵
				}
			}
			// **
			else {
				// ** ディフェンダーがパスレンジの左右どちらか **
				int _defenderColumn = df->currentTileNo % C_Common::TILE_NUM_Y; // ディフェンダーColumn
				int _middleNo = 0; // 中心のタイルNo
				bool _isRightDefender = false; // ディフェンダーが右側か
				
				// ディフェンダーが右左どちらか		
				if (_defenderColumn == 0 || ballHolder->currentTileNo % C_Common::TILE_NUM_Y < _defenderColumn) { // ディフェンダーColumnが大きいか (0は右端)
					// <右>
					_middleNo = df->currentTileNo - 1; // 中心のタイルNo
					_isRightDefender = true; // 右側フラグ
				}
				else {
					// <左>
					_middleNo = df->currentTileNo + 1; // 中心のタイルNo
					_isRightDefender = false; // 右側フラグ
				}
				int _middleNoRow = _middleNo / C_Common::TILE_NUM_Y; // 中心タイルRow
				int _middleNoColumn = _middleNo % C_Common::TILE_NUM_Y; // 中心タイルColumn
				
				for (int i : _passRangeNos) { // パスレンジが
					int _passRangeRow = i / C_Common::TILE_NUM_Y; // パスレンジRow
					int _passRangeColumn = i % C_Common::TILE_NUM_Y; // パスレンジColumn
					bool _isOutsideColumn = _isRightDefender ? _middleNoColumn <= _passRangeColumn : _middleNoColumn >= _passRangeColumn; // パスレンジが中心タイルの外側 = ture
					bool _isPaaRangeBackRow = ballHolder->direction > 0 ? _middleNoRow <= _passRangeRow : _middleNoRow >= _passRangeRow; // パスレンジがディフェンダーの背面のRow = ture

					// *パスレンジ制限条件
					// | ⓵ディフェンダーの背面(ディフェンダーRow含む) |
					// | ⓶ディフェンダーサイド |
					// | ③右端 |
					if (_isPaaRangeBackRow && _isOutsideColumn) _limitPassRangeNos.Add(i); // ⓵⓶
					
					if (_passRangeColumn == 0) { // ③
						if (ballHolder->direction > 0 && _middleNoRow < _passRangeRow) _limitPassRangeNos.Add(i); // 前向き
						if (ballHolder->direction < 0 && _middleNoRow > _passRangeRow) _limitPassRangeNos.Add(i); // 後ろ向き
					}
				}
			}
		}
	}
	else {
		// <左右>
		for (AC_Piece* df : defencePlayers) {
			// *制限
			// | パスレンジ外のディフェンダー |
			if (_passRangeNos.Contains(df->currentTileNo) == false) continue; // *パスレンジ外のディフェンダー
			
			// ** ディフェンダーがパスレンジの真ん中 **
			if ((int)df->currentTileNo / C_Common::TILE_NUM_Y == (int)ballHolder->currentTileNo / C_Common::TILE_NUM_Y) { // ディフェンダーがパスレンジ真ん中にいる (同じRow)
				for (int i : _passRangeNos) { // パスレンジが
					int _passRangeColumn = i % C_Common::TILE_NUM_Y; // パスレンジColumn
					int _defenderColumn = df->currentTileNo % C_Common::TILE_NUM_Y; // ディフェンダーColumn
					bool _isPassRangeBackColumn = ballHolder->direction > 0 ? _defenderColumn <= _passRangeColumn : _defenderColumn >= _passRangeColumn; // パスレンジがディフェンダーの背面のColumn = ture (右 : 左)

					// *パスレンジ制限条件
					// | ⓵ディフェンダーの背面(ディフェンダーColumn含む) |
					// | ⓶右端 |
					if (_isPassRangeBackColumn) _limitPassRangeNos.Add(i); // ⓵
					if (_passRangeColumn == 0) _limitPassRangeNos.Add(i); // ⓶
				}
			}
			// **
			else {
				// ** ディフェンダーがパスレンジの左右どちらかにいる **
				int _defenderRow = df->currentTileNo / C_Common::TILE_NUM_Y; // ディフェンダーRow
				int _middleNo = 0; // 中心のタイルNo
				bool _isTopDefender = false; // ディフェンダーが中心より上側か

				// ディフェンダーが上下どちらか				
				if ((int)ballHolder->currentTileNo / C_Common::TILE_NUM_Y < _defenderRow) { // ディフェンダーRowが大きいか
					// <上>
					_middleNo = df->currentTileNo - C_Common::TILE_NUM_Y; // 中心のタイルNo
					_isTopDefender = true; // 上側フラグ
				}
				else {
					// <下>
					_middleNo = df->currentTileNo + C_Common::TILE_NUM_Y; // 中心のタイルNo
					_isTopDefender = false; // 上側フラグ
				}
				int _middleNoRow = _middleNo / C_Common::TILE_NUM_Y; // 中心タイルRow
				int _middleNoColumn = _middleNo % C_Common::TILE_NUM_Y; // 中心タイルColumn

				for (int i : _passRangeNos) { // パスレンジが
					int _passRangeRow = i / C_Common::TILE_NUM_Y; // パスレンジRow
					int _passRangeColumn = i % C_Common::TILE_NUM_Y; // パスレンジClumn
					bool _isOutsideRow = _isTopDefender ? _middleNoRow <= _passRangeRow : _middleNoRow >= _passRangeRow; // パスレンジが中心タイルの外側 = ture (上 : 下)
					bool _isPassRangeBackColumn = ballHolder->direction > 0 ? _middleNoColumn <= _passRangeColumn : _middleNoColumn >= _passRangeColumn; // パスレンジがディフェンダーの背面のColumn = ture (右 : 左)

					// *パスレンジ制限条件
					// | ⓵ディフェンダーの背面(ディフェンダーColumn含む) |
					// | ⓶ディフェンダーサイド |
					// | ③右端 |
					if (_isPassRangeBackColumn && _isOutsideRow) _limitPassRangeNos.Add(i); // ⓵⓶
					if (_passRangeColumn == 0 && _middleNoRow < _passRangeRow) _limitPassRangeNos.Add(i); // ③
				}
			}
		}
	}
	// *************************

	// ** 制限タイルを削除する **
	if (_limitPassRangeNos.IsEmpty()) return _passRangeNos;

	for (int i : _limitPassRangeNos) {
		_passRangeNos.Remove(i);
	}
	// **


	return _passRangeNos;
}

// ボールホルダーの向きを取得 ( 前: 25, 後ろ: -25, 右: 1, 左: -1 )
int AC_My_Player_Controller::GetDirectionOfBallHolder()
{
	int ballHolderTileNo = ballHolder->currentTileNo; // ボールホルダータイルNo
	int ballTileNo = ball->currentTileNo; // ボールのタイルNo

	if ((ballHolderTileNo + C_Common::TILE_NUM_Y) == ballTileNo) return C_Common::TILE_NUM_Y; // 前
	if ((ballHolderTileNo - C_Common::TILE_NUM_Y) == ballTileNo) return -C_Common::TILE_NUM_Y; // 後ろ
	if ((ballHolderTileNo + 1) == ballTileNo) return 1; // 右
	return -1; // 左
}

// 対人範囲のタイルNo取得
// 対人範囲: ⓵ボールの正面 ⓶⓵の左右 ③ボールの左右 の計5タイル
TArray<int> AC_My_Player_Controller::GetTileNoInDuelRange()
{
	// タイルＮｏ
	int _forwardTileNo = ball->currentTileNo + ballHolder->direction; // 正面
	int _forwardRightTileNo = 0; // 正面右
	int _forwardLeftTileNo = 0; // 正面左
	int _rightTileNo = 0; // 右
	int _leftTileNo = 0; // 左

	// 左右のタイルを取得
	if (FMath::Abs(ballHolder->direction) == 25) { // 体の向きが前後を向いているか
		// <前後>
		_forwardRightTileNo = _forwardTileNo + 1;
		_forwardLeftTileNo = _forwardTileNo - 1;
		_rightTileNo = ball->currentTileNo + 1;
		_leftTileNo = ball->currentTileNo - 1;
	}
	else {
		// <左右>
		_forwardRightTileNo = _forwardTileNo - C_Common::TILE_NUM_Y;
		_forwardLeftTileNo = _forwardTileNo + C_Common::TILE_NUM_Y;
		_rightTileNo = ball->currentTileNo + C_Common::TILE_NUM_Y;
		_leftTileNo = ball->currentTileNo - C_Common::TILE_NUM_Y;
	}
	// **
	
	TArray<int> _tentativeRange = { _forwardTileNo, _forwardRightTileNo, _forwardLeftTileNo, _rightTileNo, _leftTileNo }; // 対人レンジ配列(仮)
	TArray<int> duelRange = {}; // 対人レンジ

	// ** コートの範囲対応 **
	for (int t : _tentativeRange) {
		if (t < 1 || t > 1000) continue; // タイル範囲(前後)

		if (allTiles[ball->currentTileNo - 1]->GetDistanceTo(allTiles[t - 1]) > C_Common::AROUNT_TILE_RANGE) continue; // *周囲のタイルか
		
		duelRange.Add(t);
	}
	// **


	return duelRange;
}

// マークレンジ取得
// | 体の正面6マス |
TArray<int> AC_My_Player_Controller::GetMarkRange(int currentTileNo, int playerDirection)
{
	// 1列目
	int forward = currentTileNo + playerDirection;
	int forwardMinusY = forward - 1;
	int forwardPlusY = forward + 1;
	// 2列目
	int forwardTwo = forward + playerDirection;
	int forwardTwoMinusY = forwardTwo - 1;
	int forwardTowPlusY = forwardTwo + 1;

	TArray <int> _range = { forward, forwardMinusY, forwardPlusY, forwardTwo, forwardTwoMinusY, forwardTowPlusY };

	return _range;
}

// ファーストディフェンダー取得
// | ボールホルダーに最も近いディフェンダー取得 |
AC_Piece* AC_My_Player_Controller::GetFirstDefender()
{
	if (defencePlayers.IsEmpty()) return nullptr; // null チェック

	// ** ボールホルダーに近い順にソート **
	TArray <AC_Piece*> players = defencePlayers;
	players.StableSort([&](const AC_Piece& A, const AC_Piece& B) {
		return ballHolder->GetDistanceTo(&A) < ballHolder->GetDistanceTo(&B);; // 距離が近い順
		});
	// **


	return players[0];
}

// 次に動く最短距離のタイルNo取得
int AC_My_Player_Controller::GetShortestNextTileNo(int fromTileNo, int toTileNo)
{
	if (fromTileNo == toTileNo) return toTileNo; // 目的地に既にいる場合 

	// ** fromTileの周囲マス取得 **
	// 十字マス
	int plusX = fromTileNo + C_Common::TILE_NUM_Y;
	int minusX = fromTileNo - C_Common::TILE_NUM_Y;
	int plusY = fromTileNo + 1;
	int minusY = fromTileNo - 1;
	// 斜めマス
	int plusXplusY = plusX + 1;
	int plusXminusY = plusX - 1;
	int minusXplusY = minusX + 1;
	int minusXminusY = minusX - 1;
	
	TArray <int> _fromAroundTileNos = { plusX, minusX, plusY, minusY, plusXplusY, plusXminusY, minusXplusY, minusXminusY }; // タイルの周囲No
	// **

	// ** toTileNoとの距離を取得 **
	TArray <int> _toTileDistances; // toTileNoとの距離の配列
	for (int n : _fromAroundTileNos) {
		// * 制限 *
		// _fromAroundTileNosと要素を合わせる
		if (n < 1 || n > 1000) { // *タイルNoがタイル数内か
			_fromAroundTileNos.Remove(n); // 削除
			
			continue;
		}
		if (allTiles[fromTileNo - 1]->GetDistanceTo(allTiles[n - 1]) > C_Common::AROUNT_TILE_RANGE) { // *周囲のタイルか (fromtileと周囲タイルの距離が150.0f以上)
			_fromAroundTileNos.Remove(n); // 削除
			
			continue;
		}
		// *

		AC_Tile* _aroundTile = allTiles[n - 1]; // 周囲タイル
		AC_Tile* _toTile = allTiles[toTileNo - 1]; // toタイル
		_toTileDistances.Add(_toTile->GetDistanceTo(_aroundTile)); // 配列セット
	}
	// **

	// ** 最も小さい距離取得 ** 
	TArray <int> _c_toTileDistances = _toTileDistances; // コピー
	// 小さい順にソート
	_c_toTileDistances.StableSort([](const int& A, const int& B) {
		return A < B;
		});
	int minDistance = _c_toTileDistances[0]; // 最も小さい距離
	// **

	// ** 最も小さい距離のタイルNo取得 **
	int minDistanceIndex = _toTileDistances.IndexOfByKey(minDistance); // index
	if (minDistanceIndex == -1) // タイルNo取得できなかった場合
	{
		if (C_Common::DEBUG_MODE) UKismetSystemLibrary::PrintString(this, "NOT GET SHOTEST TILE NO");
		return _fromAroundTileNos[0]; // 最初のタイルNoを返す
	}
	int shotestToTileNo = _fromAroundTileNos[minDistanceIndex]; // タイルNo
	// **


	return shotestToTileNo;
}

// プレイヤー配置可能エリアを表示
void AC_My_Player_Controller::DisplayPlayerPlaceableErea()
{
	// *** 配置可能エリア表示 ***
	// *HOME*
	for (int i = C_Common::HOME_PLACE_EREA[0]; i <= C_Common::HOME_PLACE_EREA[1]; i++) {
		if (i <= 250) {
			// -DF-
			allTiles[i - 1]->SetDFPlayerPlaceRangeMaterial();

			// ** ハーフレーンマテリアルセット **
			if (LCB_Erea.Contains(i) || RCB_Erea.Contains(i)) allTiles[i - 1]->SetHalfLernMaterial();
			// **
		}
		else if (i <= 375) {
			// -MF-
			allTiles[i - 1]->SetMFPlayerPlaceRangeMaterial();

			// ** ハーフレーンマテリアルセット **
			if (LIH_Erea.Contains(i) || RIH_Erea.Contains(i)) allTiles[i - 1]->SetHalfLernMaterial();
			// **
		}
		else {
			// -FW-
			allTiles[i - 1]->SetFWPlayerPlaceRangeMaterial();
			
			// ** ハーフレーンマテリアルセット **
			if (LST_Erea.Contains(i) || RST_Erea.Contains(i)) allTiles[i - 1]->SetHalfLernMaterial();
			// **
		}
	}
	// *AWAY*
	if (C_Common::POSITION_EREA_DISPLAY) { // 表示モードの場合のみ
		for (int i = C_Common::AWAY_PLACE_EREA[0]; i <= C_Common::AWAY_PLACE_EREA[1]; i++) {
			if (i <= 625) {
				// -FW-
				allTiles[i - 1]->SetFWPlayerPlaceRangeMaterial();

				// ** ハーフレーンマテリアルセット **
				if (Away_LST_Erea.Contains(i) || Away_RST_Erea.Contains(i)) allTiles[i - 1]->SetHalfLernMaterial();
				// **
			}
			else if (i <= 750) {
				// -MF-
				allTiles[i - 1]->SetMFPlayerPlaceRangeMaterial();

				// ** ハーフレーンマテリアルセット **
				if (Away_LIH_Erea.Contains(i) || Away_RIH_Erea.Contains(i)) allTiles[i - 1]->SetHalfLernMaterial();
				// **
			}
			else {
				// -DF-
				allTiles[i - 1]->SetDFPlayerPlaceRangeMaterial();

				// ** ハーフレーンマテリアルセット **
				if (Away_LCB_Erea.Contains(i) || Away_RCB_Erea.Contains(i)) allTiles[i - 1]->SetHalfLernMaterial();
				// **
			}
		}
	}
	// ***
}

// プレイヤー配置レンジを削除
void AC_My_Player_Controller::RemovePlayerPlaceRange()
{
	for (AC_Tile* t : allTiles) {
		t->RemoveSubMaterial(); // サブ
		t->RemoveMainMaterial(); // メイン
	}
}

// 全てのプレイヤーにポジションを設定
void AC_My_Player_Controller::SetPositionToAllPlayer()
{
	for (AC_Piece* p : allPieces) {
		int _tileNo = p->currentTileNo; // タイルNo
		// タイル番号チェック
		if (_tileNo == 0) {
			UKismetSystemLibrary::PrintString(this, "ERROR PLAYERS NOT HOVE TILE NO", true, true, FColor::Cyan, 5.f, TEXT("None"));

			return;
		}
		
		// ポジションセット
		if (LSB_Erea.Contains(_tileNo) || Away_LSB_Erea.Contains(_tileNo)) {
			p->position = C_Common::LSB_POSITION;

			continue;
		}
		else if (LCB_Erea.Contains(_tileNo) || Away_LCB_Erea.Contains(_tileNo)) {
			p->position = C_Common::LCB_POSITION;

			continue;
		}
		else if (CB_Erea.Contains(_tileNo) || Away_CB_Erea.Contains(_tileNo)) {
			p->position = C_Common::CB_POSITION;

			continue;
		}
		else if (RCB_Erea.Contains(_tileNo) || Away_RCB_Erea.Contains(_tileNo)) {
			p->position = C_Common::RCB_POSITION;

			continue;
		}
		else if (RSB_Erea.Contains(_tileNo) || Away_RSB_Erea.Contains(_tileNo)) {
			p->position = C_Common::RSB_POSITION;

			continue;
		}
		else if (LH_Erea.Contains(_tileNo) || Away_LH_Erea.Contains(_tileNo)) {
			p->position = C_Common::LH_POSITION;

			continue;
		}
		else if (LIH_Erea.Contains(_tileNo) || Away_LIH_Erea.Contains(_tileNo)) {
			p->position = C_Common::LIH_POSITION;

			continue;
		}
		else if (CH_Erea.Contains(_tileNo) || Away_CH_Erea.Contains(_tileNo)) {
			p->position = C_Common::CH_POSITION;

			continue;
		}
		else if (RIH_Erea.Contains(_tileNo) || Away_RIH_Erea.Contains(_tileNo)) {
			p->position = C_Common::RIH_POSITION;

			continue;
		}
		else if (RH_Erea.Contains(_tileNo) || Away_RH_Erea.Contains(_tileNo)) {
			p->position = C_Common::RH_POSITION;

			continue;
		}
		else if (LWG_Erea.Contains(_tileNo) || Away_LWG_Erea.Contains(_tileNo)) {
			p->position = C_Common::LWG_POSITION;

			continue;
		}
		else if (LST_Erea.Contains(_tileNo) || Away_LST_Erea.Contains(_tileNo)) {
			p->position = C_Common::LST_POSITION;

			continue;
		}
		else if (CF_Erea.Contains(_tileNo) || Away_CF_Erea.Contains(_tileNo)) {
			p->position = C_Common::CF_POSITION;

			continue;
		}
		else if (RST_Erea.Contains(_tileNo) || Away_RST_Erea.Contains(_tileNo)) {
			p->position = C_Common::RST_POSITION;

			continue;
		}
		else if (RWG_Erea.Contains(_tileNo) || Away_RWG_Erea.Contains(_tileNo)) {
			p->position = C_Common::RWG_POSITION;

			continue;
		}
		else {
			p->position = C_Common::GK_POSITION;

			continue;
		}

	}
}

// 全てのプレイヤーとボールにタイル位置を設定
TArray<int> AC_My_Player_Controller::SetTileNoToAllPlayersAndBall()
{
	TArray<int> _currentTileNos = {}; // プレイヤーのタイルNo
	for (AC_Piece* p : allPieces) {
		FVector l = p->GetActorLocation(); // コマの位置
		int n = GetTileNoFromLocation(l.X, l.Y); // タイルNo
		p->currentTileNo = n; // タイルNoセット
		_currentTileNos.Add(n); // 配列作成
	}

	FVector bl = ball->GetActorLocation(); // コマの位置
	int bn = GetTileNoFromLocation(bl.X, bl.Y); // タイルNo
	ball->currentTileNo = bn; // タイルNoセット
	_currentTileNos.Add(bn); // 配列作成

	return _currentTileNos;
}

// プレイヤーとボール位置をタイル位置へ設定
void AC_My_Player_Controller::SetPlayerAndBallToTilePosition()
{
	for (AC_Piece* p : allPieces) {
		FVector _currentTileLocation = allTiles[p->currentTileNo - 1]->GetActorLocation();
		_currentTileLocation.Z = C_Common::BASE_LOCATION_Z;
		
		p->SetActorLocation(_currentTileLocation);
	}

	FVector _ballCurrentTileLocation = allTiles[ball->currentTileNo - 1]->GetActorLocation();
	_ballCurrentTileLocation.Z = C_Common::BASE_LOCATION_Z;
	ball->SetActorLocation(_ballCurrentTileLocation);
}

// プレイヤー間のラインを表示
// | positionのnullチェック未 |
void AC_My_Player_Controller::DisplayLineBetweenPlayers()
{
	// ** プレイヤー **
	// FW
	AC_Piece* _LST_Player[2] = {};
	AC_Piece* _RST_Player[2] = {};
	// MF
	AC_Piece* _LH_Player[2] = {};
	AC_Piece* _LIH_Player[2] = {};
	AC_Piece* _RIH_Player[2] = {};
	AC_Piece* _RH_Player[2] = {};
	// DF
	AC_Piece* _LSB_Player[2] = {};
	AC_Piece* _LCB_Player[2] = {};
	AC_Piece* _RCB_Player[2] = {};
	AC_Piece* _RSB_Player[2] = {};
	// **

	// ** プレイヤーを取得 **
	for (AC_Piece* p : allPieces) {
		int i = 0; // Home = 0,  Away = 1
		if (p->ActorHasTag(FName("HOME")) == false) { // Awayプレイヤーか
			i = 1;
		}
		// FW
		if (p->position == C_Common::LST_POSITION) _LST_Player[i] = p;
		if (p->position == C_Common::RST_POSITION) _RST_Player[i] = p;
		// MF
		if (p->position == C_Common::LH_POSITION) _LH_Player[i] = p;
		if (p->position == C_Common::LIH_POSITION) _LIH_Player[i] = p;
		if (p->position == C_Common::RIH_POSITION) _RIH_Player[i] = p;
		if (p->position == C_Common::RH_POSITION) _RH_Player[i] = p;
		// DF
		if (p->position == C_Common::LSB_POSITION) _LSB_Player[i] = p;
		if (p->position == C_Common::LCB_POSITION) _LCB_Player[i] = p;
		if (p->position == C_Common::RCB_POSITION) _RCB_Player[i] = p;
		if (p->position == C_Common::RSB_POSITION) _RSB_Player[i] = p;
	}
	// **

	// ** ライン表示 **
	for (int i = 0; i < 2; i++) { // 2回繰り返す
		// ** ラインの色を変更 **
		FLinearColor lineColor = FLinearColor::Blue; // ラインの色
		if (i == 1) { // Awayの場合
			lineColor = FLinearColor::Red;
		}
		// **

		// FW
		_LST_Player[i]->SetDrawLineTo(_RST_Player[i]->GetActorLocation(), lineColor);
		// MF
		_LH_Player[i]->SetDrawLineTo(_LIH_Player[i]->GetActorLocation(), lineColor);
		_LIH_Player[i]->SetDrawLineTo(_RIH_Player[i]->GetActorLocation(), lineColor, _LST_Player[i]->GetActorLocation());
		_RIH_Player[i]->SetDrawLineTo(_RH_Player[i]->GetActorLocation(), lineColor, _RST_Player[i]->GetActorLocation());
		// DF
		_LSB_Player[i]->SetDrawLineTo(_LCB_Player[i]->GetActorLocation(), lineColor, _LH_Player[i]->GetActorLocation());
		_LCB_Player[i]->SetDrawLineTo(_RCB_Player[i]->GetActorLocation(), lineColor);
		_RCB_Player[i]->SetDrawLineTo(_RSB_Player[i]->GetActorLocation(), lineColor);
		_RSB_Player[i]->SetDrawLineTo(_RH_Player[i]->GetActorLocation(), lineColor);
	}
	// **
	// ---
}

// ③-⑵プレイステップフェーズを監視するタイマー設定
void AC_My_Player_Controller::SetTimerMonitorPlayStepPhase()
{
	// ** タイマー開始 **
	FTimerHandle handle;
	GetWorldTimerManager().SetTimer(handle, this, &AC_My_Player_Controller::MonitorFinishPlayStepPhase, 0.5f, true); // 0.5秒おき
	// **
}
