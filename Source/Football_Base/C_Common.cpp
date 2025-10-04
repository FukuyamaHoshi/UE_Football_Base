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
