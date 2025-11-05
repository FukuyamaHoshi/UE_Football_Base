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
    static constexpr float AROUNT_TILE_RANGE = 150.0f; // 周囲タイルの距離範囲 (前後左右: 100.0f,  斜め: 140.0 ~ 150.0f) *複数になると余計なタイルも取得してしまう
    static constexpr double BASE_LOCATION_Z = 10.0; // コマの標準の位置Z
    static constexpr float PIECE_SPEED = 4.0f; // コマの移動速度
    static constexpr bool DEBUG_MODE = true; // デバッグモード
    static constexpr bool TILE_RANGE_DISPLAY = true; // タイルレンジ表示モード
    static constexpr float INTERVAL_WITHIN_PHASE = 0.5f; // Before-inフェーズの間隔秒数　(*パスレンジを表示させる時間)
    static constexpr int FORWARD_DIRECTION = 25; // 前向き (*フィールド固定)
    static constexpr int BACKWARD_DIRECTION = -25; // 後ろ向き (*フィールド固定)
    static constexpr int HOME_PLACE_EREA[2] = { 126, 500 }; // Homeのプレイヤー配置エリア(全部)
    static constexpr int AWAY_PLACE_EREA[2] = { 501, 875 }; // Awayのプレイヤー配置エリア(全部)
    static constexpr int PASS_RANGE_TILE_NUM[2] = { 9, 6 }; // パスレンジタイル個数 (横 * 縦) *プレイヤーの向きに対して
    static constexpr int SECOND_BALL_COLLECT_RANGE_NUM[2] = { 4, 2 }; // セカンドボール回収レンジタイル個数 ( Column(横) * Row(縦) )
    static constexpr int FINISH_ZORN_ROW[2] = { 34, 5 }; // 最終局面Row (HOME, AWAY)
    static constexpr int CROSS_ZORN_COLUMN[2] = { 5, 21 }; // クロスColumn (左(以下), 右(以上)) 
    static constexpr int HANDLE_CROSS_RANGE_ROW[2] = { 31, 8 }; // クロス対応Row (HOME, AWAY)
    static constexpr int PRESS_RANGE_TILE_NUM[2] = { 2, 3 }; // プレスレンジタイル個数 (Column(横) * Row(縦)) *プレイヤーの向きに対して
    static constexpr double BALL_BASE_LOCATION_Z = 30.0; // ボールの標準の位置Z
    static constexpr double PLAYER_BASE_LOCATION_Z = 100.0; // プレイヤーの標準の位置Z
    // マッチフェーズ
    static constexpr int DEFAULT_MATCH_PHASE = 0; // デフォルト
    static constexpr int SECOND_BALL_COLLECT_MATCH_PHASE = 1; // セカンドボール回収
    static constexpr int LINE_BREAK_MATCH_PHASE = 2; // 裏抜け
    static constexpr int CROSS_MATCH_PHASE = 3; // クロス
    static constexpr int FINISH_MATCH_PHASE = 4; // フィニッシュ (シュート)
    // リレーショナルプレーパターン
    static constexpr int NO_RELATIONAL_PLAY_PATTERN = 0; // なし
    static constexpr int DRIFT_WIDE_RELATIONAL_PLAY_PATTERN = 1; // ⓵ドリフトワイド

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

    // -レーン--
    // | Homeの左から1-5レーン |
    static constexpr int LANE_FIRST = 1; 
    static constexpr int LANE_SECOND = 2;
    static constexpr int LANE_THIRD = 3;
    static constexpr int LANE_FOURTH = 4;
    static constexpr int LANE_FIFTH = 5;
    // ---

    // -- オーグメント --
    static constexpr int TACTICS_COMMANDS_NUMS = 6; // 戦術コマンド総数
    static constexpr int STATUS_COMMANDS_NUMS = 3; // ステータスコマンド総数
    
    // -- 戦術コマンドNo --
    static constexpr int SIDE_ATTACK_COMMAND_NO = 1; // サイドアタック
    static constexpr int HIGH_LINE_COMMAND_NO = 2; // ハイライン
    static constexpr int LINE_BREAK_COMMAND_NO = 3; // ラインブレイク
    static constexpr int LOW_BLOCK_COMMAND_NO = 4; // ローブロック
    static constexpr int GK_BUILD_UP_COMMAND_NO = 5; // GKビルドアップ
    static constexpr int SIDE_COMPLESSION_COMMAND_NO = 6; // サイド圧縮

    // 戦術コマンドテキスト取得
    static TArray<FString> GetTacticsCommandTexts(int tacticsCommandNo);
    // ステータスコマンドテキスト取得
    static TArray<FString> GetStatusCommandTexts(int statusCommandNo);
    // 戦術コマンドstructure
    struct FTacticsCommand
    {
        int score; // スコア
        int type; // タイプ( 1.サイド, 2,.ライン, 3.ブロック )
        bool isOnBall; //  保持・非保持
    };
    // 戦術コマンド値取得
    static FTacticsCommand GetTacticsCommandValue(int tacticsCommandNo);
    // **

    // ゲームフェーズ (ゲーム中の場面) *マッチフェーズでない
    // ---------------------
    // 0: オープニング
    // 1: マネージャー選択
    // ---------------------
    static constexpr int MANAGER_SELECT_PHASE = 1; // マネージャー選択
    static constexpr int MATCH_PHASE = 2; // 試合中
    
    
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
