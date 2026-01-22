// Fill out your copyright notice in the Description page of Project Settings.


#include "C_Route_UI.h"
#include <Kismet/KismetSystemLibrary.h>
#include <Kismet/GameplayStatics.h>
#include <Football_Base/Systems/My_Game_Instance.h>

// コンストラクター(本当の)
UC_Route_UI::UC_Route_UI(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    // *** マテリアル取得 (staticつけない) ***
    // -- チームボックス発光マテリアル --
    ConstructorHelpers::FObjectFinder<UMaterialInstance> glowConstantMaterialAsset(TEXT("/Game/Materials/UIs/Material_Instance/MI_Glow_Constant_Team_Box.MI_Glow_Constant_Team_Box"));
    if (glowConstantMaterialAsset.Succeeded())
    {
        glowContstantMaterial = glowConstantMaterialAsset.Object;
    }
    // ***
}

void UC_Route_UI::NativeConstruct()
{
    // *** クリックイベントをバインド ***
    if (Next_Match_Button) // 次の試合ボタン
    {
        Next_Match_Button->OnClicked.AddUniqueDynamic(this, &UC_Route_UI::NextMatchButtonClicked);
    }

    // チームボックス追加
    teamBoxes = {
        Team_Box_1, Team_Box_2, Team_Box_3, Team_Box_4, Team_Box_5,
        Team_Box_6, Team_Box_7, Team_Box_8, Team_Box_9, Team_Box_10,
        Team_Box_11, Team_Box_12, Team_Box_13, Team_Box_14, Team_Box_15,
        Team_Box_16, Team_Box_17, Team_Box_18, Team_Box_19, Team_Box_20,
        Team_Box_21, Team_Box_22, Team_Box_23, Team_Box_24, Team_Box_25,
        Team_Box_26, Team_Box_27, Team_Box_28, Team_Box_29, Team_Box_30,
    };
    // チームテキスト追加
    teamTextBlanks = {
        Team_Text_1, Team_Text_2, Team_Text_3, Team_Text_4, Team_Text_5,
        Team_Text_6, Team_Text_7, Team_Text_8, Team_Text_9, Team_Text_10,
        Team_Text_11, Team_Text_12, Team_Text_13, Team_Text_14, Team_Text_15,
        Team_Text_16, Team_Text_17, Team_Text_18, Team_Text_19, Team_Text_20,
        Team_Text_21, Team_Text_22, Team_Text_23, Team_Text_24, Team_Text_25,
        Team_Text_26, Team_Text_27, Team_Text_28, Team_Text_29, Team_Text_30,
    };
    // チーム名前追加
    teamNames = {
        ENEMY_TEAM_NAME_1, ENEMY_TEAM_NAME_2, ENEMY_TEAM_NAME_3, ENEMY_TEAM_NAME_4, ENEMY_TEAM_NAME_5,
        ENEMY_TEAM_NAME_6, ENEMY_TEAM_NAME_7, ENEMY_TEAM_NAME_8, ENEMY_TEAM_NAME_9, ENEMY_TEAM_NAME_10,
        ENEMY_TEAM_NAME_11, ENEMY_TEAM_NAME_12, ENEMY_TEAM_NAME_13, ENEMY_TEAM_NAME_14, ENEMY_TEAM_NAME_15
    };
    // スコア配列追加
    scoreBlanks = {
        Score_Text_1, Score_Text_2, Score_Text_3, Score_Text_4, Score_Text_5,
        Score_Text_6, Score_Text_7, Score_Text_8, Score_Text_9, Score_Text_10,
        Score_Text_11, Score_Text_12, Score_Text_13, Score_Text_14, Score_Text_15,
        Score_Text_16, Score_Text_17, Score_Text_18, Score_Text_19, Score_Text_20,
        Score_Text_21, Score_Text_22, Score_Text_23, Score_Text_24, Score_Text_25,
        Score_Text_26, Score_Text_27, Score_Text_28, Score_Text_29, Score_Text_30
    };
    // 発光ボックス配列追加
    retainerBoxes = {
        RetainerBox_1, RetainerBox_2, RetainerBox_3, RetainerBox_4, RetainerBox_5,
        RetainerBox_6, RetainerBox_7, RetainerBox_8, RetainerBox_9, RetainerBox_10,
        RetainerBox_11, RetainerBox_12, RetainerBox_13, RetainerBox_14, RetainerBox_15,
        RetainerBox_16, RetainerBox_17, RetainerBox_18, RetainerBox_19, RetainerBox_20,
        RetainerBox_21, RetainerBox_22, RetainerBox_23, RetainerBox_24, RetainerBox_25,
        RetainerBox_26, RetainerBox_27, RetainerBox_28, RetainerBox_29, RetainerBox_30
    };

    // *** 対戦情報を表示 ***
    DisplayMatchInfo();
    // ***
    
}

