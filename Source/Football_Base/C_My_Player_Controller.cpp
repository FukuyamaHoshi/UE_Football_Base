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
}

// Called every frame
void AC_My_Player_Controller::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// ** ①初期配置フェーズ処理 **
	// | 連続処理 (Tick) |
	if (isInitialPlacePhase) {
		// ** マウスホバー時 **
		// Grap中か
		if (isGrap) HoverMouse();
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
// | プレイヤーを選択　or 解除する |
void AC_My_Player_Controller::PressedLeft()
{
	// * 制限 *
	if (isInitialPlacePhase == false) return; // 初期配置フェーズのみ
	// *

	AActor* p = SelectHomePiece(); // Homeのコマを選択し、取得

	// ** 選択解除(プレイヤー以外をクリックした時) **
	if (p == nullptr) {
		selectedPlayer = nullptr; // 変数を空にする
		selectedPlayerTileNo = 0; // タイルNoを空にする
		isGrap = false; // Grap解除

		// デカール削除
		if (currentDecal != nullptr) {
			currentDecal->DestroyComponent();
		}

		// プレイヤー配置レンジ削除
		RemovePlayerPlaceRange();

		return; // 処理終了
	}
	// **

	// ** プレイヤー選択を一度のみにする **
	// (取得したプレイヤーと前にクリックされたプレイヤーが異なれば変数に保持する)
	if (p == selectedPlayer)
	{
		// * 前回と選択したプレイヤーが同じ
		return; // なにもしない
	}
	else{
		// * 前回と選択したプレイヤーが異なる
		if (currentDecal != nullptr) {
			currentDecal->DestroyComponent(); // デカール削除
		}
		
		selectedPlayer = p; // プレイヤーを変更
	}
	// **

	// ** 選択されたプレイヤーのタイルNo取得 **
	FVector playerLocation = selectedPlayer -> GetActorLocation(); // 選択されたプレイヤーの位置
	selectedPlayerTileNo = GetTileNoFromLocation(playerLocation.X, playerLocation.Y); // タイルＮｏセット
	// **

	// ** 選択されたプレイヤーのタイル位置にデカール表示 **
	FVector tileLocation = allTiles[selectedPlayerTileNo - 1]->GetActorLocation(); // タイル位置取得
	tileLocation = FVector(tileLocation.X, tileLocation.Y, 40); // 位置調整
	currentDecal = UGameplayStatics::SpawnDecalAtLocation(this, playerSelectedDecal, FVector(60, 60, 60), tileLocation, FRotator(-90, 0, 0)); // デカール表示, 格納
	// **

	// ** プレイヤー配置レンジを表示 **
	SetPlayerPlaceRange();
	// **
	

	// ** プレイヤー選択中フラグ **
	isGrap = true; // プレイヤー選択中
	// **
}

// 左クリック(リリース)イベント
// | 選択したプレイヤー移動 }
void AC_My_Player_Controller::ReleasedLeft()
{
	// Grap時のみ処理
	if (isGrap == false) return;
	isGrap = false; // フラグ切り替え

	// 光っているタイルがあるか
	if (overlayTileNo == 0) return;

	// *** プレイヤー(コマ)移動 ***
	FVector overlayTileLocation = overlayTile->GetActorLocation(); // 光っているタイルの位置
	overlayTileLocation.Z = 10; // zは固定
	selectedPlayer->SetActorLocation(overlayTileLocation); // 移動
	selectedPlayerTileNo = overlayTileNo; // タイル情報更新 (プレイヤーの)
	// ***

	// *** デカールの位置変更 ***
	currentDecal->DestroyComponent(); // 現在のデカール削除
	overlayTileLocation.Z = 40; // zだけ変更
	currentDecal = UGameplayStatics::SpawnDecalAtLocation(this, playerSelectedDecal, FVector(60, 60, 60), overlayTileLocation, FRotator(-90, 0, 0)); // デカール表示, 格納
	// ***

	// タイルのオーバーレイ削除
	overlayTile->RemoveMainMaterial();

	// オーバーレイタイルの変数をリセット
	overlayTileNo = 0; // タイルNo
	overlayTile = nullptr; // タイルの実態
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

// 自分のチームのコマを選択 (プレイヤー以外をクリックしたときはnullptrを返す)
AActor* AC_My_Player_Controller::SelectHomePiece() {
	// *** クリックしたPhisicsBodyの位置を取得 ***
	// 取得するオブジェクトタイプを設定(PhisicsBodyを設定)
	TArray<TEnumAsByte<EObjectTypeQuery>> objectTypes;
	objectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_PhysicsBody));
	FHitResult outHit; // 結果を格納

	// アクター取得
	bool b_Phisics = GetObjectFromMouseLocation(objectTypes, outHit);

	// PhisicsBodyか
	if (b_Phisics == false) return nullptr;
	AActor* selectedActor = outHit.GetActor(); // 選択されたActor取得

	//// HOMEタグか
	bool t = selectedActor -> ActorHasTag(FName(TEXT("HOME")));
	if (t == false) return nullptr;

	
	return selectedActor;
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

// マウス位置のオブジェクトを取得
	// ( return: bool(取得できたか), FHitResult&(取得したオブジェクト情報)<参照渡し> )
bool AC_My_Player_Controller::GetObjectFromMouseLocation(TArray<TEnumAsByte<EObjectTypeQuery>> objectTypes, FHitResult& outHit)
{
	// マウスの位置を取得
	FVector worldLocation;
	FVector worldDirection;
	APlayerController::DeprojectMousePositionToWorld(worldLocation, worldDirection); // マウス位置を３D座標で取得
	bool isGetObject = false; // objectを所得できたか

	// 方向を変換
	const int distance = 10000;
	worldDirection = (worldDirection * distance) + worldLocation;

	TArray<AActor*> ignoreActors; // 無視するアクター
	EDrawDebugTrace::Type drawDebugType = EDrawDebugTrace::Type::None; // デバッグモード

	// 位置取得
	isGetObject = UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(), worldLocation, worldDirection, objectTypes, false, ignoreActors, drawDebugType, outHit, true);
	
	
	return isGetObject;
}

