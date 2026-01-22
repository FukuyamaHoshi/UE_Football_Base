// Fill out your copyright notice in the Description page of Project Settings.


#include "C_Orgument_UI.h"
#include <Kismet/KismetSystemLibrary.h>
#include <Kismet/GameplayStatics.h>
#include <Football_Base/C_Common.h>
#include <Football_Base/Systems/My_Game_Instance.h>

UC_Orgument_UI::UC_Orgument_UI(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// *** ステータスベース画像取得 (staticつけない) ***
	// -- プレス耐性 --
	ConstructorHelpers::FObjectFinder<UTexture2D> _pressBaseAsset(TEXT("/Game/Images/status_press_orgument_base.status_press_orgument_base"));
	if (_pressBaseAsset.Succeeded())
	{
		pressStatusBaseImage = _pressBaseAsset.Object;
	}
	// -- サイド突破 --
	ConstructorHelpers::FObjectFinder<UTexture2D> _sideBaseAsset(TEXT("/Game/Images/status_side_break_orgument_base.status_side_break_orgument_base"));
	if (_sideBaseAsset.Succeeded())
	{
		sideStatusBaseImage = _sideBaseAsset.Object;
	}
	// -- 走力 --
	ConstructorHelpers::FObjectFinder<UTexture2D> _physicsBaseAsset(TEXT("/Game/Images/status_physics_orgument_base.status_physics_orgument_base"));
	if (_physicsBaseAsset.Succeeded())
	{
		physicsStatusBaseImage = _physicsBaseAsset.Object;
	}
	// ***

	// *** ステータス画像取得 (staticつけない) ***
	// -- プレス耐性 --
	ConstructorHelpers::FObjectFinder<UTexture2D> _pressAsset(TEXT("/Game/Images/status_press_orgument.status_press_orgument"));
	if (_pressAsset.Succeeded())
	{
		pressStatusImage = _pressAsset.Object;
	}
	// -- サイド突破 --
	ConstructorHelpers::FObjectFinder<UTexture2D> _sideAsset(TEXT("/Game/Images/status_side_break_orgument.status_side_break_orgument"));
	if (_sideAsset.Succeeded())
	{
		sideStatusImage = _sideAsset.Object;
	}
	// -- 走力 --
	ConstructorHelpers::FObjectFinder<UTexture2D> _physicsAsset(TEXT("/Game/Images/status_physics_orgument.status_physics_orgument"));
	if (_physicsAsset.Succeeded())
	{
		physicsStatusImage = _physicsAsset.Object;
	}
	// ***

	// *** 戦術アイコン画像取得 (staticつけない) ***
	// -- ラインブレイク --
	ConstructorHelpers::FObjectFinder<UTexture2D> _lineBreakAsset(TEXT("/Game/Images/tactics_icon/line_break_icon.line_break_icon"));
	if (_lineBreakAsset.Succeeded())
	{
		lineBreakTacticsImage = _lineBreakAsset.Object;
	}
	// -- GKビルドアップ --
	ConstructorHelpers::FObjectFinder<UTexture2D> _gkBuildUpAsset(TEXT("/Game/Images/tactics_icon/gk_build_up_icon.gk_build_up_icon"));
	if (_gkBuildUpAsset.Succeeded())
	{
		gkBuildUpTacticsImage = _gkBuildUpAsset.Object;
	}
	// -- ハイライン --
	ConstructorHelpers::FObjectFinder<UTexture2D> _highLineAsset(TEXT("/Game/Images/tactics_icon/high_line_icon.high_line_icon"));
	if (_highLineAsset.Succeeded())
	{
		highLineTacticsImage = _highLineAsset.Object;
	}
	// -- ローブロック --
	ConstructorHelpers::FObjectFinder<UTexture2D> _lowBlockAsset(TEXT("/Game/Images/tactics_icon/low_block_icon.low_block_icon"));
	if (_lowBlockAsset.Succeeded())
	{
		lowBlockTacticsImage = _lowBlockAsset.Object;
	}
	// -- サイドアタック --
	ConstructorHelpers::FObjectFinder<UTexture2D> _sideAttackAsset(TEXT("/Game/Images/tactics_icon/side_attack_icon.side_attack_icon"));
	if (_sideAttackAsset.Succeeded())
	{
		sideAttackTacticsImage = _sideAttackAsset.Object;
	}
	// -- サイド圧縮 --
	ConstructorHelpers::FObjectFinder<UTexture2D> _sideCompAsset(TEXT("/Game/Images/tactics_icon/side_comp_icon.side_comp_icon"));
	if (_sideCompAsset.Succeeded())
	{
		sideCompTacticsImage = _sideCompAsset.Object;
	}
	// ***
}

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
	orgumentNoteTextBlanks = { Orgument_Note_1, Orgument_Note_2, Orgument_Note_3 };
	// オーグメントベース画像配列作成
	orgumentBaseImageBlanks = { Orgument_Image_Base_1, Orgument_Image_Base_2, Orgument_Image_Base_3 };
	// オーグメント画像配列作成
	orgumentImageBlanks = { Orgument_Image_1, Orgument_Image_2, Orgument_Image_3 };

	
	// *** 表示するオーグメントNoを3つ決定 ***
	// -- 戦術コマンド --
	//while (displayOrgumentNums.Num() < 2)
	//{
	//	int _tacticsNo = FMath::RandRange(1, C_Common::TACTICS_COMMANDS_NUMS); // ランダム
	//	if (displayOrgumentNums.Contains(_tacticsNo)) continue; // 被っていたらスキップ

	//	displayOrgumentNums.Add(_tacticsNo);
	//}
	//// -- ステータス --
	//int _statusNo = FMath::RandRange(1, C_Common::STATUS_COMMANDS_NUMS); // ランダム
	//displayOrgumentNums.Add(_statusNo);
	//// ***
	//
	//// *** オーグメントテキストUI表示・変数セット ***
	//// -- 戦術コマンド --
	//for (int _i = 0; _i < 2; _i++)
	//{
	//	// ●オーグメントテキスト取得
	//	TArray<FString> _tacticsTexts = C_Common::GetTacticsCommandTexts(displayOrgumentNums[_i]);
	//	// ●UI表示
	//	orgumentNameTextBlanks[_i]->SetText(FText::FromString(_tacticsTexts[0])); // 名前
	//	orgumentTypeTextBlanks[_i]->SetText(FText::FromString(_tacticsTexts[1])); // 種類
	//	orgumentNoteTextBlanks[_i]->SetText(FText::FromString(_tacticsTexts[2])); // 説明

	//	UTexture2D* _tacticsImage = GetTacticsImage(displayOrgumentNums[_i]); // アイコン取得
	//	if (_tacticsImage != nullptr) orgumentImageBlanks[_i]->SetBrushFromTexture(_tacticsImage); // 画像
	//}

	//// -- ステータス --
	//// ●オーグメントテキスト取得
	//TArray<FString> _statusTexts = C_Common::GetStatusCommandTexts(displayOrgumentNums[2]);
	//// ●UI表示
	//orgumentNameTextBlanks[2]->SetText(FText::FromString(_statusTexts[0])); // 名前
	//orgumentTypeTextBlanks[2]->SetText(FText::FromString(_statusTexts[1])); // 種類
	//orgumentNoteTextBlanks[2]->SetText(FText::FromString(_statusTexts[2])); // 説明
	//
	//TArray<UTexture2D*> _statusImages = GetStatusImage(displayOrgumentNums[2]); // 画像配列
	//if(_statusImages[0] != nullptr) orgumentBaseImageBlanks[2]->SetBrushFromTexture(_statusImages[0]); // ベース画像
	//if (_statusImages[1] != nullptr) orgumentImageBlanks[2]->SetBrushFromTexture(_statusImages[1]); // アイコン画像
	//// --
	//// ***
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

