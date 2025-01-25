// Fill out your copyright notice in the Description page of Project Settings.


#include "C_Tile.h"
#include <Kismet/KismetSystemLibrary.h>




// Sets default values
AC_Tile::AC_Tile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	// *** コマ移動マテリアルの変数にセット(保持するだけ) ***
	static ConstructorHelpers::FObjectFinder<UMaterial> pieceMoveMaterialAsset(TEXT("/Game/Materials/Tile/M_Tile_Emphasis.M_Tile_Emphasis"));
	if (pieceMoveMaterialAsset.Succeeded())
	{
		pieceMoveMaterial = pieceMoveMaterialAsset.Object;
	}
	// ***

	// *** パスレンジマテリアルの変数にセット(保持するだけ) ***
	static ConstructorHelpers::FObjectFinder<UMaterial> passRangeMaterialAsset(TEXT("/Game/Materials/Tile/M_Tile_Pass_Range.M_Tile_Pass_Range"));
	if (passRangeMaterialAsset.Succeeded())
	{
		passRangeMaterial = passRangeMaterialAsset.Object;
	}
	// ***

	// *** マークレンジマテリアルの変数にセット(保持するだけ) ***
	static ConstructorHelpers::FObjectFinder<UMaterial> markRangeMaterialAsset(TEXT("/Game/Materials/Tile/M_Tile_Mark_Range.M_Tile_Mark_Range"));
	if (markRangeMaterialAsset.Succeeded())
	{
		markRangeMaterial = markRangeMaterialAsset.Object;
	}
	// ***

	// *** プレイヤー配置レンジマテリアルの変数にセット(保持するだけ) ***
	// - FW -
	static ConstructorHelpers::FObjectFinder<UMaterial> FWplayerPlaceRangeMaterialAsset(TEXT("/Game/Materials/Tile/M_Tile_FW_Player_Place_Range.M_Tile_FW_Player_Place_Range"));
	if (FWplayerPlaceRangeMaterialAsset.Succeeded())
	{
		FWplayerPlaceMaterial = FWplayerPlaceRangeMaterialAsset.Object;
	}
	// - MF -
	static ConstructorHelpers::FObjectFinder<UMaterial> MFplayerPlaceRangeMaterialAsset(TEXT("/Game/Materials/Tile/M_Tile_MF_Player_Place_Range.M_Tile_MF_Player_Place_Range"));
	if (MFplayerPlaceRangeMaterialAsset.Succeeded())
	{
		MFplayerPlaceMaterial = MFplayerPlaceRangeMaterialAsset.Object;
	}
	// - DF -
	static ConstructorHelpers::FObjectFinder<UMaterial> DFplayerPlaceRangeMaterialAsset(TEXT("/Game/Materials/Tile/M_Tile_DF_Player_Place_Range.M_Tile_DF_Player_Place_Range"));
	if (DFplayerPlaceRangeMaterialAsset.Succeeded())
	{
		DFplayerPlaceMaterial = DFplayerPlaceRangeMaterialAsset.Object;
	}
	// ***	
}

// Called when the game starts or when spawned
void AC_Tile::BeginPlay()
{
	Super::BeginPlay();

	// ** メッシュ(main, sub)を取得し、分ける **
	TArray<UStaticMeshComponent*> Components; // component配列
	GetComponents<UStaticMeshComponent>(Components); // stacic componentを取得
	
	for (UStaticMeshComponent* c : Components) { // MAINタグか
		if (c->ComponentHasTag(FName("MAIN"))) {
			mainMesh = c;
		}
		else {
			subMesh = c;
		}
	}
	// **
}

// Called every frame
void AC_Tile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// コマ移動マテリアルをセット
void AC_Tile::SetMaterial()
{
	// マテリアルをセット
	if (pieceMoveMaterial != nullptr) mainMesh->SetOverlayMaterial(pieceMoveMaterial);
}

// メインのオーバーレイマテリアルを削除する
void AC_Tile::RemoveMainMaterial()
{
	if (mainMesh == nullptr) return; // main meshのnullチェック

	// マテリアルを削除
	mainMesh->SetOverlayMaterial(nullptr);
}

// サブのオーバーレイマテリアルを削除する
void AC_Tile::RemoveSubMaterial()
{
	if (subMesh == nullptr) return; // sub meshのnullチェック

	// マテリアルを削除
	subMesh->SetOverlayMaterial(nullptr);
}

// パスレンジマテリアルをセット
void AC_Tile::SetPassRangeMaterial()
{
	// マテリアルをセット
	if (passRangeMaterial != nullptr) mainMesh->SetOverlayMaterial(passRangeMaterial);
}

// マークレンジマテリアルをセット
void AC_Tile::SetMarkRangeMaterial()
{
	// マテリアルをセット
	if (markRangeMaterial != nullptr) mainMesh->SetOverlayMaterial(markRangeMaterial);
}

// プレイヤー配置レンジマテリアルをセット
// - FW -
void AC_Tile::SetFWPlayerPlaceRangeMaterial()
{
	// マテリアルをセット
	if (FWplayerPlaceMaterial != nullptr) subMesh->SetOverlayMaterial(FWplayerPlaceMaterial);
}
// - MF -
void AC_Tile::SetMFPlayerPlaceRangeMaterial()
{
	// マテリアルをセット
	if (MFplayerPlaceMaterial != nullptr) subMesh->SetOverlayMaterial(MFplayerPlaceMaterial);
}
// - DF -
void AC_Tile::SetDFPlayerPlaceRangeMaterial()
{
	// マテリアルをセット
	if (DFplayerPlaceMaterial != nullptr) subMesh->SetOverlayMaterial(DFplayerPlaceMaterial);
}
