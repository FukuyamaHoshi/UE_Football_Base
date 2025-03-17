// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class FOOTBALL_BASE_API C_Common
{
public:
    C_Common();
    ~C_Common();

    // ** Global変数 **
    static constexpr int TILE_NUM_Y = 25; // 横(y)のタイル個数
    static constexpr float NEXT_TO_TILE_DISTANCE = 100.0f; // 前後左右のタイル距離
    static constexpr float AROUNT_TILE_RANGE = 150.0f; // 周囲タイルの距離範囲 (前後左右: 100.0f,  斜め: 140.0 ~ 150.0f)
    static constexpr double BASE_LOCATION_Z = 10.0; // コマの標準の位置Z
    static constexpr float PIECE_SPEED = 4.0f; // コマの移動速度
    static constexpr bool DEBUG_MODE = true; // デバッグモード
    static constexpr bool POSITION_EREA_DISPLAY = false; // ポジションエリア表示モード
    static constexpr float INTERVAL_WITHIN_PHASE = 0.5f; // Before-inフェーズの間隔秒数　(*パスレンジを表示させる時間)
    static constexpr int FORWARD_DIRECTION = 25; // 前向き (*フィールド固定)
    static constexpr int BACKWARD_DIRECTION = -25; // 後ろ向き (*フィールド固定)
    static constexpr int HOME_PLACE_EREA[2] = { 126, 500 }; // Homeのプレイヤー配置エリア(全部)
    static constexpr int AWAY_PLACE_EREA[2] = { 501, 875 }; // Awayのプレイヤー配置エリア(全部)
    static constexpr int PASS_RANGE_TILE_NUM[2] = { 9, 6 }; // パスレンジタイル個数 (横 * 縦) *プレイヤーの向きに対して
    // -ポジション---
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
    // ----
    // **


    
    
    // ***** ゲーム内時間ごとの名称 *****************
    // ーーラウンド （試合数）ーー
    //  ①初期配置フェーズ：プレイヤーを手移動可能な時間
    //  ②2次配置フェーズ：対戦準備位置へ移動する
    //  ⓷対戦フェーズ
    //    ーステップー
    //    ⑴準備ステップフェーズ
    //    ⑵プレイステップフェーズ
    //    ⑶リセットステップフェーズ
    //    ー
    // ーー
    // ********************************************
};