// ステータス画像を取得
// | 0 = ベース画像, 1 = ステータス画像 |
TArray<UTexture2D*> UC_Orgument_UI::GetStatusImage(int statusNo)
{
	TArray<UTexture2D*> _textures;

	if (statusNo == 1) { // -- サイド突破 --
		_textures.Add(sideStatusBaseImage);
		_textures.Add(sideStatusImage);
	}
	else if (statusNo == 2) { // -- 走力 --
		_textures.Add(physicsStatusBaseImage);
		_textures.Add(physicsStatusImage);
	}
	else { // -- プレス耐性 --
		_textures.Add(pressStatusBaseImage);
		_textures.Add(pressStatusImage);
	}

	return _textures;
}

// 戦術アイコン画像取得
UTexture2D* UC_Orgument_UI::GetTacticsImage(int tacticsCommandNo)
{
	//switch (tacticsCommandNo)
	//{
	//case C_Common::SIDE_ATTACK_COMMAND_NO: // サイドアタック
	//	
	//	return sideAttackTacticsImage;
	//case C_Common::HIGH_LINE_COMMAND_NO: // ハイライン
	//	
	//	return highLineTacticsImage;
	//case C_Common::LINE_BREAK_COMMAND_NO: // ラインブレイク
	//	
	//	return lineBreakTacticsImage;
	//case C_Common::LOW_BLOCK_COMMAND_NO: // ローブロック
	//	
	//	return lowBlockTacticsImage;
	//case C_Common::GK_BUILD_UP_COMMAND_NO: // GKビルドアップ
	//	
	//	return gkBuildUpTacticsImage;
	//case C_Common::SIDE_COMPLESSION_COMMAND_NO: // サイド圧縮
	//	
	//	return sideCompTacticsImage;
	//default:
		
		return nullptr;;
	//}
}
