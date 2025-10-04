// Fill out your copyright notice in the Description page of Project Settings.


#include "C_Orgument_UI.h"
#include <Kismet/KismetSystemLibrary.h>
#include "C_Common.h"
#include "My_Game_Instance.h"
#include <Kismet/GameplayStatics.h>

void UC_Orgument_UI::NativeConstruct()
{
	// *** クリックイベントをバインド ***
	// オーグメント選択ボタン
	if (Orgument_Button_1)
	{
		Orgument_Button_1->OnClicked.AddUniqueDynamic(this, &UC_Orgument_UI::OrgumentButton1Clicked);
	}
	if (Orgument_Button_2)
	{
		Orgument_Button_2->OnClicked.AddUniqueDynamic(this, &UC_Orgument_UI::OrgumentButton2Clicked);
	}
	if (Orgument_Button_3)
	{
		Orgument_Button_3->OnClicked.AddUniqueDynamic(this, &UC_Orgument_UI::OrgumentButton3Clicked);
	}
	
	// オーグメント名前配列作成
	orgumentNameTextBlanks = { Orgument_Name_1, Orgument_Name_2, Orgument_Name_3 };
	// オーグメント種類配列作成
	orgumentTypeTextBlanks = { Orgument_Type_1, Orgument_Type_2, Orgument_Type_3 };
	// オーグメント説明配列作成
	orgumentNoteMultiTextBlanks = { Orgument_Note_1, Orgument_Note_2, Orgument_Note_3 };

	
	// *** 表示するオーグメントNoを3つ決定 ***
	// -- 戦術コマンド --
	while (displayOrgumentNums.Num() < 2)
	{
		int _tacticsNo = FMath::RandRange(1, C_Common::TACTICS_COMMANDS_NUMS); // ランダム
		if (displayOrgumentNums.Contains(_tacticsNo)) continue; // 被っていたらスキップ

		displayOrgumentNums.Add(_tacticsNo);
	}
	// -- ステータス --
	int _statusNo = FMath::RandRange(1, C_Common::STATUS_COMMANDS_NUMS); // ランダム
	displayOrgumentNums.Add(_statusNo);
	// ***
	
	// *** オーグメントテキストUI表示・変数セット ***
	// -- 戦術コマンド --
	for (int _i = 0; _i < 2; _i++)
	{
		// ●オーグメントテキスト取得
		TArray<FString> _tacticsTexts = C_Common::GetTacticsCommandTexts(displayOrgumentNums[_i]);
		// ●UI表示
		orgumentNameTextBlanks[_i]->SetText(FText::FromString(_tacticsTexts[0])); // 名前
		orgumentTypeTextBlanks[_i]->SetText(FText::FromString(_tacticsTexts[1])); // 種類
		orgumentNoteMultiTextBlanks[_i]->SetText(FText::FromString(_tacticsTexts[2])); // 説明
	}
	// -- ステータス --
	// ●オーグメントテキスト取得
	TArray<FString> _statusTexts = C_Common::GetStatusCommandTexts(displayOrgumentNums[2]);
	// -- UI表示 --
	orgumentNameTextBlanks[2]->SetText(FText::FromString(_statusTexts[0])); // 名前
	orgumentTypeTextBlanks[2]->SetText(FText::FromString(_statusTexts[1])); // 種類
	orgumentNoteMultiTextBlanks[2]->SetText(FText::FromString(_statusTexts[2])); // 説明
	// --
	// ***
}

// オーグメント選択ボタンクリック (1)
void UC_Orgument_UI::OrgumentButton1Clicked()
{
	UKismetSystemLibrary::PrintString(this, "ORGUMENT 1: " + FString::FromInt(displayOrgumentNums[0]), true, true, FColor::Cyan, 10.0f, TEXT("None"));

	// *** 戦術コマンドNoを保存 ***
	UMy_Game_Instance* instance = Cast<UMy_Game_Instance>(UGameplayStatics::GetGameInstance(GetWorld())); // ゲームインスタンス
	if (instance == nullptr) return; // nullチェック

	// *戦術コマンドは4つに制限
	if (instance->players_tactics_command_nums.Num() < 4) instance->players_tactics_command_nums.Add(displayOrgumentNums[0]);
	// ***

	// *** ゲームフィールドへ移動 **
	UGameplayStatics::OpenLevel(GetWorld(), TEXT("Game_Field_Map")); // ゲームフィールドMAPへ
	// ***
}

// オーグメント選択ボタンクリック (2)
void UC_Orgument_UI::OrgumentButton2Clicked()
{
	UKismetSystemLibrary::PrintString(this, "ORGUMENT 2: " + FString::FromInt(displayOrgumentNums[1]), true, true, FColor::Cyan, 10.0f, TEXT("None"));

	// *** 戦術コマンドNoを保存 ***
	UMy_Game_Instance* instance = Cast<UMy_Game_Instance>(UGameplayStatics::GetGameInstance(GetWorld())); // ゲームインスタンス
	if (instance == nullptr) return; // nullチェック

	// *戦術コマンドは4つに制限
	if (instance->players_tactics_command_nums.Num() < 4) instance->players_tactics_command_nums.Add(displayOrgumentNums[1]);
	// ***

	// *** ゲームフィールドへ移動 **
	UGameplayStatics::OpenLevel(GetWorld(), TEXT("Game_Field_Map")); // ゲームフィールドMAPへ
	// ***
}

// オーグメント選択ボタンクリック (3)
void UC_Orgument_UI::OrgumentButton3Clicked()
{
	UKismetSystemLibrary::PrintString(this, "ORGUMENT 3: " + FString::FromInt(displayOrgumentNums[2]), true, true, FColor::Cyan, 10.0f, TEXT("None"));

	// *** ステータスを変更する ***
	UMy_Game_Instance* instance = Cast<UMy_Game_Instance>(UGameplayStatics::GetGameInstance(GetWorld())); // ゲームインスタンス
	if (instance) {

		int _statusCommandNo = displayOrgumentNums[2]; // ステータスコマンドNo
		if (_statusCommandNo == 1) // サイドアタック1UP
		{
			instance->player_side_break_abilty++;
		}
		else if (_statusCommandNo == 2) // 体力1UP
		{
			instance->player_physical_abilty++;
		}
		else if (_statusCommandNo == 3) // プレス耐性1UP
		{
			instance->player_press_resistance_abilty++;
		}
		else {}
	}
	// ***

	// *** ゲームフィールドへ移動 **
	UGameplayStatics::OpenLevel(GetWorld(), TEXT("Game_Field_Map")); // ゲームフィールドMAPへ
	// ***
}
