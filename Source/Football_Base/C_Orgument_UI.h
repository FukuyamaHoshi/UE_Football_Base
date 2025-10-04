// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include <Components/TextBlock.h>
#include <Components/MultiLineEditableTextBox.h>
#include <Components/Button.h>
#include "C_Orgument_UI.generated.h"

/**
 * 
 */
UCLASS()
class FOOTBALL_BASE_API UC_Orgument_UI : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override; // コンストラクタ override

protected:
	// ********* バインド *********
	// オーグメントカード1
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Orgument_Name_1;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Orgument_Type_1;
	UPROPERTY(meta = (BindWidget))
	UMultiLineEditableTextBox* Orgument_Note_1;
	// オーグメントカード2
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Orgument_Name_2;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Orgument_Type_2;
	UPROPERTY(meta = (BindWidget))
	UMultiLineEditableTextBox* Orgument_Note_2;
	// オーグメントカード3
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Orgument_Name_3;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Orgument_Type_3;
	UPROPERTY(meta = (BindWidget))
	UMultiLineEditableTextBox* Orgument_Note_3;
	// オーグメント選択ボタン
	UPROPERTY(meta = (BindWidget))
	UButton* Orgument_Button_1;
	UPROPERTY(meta = (BindWidget))
	UButton* Orgument_Button_2;
	UPROPERTY(meta = (BindWidget))
	UButton* Orgument_Button_3;
	// ***

	// ********* Wiget関数 *********
	// オーグメント選択ボタンクリック
	UFUNCTION()
	void OrgumentButton1Clicked();
	UFUNCTION()
	void OrgumentButton2Clicked();
	UFUNCTION()
	void OrgumentButton3Clicked();
	// ***
	
	
private:
	TArray< UTextBlock*> orgumentNameTextBlanks = {}; // オーグメント名前配列
	TArray< UTextBlock*> orgumentTypeTextBlanks = {}; // オーグメント種類配列
	TArray< UMultiLineEditableTextBox*> orgumentNoteMultiTextBlanks = {}; // オーグメント説明配列
	TArray<int> displayOrgumentNums = {}; // 表示するオーグメントNos
};
