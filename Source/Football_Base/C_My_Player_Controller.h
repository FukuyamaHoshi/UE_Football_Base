// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "C_Opening_UI.h"
#include "C_Player.h"
#include <Engine/DecalActor.h>
#include "C_Manager.h"
#include "C_Common.h"
#include "C_My_Player_Controller.generated.h"

/**
 * input and UI change controller
 */
UCLASS()
class FOOTBALL_BASE_API AC_My_Player_Controller : public APlayerController
{
	GENERATED_BODY()

public:
	// Sets default values for this controller's properties
	AC_My_Player_Controller();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	// ********* オープニングUI関連 *********
	TSubclassOf<UUserWidget> openingUISubClass = nullptr; // オープニングUI (サブクラス)
	UC_Opening_UI* openingUI = nullptr; // オープニングUI (インスタンス)
	// ********* プレイヤーSpawn時使用 *********
	TSubclassOf<AC_Player> playerSubClass_1 = nullptr;
	TSubclassOf<AC_Player> playerSubClass_2 = nullptr;
	TSubclassOf<AC_Player> playerSubClass_3 = nullptr;
	TSubclassOf<AC_Player> playerSubClass_4 = nullptr;
	TSubclassOf<AC_Player> playerSubClass_5 = nullptr;
	TArray <AC_Player*> subPlayers = {}; // サブプレイヤー
	TArray<FVector> SUB_LOCATION =
	{
		FVector(-650, 1000, C_Common::PLAYER_BASE_LOCATION_Z),
		FVector(-800, 1000, C_Common::PLAYER_BASE_LOCATION_Z),
		FVector(-950, 1000, C_Common::PLAYER_BASE_LOCATION_Z) }; // サブプレイヤー位置
	// ********* マネージャー選択フェーズ用 *********
	UMaterial* managerSelectedDecalMaterial = nullptr; // マネージャー選択デカール (マテリアル)
	ADecalActor* managerSelectedDecalActor = nullptr; // マネージャー選択デカール (アクター)
	AC_Manager* currentHoverManager = nullptr; // 現在ホバー中のマネージャー
	AC_Manager* homeManager = nullptr; // マネージャー(HOME)
	// ********* プレイヤー選択フェーズ用 *********
	AC_Player* selectedPlayer = nullptr; // 選択中のプレイヤー
	bool isPlayerGrap = false; // 選択中か
	AC_Tile* selectedTile = nullptr; // 選択中のタイル
	TArray<AC_Player*> allPlayers = {}; // 全てのプレイヤー
	TArray<AC_Player*> homePlayers = {}; // HOMEプレイヤー
	TArray<AC_Player*> awayPlayers = {}; // AWAYプレイヤー
	TArray <AC_Tile*> tiles = {}; // 全てのタイル

private:
	// ********* input *********
	// 入力設定
	void SetupInput();
	// 左クリック(プレス)イベント
	// | マネージャー選択 |
	void PressedLeft();
	// 左クリック(リリース)イベント
	void ReleasedLeft();
	// 左クリック (プレイヤー選択)
	void PressedLeftForPlayerSelect();
	// プレイヤーをマウスに追従させる (プレイヤー選択)
	void FollowPlayerToMouse();
	// Wキー(プレス)イベント
	void PressedW();
	// Aキー(プレス)イベント
	void PressedA();
	// Sキー(プレス)イベント
	void PressedS();
	// Dキー(プレス)イベント
	void PressedD();
	// ホバー処理
	void Hover();
	// マウス位置に目的のオブジェクトがあるか判定して情報取得
	// | retrun: 目的のオブジェクトか判定, hitRusult: 取得するオブジェクト情報, objectTypes: 目的のオブジェクトの種類(コリジョン) |
	bool GetResultFromMouseLocation(FHitResult& hitResult, TArray<TEnumAsByte<EObjectTypeQuery>> objectTypes);

	// プレイヤースポーン (プールから選ばれた)
	void SpawnPlayerInPool(int selectedNo);
};
