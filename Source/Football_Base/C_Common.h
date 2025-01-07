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
    static constexpr double BASE_LOCATION_Z = 10.0; // コマの標準の位置Z
    static constexpr float PIECE_SPEED = 4.0f; // コマの移動速度
    // **

};
