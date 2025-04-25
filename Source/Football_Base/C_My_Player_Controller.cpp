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

	// *** プレイヤー取得 ***
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
	SetPositionToAllPlayer(); // ←ゲーム起動時に呼ばないとコマが移動できなくなる
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

			// ** ポジションを設定する **
			SetPositionToAllPlayer(); // ←*対戦フェーズで呼ぶと2次配置後の配置でポジションが決まってしまうため
			//**
			
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
	AC_Piece* _shortPassTarget = nullptr; // ショートパスターゲット
	AC_Piece* _longPassTarget = nullptr; // ロングパスターゲット

	// ** ロングボール動作 **
	if (isPreActionedLongBall) {
		if (longPassTarget != nullptr) {
			// <ターゲットがいる>

			// --ロングパス
			LongPass(longPassTarget);
		}
		else {
			// <ターゲットがいない>

			// --ロングアタック
			LongAttack();
		}

		isLongBalled = true; // ロングボール終了ON

		return;
	}
	// ** 

	// ** エアバトル **
	// | 勝者:                            |
	// |  ボールホルダー: ボールのみ4マス前進 |
	// |  ディフェンダー: クリアリング       |
	if (isFloatingBall) { // ボールが浮いている
		for (AC_Piece* p : defencePlayers) { // ディフェンダーの中で
			if (ballHolderAroundTileNos.Contains(p->currentTileNo)) { // ボールホルダー周囲にいれば
				bool _isBallHolderWin = AirBattle(p);
				if (C_Common::DEBUG_MODE) UKismetSystemLibrary::PrintString(this, "AIRBATTLE", true, true, FColor::Cyan, 2.f, TEXT("None"));
				
				if (_isBallHolderWin) { // どちかが対人に処理したか
					// <ボールホルダーが勝利>

					// ** ボールのみ5マス前進する(*ボールを基準として) **
					// | 距離の調整はクリアリングと同じ |

					const int _moveTileNum = 5; // 移動するタイル数
					int _moveToTileNo = 0; // 移動先のタイルNo
					int _num = _moveTileNum; // 可変するタイル数(デクリメント用)
					// 移動先のタイルを決定する
					for (int i = 0; i < _moveTileNum; i++) { // *ボール移動の距離を調整
						if (isHomeBall) { // 移動の方向を決める
							// <ホーム>
							_moveToTileNo = ball->currentTileNo + (C_Common::FORWARD_DIRECTION * _num);
						}
						else {
							// <アウェイ>
							_moveToTileNo = ball->currentTileNo - (C_Common::FORWARD_DIRECTION * _num);
						}
						// ボール移動の距離を調整
						if (currentTileNos.Contains(_moveToTileNo) || moveToTileNos.Contains(_moveToTileNo)) { // ボールの移動先にプレイヤーがいれば
							_num--; // ボール移動の距離を短縮

							continue;
						}

						break;
					}
					
					// ボール移動
					ball->SetMoveTo(allTiles[_moveToTileNo - 1]->GetActorLocation());

					isAirBattle = true; // フラグON
					if (C_Common::DEBUG_MODE) UKismetSystemLibrary::PrintString(this, "MOVE 5 TILES", true, true, FColor::Cyan, 2.f, TEXT("None"));
				}
				else {
					// <ディフェンダーが勝利>

					// ** クリアリング **
					if (C_Common::DEBUG_MODE) UKismetSystemLibrary::PrintString(this, "CLEARING", true, true, FColor::Cyan, 2.f, TEXT("None"));
					Clearing(p);
					// **
				}

				return;
			}
		}
	}
	
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
							_shortPassTarget = op;

							break;
						}
					}

					if (_shortPassTarget != nullptr) { // ショートパスターゲットがいるか
						// 〇パスターゲット存在

						// ** ショートパス (落としのプレー) **
						if (C_Common::DEBUG_MODE) UKismetSystemLibrary::PrintString(this, "POST PLAY", true, true, FColor::Cyan, 2.f, TEXT("None"));
						ShortPass(_shortPassTarget);
						// **

					}
					else {
						// ●パスターゲット不在

						// ** キープ **
						if (C_Common::DEBUG_MODE) UKismetSystemLibrary::PrintString(this, "KEEP", true, true, FColor::Cyan, 2.f, TEXT("None"));

						// *********************
						// ここにキープアニメーション追加!!!!
						// *********************

						isBallKeeping = true; // フラグON
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
			// ショートパスターゲット取得 (**対人前に取得)
			for (AC_Piece* op : offencePlayers) { // オフェンスプレイヤーの中で
				if (passRangeTileNos.Contains(op->currentTileNo) && op != preBallHolder) { // パスレンジ内のHomeコマ && 前回のボールホルダーでない ( *リターンパス禁止処理 )
					_shortPassTarget = op;

					break;
				}
			}

			if (_shortPassTarget == nullptr) { // パスターゲットが取得できなかった場合
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

	// ** ロングアタック **
	//    →DFラインの裏へボールを配給する
	// *条件*
	// | ⓵ボールホルダーが前向き          |
	// | ⓶パスターゲットが同じレーンにいる  |
	// | ③ターゲットがラインブレイク前動作をしているか |
	if (ballHolder->direction == _ballSideForward) { // 1
		for (AC_Piece* p : offencePlayers) {
			if (ballHolder->currentLane != p->currentLane) continue; // 2
			
			if (p->isLineBreakPreAction) { // 3
				// ** ロングアタック (*予約) **
				PreActionForLongBall(); // ロングボール事前動作

				isPreActionedLongBall = true; // フラグON
				// **


				return;
			}
		}
	}

	// ** ロングパスターゲット取得 **
	//    →DFからFWへボールを配給する
	// *条件*
	// | ⓵ボールホルダーがGK or DF                 |
	// | ⓶ボールホルダーが前向き                    |
	// | ③パスターゲットがショートパスレンジ内にいない |
	// | ⓸パスターゲットがFW                       |
	// | ⓹パスターゲットが同じレーンにいる           |
	if (ballHolder->position == C_Common::GK_POSITION || DFPositionRange.Contains(ballHolder->position)) { // 1
		if (ballHolder->direction == _ballSideForward) { // 2
			// FWエリアのプレイヤーをターゲットに設定
			for (AC_Piece* p : offencePlayers) {
				if (passRangeTileNos.Contains(p->currentTileNo)) continue; // 3

				if (FWPositionRange.Contains(p->position)) { // 4
					if (ballHolder->currentLane == p->currentLane) { // 5
						_longPassTarget = p;

						break;
					}
				}
			}
		}
	}
	// **

	// ** ボール移動判定 **
	// | *パス処理の前に必ず実行 |
	// *制限
	// ⓵移動先タイルにプレイヤーがいる
	// ⓶移動先タイルに動く予定のプレイヤーがいる
	if (_shortPassTarget != nullptr) { // ショートパス時
		int _ballMoveToTileNo = _shortPassTarget->currentTileNo + _shortPassTarget->direction; // ボールを動かすタイルNo

		if (currentTileNos.Contains(_ballMoveToTileNo) || moveToTileNos.Contains(_ballMoveToTileNo)) _shortPassTarget = nullptr; // パスターゲット削除
	}

	if (_longPassTarget != nullptr) { // ロングパス時
		int _ballMoveToTileNo = isHomeBall ? _longPassTarget->currentTileNo - C_Common::TILE_NUM_Y : _longPassTarget->currentTileNo + C_Common::TILE_NUM_Y; // ボールを動かすタイルNo

		if (currentTileNos.Contains(_ballMoveToTileNo) || moveToTileNos.Contains(_ballMoveToTileNo)) _longPassTarget = nullptr; // パスターゲット削除
	}
	// **

	// ** ショートパス or ロングボール事前動作(ロングパスの) or ドリブル or 方向転換 **
	// * 制限 *
	// |  ①パスターゲット取得可能 |
	//if (_passTarget != nullptr && _passTarget->isMarked == false) { // ターゲットがマークされていない
	if (_shortPassTarget != nullptr) {
		// <ショートパスターゲットが存在>

		// ** ショートパス **
		ShortPass(_shortPassTarget);
		// **
	}
	else if (_longPassTarget != nullptr) {
		// <ロングパスターゲットが存在>

		// ** ロングパス (*予約) **
		PreActionForLongBall(); // ロングボール事前動作

		longPassTarget = _longPassTarget; // ロングパスターゲット保持
		isPreActionedLongBall = true; // フラグON
		// **


		return;
	}
	else {
		// <パスターゲットが不在>
		if (ballHolder->direction == _ballSideForward) { // 前向きか
			// <前向き>

			// ** ドリブル **
			Drrible(); // *** 前進のみ
			// **


			return;
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
void AC_My_Player_Controller::SelectPlayForDefender(AC_Piece* defensePlayer)
{
	// ** プレス **
	//  →ファーストディフェンダーがボールホルダーにプレス
	if (firstDefender == defensePlayer) { // ファーストディフェンダーの場合
		Press();

		return;
	}
	// **
	
	// ** マーキング **
	//  →マークレンジ内の非オフェンスプレイヤーの前タイルへ移動
	for (AC_Piece* _offencePlayer : offencePlayers) {
		// * 条件 *
		// | 1.マークレンジ内 |
		// | 2.ボールホルダーでない |
		if (defensePlayer->markRange.Contains(_offencePlayer->currentTileNo)) { // 1
			if (_offencePlayer != ballHolder) { // 2

				Marking(defensePlayer, _offencePlayer);

				return;
			}
		}
	}
	// **
}
// *****************************



// ********* プレイ *********
// ショートパス
// | 体の方向へパスを出す |
void AC_My_Player_Controller::ShortPass(AC_Piece* targetPiece)
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

// ロングパス
// | ターゲットの1個下タイルにボールを移動させる |
void AC_My_Player_Controller::LongPass(AC_Piece* targetPiece)
{
	// ** ターゲットの背後のタイル取得 **
	int _targetBackTileNo = isHomeBall ? targetPiece->currentTileNo - C_Common::TILE_NUM_Y: targetPiece->currentTileNo + C_Common::TILE_NUM_Y;
	FVector _moveToLocation = allTiles[_targetBackTileNo - 1]->GetActorLocation(); // ボールを動かす位置
	// **

	// ** 移動先のタイルを予約する **
	if (moveToTileNos.Contains(_targetBackTileNo)) { // タイルが予約されているか
		// 予約あり

		return; // 移動しない
	}
	else {
		// 予約なし

		moveToTileNos.Add(_targetBackTileNo); // 予約する
	}
	// **

	// ** パスターゲットを設定 **
	// | 次の3-⓵準備ステップ時にボールホルダーを変更 |
	passTarget = targetPiece;
	// **

	// ** ボールを移動させる **
	ball->SetMoveTo(_moveToLocation);
	// **
}

// ロングアタック
// →DFラインの背後にボールを配給
// | ターゲットのレーンに対してボールを配給する |
void AC_My_Player_Controller::LongAttack()
{
	const int _attackTileNum = 3; // *ディフェンスラインから何タイル目

	// ** 最後尾の相手プレイヤー取得 **
	AC_Piece* _defenseLinePlayer = nullptr; // *最後尾のディフェンダー
	TArray<AC_Piece*> _fieldPlayeres = {}; // フィールドプレイヤー
	
	// *制限*
	// ⓵GK
	for (AC_Piece* _p : defencePlayers) {
		if (_p->position == C_Common::GK_POSITION) continue; // 1

		_fieldPlayeres.Add(_p);
	}
	// タイルNoが大きい順にソート
	if (isHomeBall) {
		// <Home>
		_fieldPlayeres.StableSort([](const AC_Piece& A, const AC_Piece& B) {
				return A.currentTileNo > B.currentTileNo; // 大きい
			});
	}
	else {
		// <Away>
		_fieldPlayeres.StableSort([](const AC_Piece& A, const AC_Piece& B) {
			return A.currentTileNo < B.currentTileNo; // 小さい
			});
	}

	_defenseLinePlayer = _fieldPlayeres[0];
	// **

	// ** 最終ラインのレーンのタイルNosを取得 **
	TArray<int> _targetTileNos = {}; // *
	
	int _defenseLineFrontRow = _defenseLinePlayer->currentTileNo / C_Common::TILE_NUM_Y; // ディフェンスライン前(*1行)の行数 (横)
	int _leftMostLaneTile = 0; // レーンの左端タイルNo
	// 左端タイル取得
	switch (ballHolder->currentLane)
	{
	case (1):
		_leftMostLaneTile = 1;
		break;
	case (2):
		_leftMostLaneTile = 6;
		break;
	case (3):
		_leftMostLaneTile = 11;
		break;
	case (4):
		_leftMostLaneTile = 16;
		break;
	default:
		_leftMostLaneTile = 21;
	}
	// レーンのタイルを追加していく(5タイル)
	for (int i = 0; i < 5; i++) {
		int n = (_defenseLineFrontRow * C_Common::TILE_NUM_Y) + (_leftMostLaneTile + i);

		_targetTileNos.Add(n);
	}
	// **
	
	// ** タイルをランダムに選択 **
	// | (未実装)キック精度に合わせて |
	int _randomTileNo = FMath::RandRange(_targetTileNos[0], _targetTileNos[_targetTileNos.Num() - 1]); // ランダムタイルNo
	// **
	
	// ** ディフェンスラインの背後にターゲットタイルを設定 **
	int _targetTileNo = 0; // *最終ターゲットタイル
	// *制限*
	// | ⓵タイル数 |
	int _tileNum = _attackTileNum; // タイル数
	for (int i = 0;i <= _attackTileNum; i++) {
		_targetTileNo = _randomTileNo + (C_Common::TILE_NUM_Y * _tileNum);

		if (_targetTileNo <= 1000) break; // 1
		_tileNum--;
	}
	// **

	// ** 移動先のタイルを予約する **
	if (moveToTileNos.Contains(_targetTileNo)) { // タイルが予約されているか
		// 予約あり

		return; // 移動しない
	}
	else {
		// 予約なし

		moveToTileNos.Add(_targetTileNo); // 予約する
	}
	// **

	// ** 移動 **
	ball->SetMoveTo(allTiles[_targetTileNo - 1]->GetActorLocation());
	// **
}

// ロングボールの事前動作
// (暫定)後でアニメーションで実装するか？
void AC_My_Player_Controller::PreActionForLongBall()
{
	// ** 背後タイル位置との中間位置を取得 **
	FVector _ballHolderLocation = allTiles[ballHolder->currentTileNo - 1]->GetActorLocation(); // ボールホルダータイル
	FVector _backLocation = _ballHolderLocation; // 背後タイル位置 (*位置情報のみ)
	_backLocation.X = isHomeBall ? _backLocation.X - 100: _backLocation.X + 100;

	FVector _betweenLocation = FMath::Lerp(_ballHolderLocation, _backLocation, 0.3); // 中間位置
	// **

	// ** 移動 **
	ballHolder->SetMoveTo(_betweenLocation);
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

// エアバトル
// | dueledPlayer: 対人をされるプレイヤー (発火時,ディフェンダー) |
// | return bool: デゥエルの勝者(ボールホルダー = ture, ディフェンダー = false) |
bool AC_My_Player_Controller::AirBattle(AC_Piece* dueledPlayer)
{
	// 能力値が同じだった場合
	if (ballHolder->heading == dueledPlayer->heading) {
		// ランダムに勝者がきまる
		bool r = FMath::RandBool(); // ランダムBool

		return r;
	}

	// ヘディングの能力値を比較
	if (ballHolder->heading > dueledPlayer->heading) {
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
	// | 既に対人レンジ内にいる |
	// | 既にボールホルダー周囲にいる |
	// | 既にプレイヤーがいるタイル or 移動予約済みタイル (*移動するタイルが) |
	if (duelRangeTileNos.Contains(firstDefender->currentTileNo)) return; // *既に対人レンジ内にいる
	if (ballHolderAroundTileNos.Contains(firstDefender->currentTileNo)) return; // *既にボールホルダー周囲にいる
	// *

	int nextTileNo = GetShortestNextTileNo(firstDefender->currentTileNo, ballHolder->currentTileNo); // 移動先のタイルNo

	// * 制限 *
	if (currentTileNos.Contains(nextTileNo) || moveToTileNos.Contains(nextTileNo)) return; // *既にプレイヤーがいるタイル or 移動予約済みタイル
	// *

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
// | マークレンジ内の相手の前のマスに向かって移動する |
void AC_My_Player_Controller::Marking(AC_Piece* defencePlayer, AC_Piece* offensePlayer)
{
	// 動くタイルとマーク位置を取得
	int _markedPlayersFront = offensePlayer->ActorHasTag(FName("HOME")) ? C_Common::FORWARD_DIRECTION : C_Common::BACKWARD_DIRECTION; // マークされるプレイヤーの前方
	int _markingLocationTileNo = offensePlayer->currentTileNo + _markedPlayersFront; // マーク位置のタイルNo (マークしていると判断する最終的な位置)
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

// キープ中、ボールホルダーへ味方が近づく
// | ボールホルダーより後ろの味方で最も近い一人のみ |
void AC_My_Player_Controller::ApproachBallHolderWhenBallKeeping()
{
	int _ballHolderRow = ballHolder->currentTileNo / C_Common::TILE_NUM_Y; // ボールホルダーRow
	TArray<int> _tileNosLessThanBallHolder = {}; // ボールホルダー以下のタイルNo (現在プレイヤーが存在する中で)
	
	// ** ⓵ボールホルダー以下のタイルNo取得 (現在プレイヤーが存在する中で) **
	for (int _tileNo : currentTileNos) {
		int _tileNoRow = _tileNo / C_Common::TILE_NUM_Y; // タイルRow
		bool _isLessThanBallHolder = isHomeBall ? _tileNoRow < _ballHolderRow : _tileNoRow > _ballHolderRow; // ボールホルダー以下のタイルか
		
		if (_isLessThanBallHolder) {
			_tileNosLessThanBallHolder.Add(_tileNo);
		}
	}
	// **

	// ** ⓶味方プレイヤーとボールホルダーとの距離取得 **
	TArray<FDistanceToBallHolder> _playerAndDistances = {}; // 構造体(味方プレイヤーとボールホルダーとの距離)配列
	TArray<AC_Piece*> _allyPlayers = isHomeBall ? allHomePieces : allAwayPieces; // 味方プレイヤー
	AC_Tile* _ballTile = allTiles[ball->currentTileNo - 1]; // ボールタイル
	// 構造体配列作成
	for (AC_Piece* _p : _allyPlayers) {
		AC_Tile* _pTile = (allTiles[_p->currentTileNo - 1]); // プレイヤータイル
		FDistanceToBallHolder _distanceAndPlayer = { _p, _ballTile->GetDistanceTo(_pTile)}; // 構造体
		
		_playerAndDistances.Add(_distanceAndPlayer); // 追加
	}
	// **

	// ** ⓶ボールホルダーと最も近く、⓵ボールホルダー以下のタイルに存在するプレイヤーを取得 **
	AC_Piece* _nearestPlayer = nullptr; // プレイヤー取得
	
	// ボールホルダーの距離でソート
	_playerAndDistances.Sort([](const FDistanceToBallHolder& A, const FDistanceToBallHolder& B) {
		return A.distance < B.distance; // 距離が小さい順に
		});

	// ボールホルダー以下のタイルに存在するプレイヤー取得
	for (FDistanceToBallHolder _pAndD : _playerAndDistances) {
		AC_Piece* _player = _pAndD.player; // プレイヤー

		if (_tileNosLessThanBallHolder.Contains(_player->currentTileNo)) {
			
			_nearestPlayer = _player; // 取得
			break;
		}
	}
	// **
	
	// ** 移動 **
	if (_nearestPlayer != nullptr) {
		// < プレイヤー取得済み >
		
		int _moveToTileNo = GetShortestNextTileNo(_nearestPlayer->currentTileNo, ballHolder->currentTileNo); // 移動するタイルNo

		// 移動先のタイルを予約
		if (moveToTileNos.Contains(_moveToTileNo)) { // タイルが予約されているか
			// <予約あり>
			if (C_Common::DEBUG_MODE) UKismetSystemLibrary::PrintString(this, "NOT GET NEAREST PLAYER DURING BALL KEEPING", true, true, FColor::Cyan, 2.f, TEXT("None"));

			return; // 移動しない
		}
		else {
			// <予約なし>

			moveToTileNos.Add(_moveToTileNo); // 予約する
		}

		_nearestPlayer->SetMoveTo(allTiles[_moveToTileNo - 1]->GetActorLocation());
	}
	else {
		// < プレイヤー取得未 >

		if(C_Common::DEBUG_MODE) UKismetSystemLibrary::PrintString(this, "NOT GET NEAREST PLAYER DURING BALL KEEPING", true, true, FColor::Cyan, 2.f, TEXT("None"));
	}
	// **


	isBallKeeping = false; // フラグOFF
}

// ディフェンスラインコントロール
//  →目標ディフェンスラインへ移動
void AC_My_Player_Controller::DefenseLineControl(AC_Piece* defensePlayer, int targetRow)
{
	// 最終目標タイル取得 (目標Rowとディフェンダー位置の交点)
	int _defensePlayerColumn = defensePlayer->currentTileNo % C_Common::TILE_NUM_Y; // ディフェンダーColumn
	int _finalTargetTileNo = (C_Common::TILE_NUM_Y * targetRow) + _defensePlayerColumn; // 最終目標タイル

	// 次に動くタイルNo取得
	int _moveToTileNo = GetShortestNextTileNo(defensePlayer->currentTileNo, _finalTargetTileNo);

	// 移動
	defensePlayer->SetMoveTo(allTiles[_moveToTileNo - 1]->GetActorLocation());
}

// セカンドボールに反応する
//  →セカンドボール位置へ移動する
// | *プレイステップフェーズ処理 |
void AC_My_Player_Controller::AproachToSecondBall(AC_Piece* secondBallPlayer)
{
	// ボール十字タイル
	int _plusX = ball->currentTileNo + (C_Common::TILE_NUM_Y * 1);
	int _minusX = ball->currentTileNo - (C_Common::TILE_NUM_Y * 1);
	int _plusY = ball->currentTileNo + 1;
	int _minusY = ball->currentTileNo - 1;

	// 十字タイルの中で最も距離が近いタイル取得
	TArray <int> _crossTiles = { _plusX, _minusX, _plusY, _minusY }; // 十字タイルNo配列
	TArray <int> _tileDistances = { // 十字タイルとプレイヤーの距離配列
		(int)allTiles[_plusX - 1]->GetDistanceTo(secondBallPlayer),
		(int)allTiles[_minusX - 1]->GetDistanceTo(secondBallPlayer),
		(int)allTiles[_plusY - 1]->GetDistanceTo(secondBallPlayer),
		(int)allTiles[_minusY - 1]->GetDistanceTo(secondBallPlayer)
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

	// 移動するタイルNo取得
	int _moveToTileNo = GetShortestNextTileNo(secondBallPlayer->currentTileNo, _shotestTileNo); // 最も近いタイルへの最短距離
	if (secondBallPlayer->currentTileNo == _moveToTileNo) return; // すでに目的地にいる場合は移動しない

	// 移動
	secondBallPlayer->SetMoveTo(allTiles[_moveToTileNo - 1]->GetActorLocation());
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

// ③-⑵プレイステップフェーズを監視するタイマー設定
void AC_My_Player_Controller::SetTimerMonitorPlayStepPhase()
{
	// ** タイマー開始 **
	FTimerHandle handle;
	GetWorldTimerManager().SetTimer(handle, this, &AC_My_Player_Controller::MonitorFinishPlayStepPhase, 0.5f, true); // 0.5秒おき
	// **
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

// 試合開始時処理
void AC_My_Player_Controller::StartOfMatch()
{
	// ** 全てのプレイヤーとボールのタイルNoを設定、配列を作成する **
	currentTileNos = SetTileNoToAllPlayersAndBall();
	// **
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

	// ** ゲーム開始時処理 **
	if (stepCount == 1) { // Stepの初回か
		// <初回時>

		// ** 試合開始時処理 **
		StartOfMatch();
		// **
	}
	else {
		// <初回時以外>

		// ** 全てのプレイヤーとボールのタイルNoを設定、配列を作成する **
		currentTileNos = SetTileNoToAllPlayersAndBall();
		// **
	}
	// **

	// ** プレイヤーにレーンを設定 **
	SetLaneForPlayers();
	// **

	// ** 現在のディフェンスラインRowを設定 **
	// --Home-
	allHomePieces.StableSort([](const AC_Piece& A, const AC_Piece& B) { // タイル順にソート
		return A.currentTileNo < B.currentTileNo; // 小さい
		});
	AC_Piece* _home_defenseLinePlayer = allHomePieces[1]; // ディフェンスラインのプレイヤー
	home_currentDefenseLineRow = (_home_defenseLinePlayer->currentTileNo - 1) / C_Common::TILE_NUM_Y; // ディフェンスラインRow

	// --Away-
	allAwayPieces.StableSort([](const AC_Piece& A, const AC_Piece& B) { // タイル順にソート
		return A.currentTileNo > B.currentTileNo; // 大きい
		});
	AC_Piece* _away_defenseLinePlayer = allAwayPieces[1]; // ディフェンスラインのプレイヤー
	away_currentDefenseLineRow = (_away_defenseLinePlayer->currentTileNo - 1) / C_Common::TILE_NUM_Y; // ディフェンスラインRow
	// **
	
	// ** ショートパス時 (前回のSTEPにて) **
	if (passTarget != nullptr) { // パスターゲットが設定されているか (前回のStepでパスが実行されているか)
		preBallHolder = ballHolder; // ←リターンパスを禁止にする
		ballHolder = passTarget; // ボールホルダー切り替え

		passTarget = nullptr; // 空にする
	}
	// **

	// ** 目標ディフェンスラインを設定 **
	int _ballRow = (ball->currentTileNo - 1) / C_Common::TILE_NUM_Y; // ボールRow
	// ボールがディフェンスラインの背後にあれば目標ラインを合わせる
	// --HOME--
	if (home_currentDefenseLineRow > _ballRow) {
		// <ボールがディフェンスラインの背後にある (HOME)>
		home_targetDefenseLineRow = _ballRow; // 目標をボール位置にする
	}
	else {
		// <ボールがディフェンスラインの前にある>
		home_targetDefenseLineRow = home_currentDefenseLineRow; // 目標を現在のディフェンスライン位置にする
	}
	// --AWAY--
	if (away_currentDefenseLineRow < _ballRow) {
		// <ボールがディフェンスラインの背後にある (AWAY)>
		away_targetDefenseLineRow = _ballRow; // 目標をボール位置にする
	}
	else {
		// <ボールがディフェンスラインの前にある>
		away_targetDefenseLineRow = away_currentDefenseLineRow; // 目標を現在のディフェンスライン位置にする
	}
	// **

	// ** ロングボール時 **
	if (isLongBalled) {
		if (longPassTarget != nullptr) {
			// <ターゲットがいる>
			
			// -- ロングパス
			preBallHolder = ballHolder; // ←リターンパスを禁止にする
			ballHolder = longPassTarget; // ボールホルダー切り替え
			
			longPassTarget = nullptr; // ターゲットを空へ
		}
		else {
			// <ターゲットがいない>
			
			// -- ロングアタック
			preBallHolder = ballHolder; // 前回のボールホルダーを取得
			ballHolder = nullptr; // ボールホルダーを空へ
		}

		isPreActionedLongBall = false; // ロングボール事前動作終了
		isLongBalled = false; // ロングボール終了
		isFloatingBall = true; // ボールが浮いている
	}
	else {
		isFloatingBall = false; // ボールが浮いていない
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

	// ** エアバトル時 (前回のSTEPにて) **
	if (isAirBattle) { // 前回のSTEPにてエアバトルが実行されているか
		// ** 前回のボールホルダーを取得 **
		preBallHolder = ballHolder;
		// **

		// ** ボールホルダーを空へ **
		ballHolder = nullptr;
		// **

		isAirBattle = false; // エアバトルOFF
	}
	// **


	// ** ボールホルダー取得 (*ホールホルダーがいない場合のみ) **
	// | ボール周囲(十字)にプレイヤーがいるか |
	if (ballHolder == nullptr) { // ボールホルダーがいない場合
	   // ●ボールの十字タイルNoを作成
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
			if (C_Common::TILE_RANGE_DISPLAY) { // タイル範囲表示モードON
				for (int i : passRangeTileNos) {
					AC_Tile* t = allTiles[i - 1];
					t->SetPassRangeMaterial();
				}
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
				if (C_Common::TILE_RANGE_DISPLAY) { // タイル範囲表示モードON
					for (int i : _range) {
						AC_Tile* t = allTiles[i - 1];
						t->SetMarkRangeMaterial();
					}
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

		// ** ボールホルダーの周囲マス取得 **
		// 十字マス
		int _plusX = ballHolder->currentTileNo + C_Common::TILE_NUM_Y;
		int _minusX = ballHolder->currentTileNo - C_Common::TILE_NUM_Y;
		int _plusY = ballHolder->currentTileNo + 1;
		int _minusY = ballHolder->currentTileNo - 1;
		// 斜めマス
		int _plusXplusY = _plusX + 1;
		int _plusXminusY = _plusX - 1;
		int _minusXplusY = _minusX + 1;
		int _minusXminusY = _minusX - 1;
		
		TArray <int> _aroundTileNos = { _plusX, _minusX, _plusY, _minusY, _plusXplusY, _plusXminusY, _minusXplusY, _minusXminusY }; // タイルの周囲No
		
		for (int n : _aroundTileNos) {
			// * 制限 *
			if (n < 1 || n > 1000) { // *タイルNoがタイル数内か

				continue;
			}
			if (allTiles[ballHolder->currentTileNo - 1]->GetDistanceTo(allTiles[n - 1]) > C_Common::AROUNT_TILE_RANGE) { // *周囲のタイルか (周囲タイルの距離が150.0f以上)

				continue;
			}

			ballHolderAroundTileNos.Add(n);
		}
		// **

		// ** セカンドボール範囲リセット **
		secondBallRangeTileNum = 0;
		// **
	}
	else {
		// <ボールホルダーなし>
		if (C_Common::DEBUG_MODE) UKismetSystemLibrary::PrintString(this, "BALL HOLDER NULL !!");

		// ** セカンドボール範囲設定 **
		// セカンドボール範囲 拡大
		secondBallRangeTileNum++;
		AC_Tile* _ballTile = allTiles[ball->currentTileNo - 1]; // ボールタイル
		// セカンドボール範囲取得処理
		int _outSideTileNum = secondBallRangeTileNum + 1; // セカンドボールに反応するタイル個数 + 1 (これより内側のタイルを周囲タイルとみなすため)
		float _aroundDistance = C_Common::NEXT_TO_TILE_DISTANCE * _outSideTileNum; // 周囲タイル範囲 (*この値以下だと周囲タイルとみなす)
		// *制限
		// | ⓵周囲タイル範囲以上 |
		for (AC_Tile* t : allTiles) {
			float _distanceFromBallTile = _ballTile->GetDistanceTo(t); // ボールタイルからの距離
			if (_aroundDistance < _distanceFromBallTile) continue; // ⓵周囲タイル範囲以上

			secondBallRangeTileNos.Add(t->tileNo); // セカンドボール範囲取得
		}
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
		// <ボールホルダー存在>
		
		if (isBallKeeping) { // ボールキープ中か (*前回のSTEP時)
			// < ボールキープ中 >

			// ** ボールホルダーへ味方が近づく **
			ApproachBallHolderWhenBallKeeping();
			// **
		}
		else {
			// < ボールキープ中でない >
			
			// ** ボールホルダーのプレイ選択 **
			SelectPlayForBallHolder();
			// **

			for (AC_Piece* dp : defencePlayers) {
				// ** ディフェンダーのプレイ選択 **
				SelectPlayForDefender(dp);
				// **
			}
		}
	}
	else {
		// <ボールホルダー不在>
		
		// ** セカンドボールプレイヤー取得 **
		TArray < AC_Piece*> _secondBallPlayers = {}; // セカンドボールに反応するプレイヤー
		for (AC_Piece* p : allPieces) {
			if (secondBallRangeTileNos.Contains(p->currentTileNo)) _secondBallPlayers.Add(p);
		}
		// **


		// ** ラインコントロール **
		//  →目標ディフェンスラインへ移動
		for (AC_Piece* _defensePlayer : defencePlayers) { // すべてのディフェンダー対象
			// *条件*
			// | 1.DFポジションのみ
			// | 2.目標ディフェンスラインが自身Rowと異なる
			// | 3.セカンドボールに反応していない
			if (DFPositionRange.Contains(_defensePlayer->position)) { // 1
				
				int _myTargetDefenseLineRow = isHomeBall ? away_targetDefenseLineRow : home_targetDefenseLineRow; // 目標のディフェンスライン
				int _myRow = (_defensePlayer->currentTileNo - 1) / C_Common::TILE_NUM_Y; // 自分のRow
				if (_myRow != _myTargetDefenseLineRow) { // 2
					
					if (_secondBallPlayers.Contains(_defensePlayer) == false) { // 3
						
						DefenseLineControl(_defensePlayer, _myTargetDefenseLineRow);
					}
				}
			}
		}
		// **

		// ** セカンドボール処理 **
		for (AC_Piece* _p : _secondBallPlayers) {
			AproachToSecondBall(_p);
		}
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

	// ** ボールホルダー周囲タイル削除 **
	ballHolderAroundTileNos.Empty(); // ボールホルダー周囲タイル
	ballCrossTileNos.Empty(); // 空にする
	// **

	// ** セカンドボール範囲削除 **
	secondBallRangeTileNos.Empty();
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

	// ** ⓵制限配列と⓶目的地タイルとの距離を作成 **
	TArray <int> _limitFromAroundTileNos = {}; // ⓵タイル周囲No (*制限あり)
	TArray <int> _toTileDistances = {}; // ⓶目的地タイルとの距離の配列
	for (int n : _fromAroundTileNos) {
		// _toTileDistancesと_limitFromAroundTileNosの要素を合わせる
		
		// * 制限 *
		if (n < 1 && n > 1000) continue; // *タイル数外か
		if (allTiles[fromTileNo - 1]->GetDistanceTo(allTiles[n - 1]) > C_Common::AROUNT_TILE_RANGE) continue; // *周囲タイルの距離が150.0f以上か
		if (currentTileNos.Contains(n) || moveToTileNos.Contains(n)) continue; // *現在プレイヤーがいる or 移動予約されているか
		// *

		// タイル周囲No取得
		_limitFromAroundTileNos.Add(n);

		// 目的タイルとの距離取得
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
		return _limitFromAroundTileNos[0]; // 最初のタイルNoを返す
	}
	int shotestToTileNo = _limitFromAroundTileNos[minDistanceIndex]; // タイルNo
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

// プレイヤーのレーンを設定
void AC_My_Player_Controller::SetLaneForPlayers()
{
	for (AC_Piece* _p : allPieces) {
		int _column = ( (_p->currentTileNo + C_Common::TILE_NUM_Y) % C_Common::TILE_NUM_Y ); // 縦


		if (1 <= _column && _column <= 5) {
			_p->currentLane = C_Common::LANE_FIRST;
			continue;
		}
		else if (_column <= 10) {
			_p->currentLane = C_Common::LANE_SECOND;
			continue;
		}
		else if (_column <= 15) {
			_p->currentLane = C_Common::LANE_THIRD;
			continue;
		}
		else if (_column <= 20) {
			_p->currentLane = C_Common::LANE_FOURTH;
			continue;
		}
		else if (_column <= 24 || _column == 0) { // ←右端対応
			_p->currentLane = C_Common::LANE_FIFTH;
			continue;
		}
		else {
			_p->currentLane = 0;

			if (C_Common::DEBUG_MODE) UKismetSystemLibrary::PrintString(this, "ERROR SET LANE FOR PLAYERS", true, true, FColor::Cyan, 2.f, TEXT("None"));
			continue;
		}
	}
}
