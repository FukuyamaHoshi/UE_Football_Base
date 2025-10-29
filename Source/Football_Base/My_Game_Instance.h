// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "My_Game_Instance.generated.h"

/**
 * 
 */
UCLASS()
class FOOTBALL_BASE_API UMy_Game_Instance : public UGameInstance
{
	GENERATED_BODY()

public:
	int match_round = 0; // *トーナメントのラウンド数
	// チームボックス情報structure
	struct FTeamBoxElem
	{
		int boxNum; // ボックスNo
		FString teamName; // チーム名前
		int state; // チームの状態(色を決定する): 0 = 対戦, 1 = 勝利, 2 = 敗戦, 3 = プレイヤー
		FString score; // 得点
	};
	TArray<FTeamBoxElem> team_box_elems = {}; // チームボックス情報(Route画面)
	TArray<int> players_tactics_command_nums = { 1, 2 }; // プレイヤーの戦術コマンドNos (***仮の初期コマンドセット)
	// プレイヤーステータス
	int player_side_break_abilty = 1; // サイド突破力
	int player_physical_abilty = 1; // 走力
	int player_press_resistance_abilty = 1; // プレス耐性

	// ゲームフェーズ (ゲーム中の場面) *マッチフェーズでない
	// 0: オープニング
	// 1: マネージャー選択
	int game_phase = 0;
};
