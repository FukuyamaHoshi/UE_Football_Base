// Fill out your copyright notice in the Description page of Project Settings.


#include "C_Common.h"
#include <Kismet/KismetSystemLibrary.h>

C_Common::C_Common()
{
}

C_Common::~C_Common()
{
}

// 戦術コマンドテキスト取得
TArray<FString> C_Common::GetTacticsCommandTexts(int tacticsCommandNo)
{
	TArray<FString> _orgumentTexts = {};

	switch (tacticsCommandNo)
	{
	case SIDE_ATTACK_COMMAND_NO: // サイドアタック
		_orgumentTexts.Add(TEXT("サイドアタック"));
		_orgumentTexts.Add(TEXT("戦術コマンド (保持)"));
		_orgumentTexts.Add(TEXT("サイドからゴールに向かう。"));

		break;
	case HIGH_LINE_COMMAND_NO: // ハイライン
		_orgumentTexts.Add(TEXT("ハイライン"));
		_orgumentTexts.Add(TEXT("戦術コマンド (非保持)"));
		_orgumentTexts.Add(TEXT("ディフェンスラインを高く保ち、ゴールを守る。"));

		break;
	case LINE_BREAK_COMMAND_NO: // ラインブレイク
		_orgumentTexts.Add(TEXT("ラインブレイク"));
		_orgumentTexts.Add(TEXT("戦術コマンド (保持)"));
		_orgumentTexts.Add(TEXT("相手の裏のスペースを狙う。"));

		break;
	case LOW_BLOCK_COMMAND_NO: // ローブロック
		_orgumentTexts.Add(TEXT("ローブロック"));
		_orgumentTexts.Add(TEXT("戦術コマンド (非保持)"));
		_orgumentTexts.Add(TEXT("自陣のゴールまで引いてゴールを守る。"));

		break;
	case GK_BUILD_UP_COMMAND_NO: // GKビルドアップ
		_orgumentTexts.Add(TEXT("GKビルドアップ"));
		_orgumentTexts.Add(TEXT("戦術コマンド (保持)"));
		_orgumentTexts.Add(TEXT("GK(ゴールキーパー)からボールをつなぎ、ゴールを狙う。"));

		break;
	case SIDE_COMPLESSION_COMMAND_NO: // サイド圧縮
		_orgumentTexts.Add(TEXT("サイド圧縮"));
		_orgumentTexts.Add(TEXT("戦術コマンド (非保持)"));
		_orgumentTexts.Add(TEXT("サイドにボールがある場合、そのサイドへ人を集中させる。"));

		break;
	default:
		_orgumentTexts.Add("tactics command error");
		_orgumentTexts.Add("tactics command error");
		_orgumentTexts.Add("tactics command error");

		break;
	}

	return _orgumentTexts;
}

// ステータスコマンドテキスト取得
TArray<FString> C_Common::GetStatusCommandTexts(int statusCommandNo)
{
	TArray<FString> _orgumentTexts = {};

	switch (statusCommandNo)
	{
	case 1: // サイド突破1UP
		_orgumentTexts.Add(TEXT("サイド突破 1UP"));
		_orgumentTexts.Add(TEXT("ステータス"));
		_orgumentTexts.Add(TEXT("サイド突破ステータスを1UPさせます"));

		break;
	case 2: // 体力1UP
		_orgumentTexts.Add(TEXT("走力 1UP"));
		_orgumentTexts.Add(TEXT("ステータス"));
		_orgumentTexts.Add(TEXT("走力ステータスを1UPさせます"));

		break;
	case 3: // プレス耐性1UP
		_orgumentTexts.Add(TEXT("プレス耐性 1UP"));
		_orgumentTexts.Add(TEXT("ステータス"));
		_orgumentTexts.Add(TEXT("プレス耐性ステータスを1UPさせます"));

		break;
	default:
		_orgumentTexts.Add("status error");
		_orgumentTexts.Add("status error");
		_orgumentTexts.Add("status error");

		break;
	}

	return _orgumentTexts;
}

// 戦術コマンド値取得
C_Common::FTacticsCommand C_Common::GetTacticsCommandValue(int tacticsCommandNo)
{
	FTacticsCommand _tacticsCommand; // コマンド変数

	switch (tacticsCommandNo)
	{
	case SIDE_ATTACK_COMMAND_NO: // サイドアタック
		_tacticsCommand.score = 30;
		_tacticsCommand.type = 1; // ( 1.サイド, 2,.ライン, 3.ブロック )
		_tacticsCommand.isOnBall = true;

		break;
	case HIGH_LINE_COMMAND_NO: // ハイライン
		_tacticsCommand.score = 15;
		_tacticsCommand.type = 2; // ( 1.サイド, 2,.ライン, 3.ブロック )
		_tacticsCommand.isOnBall = false;

		break;
	case LINE_BREAK_COMMAND_NO: // ラインブレイク
		_tacticsCommand.score = 30;
		_tacticsCommand.type = 2; // ( 1.サイド, 2,.ライン, 3.ブロック )
		_tacticsCommand.isOnBall = true;

		break;
	case LOW_BLOCK_COMMAND_NO: // ローブロック
		_tacticsCommand.score = 15;
		_tacticsCommand.type = 3; // ( 1.サイド, 2,.ライン, 3.ブロック )
		_tacticsCommand.isOnBall = false;

		break;
	case GK_BUILD_UP_COMMAND_NO: // GKビルドアップ
		_tacticsCommand.score = 30;
		_tacticsCommand.type = 3; // ( 1.サイド, 2,.ライン, 3.ブロック )
		_tacticsCommand.isOnBall = true;

		break;
	case SIDE_COMPLESSION_COMMAND_NO: // サイド圧縮
		_tacticsCommand.score = 30;
		_tacticsCommand.type = 1; // ( 1.サイド, 2,.ライン, 3.ブロック )
		_tacticsCommand.isOnBall = false;

		break;
	default:
		// errorは0にする
		_tacticsCommand.score = 0;
		_tacticsCommand.type = 0;
		_tacticsCommand.isOnBall = true;

		break;
	}

	return _tacticsCommand;
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