// 次の試合ボタンクリック
void UC_Route_UI::NextMatchButtonClicked()
{
    UKismetSystemLibrary::PrintString(this, "next match", true, true, FColor::Cyan, 2.f, TEXT("None"));
    
    UGameplayStatics::OpenLevel(GetWorld(), TEXT("Orgument")); // オーグメントMAPへ
}

// 対戦情報を表示する
void UC_Route_UI::DisplayMatchInfo()
{
    UMy_Game_Instance* instance = Cast<UMy_Game_Instance>(UGameplayStatics::GetGameInstance(GetWorld())); // ゲームインスタンス
    if (instance == nullptr) return; // nullチェック

    int _matchRound = instance->match_round; // 試合ラウンド
    
    // ***  ラウンド16 *** 
    if (_matchRound == 1)
    {
        // -- プレイヤーの情報追加 --
        UMy_Game_Instance::FTeamBoxElem _playerInfo = { PLAYER_BOX_NUMS[0], MY_TEAM_NAME,  3, "" };
        instance->team_box_elems.Add(_playerInfo);
        // --

        // -- 対戦情報追加(ラウンド16) --
        teamNames.Insert(MY_TEAM_NAME, PLAYER_BOX_NUMS[0] - 1); // 配列に追加(プレイヤー名) *クラッシュ対応
        for (int _i = 0; _i < 16; _i++) {
            if (_i == (PLAYER_BOX_NUMS[0] - 1)) continue; // プレイヤー情報スキップ

            UMy_Game_Instance::FTeamBoxElem _teamInfo = { _i + 1, teamNames[_i], 0, "" };
            instance->team_box_elems.Add(_teamInfo);
        }
        // --

        // -- UIセット --
        SetInfoUI();
        // --
    }
    
    // *** 準々決勝 ***
    else if (_matchRound == 2)
    {
        // -- 勝利チーム決定(ラウンド16) --
        TArray<int> _winTeamNums = {}; // 勝利チームNo配列
        TArray<FString> _winTeamNames = {}; // 勝利チーム名前配列
        for (int _i = 0; _i < 16; _i += 2) { // 2ずつ
            // プレイヤーが勝利するように対応
            if (_i < PLAYER_BOX_NUMS[0]) { // *プレイヤーが一番上のボックスにいる時のみ対応可
                _winTeamNums.Add(PLAYER_BOX_NUMS[0]);

                continue;
            }
            int _random = FMath::RandRange(0, 1); // 0 or 1
            int _num = _i + _random;

            _winTeamNums.Add(_num + 1); // チームNoへ変換
        }
        // --

        // -- info更新  --
        for (UMy_Game_Instance::FTeamBoxElem& _teamBox : instance->team_box_elems) {
            if (_winTeamNums.Contains(_teamBox.boxNum)) { // ●勝利チームの場合
                // state
                _teamBox.state = 1; // 勝利に更新
                _winTeamNames.Add(_teamBox.teamName); // 名前保存
                // 得点
                _teamBox.score = "2";

                continue;
            }

            // ●敗戦チーム
            // state
            _teamBox.state = 2; // 敗戦に更新
            // 得点
            _teamBox.score = FString::FromInt(FMath::RandRange(0, 1)); // 0 or 1
        }
        // --

        //　-- 対戦情報追加(準々決勝) --
        int _winCounter = 0; // 勝利チームカウンター
        for (int _i = 16; _i < 24; _i++) {
            UMy_Game_Instance::FTeamBoxElem _teamInfo; // チーム情報

            if (_winTeamNames[_winCounter] == MY_TEAM_NAME) { // 勝利チームがプレイヤーの場合
                // プレイヤー
                _teamInfo = { _i + 1, MY_TEAM_NAME, 3, "" };
            }
            else {
                // その他チーム
                _teamInfo = { _i + 1, _winTeamNames[_winCounter], 0, "" };
            }

            instance->team_box_elems.Add(_teamInfo); // 追加
            _winCounter++; // インクリメント
        }
        // --

        // -- UIセット --
        SetInfoUI();
        // --

    }
    
    // *** 準決勝 ***
    else if (_matchRound == 3)
    {
        // -- 勝利チーム決定(準々決勝) --
        TArray<int> _winTeamNums = {}; // 勝利チームNo配列
        TArray<FString> _winTeamNames = {}; // 勝利チーム名前配列
        for (int _i = 16; _i < 24; _i += 2) { // 2ずつ
            // プレイヤーが勝利するように対応
            if (_i < PLAYER_BOX_NUMS[1]) { // *プレイヤーが一番上のボックスにいる時のみ対応可
                _winTeamNums.Add(PLAYER_BOX_NUMS[1]);

                continue;
            }
            int _random = FMath::RandRange(0, 1); // 0 or 1
            int _num = _i + _random;

            _winTeamNums.Add(_num + 1); // チームNoへ変換
        }
        // --

        // -- info更新 --
        for (UMy_Game_Instance::FTeamBoxElem& _teamBox : instance->team_box_elems) {
            if (_winTeamNums.Contains(_teamBox.boxNum)) { // ●勝利チームの場合
                // state
                _teamBox.state = 1; // 勝利に更新
                _winTeamNames.Add(_teamBox.teamName); // 名前保存
                // 得点
                _teamBox.score = "2";

                continue;
            }

            if (_teamBox.boxNum > 16) { // 準々決勝以上のチーム
                // ●敗戦チーム
                // state
                _teamBox.state = 2; // 敗戦に更新
                // 得点
                _teamBox.score = FString::FromInt(FMath::RandRange(0, 1)); // 0 or 1
            }
        }
        // --

        //　-- 対戦情報追加(準決勝) --
        int _winCounter = 0; // 勝利チームカウンター
        for (int _i = 24; _i < 28; _i++) {
            UMy_Game_Instance::FTeamBoxElem _teamInfo; // チーム情報

            if (_winTeamNames[_winCounter] == MY_TEAM_NAME) { // 勝利チームがプレイヤーの場合
                // プレイヤー
                _teamInfo = { _i + 1, MY_TEAM_NAME, 3, "" };
            }
            else {
                // その他チーム
                _teamInfo = { _i + 1, _winTeamNames[_winCounter], 0, "" };
            }

            instance->team_box_elems.Add(_teamInfo); // 追加
            _winCounter++; // インクリメント
        }
        // --

        // -- UIセット --
        SetInfoUI();
        // --

    }
    
    // *** 決勝 ***
    else if (_matchRound == 4)
    {
        // -- 勝利チーム決定(準決勝) --
        TArray<int> _winTeamNums = {}; // 勝利チームNo配列
        TArray<FString> _winTeamNames = {}; // 勝利チーム名前配列
        for (int _i = 24; _i < 28; _i += 2) { // 2ずつ
            // プレイヤーが勝利するように対応
            if (_i < PLAYER_BOX_NUMS[2]) { // *プレイヤーが一番上のボックスにいる時のみ対応可
                _winTeamNums.Add(PLAYER_BOX_NUMS[2]);

                continue;
            }
            int _random = FMath::RandRange(0, 1); // 0 or 1
            int _num = _i + _random;

            _winTeamNums.Add(_num + 1); // チームNoへ変換
        }
        // --

        // -- info更新 --
        for (UMy_Game_Instance::FTeamBoxElem& _teamBox : instance->team_box_elems) {
            if (_winTeamNums.Contains(_teamBox.boxNum)) { // ●勝利チームの場合
                // state
                _teamBox.state = 1; // 勝利に更新
                _winTeamNames.Add(_teamBox.teamName); // 名前保存
                // 得点
                _teamBox.score = "2";

                continue;
            }

            if (_teamBox.boxNum > 24) { // 準決勝以上のチーム
                // ●敗戦チーム
                // state
                _teamBox.state = 2; // 敗戦に更新
                // 得点
                _teamBox.score = FString::FromInt(FMath::RandRange(0, 1)); // 0 or 1
            }
        }
        // --

        //　-- 対戦情報追加(決勝) --
        int _winCounter = 0; // 勝利チームカウンター
        for (int _i = 28; _i < 30; _i++) {
            UMy_Game_Instance::FTeamBoxElem _teamInfo; // チーム情報

            if (_winTeamNames[_winCounter] == MY_TEAM_NAME) { // 勝利チームがプレイヤーの場合
                // プレイヤー
                _teamInfo = { _i + 1, MY_TEAM_NAME, 3, "" };
            }
            else {
                // その他チーム
                _teamInfo = { _i + 1, _winTeamNames[_winCounter], 0, "" };
            }

            instance->team_box_elems.Add(_teamInfo); // 追加
            _winCounter++; // インクリメント
        }
        // --

        // -- UIセット --
        SetInfoUI();
        // --
    }
    else {
        // -- UIセット --
        SetInfoUI();
        // --
    }
}

