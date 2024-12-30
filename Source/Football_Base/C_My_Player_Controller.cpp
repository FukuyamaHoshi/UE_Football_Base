// Fill out your copyright notice in the Description page of Project Settings.


#include "C_My_Player_Controller.h"
#include "Kismet/KismetSystemLibrary.h"
#include <Kismet/GameplayStatics.h>
#include "C_Tile.h"
#include <Kismet/KismetStringLibrary.h>
#include "Components/DecalComponent.h"

// Called when the game starts or when spawned
void AC_My_Player_Controller::BeginPlay()
{
	Super::BeginPlay();

	// UKismetSystemLibrary::PrintString(this, "C++ hoge!", true, true, FColor::Cyan, 2.f, TEXT("None"));

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
		}
		allPieces.Add(piece); // 追加
	}
	// ***
}

// Called every frame
void AC_My_Player_Controller::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// ** フェーズ処理 **
	// フェーズが終了しているか
	if (isInPhase == false) {
		isInPhase = true; // フェーズ開始
		phaseCount++; // フェーズカウンター
		
		// ** フェーズ時の処理 ***
		InPhase();
		// **

		SetTimerMonitorPhase(); // フェーズ監視タイマーセット
	}
	// **

	// ** マウスホバー時 **
	// Grap中か
	if (isGrap) HoverMouse();
	// **
}