// ボールホルダーのプレイ選択
void AC_My_Player_Controller::SelectPlayForBallHolder()
{
	if (ballHolder == nullptr) return; // ボールホルダーがいない場合、処理しない

	float f_ballHolderRow = float(ballHolder->currentTileNo) / float(C_Common::TILE_NUM_Y); // ボールホルダーが何列目にいるか (小数点含む) **右端タイル処理
	int ballSideForward = isHomeBall ? C_Common::TILE_NUM_Y : -C_Common::TILE_NUM_Y; // ボールホルダー側の前向き

	// ** 対人 **
	// | ボールホルダー側が発火 (処理負荷軽減のため) |
	if (duelRangeTileNos.IsEmpty() == false) { // 対人レンジが取得できているか
		for (AC_Piece* p : defencePlayers) { // ディフェンダーの中で
			if (duelRangeTileNos.Contains(p->currentTileNo)) { // 対人レンジ内にいれば
				Duel(p);

				return;
			}
		}
	}
	// **
	
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
	
	// ** パスターゲット取得 **
	// | パスレンジはBoforeフェーズで取得済み |
	// | リターンパスを禁止 |
	AC_Piece* passTarget = nullptr; // パスターゲットのコマ
	for (AC_Piece* p : offencePlayers) {
		// パスレンジ内のHomeコマ && 前回のボールホルダーでない ( *リターンパス禁止処理 )
		if (passRangeTileNos.Contains(p->currentTileNo) && p != preBallHolder) {
			passTarget = p;
		}
	}
	// **

	// ** パス or ドリブル or 方向転換 **
	// * 制限 *
	// |  ①パスターゲット取得可能 ②ターゲットがマークされていない |
	if (passTarget != nullptr && passTarget->isMarked == false) {
		// *
		// パスターゲットが存在
		
		// ** パス **
		Pass(passTarget);
		// **

	}
	else {
		// パスターゲットが不在
		int ballHolderDirection = GetDirectionOfBallHolder(); // ボールホルダーの方向
		if (ballHolderDirection == ballSideForward) { // 前向きか
			// 前向き
			
			// ** ドリブル **
			Drrible(); // *** 前進のみ
			// **

		}
		else {
			// 前向きでない
			
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
	if (ballHolder == nullptr) return; // ボールホルダー
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

// パス
void AC_My_Player_Controller::Pass(AC_Piece* targetPiece)
{	
	// ** ターゲットの後ろのマスの位置取得 (***暫定) **
	int targetTileNo = targetPiece->currentTileNo; // ターゲットのタイルNo
	int targetBackTileNo = isHomeBall ? targetTileNo - C_Common::TILE_NUM_Y: targetTileNo + C_Common::TILE_NUM_Y; // ターゲットの後ろのタイルNo
	FVector moveToLocation = allTiles[targetBackTileNo - 1]->GetActorLocation(); // ボールを動かす位置
	// **

	// ** 移動先のタイルを予約する **
	if (moveToTileNos.Contains(targetBackTileNo)) { // タイルが予約されているか
		// 予約あり

		return; // 移動しない
	}
	else {
		// 予約なし

		moveToTileNos.Add(targetBackTileNo); // 予約する
	}
	// **
	
	// ** 前回のボールホルダーを取得 **
	// | リターンパスを禁止にする |
	preBallHolder = ballHolder;
	// **
	
	// ** ボールホルダーをターゲットへ切り替える **
	ballHolder = targetPiece;
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
	FVector goalLocation; // ゴール位置
	if (ballHolder->ActorHasTag(FName("HOME"))) { // Homeプレイヤーか
		// Home
		goalLocation = FVector(2050, 0, -40);
	}
	else {
		// Away
		goalLocation = FVector(-2050, 0, -40);
	}

	// ** ボールホルダー削除 **
	ballHolder = nullptr;
	// **

	// ** ボールを移動させる **
	ball->SetMoveTo(goalLocation);
	// **
}

// 対人
// | dueledPlayer: 対人をされるプレイヤー (発火時,ディフェンダー) |
// | ディフェンダーを2マス進める |
void AC_My_Player_Controller::Duel(AC_Piece* dueledPlayer)
{
	// ** 前回のボールホルダーを取得 **
	preBallHolder = ballHolder;
	// **

	// ** ボールホルダーをディフェンダーへ切り替える **
	ballHolder = dueledPlayer;
	// **

	// ** 切り替え後のタイルNo取得 ** 
	int ballHolderTileNo = ballHolder->currentTileNo; // ボールホルダータイルNo
	int preBallHolderTileNo = preBallHolder->currentTileNo; // 前回のボールホルダーのタイルNo
	int ballTileNo = ball->currentTileNo; // ボールタイルNo
	// **

	// ** 前進する **
	// | ボール奪取側 (現在オフェンス) : 2マス前進 |
	// | ボール失った側 (現在ディフェンダー) : 1マス前進 |
	int forwordTileNo = C_Common::TILE_NUM_Y; // 前1マスのタイルNo
	int forwordTwoTileNo = C_Common::TILE_NUM_Y * 2; // 前2マスのタイルNo
	if (ballHolder->ActorHasTag(FName("HOME"))) { // Homeボールか
		// Home
		// * ボール失った側 (Away) *
		preBallHolder->SetMoveTo(allTiles[preBallHolderTileNo - forwordTileNo - 1]->GetActorLocation()); // 前回のボールホルダー

		// * ボール奪取側 (Home) *
		ballHolder->SetMoveTo(allTiles[ballHolderTileNo + forwordTwoTileNo - 1]->GetActorLocation()); // ボールホルダー
		ball->SetMoveTo(allTiles[ballTileNo + forwordTwoTileNo - 1]->GetActorLocation()); // ボール

	}
	else {
		// Away
		// * ボール失った側 (Home) *
		preBallHolder->SetMoveTo(allTiles[preBallHolderTileNo + forwordTileNo - 1]->GetActorLocation()); // 前回のボールホルダー

		// * ボール奪取側 (Away) *
		ballHolder->SetMoveTo(allTiles[ballHolderTileNo - forwordTwoTileNo - 1]->GetActorLocation()); // ボールホルダー
		ball->SetMoveTo(allTiles[ballTileNo - forwordTwoTileNo - 1]->GetActorLocation()); // ボール
	}
	// **
}

// プレス
// | ファーストディフェンダーがボールホルダーへ移動する |
void AC_My_Player_Controller::Press()
{
	// * 制限 * 
	if (duelRangeTileNos.IsEmpty() == false) { // 対人レンジが取得できているか
		if (duelRangeTileNos.Contains(firstDefender->currentTileNo)) return; // ファーストディフェンダーが対人レンジ内にいるか
	}
	// *

	int nextTileNo = GetShortestNextTileNo(firstDefender->currentTileNo, ballHolder->currentTileNo); // 移動先のタイルNo

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
// | マークレンジに相手がいるかチェックし、相手の前のマスに移動する |
void AC_My_Player_Controller::Marking(AC_Piece* defencePlayer)
{
	// ** オフェンス側の前向きを取得 **
	int forward = defencePlayer->ActorHasTag(FName("HOME")) ? C_Common::BACKWARD_DIRECTION : C_Common::FORWARD_DIRECTION;
	// **

	for (AC_Piece* p : offencePlayers) { // オフェンスプレイヤーごとに
		// * 制限 *
		if (p == ballHolder) continue; // ボールホルダー
		// *

		// * マークレンジに相手がいるかチェック * //
		if (defencePlayer->markRange.Contains(p->currentTileNo)) { // マークレンジ内に相手がいる
			int markLocationTileNo = p->currentTileNo + forward; // マーク位置のタイルNo (相手側の前方マス)
			// * 制限 *
			if (defencePlayer->currentTileNo == markLocationTileNo) return; // ディフェンダーがマーク位置に既にいる
			// *
			// * //

			// ** 移動先のタイルを予約する **
			if (moveToTileNos.Contains(markLocationTileNo)) { // タイルが予約されているか
				// 予約あり

				return; // 移動しない
			}
			else {
				// 予約なし

				moveToTileNos.Add(markLocationTileNo); // 予約する
			}
			// **
			
			// * 移動 *
			defencePlayer->SetMoveTo(allTiles[markLocationTileNo - 1]->GetActorLocation());
			// *

			// * フラグON *
			p->isMarked = true;
			// *

			return;
		}
	}
}

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

// マウスホバー時処理
// | 選択中のプレイヤー移動時にタイルを光らせる |
void AC_My_Player_Controller::HoverMouse()
{
	// *** ホバー中のタイル(WorldStatic)の位置を取得 ***
	// 取得するオブジェクトタイプを設定(WorldStaticを設定)
	TArray<TEnumAsByte<EObjectTypeQuery>> objectTypes;
	objectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic));
	FHitResult outHit; // 結果を格納

	bool isTile = GetObjectFromMouseLocation(objectTypes, outHit); // アクター情報取得
	// ***

	// タイルでなければ処理しない
	if (isTile == false) return;

	// *** 現在ホバー中のタイルNo ***
	currentHoverTileNo = GetTileNoFromLocation(outHit.GetActor()->GetActorLocation().X, outHit.GetActor()->GetActorLocation().Y); // 現在ホバーしているタイルNo
	// ***

	// *** 選択されたプレイヤーがいるタイルと、現在光っているタイルには移動できないようにする ***
	// ホバー中のタイルと選択中のプレイヤーがいるタイルが同じなら処理しない
	if (currentHoverTileNo == selectedPlayerTileNo) return;
	// ホバー中のタイルと現在光っているタイルが同じなら処理しない
	if (currentHoverTileNo == overlayTileNo) return;
	// ***


	// *** タイルを光らせる処理 ***
	AC_Tile* hoverTile = allTiles[currentHoverTileNo - 1]; // Hoverしているタイル取得
	// タイルの1つのみ光らせるようにする ↓↓↓
	// マテリアル削除
	if (overlayTile != nullptr) overlayTile->RemoveMainMaterial();
	overlayTile = hoverTile; // 光っているタイルを格納
	// ↑↑↑
	// マテリアルセット(タイルを光らせる)
	overlayTile->SetMaterial();
	overlayTileNo = currentHoverTileNo; // ホバー中のタイルと現在光っているタイルが同じにする(複数回マテリアルをセットしないようにする)
	// ***
}

// ③-⑴ 準備ステップフェーズ
// | フェーズカウント |
// | プレイヤー・ボールの初期値セット |
void AC_My_Player_Controller::PrepareStepPhase()
{
	// ** ステップ数カウント **
	stepCount++;
	// フェーズ数をデバッグ表示する
	if (C_Common::DEBUG_MODE) UKismetSystemLibrary::PrintString(this, "STEP: " + FString::FromInt(stepCount));
	// **

	// ** コマごとに現在のタイルNoを取得 **
	for (AC_Piece* p : allPieces) {
		FVector l = p->GetActorLocation(); // コマの位置
		int n = GetTileNoFromLocation(l.X, l.Y); // タイルNo
		p->currentTileNo = n; // タイルNoセット
	}
	// **

	// ** ボールの現在のタイルNoを取得 **
	FVector bL = ball->GetActorLocation(); // ボールの位置
	int bN = GetTileNoFromLocation(bL.X, bL.Y); // タイルNo
	ball->currentTileNo = bN; // タイルNoセット
	// **

	// ** ボールホルダー取得( ***初回のみの処理 ) **
	// | Homeプレイヤーを優先してボールホルダーへ |
	if (stepCount <= 1) { // ステップ1以下か
		if (allHomePieces.Num() > 0) { // Homeコマがあるか
			// Home
			for (AC_Piece* p : allHomePieces) {
				if (p->ActorHasTag(FName("GK"))) {
					ballHolder = p;

					break;
				}
			}
		}
		else {
			// Away
			for (AC_Piece* p : allAwayPieces) {
				if (p->ActorHasTag(FName("GK"))) {
					ballHolder = p;

					break;
				}
			}
		}
	}
	// **

	// ** パスレンジを取得・マテリアル表示 **
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

		// ** 対人レンジを取得 **
		duelRangeTileNos = GetTileNoInDuelRange(passRangeTileNos); // 対人レンジ取得
		// **

	}
	// **

	if (ballHolder != nullptr) { // null チェック (*現在シュート後,nullになる)
		// ボールホルダーあり
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
		// | ボールホルダー以外 |
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
	}
	else {
		// ボールホルダーなし
		if (C_Common::DEBUG_MODE) UKismetSystemLibrary::PrintString(this, "BALL HOLDER NULL !!");
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
	 
	// ** ボールホルダーのプレイ選択 **
	SelectPlayForBallHolder();
	// **

	// ** ディフェンダーのプレイ選択 **
	SelectPlayForDefender();
	// **

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

	// ** タイル予約削除 **
	moveToTileNos.Empty();
	// **

	return false;
}

// パスレンジのタイルＮｏ取得
TArray<int> AC_My_Player_Controller::GetTileNoInPassRange()
{
	TArray<int> passRangeNos; // パスレンジ (*最終)
	if (ballHolder == nullptr) return passRangeNos; // ボールホルダーnullチェック

	int startPassRangeRowTileNo = 0; // パスレンジ行の最初のタイル (*yが小さい)
	int endPassRangeRowTileNo = 0; // パスレンジ行の最後のタイル (*yが大きい)
	bool isForward = false; // 前向き判定
	bool isSide = false; // 横向きか

	int ballHolderTileNo = ballHolder->currentTileNo; // 現在のボールホルダーのタイルNo
	int ballHolderRow = ballHolderTileNo / C_Common::TILE_NUM_Y; // 現在のボールホルダーの行
	
	// ** パスレンジの行をすべて取得 **
	// | startPassRangeRowTileNo: xが最も小さく, 最も左端 |
	// | endPassRangeRowTileNo : xが最も大きく, 最も右端  |

	int direction = GetDirectionOfBallHolder(); // 体の向き
	if (direction == 25) {
		// * 前向き *
		isForward = true; // フラグON

		// 最も右端の場合
		if (ballHolderTileNo % C_Common::TILE_NUM_Y == 0) {
			// 最も右端
			startPassRangeRowTileNo = (ballHolderRow * C_Common::TILE_NUM_Y) + 1; // そのまま掛ける (*割り切れて,1行目はballHolderRow = 1になる)
		}
		else {
			// それ以外
			startPassRangeRowTileNo = ((ballHolderRow + 1) * C_Common::TILE_NUM_Y) + 1; // ボールホルダーの行に+1 (*小数点切り捨てのため, 1行目はballHolderRow = 0になる)
		}
		endPassRangeRowTileNo = (startPassRangeRowTileNo + (C_Common::TILE_NUM_Y * 6)) - 1; // パスレンジ最後のタイル
		// *
	}
	else if (direction == -25) {
		// * 後ろ向き *
		isForward = false; // フラグOFF

		// 最も右端の場合
		if (ballHolderTileNo % C_Common::TILE_NUM_Y == 0) {
			// 右端
			endPassRangeRowTileNo = (ballHolderRow * C_Common::TILE_NUM_Y) - C_Common::TILE_NUM_Y; // そのまま掛ける (*割り切れて,1行目はballHolderRow = 1になる)
		}
		else {
			// 右端以外
			endPassRangeRowTileNo = ((ballHolderRow + 1) * C_Common::TILE_NUM_Y) - C_Common::TILE_NUM_Y; // 現在のボールホルダーの行に+1してかける
		}
		startPassRangeRowTileNo = (endPassRangeRowTileNo - (C_Common::TILE_NUM_Y * 6) + 1); // パスレンジの最初のタイル
		// *
	}
	else{
		// * 右・左向き *
		isSide = true; // フラグON
		startPassRangeRowTileNo = ((ballHolderRow + 1) * C_Common::TILE_NUM_Y) - (C_Common::TILE_NUM_Y * 4); // 最初のタイル
		endPassRangeRowTileNo = ((ballHolderRow + 1) * C_Common::TILE_NUM_Y) + (C_Common::TILE_NUM_Y * 4); // 最後のタイル
		// *
	}
	// **

	// ** 上端(xが最も大きい)のタイル取得 **
	// * 左上端のタイルNo取得
	int leftEndTileNo;
	// 現在ボールホルダーが左側5マスにいるか
	if ( (ballHolderTileNo + C_Common::TILE_NUM_Y) % C_Common::TILE_NUM_Y < 5 && (ballHolderTileNo + C_Common::TILE_NUM_Y) % C_Common::TILE_NUM_Y > 0) { // *5以下とすると0（右端）も範囲に入っていまうため
		// * 左側5マス *
		// 横向きか
		if (isSide) {
			// 左向きか
			if (direction == -1) {
				// *左向き
				leftEndTileNo = (startPassRangeRowTileNo + (C_Common::TILE_NUM_Y * 8) - C_Common::TILE_NUM_Y) + 1;
			}
			else {
				// *右向き
				leftEndTileNo = (ballHolderTileNo + 1) + (C_Common::TILE_NUM_Y * 4);
			}
		}
		else {
			// *前向き　or 後ろ向き
			leftEndTileNo = startPassRangeRowTileNo + (C_Common::TILE_NUM_Y * 5);
		}
		// *
	}
	else {
		// * 5マス以外 *
		// 横向きか
		if (isSide) {
			// *右・左向き
			leftEndTileNo = direction == 1 ? (ballHolderTileNo + 1) + (C_Common::TILE_NUM_Y * 4) : (ballHolderTileNo - 6) + (C_Common::TILE_NUM_Y * 4);
		}
		else {
			// *前向き　or 後ろ向き
			leftEndTileNo = isForward ? (ballHolderTileNo - 4) + (C_Common::TILE_NUM_Y * 6) : (ballHolderTileNo - 4) - C_Common::TILE_NUM_Y;
		}
		// *
	}
	// *

	// 右上端のタイルNo取得
	int rightEndTileNo;
	// 現在ボールホルダーが右側5マスにいるか
	if ((ballHolderTileNo + C_Common::TILE_NUM_Y) % C_Common::TILE_NUM_Y > (C_Common::TILE_NUM_Y - 4) || ballHolderTileNo % C_Common::TILE_NUM_Y == 0) { // *21以上とすると0（右端）が範囲に入らないため
		// * 右側5マス *
		rightEndTileNo = direction == -1 ? (ballHolderTileNo - 1) + (C_Common::TILE_NUM_Y * 4) : endPassRangeRowTileNo;
		// *
	}
	else {
		// * 5マス以外 *
		// 横向きか
		if (isSide) {
			// 右 or 左向き
			rightEndTileNo = direction == 1 ? (ballHolderTileNo + 6) + (C_Common::TILE_NUM_Y * 4) : (ballHolderTileNo - 1) + (C_Common::TILE_NUM_Y * 4);
		}
		else {
			// 前向き　or 後ろ向き
			rightEndTileNo = isForward ? (ballHolderTileNo + 4) + (C_Common::TILE_NUM_Y * 6) : (ballHolderTileNo + 4) - C_Common::TILE_NUM_Y;
		}
		// *
	}
	// **

	// ** パスレンジ取得 **
	// | 前・後ろ向き: 4 * 6 マス |
	// | 右・左向き: 6 * 9 マス   |
	int columnCount = 0; // 列カウンター
	int rowCount = 0; // 行カウンター
	int rowRange = isSide ? 9 : 6; // 縦(行)のマス数
	TArray <int> passRangeOutOfFieldNos; // パスレンジ列No配列(フィールド外も含む)
	// 行(x)の処理
	// | rowRange: 縦幅(x, 行)                       |
	// | leftEndTileNo → rightEndTileNo: 横幅(y, 列) |
	// 縦(x)の処理
	for (int n = 0; rowCount < rowRange; rowCount++) {
		columnCount = 0; // リセット
		
		// 横(y)の処理
		for (int i = 0; columnCount < (rightEndTileNo - (C_Common::TILE_NUM_Y * rowCount)); i++) { // 左端 - 右端まで
			columnCount = leftEndTileNo - (C_Common::TILE_NUM_Y * rowCount) + i; // 列をインクリメント
			passRangeOutOfFieldNos.Add(columnCount);
		}

	}
	// **

	// ** パスレンジがタイル範囲を超えないようにする **
	for (int n : passRangeOutOfFieldNos) {
		if (n > allTiles.Num() || n < 0) continue; // 0 < tileNos < 1000 の範囲

		passRangeNos.Add(n);
	}
	// **


	return passRangeNos;
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
// | passRange: パス範囲から取得 |
TArray<int> AC_My_Player_Controller::GetTileNoInDuelRange(TArray<int> passRange)
{
	TArray<int> duelRange; // 対人範囲タイルNo
	// ** ボールの周囲マスを全て取得 **
	TArray<int> aroundBallTileNos; // ボール周囲タイルNo
	int ballTileNo = ball->currentTileNo; // ボールタイルNo
	// 同列
	aroundBallTileNos.Add(ballTileNo - 1);
	aroundBallTileNos.Add(ballTileNo + 1);
	// 下(x)の列
	aroundBallTileNos.Add((ballTileNo - C_Common::TILE_NUM_Y) - 1);
	aroundBallTileNos.Add(ballTileNo - C_Common::TILE_NUM_Y);
	aroundBallTileNos.Add((ballTileNo - C_Common::TILE_NUM_Y) + 1);
	// 上(x)の列
	aroundBallTileNos.Add((ballTileNo + C_Common::TILE_NUM_Y) - 1);
	aroundBallTileNos.Add(ballTileNo + C_Common::TILE_NUM_Y);
	aroundBallTileNos.Add((ballTileNo + C_Common::TILE_NUM_Y) + 1);
	// **

	// ** パスレンジと同じマスを対人レンジとする **
	for (int n : aroundBallTileNos) {
		if (passRange.Contains(n)) duelRange.Add(n);
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
		if (allTiles[fromTileNo - 1]->GetDistanceTo(allTiles[n - 1]) > 150.0f) { // *周囲のタイルか (fromtileと周囲タイルの距離が150.0f以内)
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

// プレイヤー配置レンジを表示
// | 表示するだけ(制限なし) |
void AC_My_Player_Controller::SetPlayerPlaceRange()
{
	int homePlaceRange[2] = { 126, 500 }; // Homeのプレイヤー配置レンジ(全部)
	int awayPlaceRange[2] = { 501, 875 }; // Awayのプレイヤー配置レンジ(全部)

	// *** 配置可能エリア表示 ***
	// *HOME*
	for (int i = homePlaceRange[0]; i <= homePlaceRange[1]; i++) {
		if (i <= 250) {
			// -DF-
			allTiles[i - 1]->SetDFPlayerPlaceRangeMaterial();
		}
		else if (i <= 375) {
			// -MF-
			allTiles[i - 1]->SetMFPlayerPlaceRangeMaterial();
		}
		else {
			// -FW-
			allTiles[i - 1]->SetFWPlayerPlaceRangeMaterial();
		}
	}
	// *AWAY*
	for (int i = awayPlaceRange[0]; i <= awayPlaceRange[1]; i++) {
		if (i <= 625) {
			// -FW-
			allTiles[i - 1]->SetFWPlayerPlaceRangeMaterial();
		}
		else if (i <= 750) {
			// -MF-
			allTiles[i - 1]->SetMFPlayerPlaceRangeMaterial();
		}
		else {
			// -DF-
			allTiles[i - 1]->SetDFPlayerPlaceRangeMaterial();
		}
	}
	// ***
}

// プレイヤー配置レンジを削除
void AC_My_Player_Controller::RemovePlayerPlaceRange()
{
	for (AC_Tile* t : allTiles) {
		t->RemoveSubMaterial();
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