// UIに情報セット
void UC_Route_UI::SetInfoUI()
{
    UMy_Game_Instance* instance = Cast<UMy_Game_Instance>(UGameplayStatics::GetGameInstance(GetWorld())); // ゲームインスタンス
    if (instance == nullptr) return; // nullチェック

    for (UMy_Game_Instance::FTeamBoxElem _teamBox : instance->team_box_elems) {
        // 名前
        teamTextBlanks[_teamBox.boxNum - 1]->SetText(FText::FromString(_teamBox.teamName));

        // ボックスカラー
        if (_teamBox.state == 0) {
            // 対戦
            teamBoxes[_teamBox.boxNum - 1]->SetBrushColor(TEAM_BOX_MATCH_COLOR);
        }
        else if (_teamBox.state == 1) {
            // 勝利
            teamBoxes[_teamBox.boxNum - 1]->SetBrushColor(TEAM_BOX_WIN_COLOR);
        }
        else if (_teamBox.state == 2) {
            // 敗戦
            teamBoxes[_teamBox.boxNum - 1]->SetBrushColor(TEAM_BOX_LOSE_COLOR);
            teamTextBlanks[_teamBox.boxNum - 1]->SetColorAndOpacity(TEAM_TEXT_LOSE_COLOR); // ←テキスト色
        }
        else if (_teamBox.state == 3) {
            // プレイヤー
            if (glowContstantMaterial != nullptr) retainerBoxes[_teamBox.boxNum - 1]->SetEffectMaterial(glowContstantMaterial);
        }
        else {
            UKismetSystemLibrary::PrintString(this, "route UI error", true, true, FColor::Red, 2.f, TEXT("None"));
        }

        // スコア
        scoreBlanks[_teamBox.boxNum - 1]->SetText(FText::FromString(_teamBox.score));

    }
}