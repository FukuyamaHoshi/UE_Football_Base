// Fill out your copyright notice in the Description page of Project Settings.


#include "C_Opening_Level_Instance.h"
#include <Kismet/KismetSystemLibrary.h>
#include <Kismet/GameplayStatics.h>
#include <Engine/DecalActor.h>
#include "My_Game_Instance.h"
#include "C_Common.h"
#include "C_Opening_UI.h"

// Sets default values
AC_Opening_Level_Instance::AC_Opening_Level_Instance()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AC_Opening_Level_Instance::BeginPlay()
{
	Super::BeginPlay();

	//UKismetSystemLibrary::PrintString(this, "level instance", true, true, FColor::Red, 10.0f, TEXT("None"));

	SetupInput(); // 入力設定

	// *** デカール取得 ***
	// (コンストラクタ以外のマテリアルはLoadObject使用)
	playerSelectedDecal = LoadObject<UMaterial>(NULL, TEXT("/Game/Materials/M_Decal_Selected_Player.M_Decal_Selected_Player"), NULL, LOAD_None, NULL);
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
}

// Called every frame
void AC_Opening_Level_Instance::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// *** ホバー時処理 ***
	UMy_Game_Instance* _instance = Cast<UMy_Game_Instance>(UGameplayStatics::GetGameInstance(GetWorld())); // ゲームインスタンス
	if (_instance == nullptr)  return;
	
	if (_instance->game_phase == C_Common::MANAGER_SELECT_PHASE) { // マネージャー選択フェーズのみ
		Hover();
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
		// シュートレンジ取得
		TArray<int> _shortRangeNo = {}; // シュートレンジ(タイルNo)
		if (ballHolder->ActorHasTag("HOME")) {
			_shortRangeNo.Append( { 27, 28, 29 } );
		}
		else {
			_shortRangeNo.Append( { 2, 3, 4 } );
		}
		
		// シュート
		if (_shortRangeNo.Contains(ballHolder->tileNo)) {
			Shoot();

			return;
		}
	}
	// ***

	// -------------- コマンド処理 ---------------------
	// *** プレス回避行動 ***
	if (command == C_Common::ESCAPE_PRESSING_COMMAND_NO) {
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
	if (command == C_Common::LONG_ATTACK_COMMAND_NO) {
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
		if (_instance->game_phase == C_Common::MANAGER_SELECT_PHASE) { // *(制限)マネージャー選択フェーズのみ
			
			// マウス位置情報取得処理
			TArray<TEnumAsByte<EObjectTypeQuery>> _objectTypes = {}; // 取得するオブジェクトタイプ
			_objectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_PhysicsBody)); // プレイヤー: PhysicsBody
			FHitResult hitResult; // オブジェクト情報
			bool _isPhysicsBodyExist = GetResultFromMouseLocation(hitResult, _objectTypes);
			
			if (_isPhysicsBodyExist == false) return; // PhisicsBody(マネージャーメッシュ)以外


			// ●デカール表示
			if (playerSelectedDecal) // マテリアルが取得できている
			{
				FVector _location = hitResult.GetActor()->GetActorLocation();
				_location.Z = 0; // 高さ調整
				FRotator _rotation = GetActorRotation();
				FVector _size = FVector(0.4f, 0.4f, 0.4f);
				ADecalActor* _decalActor = GetWorld()->SpawnActor<ADecalActor>(
					_location,
					_rotation
				);
				// マテリアル表示
				if (_decalActor)
				{
					_decalActor->SetDecalMaterial(playerSelectedDecal); // マテリアルセット
					_decalActor->SetActorScale3D(_size); // サイズセット
				}
			}

			// ●試合開始処理
			MatchStart();
		}
	}
}

// Wキー(プレス)イベント
void AC_Opening_Level_Instance::PressedW()
{
	UKismetSystemLibrary::PrintString(this, "W", true, true, FColor::Blue, 2.0f, TEXT("None"));

	// *** ロングアタックコマンド ***
	UMy_Game_Instance* _instance = Cast<UMy_Game_Instance>(UGameplayStatics::GetGameInstance(GetWorld())); // ゲームインスタンス
	if (_instance == nullptr)  return;

	if (_instance->game_phase == C_Common::MATCH_PHASE) { // *(制限)試合フェーズのみ
		// コマンド切り替え
		if (ballHolder) {
			command = C_Common::LONG_ATTACK_COMMAND_NO;
		}
		else {
			command = 0;
		}
	}
}

// Aキー(プレス)イベント
void AC_Opening_Level_Instance::PressedA()
{
	UKismetSystemLibrary::PrintString(this, "A", true, true, FColor::Blue, 2.0f, TEXT("None"));

	// *** プレス回避コマンド ***
	UMy_Game_Instance* _instance = Cast<UMy_Game_Instance>(UGameplayStatics::GetGameInstance(GetWorld())); // ゲームインスタンス
	if (_instance == nullptr)  return;

	if (_instance->game_phase == C_Common::MATCH_PHASE) { // *(制限)試合フェーズのみ
		// コマンド切り替え
		if (ballHolder) {
			command = C_Common::ESCAPE_PRESSING_COMMAND_NO;
		}
		else {
			command = 0;
		}
	}
}

// Sキー(プレス)イベント
void AC_Opening_Level_Instance::PressedS()
{
	UKismetSystemLibrary::PrintString(this, "S", true, true, FColor::Blue, 2.0f, TEXT("None"));

	// *** アイドルコマンド ***
	UMy_Game_Instance* _instance = Cast<UMy_Game_Instance>(UGameplayStatics::GetGameInstance(GetWorld())); // ゲームインスタンス
	if (_instance == nullptr)  return;

	if (_instance->game_phase == C_Common::MATCH_PHASE) { // *(制限)試合フェーズのみ
		// コマンド切り替え
		command = 0;
	}
}

// Dキー(プレス)イベント
void AC_Opening_Level_Instance::PressedD()
{
	UKismetSystemLibrary::PrintString(this, "D", true, true, FColor::Blue, 2.0f, TEXT("None"));

	// *** レーンアタックコマンド ***
	UMy_Game_Instance* _instance = Cast<UMy_Game_Instance>(UGameplayStatics::GetGameInstance(GetWorld())); // ゲームインスタンス
	if (_instance == nullptr)  return;

	if (_instance->game_phase == C_Common::MATCH_PHASE) { // *(制限)試合フェーズのみ
		// コマンド切り替え
		if (ballHolder) {
			command = C_Common::LANE_ATTACK_COMMAND_NO;
		}
		else {
			command = 0;
		}
	}
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
	if (_instance) {
		// -- フェーズ切り替え --
		_instance->game_phase = C_Common::MATCH_PHASE;
		// --

		// -- プレイヤーを表示 --
		for (AC_Player* _p : allPlayers) {
			_p->DisplayMesh();
		}
		// --

		// -- ボールホルダー決定 --
		for (AC_Player* _p : allPlayers) {
			if (_p->ActorHasTag(FName("GK"))) { // GKをボールホルダーへ
				SetBallHolder(_p);

				break;
			}
		}
		// --
	}

	//LongAttack(); // test
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
	}
}

// 裏抜け (走る)
void AC_Opening_Level_Instance::GetBehind(AC_Player* runPlayer, FVector toLocation)
{
	runPlayer->MoveTo(toLocation);
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