// 入力設定
void AC_My_Player_Controller::SetupInput()
{
	// 入力を有効にする
	EnableInput(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	// 左クリックのプレスのみバインド
	InputComponent->BindKey(EKeys::LeftMouseButton, IE_Pressed, this, &AC_My_Player_Controller::PressedLeft);
	// 左クリックのプレスのみバインド
	InputComponent->BindKey(EKeys::LeftMouseButton, IE_Released, this, &AC_My_Player_Controller::ReleasedLeft);
}

// 左クリック(プレス)イベント
void AC_My_Player_Controller::PressedLeft()
{
	AActor* p = SelectHomePiece(); // Homeのコマを選択し、取得

	// ** プレイヤー以外をクリックした時 (選択解除) **
	if (p == nullptr) {
		selectedPlayer = nullptr; // 変数を空にする
		selectedPlayerTileNo = 0; // タイルNoを空にする
		isGrap = false; // Grap解除

		// デカール削除
		if (currentDecal != nullptr) {
			currentDecal->DestroyComponent();
		}

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

	// ** プレイヤー選択中フラグ **
	isGrap = true; // プレイヤー選択中
	// **
}

// 左クリック(リリース)イベント
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
	overlayTile->RemoveMaterial();

	// オーバーレイタイルの変数をリセット
	overlayTileNo = 0; // タイルNo
	overlayTile = nullptr; // タイルの実態
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

// (***暫定) ボールホルダーのプレイ選択
// ( Homeプレイヤーのみ)
void AC_My_Player_Controller::SelectPlayForBallHolder()
{
	if (ballHolder == nullptr) return; // ボールホルダーがいない場合、処理しない

	// ** パスレンジを表示 **
	TArray <int> passRangeTileNos = GetTileNoInPassRange();
	// マテリアル表示
	for (int i : passRangeTileNos) {
		AC_Tile* t = allTiles[i - 1];
		t->SetPassRangeMaterial();
	}
	// **
	
	// ** パスレンジ内の味方取得 **
	AC_Piece* passTarget = nullptr; // パスターゲットのコマ
	for (AC_Piece* p : allHomePieces) {
		if (passRangeTileNos.Contains(p->currentTileNo)) {
			passTarget = p;
		}
	}
	// **

	if (passTarget != nullptr) { // パスターゲット取得できていれば
		// パスターゲットが存在
		// ** パス処理 **
		Pass(passTarget);
		// **
	}
	else {
		// パスターゲットが不在
		// ボールホルダーが前向きか
		int ballHolderDirection = GetDirectionOfBallHolder(); // ボールホルダーの方向
		if (ballHolderDirection == TILE_NUM_Y) {
			// 前向き
			// ** ドリブル処理 **
			Drrible(); // *** 前進のみ
			// **
		}
		else {
			// 前向きでない
			// ** 方向転換処理 **
			ChangeOfDirection();
		}
	}
}

// パス
void AC_My_Player_Controller::Pass(AC_Piece* targetPiece)
{
	// ** ターゲットの後ろのマスの位置取得 (***暫定) **
	int targetTileNo = targetPiece->currentTileNo; // ターゲットのタイルNo
	AC_Tile* targetBackTile = allTiles[(targetTileNo - 25) - 1]; // ターゲットの後ろのタイル
	FVector t = targetBackTile->GetActorLocation(); // 目標位置
	// **

	// ** ボールホルダーをターゲットへ切り替える **
	ballHolder = targetPiece;
	// **

	// ** ボールを移動させる **
	ball->SetMoveTo(t);
	// **
}

// ドリブル (前進のみ)
void AC_My_Player_Controller::Drrible()
{
	int ballHolderTileNo = ballHolder->currentTileNo; // ボールホルダータイルNo
	FVector pieceTargetLocation = allTiles[(ballHolderTileNo + TILE_NUM_Y) - 1]->GetActorLocation(); // コマのターゲット位置
	FVector ballTargetLocation = allTiles[(ballHolderTileNo + (TILE_NUM_Y * 2) ) - 1]->GetActorLocation(); // ボールのターゲット位置

	ballHolder->SetMoveTo(pieceTargetLocation); // 移動(コマ)
	ball->SetMoveTo(ballTargetLocation); // 移動(ボール)
}

// (***暫定)方向転換
// ( Homeプレイヤーのみ )
void AC_My_Player_Controller::ChangeOfDirection()
{
	int ballHolderTileNo = ballHolder->currentTileNo; // ボールホルダーのタイルNo
	int direction = GetDirectionOfBallHolder(); // ボールホルダーの向き
	FVector targetL; // ターゲット位置

	// 現在。後ろ向きの場合
	if (direction == -TILE_NUM_Y) {
		// 右タッチライン上か
		if (ballHolderTileNo % TILE_NUM_Y == 0) {
			// 右タッチライン上
			targetL = allTiles[(ballHolderTileNo - 1) - 1]->GetActorLocation(); // 左向き
		}
		else {
			// 右タッチラインでない
			targetL = allTiles[(ballHolderTileNo + 1) - 1]->GetActorLocation(); // 右向き
		}
	}
	// 現在、右向き or 左向き
	if (direction == 1 || direction == -1) targetL = allTiles[(ballHolderTileNo + TILE_NUM_Y) - 1]->GetActorLocation(); // 前向きになる

	ball->SetMoveTo(targetL); // 移動
}

// フェーズを終了していいか監視する
void AC_My_Player_Controller::MonitorFinishPhase()
{
	// ** コマとボールが移動していないか **
	for (AC_Piece* p : allPieces) {
		if (p->isMoving) return;
	}
	if (ball->isMoving) return;
	// **

	FinishTimerAndPhase(); // タイマーとフェーズを終了
}

// (フェーズ)タイマーとフェーズを終了する
void AC_My_Player_Controller::FinishTimerAndPhase()
{
	// タイマー終了
	GetWorldTimerManager().ClearAllTimersForObject(this);

	// フェーズ終了
	isInPhase = false;
}

// マウスホバー時処理
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
	if (overlayTile != nullptr) overlayTile->RemoveMaterial();
	overlayTile = hoverTile; // 光っているタイルを格納
	// ↑↑↑
	// マテリアルセット(タイルを光らせる)
	overlayTile->SetMaterial();
	overlayTileNo = currentHoverTileNo; // ホバー中のタイルと現在光っているタイルが同じにする(複数回マテリアルをセットしないようにする)
	// ***
}

// フェーズ時処理
void AC_My_Player_Controller::InPhase()
{
	// ** フェーズ数をデバッグする **
	FString s = "Phase: ";
	FString s_phaseCount = FString::FromInt(phaseCount);
	UKismetSystemLibrary::PrintString(this, s + s_phaseCount);
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
	// ( ***最初はHomeのGKがボールホルダー ***)
	if (phaseCount <= 1) { // フェーズ1以下か
		for (AC_Piece* p : allHomePieces) {
			if (p->ActorHasTag(FName("GK"))) {
				ballHolder = p;

				break;
			}
		}
	}
	// **

	// *****************************************************************
	// ↓↓↓ プレイヤー判断処理を書いていく ↓↓↓

	SelectPlayForBallHolder(); // ボールホルダーのプレイ選択
}

// ( ***暫定)パスレンジのタイルＮｏ取得
// ( Homeプレイヤーのみ )
// ( 体の向きは考慮しない )
TArray<int> AC_My_Player_Controller::GetTileNoInPassRange()
{
	TArray <int> passRangeNos; // パスレンジ列No配列
	int ballHolderTileNo = ballHolder->currentTileNo; // ボールホルダーのタイルNo

	int ballHolderRow = ballHolderTileNo / TILE_NUM_Y; // 現在のボールホルダーの行
	int startPassRangeRowTileNo = 0; // パスレンジ行の最初のタイル
	// 右端か判定
	if (ballHolderTileNo % TILE_NUM_Y == 0) {
		// 右端
		startPassRangeRowTileNo = (ballHolderRow * TILE_NUM_Y) + 1; // そのまま掛ける
	}
	else {
		// 右端以外
		startPassRangeRowTileNo = ((ballHolderRow + 1) * TILE_NUM_Y) + 1; // 現在のボールホルダーの行に+1してかける
	}
	int endPassRangeRowTileNo = (startPassRangeRowTileNo + (TILE_NUM_Y * 6)) - 1; // パスレンジ行の最後のタイル

	// ** 上端のタイル取得 **
	// 左端のタイルNo取得
	int leftEndTileNo;
	// タイルの5列目以下か
	if ( (ballHolderTileNo + TILE_NUM_Y) % TILE_NUM_Y < 5 && (ballHolderTileNo + TILE_NUM_Y) % TILE_NUM_Y > 0) {
		// 以下
		leftEndTileNo = startPassRangeRowTileNo + (TILE_NUM_Y * 5);
	}
	else {
		// 以上
		leftEndTileNo = (ballHolderTileNo - 4) + (TILE_NUM_Y * 6);
	}

	// 右端のタイルNo取得
	int rightEndTileNo;
	// タイルの21列目(タイル個数y - 4個)以上か
	if ((ballHolderTileNo + TILE_NUM_Y) % TILE_NUM_Y > (TILE_NUM_Y - 4) || ballHolderTileNo % TILE_NUM_Y == 0) {
		// 以上
		rightEndTileNo = endPassRangeRowTileNo;
	}
	else {
		// 以下
		rightEndTileNo = (ballHolderTileNo + 4) + (TILE_NUM_Y * 6);
	}
	// **

	// ** 4-6行目のパスレンジ (4 * 6マス) 取得 **
	int columnCount = 0; // 列カウンター
	int rowCount = 0; // 行カウンター
	// 行(x)の処理
	for (int n = 0; rowCount < 6; rowCount++) {
		columnCount = 0; // リセット
		
		// 列(y)の処理
		for (int i = 0; columnCount < (rightEndTileNo - (TILE_NUM_Y * rowCount)); i++) { // 左端-右端まで
			columnCount = leftEndTileNo - (TILE_NUM_Y * rowCount) + i; // 列をインクリメント
			passRangeNos.Add(columnCount);
		}

	}
	// **


	return passRangeNos;
}

// ( ***暫定) ボールホルダーの向きを取得 ( 前: 25, 後ろ: -25, 右: 1, 左: -1 )
// ( Homeプレイヤーのみ )
int AC_My_Player_Controller::GetDirectionOfBallHolder()
{
	int ballHolderTileNo = ballHolder->currentTileNo; // ボールホルダータイルNo
	int ballTileNo = ball->currentTileNo; // ボールのタイルNo

	if ((ballHolderTileNo + TILE_NUM_Y) == ballTileNo) return TILE_NUM_Y; // 前
	if ((ballHolderTileNo - TILE_NUM_Y) == ballTileNo) return -TILE_NUM_Y; // 後ろ
	if ((ballHolderTileNo + 1) == ballTileNo) return 1; // 右
	return -1; // 左
}

// フェーズ監視タイマー設定
void AC_My_Player_Controller::SetTimerMonitorPhase()
{
	// ** タイマー開始 **
	FTimerHandle handle;
	GetWorldTimerManager().SetTimer(handle, this, &AC_My_Player_Controller::MonitorFinishPhase, 0.5f, true); // 0.5秒おき
	// **
}
