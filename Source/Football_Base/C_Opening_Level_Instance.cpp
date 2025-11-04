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
		allPlayers.Add(_player); // 追加
	}
	// ***
}

// Called every frame
void AC_Opening_Level_Instance::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// ホバー時処理
	UMy_Game_Instance* _instance = Cast<UMy_Game_Instance>(UGameplayStatics::GetGameInstance(GetWorld())); // ゲームインスタンス
	if (_instance) {
		if (_instance->game_phase == C_Common::MANAGER_SELECT_PHASE) { // マネージャー選択フェーズのみ
			Hover();
		}
	}
}

// ********* input *********
// 入力設定
void AC_Opening_Level_Instance::SetupInput()
{
	// 入力を有効にする
	EnableInput(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	// 左クリックのプレスをバインド
	InputComponent->BindKey(EKeys::LeftMouseButton, IE_Pressed, this, &AC_Opening_Level_Instance::PressedLeft);
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
				FVector _size = FVector(0.4f, 0.4f, 0.4f); // Example size
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

			// *** 試合開始処理 ***
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

			// -- test test 
			//AC_Player* _target = nullptr;
			//for (AC_Player* _p : allPlayers) { // test
			//	if (_p->ActorHasTag(FName("RB"))) {
			//		_target = _p;

			//		break;
			//	}
			//}
			//if (ballHolder && _target) LongPass(ballHolder, _target);
			//ballHolder->BallKeeping();
			// ---------------

			// ***
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

// ショートパス
void AC_Opening_Level_Instance::ShortPass(AC_Player* fromPlayer, AC_Player* toPlayer)
{
	// アニメーション
	fromPlayer->ShotPass(toPlayer); // ショートパス
	toPlayer->Trap(fromPlayer); // トラップ

	// ボールホルダー切り替え
	SetBallHolder(toPlayer);
}

// ロングパス
void AC_Opening_Level_Instance::LongPass(AC_Player* fromPlayer, AC_Player* toPlayer)
{
	// アニメーション
	fromPlayer->LongPass(toPlayer); // ロングパス
	toPlayer->Trap(fromPlayer); // トラップ

	// ボールホルダー切り替え
	SetBallHolder(toPlayer);
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

