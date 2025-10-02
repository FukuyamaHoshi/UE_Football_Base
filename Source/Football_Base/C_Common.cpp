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
	case 1: // サイドアタック
		_orgumentTexts.Add("サイドアタック");
		_orgumentTexts.Add("戦術コマンド (保持)");
		_orgumentTexts.Add("サイドからゴールに向かう。");

		break;
	case 2: // ハイプレス
		_orgumentTexts.Add("ハイプレス");
		_orgumentTexts.Add("戦術コマンド (非保持)");
		_orgumentTexts.Add("前線から相手にプレッシャーをかける。");

		break;
	case 3: // ラインブレイク
		_orgumentTexts.Add("ラインブレイク");
		_orgumentTexts.Add("戦術コマンド (保持)");
		_orgumentTexts.Add("相手の裏のスペースを狙う。");

		break;
	case 4: // ローブロック
		_orgumentTexts.Add("ローブロック");
		_orgumentTexts.Add("戦術コマンド (非保持)");
		_orgumentTexts.Add("自陣のゴールまで引いてゴールを守る。");

		break;
	case 5: // GKビルドアップ
		_orgumentTexts.Add("GKビルドアップ");
		_orgumentTexts.Add("戦術コマンド (保持)");
		_orgumentTexts.Add("GK(ゴールキーパー)からボールをつなぎ、ゴールを狙う。");

		break;
	case 6: // サイド圧縮
		_orgumentTexts.Add("サイド圧縮");
		_orgumentTexts.Add("戦術コマンド (非保持)");
		_orgumentTexts.Add("サイドにボールがある場合、そのサイドへ人を集中させる。");

		break;
	default:
		_orgumentTexts.Add("error");
		_orgumentTexts.Add("error");
		_orgumentTexts.Add("error");

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
		_orgumentTexts.Add("サイド突破 1UP");
		_orgumentTexts.Add("ステータス");
		_orgumentTexts.Add("サイド突破ステータスを1UPさせます");

		break;
	case 2: // 体力1UP
		_orgumentTexts.Add("体力 1UP");
		_orgumentTexts.Add("ステータス");
		_orgumentTexts.Add("体力ステータスを1UPさせます");

		break;
	case 3: // プレス耐性1UP
		_orgumentTexts.Add("プレス耐性 1UP");
		_orgumentTexts.Add("ステータス");
		_orgumentTexts.Add("プレス耐性ステータスを1UPさせます");

		break;
	default:
		_orgumentTexts.Add("status error");
		_orgumentTexts.Add("status error");
		_orgumentTexts.Add("status error");

		break;
	}

	return _orgumentTexts;
}
