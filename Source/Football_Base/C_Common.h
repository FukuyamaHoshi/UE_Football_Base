// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * common class
 */
class FOOTBALL_BASE_API C_Common
{
public:
    C_Common();
    ~C_Common();

    // タイル情報
    static constexpr int TILE_NUM_Y = 5; // 横(y)のタイル個数
    static constexpr int TILE_SIZE = 330; // タイルのサイズ
	// 位置Z
    static constexpr double BALL_BASE_LOCATION_Z = 30.0; // ボールの標準の位置Z
    static constexpr double PLAYER_BASE_LOCATION_Z = 100.0; // プレイヤーの標準の位置Z
    // ポジション
    // DF
    static constexpr int GK_POSITION = 0; // GK
    static constexpr int LSB_POSITION = 1; // LSB
    static constexpr int LCB_POSITION = 2; // LCB
    static constexpr int CB_POSITION = 3; // CB
    static constexpr int RCB_POSITION = 4; // RCB
    static constexpr int RSB_POSITION = 5; // RSB
    // MF
    static constexpr int LH_POSITION = 6; // LH
    static constexpr int LIH_POSITION = 7; // LIH
    static constexpr int CH_POSITION = 8; // CH
    static constexpr int RIH_POSITION = 9; // RIH
    static constexpr int RH_POSITION = 10; // RH
    // FW
    static constexpr int LWG_POSITION = 11; // LWG
    static constexpr int LST_POSITION = 12; // LST
    static constexpr int CF_POSITION = 13; // CF
    static constexpr int RST_POSITION = 14; // RST
    static constexpr int RWG_POSITION = 15; // RWG
    // コマンド
    static constexpr int POSSETION_COMMAND_NO = 1; // ポゼッション
    static constexpr int LONG_ATTACK_COMMAND_NO = 2; // ロングアタック
    static constexpr int TECNICAL_ATTACK_COMMAND_NO = 3; // テクニカルアタック
    static constexpr int LOW_BLOCK_COMMAND_NO = 4; // ローブロック
    static constexpr int SIDE_PRESS_COMMAND_NO = 5; // サイドプレス
    static constexpr int HIGH_PRESS_COMMAND_NO = 6; // ハイプレス
    // プレイヤータイプNo
    static constexpr int RUNNER_TYPE_NO = 1; // ランナー
    static constexpr int TARGET_MAN_TYPE_NO = 2; // ターゲットマン
    static constexpr int POST_MAN_TYPE_NO = 3; // ポストマン
    static constexpr int POKET_MAN_TYPE_NO = 4; // ポケットマン
	// フェーズ
    static constexpr int MANAGER_SELECT_BEFORE_PHASE = 1; // マネージャー選択
    static constexpr int PLAYER_SELECT_PLACE_PHASE = 2; // プレイヤー選択・配置
    static constexpr int MATCH_PHASE = 3; // 試合中
    static constexpr int MATCH_READY_PHASE = 4; // 試合準備
    // インターバル
    static constexpr float MATCH_READY_TIME = 3.0f; // 試合準備
    static constexpr float MATCH_TIME = 30.0f; // 試合中
    static constexpr float PLAY_INTERVAL = 0.0f; // プレイ間隔

    // 位置からタイルＮｏ取得
    static int GetTileNoFromLocation(FVector location);
};
