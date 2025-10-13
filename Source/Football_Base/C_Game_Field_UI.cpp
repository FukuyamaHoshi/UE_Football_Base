// Fill out your copyright notice in the Description page of Project Settings.


#include "C_Game_Field_UI.h"
#include <Kismet/KismetSystemLibrary.h>
#include <Kismet/GameplayStatics.h>
#include "My_Game_Instance.h"
#include "C_Common.h"

UC_Game_Field_UI::UC_Game_Field_UI(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
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

void UC_Game_Field_UI::NativeConstruct()
{
    // *** クリックイベントをバインド ***
    if (Match_Start_Button) // 試合開始ボタン
    {
        Match_Start_Button->OnClicked.AddUniqueDynamic(this, &UC_Game_Field_UI::MatchStartButtonClicked);
    }
    if (Tactics_Command_Button_1) // 戦術コマンド1
    {
        Tactics_Command_Button_1->OnClicked.AddUniqueDynamic(this, &UC_Game_Field_UI::TacticsCommand1Clicked);
    }
    if (Tactics_Command_Button_2) // 戦術コマンド2
    {
        Tactics_Command_Button_2->OnClicked.AddUniqueDynamic(this, &UC_Game_Field_UI::TacticsCommand2Clicked);
    }
    if (Tactics_Command_Button_3) // 戦術コマンド3
    {
        Tactics_Command_Button_3->OnClicked.AddUniqueDynamic(this, &UC_Game_Field_UI::TacticsCommand3Clicked);
    }
    if (Tactics_Command_Button_4) // 戦術コマンド4
    {
        Tactics_Command_Button_4->OnClicked.AddUniqueDynamic(this, &UC_Game_Field_UI::TacticsCommand4Clicked);
    }
    // ***

    // *** 変数・配列セット　***
    UMy_Game_Instance* _instance = Cast<UMy_Game_Instance>(UGameplayStatics::GetGameInstance(GetWorld())); // ゲームインスタンス

    if (_instance) {
        // UI(表示・非表示)
        tacticsCommandBorders = { Tactics_Command_Border_1, Tactics_Command_Border_2, Tactics_Command_Border_3, Tactics_Command_Border_4 }; // ボーダー
        tacticsCommandButtons = { Tactics_Command_Button_1, Tactics_Command_Button_2, Tactics_Command_Button_3, Tactics_Command_Button_4 }; // ボタン
        // UI(テキストセット)
        tacticsCommandNameBlanks = { Tactics_Command_Name_1, Tactics_Command_Name_2, Tactics_Command_Name_3, Tactics_Command_Name_4 }; // 名前
        tacticsCommandScoreBlanks = { Tactics_Command_Value_1, Tactics_Command_Value_2, Tactics_Command_Value_3, Tactics_Command_Value_4 }; // スコア
        tacticsImageBlanks = {Tactics_Image_1, Tactics_Image_2, Tactics_Image_3, Tactics_Image_4 }; // 画像
    }
    // ***
    
    // *** 能力値をセット (プレイヤー) ***
    if (_instance) {
        // 〇サイド突破
        TArray<UBorder*> _mySideBreakBoxes = {
            My_Side_Break_Abilty_Box_1, My_Side_Break_Abilty_Box_2, My_Side_Break_Abilty_Box_3, My_Side_Break_Abilty_Box_4, My_Side_Break_Abilty_Box_5 };
        // 色をセット
        for (int i = 0; i < _instance->player_side_break_abilty; ++i) {
            _mySideBreakBoxes[i]->SetBrushColor(SIDE_BREAK_ABILTY_COLOR);
        }
        // テキストをセット
        FString _mySideBreakText = FString::FromInt(_instance->player_side_break_abilty) + ABILTY_END_TEXT;
        My_Side_Break_Abilty_Text->SetText(FText::FromString(_mySideBreakText));

        // 〇走力
        TArray<UBorder*> _myPhisicalBoxes = {
            My_Phisical_Abilty_Box_1, My_Phisical_Abilty_Box_2, My_Phisical_Abilty_Box_3, My_Phisical_Abilty_Box_4, My_Phisical_Abilty_Box_5 };
        // 色をセット
        for (int i = 0; i < _instance->player_physical_abilty; ++i) {
            _myPhisicalBoxes[i]->SetBrushColor(PHISICAL_ABILTY_COLOR);
        }
        // テキストをセット
        FString _myPhysicalText = FString::FromInt(_instance->player_physical_abilty) + ABILTY_END_TEXT;
        My_Phisical_Abilty_Text->SetText(FText::FromString(_myPhysicalText));

        // 〇プレス耐性
        TArray<UBorder*> _myPressResistanceBoxes = {
            My_Press_Resistance_Abilty_Box_1, My_Press_Resistance_Abilty_Box_2, My_Press_Resistance_Abilty_Box_3, My_Press_Resistance_Abilty_Box_4, My_Press_Resistance_Abilty_Box_5 };
        // 色をセット
        for (int i = 0; i < _instance->player_press_resistance_abilty; ++i) {
            _myPressResistanceBoxes[i]->SetBrushColor(PRESS_RESISTANCE_ABILTY_COLOR);
        }
        // テキストをセット
        FString _myPressResistanceText = FString::FromInt(_instance->player_press_resistance_abilty) + ABILTY_END_TEXT;
        My_Press_Resistance_Abilty_Text->SetText(FText::FromString(_myPressResistanceText));
    }
    // ***

    // *** ラウンド数更新 ***
    UpdateRoundNum();
    // ***

    // *** ボール保持アイコン更新 ***
    UpdateBallPossessIcon();
    // ***
    
    // *** 戦術コマンドUI表示 ***
    if (_instance) {
        if (_instance->players_tactics_command_nums.Num() > 0) { // オーグメントがある場合
            for (int _i = 0; _i < _instance->players_tactics_command_nums.Num(); _i++)
            {
                // -- ネームセット --
                TArray<FString> _texts = C_Common::GetTacticsCommandTexts(_instance->players_tactics_command_nums[_i]); // コマンドテキスト
                tacticsCommandNameBlanks[_i]->SetText(FText::FromString(_texts[0])); // 名前セット
                // --
                
                // -- スコアセット --
                C_Common::FTacticsCommand _value = C_Common::GetTacticsCommandValue(_instance->players_tactics_command_nums[_i]); // コマンド値
                
                // ●タイプ別にスコア計算とボーダーカラー変更
                int _score = 0; // スコア
                if (_value.type == 1) { // サイド
                    _score = _value.score * _instance->player_side_break_abilty; // サイド突破 (スコア計算)
                    tacticsCommandBorders[_i]->SetBrushColor(SIDE_BREAK_BORDER_COLOR); // ボーダー色変更
                }
                else if (_value.type == 2) { // ライン
                    _score = _value.score * _instance->player_physical_abilty; // 体力 (スコア計算)
                    tacticsCommandBorders[_i]->SetBrushColor(PHISICAL_BORDER_COLOR); // ボーダー色変更
                }
                else { // ブロック
                    _score = _value.score * _instance->player_press_resistance_abilty; // プレス耐性 (スコア計算)
                    tacticsCommandBorders[_i]->SetBrushColor(PRESS_RESISTANCE_BORDER_COLOR); // ボーダー色変更
                }
                tacticsCommandScoreBlanks[_i]->SetText(FText::FromString(FString::FromInt(_score))); // *スコアセット
                // --

                // -- アイコンセット --
                UTexture2D* _tacticsImage = GetTacticsImage(_instance->players_tactics_command_nums[_i]); // アイコン取得
                if (_tacticsImage != nullptr) tacticsImageBlanks[_i]->SetBrushFromTexture(_tacticsImage); // アイコンセット
                // --
                
                // -- UI表示 --
                tacticsCommandButtons[_i]->SetVisibility(ESlateVisibility::Visible); // ボタン
                tacticsCommandBorders[_i]->SetVisibility(ESlateVisibility::Visible); // ボーダー
                // --
            }
        }
    }
    // ***

    // *** 戦術コマンドの使用制限(ボール保持・非保持) ***
    UpdateEnableTacticsCommand();
    // ***

    // *** 敵のコマンド更新 **
    UpdateEnemyTacticsCommand();
    // ***

}

// 試合開始ボタンクリック
void UC_Game_Field_UI::MatchStartButtonClicked()
{
}

// 戦術コマンド1クリック
void UC_Game_Field_UI::TacticsCommand1Clicked()
{
    UKismetSystemLibrary::PrintString(this, "tactick command 1 button", true, true, FColor::Cyan, 2.f, TEXT("None"));
    
    // -- 選択したコマンドセット --
    UMy_Game_Instance* _instance = Cast<UMy_Game_Instance>(UGameplayStatics::GetGameInstance(GetWorld())); // ゲームインスタンス
    if (_instance) {
        selectPlayerTacticsCommand = _instance->players_tactics_command_nums[0]; // コマンドセット
    }
    // --
    
    // -- 対戦処理 --
    BattlePhase();
    // --
}

// 戦術コマンド2クリック
void UC_Game_Field_UI::TacticsCommand2Clicked()
{
    UKismetSystemLibrary::PrintString(this, "tactics command 2 button", true, true, FColor::Cyan, 2.f, TEXT("None"));

    // -- 選択したコマンドセット --
    UMy_Game_Instance* _instance = Cast<UMy_Game_Instance>(UGameplayStatics::GetGameInstance(GetWorld())); // ゲームインスタンス
    if (_instance) {
        selectPlayerTacticsCommand = _instance->players_tactics_command_nums[1]; // コマンドセット
    }
    // --
    
    // *** 対戦処理 ***
    BattlePhase();
    // ***
}

// 戦術コマンド3クリック
void UC_Game_Field_UI::TacticsCommand3Clicked()
{
    UKismetSystemLibrary::PrintString(this, "tactick command 3 button", true, true, FColor::Cyan, 2.f, TEXT("None"));

    // -- 選択したコマンドセット --
    UMy_Game_Instance* _instance = Cast<UMy_Game_Instance>(UGameplayStatics::GetGameInstance(GetWorld())); // ゲームインスタンス
    if (_instance) {
        selectPlayerTacticsCommand = _instance->players_tactics_command_nums[2]; // コマンドセット
    }
    // --
    
    // *** 対戦処理 ***
    BattlePhase();
    // ***
}

// 戦術コマンド4クリック
void UC_Game_Field_UI::TacticsCommand4Clicked()
{
    UKismetSystemLibrary::PrintString(this, "tactick command 4 button", true, true, FColor::Cyan, 2.f, TEXT("None"));

    // -- 選択したコマンドセット --
    UMy_Game_Instance* _instance = Cast<UMy_Game_Instance>(UGameplayStatics::GetGameInstance(GetWorld())); // ゲームインスタンス
    if (_instance) {
        selectPlayerTacticsCommand = _instance->players_tactics_command_nums[3]; // コマンドセット
    }
    // --
    
    // *** 対戦処理 ***
    BattlePhase();
    // ***
}

// 対戦処理
void UC_Game_Field_UI::BattlePhase()
{
    // *** ダメージ取得 ***
    int _damage = GetDanage();
    // ***
    
    // *** HP更新 ***
    UpdateHP(_damage);
    // ***

    // *** ラウンド更新時、処理 ***
    //  →HPが0になれば発火
    int _myHp = My_HP_Bar->GetPercent() * 100; // myHP
    if (_myHp <= 0) { // -- プレイヤーHPが0の場合 --

        // * ポイント更新 (敵側の) *
        // 〇数更新
        enemyPoint++;
        // 〇色セット
        if (enemyPoint <= 1) { // ポイント1以下か
            // Point 1
            Enemy_Point_1->SetBrushColor(POINT_COLOR);
        }
        else {
            // point 2
            Enemy_Point_2->SetBrushColor(POINT_COLOR);
        }
        // *

        // * 試合終了時処理 *
        if (enemyPoint >= 2) {
            UGameplayStatics::OpenLevel(GetWorld(), TEXT("Opening")); // オープニングMAPへ

            return;
        }
        // *

        // * HPリセット (プレイヤー側の) *
        My_HP_Bar->SetPercent(1.0f); // HP値
        FString _myHpText = "100" + HP_END_TEXT;
        My_HP_Text->SetText(FText::FromString(_myHpText)); // HPテキスト
        // *

        // * ボール保持者・アイコン更新 *
        isMyBall = !isMyBall;
        UpdateBallPossessIcon();
        // *

        // * 戦術コマンドの使用制限(ボール保持・非保持) *
        UpdateEnableTacticsCommand();
        // *

        // * ラウンド数更新 *
        UpdateRoundNum();
        // *

        // * 敵のコマンド更新 *
        UpdateEnemyTacticsCommand();
        // *
    }

    int _enemyHp = Enemy_HP_Bar->GetPercent() * 100; // 敵HP
    if (_enemyHp <= 0) { // -- 敵HPが0の場合 --

        //  * ポイント更新 (プレイヤー側の) *
        // 〇ポイント数更新
        myPoint++;
        // 〇色セット
        if (myPoint <= 1) { // ポイント1以下か
            // Point 1
            My_Point_1->SetBrushColor(POINT_COLOR);
        }
        else {
            // point 2
            My_Point_2->SetBrushColor(POINT_COLOR);
        }
        // *

        //  * 試合終了時処理 *
        if (myPoint >= 2) {
            
            // ●試合ラウンド(決勝とかの)更新
            UMy_Game_Instance* _instance = Cast<UMy_Game_Instance>(UGameplayStatics::GetGameInstance(GetWorld())); // ゲームインスタンス
            if (_instance)
            {
                _instance->match_round++; // 次のラウンド
            }

            UGameplayStatics::OpenLevel(GetWorld(), TEXT("Route")); // ルートMAPへ

            return;
        }
        // *

        // * HPリセット (敵側の) *
        Enemy_HP_Bar->SetPercent(1.0f); // HP値
        FString _enemyHpText = "100" + HP_END_TEXT;
        Enemy_HP_Text->SetText(FText::FromString(_enemyHpText)); // HPテキスト
        // *

        // * ボール保持者・アイコン更新 *
        isMyBall = !isMyBall;
        UpdateBallPossessIcon();
        // *

        // * 戦術コマンドの使用制限(ボール保持・非保持) *
        UpdateEnableTacticsCommand();
        // *

        // * ラウンド数更新 *
        UpdateRoundNum();
        // *

        // * 敵のコマンド更新 *
        UpdateEnemyTacticsCommand();
        // *
    }
}

// HP値を更新
void UC_Game_Field_UI::UpdateHP(int damage)
{
    // 更新するHPバー取得
    UProgressBar* _HPBar = nullptr; // HPバー
    UTextBlock* _HPText = nullptr; // HPテキスト
    if (damage > 0) { // どっちのダメージ量が多いか
        // 敵にダメージを与える
        _HPBar = Enemy_HP_Bar;
        _HPText = Enemy_HP_Text;
    }
    else {
        // 自分がダメージを受ける
        _HPBar = My_HP_Bar;
        _HPText = My_HP_Text;
    }
    // ***
    
    // *** HP値セット ***
    int _currentHp = _HPBar->GetPercent() * 100; // 現在のHP
    // HP値を更新
    damage = FMath::Abs(damage); // 正の数へ変換
    int _newHp = _currentHp - damage; // 更新後HP
    // HP値をセット
    float f_newHp = float(_newHp) / 100; // floatへ変換
    _HPBar->SetPercent(f_newHp);
    // ***

    // *** HPテキストセット ***
    FString _HPTextValue = FString::FromInt(_newHp) + HP_END_TEXT;
    _HPText->SetText(FText::FromString(_HPTextValue)); // テキスト更新
    // ***
}

// ラウンド数更新・セット
void UC_Game_Field_UI::UpdateRoundNum()
{
    // -- 更新 --
    round++;

    // -- セット --
    FString _roundText = ROUND_TOP_TEXT + FString::FromInt(round);
    Rount_Text->SetText(FText::FromString(_roundText));
}

// ボール保持アイコン更新
void UC_Game_Field_UI::UpdateBallPossessIcon()
{
    if (isMyBall) {
        // プレイヤー
        My_Ball_Image->SetOpacity(1.0f);
        Enemy_Ball_Image->SetOpacity(0);
        Abilty_Erea_My_Ball_Image->SetOpacity(1.0f);
    }
    else {
        // 敵
        My_Ball_Image->SetOpacity(0);
        Enemy_Ball_Image->SetOpacity(1.0f);
        Abilty_Erea_My_Ball_Image->SetOpacity(0);
    }
}

// ダメージ計算・取得
// | 返り値: + = 敵へのダメージ, - = プレイヤーへのダメージ |
int UC_Game_Field_UI::GetDanage()
{
    UMy_Game_Instance* _instance = Cast<UMy_Game_Instance>(UGameplayStatics::GetGameInstance(GetWorld())); // ゲームインスタンス
    if (_instance == nullptr) return 0;
    
    C_Common::FTacticsCommand _playerCommandValue = C_Common::GetTacticsCommandValue(selectPlayerTacticsCommand); // コマンド値 (プレイヤー)
    C_Common::FTacticsCommand _enemyCommandValue = C_Common::GetTacticsCommandValue(enemyTacticsCommand); // コマンド値 (敵)

    // -- スコア計算 --
    // ●プレイヤー
    int _playerScore = 0; // プレイヤースコア
    if (_playerCommandValue.type == 1) { // サイド (タイプコマンド)
        // サイド突破アビリティ (バフ)
        _playerScore = _playerCommandValue.score * _instance->player_side_break_abilty; 
    }
    else if (_playerCommandValue.type == 2) { // ライン (タイプコマンド)
        // 走力アビリティ (バフ)
        _playerScore = _playerCommandValue.score * _instance->player_physical_abilty;
    }
    else { // ブロック (タイプコマンド)
        // プレス耐性アビリティ (バフ)
        _playerScore = _playerCommandValue.score * _instance->player_press_resistance_abilty;
    }

    // ●敵
    int _enemyScore = 0; // 敵スコア
    if (_enemyCommandValue.type == 1) { // サイド (タイプコマンド)
        // サイド突破アビリティ (バフ)
        _enemyScore = _enemyCommandValue.score *enemySideBreakAbilty;
    }
    else if (_enemyCommandValue.type == 2) { // ライン (タイプコマンド)
        // 走力アビリティ (バフ)
        _enemyScore = _enemyCommandValue.score * enemyPhisicalAbilty;
    }
    else { // ブロック (タイプコマンド)
        // プレス耐性アビリティ (バフ)
        _enemyScore = _enemyCommandValue.score *enemyPressResistanceAbilty;
    }
    // --

    // -- 戦術コマンドがない場合 --
    if (selectPlayerTacticsCommand == 0) _playerScore = NONE_TACTICS_COMMAND_SCORE;
    if (enemyTacticsCommand == 0) _enemyScore = NONE_TACTICS_COMMAND_SCORE;
    // --

    // -- ダメージ計算 --
    int _damage = _playerScore - _enemyScore;
    // --
    
    
    return _damage;
}

// 敵の戦術コマンド更新
void UC_Game_Field_UI::UpdateEnemyTacticsCommand()
{
    if (isMyBall) {
        // ボール非保持
        enemyTacticsCommand = C_Common::LOW_BLOCK_COMMAND_NO;
    }
    else {
        // ボール保持
        enemyTacticsCommand = C_Common::LINE_BREAK_COMMAND_NO;
    }
}

// 戦術コマンドの使用制限(ボール保持・非保持)を更新
void UC_Game_Field_UI::UpdateEnableTacticsCommand()
{
    UMy_Game_Instance* _instance = Cast<UMy_Game_Instance>(UGameplayStatics::GetGameInstance(GetWorld())); // ゲームインスタンス

    if (_instance) {
        for (int _i = 0; _i < _instance->players_tactics_command_nums.Num(); _i++) {
            C_Common::FTacticsCommand _value = C_Common::GetTacticsCommandValue(_instance->players_tactics_command_nums[_i]); // コマンド値

            if (isMyBall) {
                // ボール保持時
                if (_value.isOnBall) {
                    tacticsCommandButtons[_i]->SetIsEnabled(true); // 使用可
                    // テキストカラー
                    tacticsCommandNameBlanks[_i]->SetColorAndOpacity(FColor::White);
                    tacticsCommandScoreBlanks[_i]->SetColorAndOpacity(FColor::White);
                }
                else {
                    tacticsCommandButtons[_i]->SetIsEnabled(false); // 使用不可
                    // テキストカラー
                    tacticsCommandNameBlanks[_i]->SetColorAndOpacity(DISABLE_BORDER_TEXT_COLOR);
                    tacticsCommandScoreBlanks[_i]->SetColorAndOpacity(DISABLE_BORDER_TEXT_COLOR);
                }
            }
            else {
                // ボール非保持
                if (_value.isOnBall) {
                    tacticsCommandButtons[_i]->SetIsEnabled(false); // 使用不可
                    // テキストカラー
                    tacticsCommandNameBlanks[_i]->SetColorAndOpacity(DISABLE_BORDER_TEXT_COLOR);
                    tacticsCommandScoreBlanks[_i]->SetColorAndOpacity(DISABLE_BORDER_TEXT_COLOR);
                }
                else {
                    tacticsCommandButtons[_i]->SetIsEnabled(true); // 使用可
                    // テキストカラー
                    tacticsCommandNameBlanks[_i]->SetColorAndOpacity(FColor::White);
                    tacticsCommandScoreBlanks[_i]->SetColorAndOpacity(FColor::White);
                }
            }
        }
    }
}

// 戦術アイコン画像取得
UTexture2D* UC_Game_Field_UI::GetTacticsImage(int tacticsCommandNo)
{
    switch (tacticsCommandNo)
    {
    case C_Common::SIDE_ATTACK_COMMAND_NO: // サイドアタック

        return sideAttackTacticsImage;
    case C_Common::HIGH_LINE_COMMAND_NO: // ハイライン

        return highLineTacticsImage;
    case C_Common::LINE_BREAK_COMMAND_NO: // ラインブレイク

        return lineBreakTacticsImage;
    case C_Common::LOW_BLOCK_COMMAND_NO: // ローブロック

        return lowBlockTacticsImage;
    case C_Common::GK_BUILD_UP_COMMAND_NO: // GKビルドアップ

        return gkBuildUpTacticsImage;
    case C_Common::SIDE_COMPLESSION_COMMAND_NO: // サイド圧縮

        return sideCompTacticsImage;
    default:

        return nullptr;;
    }
}
