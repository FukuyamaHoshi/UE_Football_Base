// Fill out your copyright notice in the Description page of Project Settings.


#include "C_Common.h"
#include <Kismet/KismetSystemLibrary.h>

C_Common::C_Common()
{
}

C_Common::~C_Common()
{
}

// 位置からタイルＮｏ取得
int C_Common::GetTileNoFromLocation(FVector location)
{
	int _tileNo = 0;

	// プレイヤー位置
	FVector _location = location;
	double _x = _location.X;
	double _y = _location.Y;
	// タイルNo
	int _tileNoY = 0; // タイルNo y (横から何個目か)
	int _tileNoX = 0; // タイルNo x (下から何列目か)

	// -- タイルNo y(横から何個目か)を求める --
	// -の時の処理
	bool _isPositiveY = true; // xが正の整数か
	if (_y < 0) {
		_y = abs(_y); // 絶対値に変換
		_isPositiveY = false; // フラグ変更
	}
	if (_y < (C_Common::TILE_SIZE / 2))	_tileNoY = 3;
	else if (_y < (C_Common::TILE_SIZE / 2) + C_Common::TILE_SIZE) _tileNoY = (_isPositiveY) ? 4 : 2;
	else if (_y < (C_Common::TILE_SIZE / 2) + C_Common::TILE_SIZE * 2) _tileNoY = (_isPositiveY) ? 5 : 1;
	else return _tileNo; // 横が2.5タイル以上 (タイルNo.0)

	// -- タイルNo x(下から何列目か)を求める --
	// -の時の処理
	bool _isPositiveX = true; // xが正の整数か
	if (_x < 0) {
		_x = abs(_x); // 絶対値に変換
		_isPositiveX = false; // フラグ変更
	}
	if (_x < C_Common::TILE_SIZE)	_tileNoX = (_isPositiveX) ? 4 : 3;
	else if (_x < C_Common::TILE_SIZE * 2) _tileNoX = (_isPositiveX) ? 5 : 2;
	else if (_x < C_Common::TILE_SIZE * 3)_tileNoX = (_isPositiveX) ? 6 : 1;
	else return _tileNo; // 縦が3タイル以上 (タイルNo.0)

	// -- タイルNo取得 --
	// ( (タイル縦(x) - 1個) * フィールドの横のタイル個数 ) + タイル横(y)
	_tileNo = ((_tileNoX - 1) * C_Common::TILE_NUM_Y) + _tileNoY;


	return _tileNo;
}
