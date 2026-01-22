// Fill out your copyright notice in the Description page of Project Settings.


#include "C_Tile.h"
#include <Kismet/KismetSystemLibrary.h>
#include <Football_Base/C_Common.h>




// Sets default values
AC_Tile::AC_Tile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	// *** コマ移動マテリアルの変数にセット(保持するだけ) ***
	static ConstructorHelpers::FObjectFinder<UMaterial> pieceMoveMaterialAsset(TEXT("/Game/Materials/Tile/M_Tile_Dark.M_Tile_Dark"));
	if (pieceMoveMaterialAsset.Succeeded())
	{
		pieceMoveMaterial = pieceMoveMaterialAsset.Object;
	}
	// ***

	//// *** パスレンジマテリアルの変数にセット(保持するだけ) ***
	//static ConstructorHelpers::FObjectFinder<UMaterial> passRangeMaterialAsset(TEXT("/Game/Materials/Tile/M_Tile_Pass_Range.M_Tile_Pass_Range"));
	//if (passRangeMaterialAsset.Succeeded())
	//{
	//	passRangeMaterial = passRangeMaterialAsset.Object;
	//}
	//// ***

	//// *** マークレンジマテリアルの変数にセット(保持するだけ) ***
	//static ConstructorHelpers::FObjectFinder<UMaterial> markRangeMaterialAsset(TEXT("/Game/Materials/Tile/M_Tile_Mark_Range.M_Tile_Mark_Range"));
	//if (markRangeMaterialAsset.Succeeded())
	//{
	//	markRangeMaterial = markRangeMaterialAsset.Object;
	//}
	//// ***

	//// *** プレイヤー配置レンジマテリアルの変数にセット(保持するだけ) ***
	//// - FW -
	//static ConstructorHelpers::FObjectFinder<UMaterial> FWplayerPlaceRangeMaterialAsset(TEXT("/Game/Materials/Tile/M_Tile_FW_Player_Place_Range.M_Tile_FW_Player_Place_Range"));
	//if (FWplayerPlaceRangeMaterialAsset.Succeeded())
	//{
	//	FWplayerPlaceMaterial = FWplayerPlaceRangeMaterialAsset.Object;
	//}
	//// - MF -
	//static ConstructorHelpers::FObjectFinder<UMaterial> MFplayerPlaceRangeMaterialAsset(TEXT("/Game/Materials/Tile/M_Tile_MF_Player_Place_Range.M_Tile_MF_Player_Place_Range"));
	//if (MFplayerPlaceRangeMaterialAsset.Succeeded())
	//{
	//	MFplayerPlaceMaterial = MFplayerPlaceRangeMaterialAsset.Object;
	//}
	//// - DF -
	//static ConstructorHelpers::FObjectFinder<UMaterial> DFplayerPlaceRangeMaterialAsset(TEXT("/Game/Materials/Tile/M_Tile_DF_Player_Place_Range.M_Tile_DF_Player_Place_Range"));
	//if (DFplayerPlaceRangeMaterialAsset.Succeeded())
	//{
	//	DFplayerPlaceMaterial = DFplayerPlaceRangeMaterialAsset.Object;
	//}
	//// ***
	//
	//// *** ハーフレーンマテリアルの変数にセット(保持するだけ) ***
	//static ConstructorHelpers::FObjectFinder<UMaterial> halfLernMaterialAsset(TEXT("/Game/Materials/Tile/Position_Erea/M_Tile_Half_Lern.M_Tile_Half_Lern"));
	//if (halfLernMaterialAsset.Succeeded())
	//{
	//	halfLernMaterial = halfLernMaterialAsset.Object;
	//}
}

// Called when the game starts or when spawned
void AC_Tile::BeginPlay()
{
	Super::BeginPlay();

	// ** メッシュを取得 **
	mesh = FindComponentByClass<UStaticMeshComponent>();

	// *** タイルNO取得 ***
	tileNo = GetTileNoFromLocation(GetActorLocation().X, GetActorLocation().Y);
}

// Called every frame
void AC_Tile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// 位置からタイルＮｏ取得
// ( unreal座標  x: 縦, y: 横 )
int AC_Tile::GetTileNoFromLocation(float x, float y)
{
	// タイルNo
	int tileNoY = 0; // タイルNo y (横から何個目か)
	int tileNoX = 0; // タイルNo x (下から何列目か)

	// *** タイルNo y(横から何個目か)を求める ***
	// -の時の処理
	bool isPositiveY = true; // xが正の整数か
	if (y < 0) {
		y = abs(y); // 絶対値に変換
		isPositiveY = false; // フラグ変更
	}

	if (y < (C_Common::TILE_SIZE / 2))	tileNoY = 3;
	else if (y < (C_Common::TILE_SIZE / 2) + C_Common::TILE_SIZE) tileNoY = (isPositiveY) ? 4 : 2;
	else tileNoY = (isPositiveY) ? 5 : 1;
	// ***

	// *** タイルNo x(下から何列目か)を求める ***
	// -の時の処理
	bool isPositiveX = true; // xが正の整数か
	if (x < 0) {
		x = abs(x); // 絶対値に変換
		isPositiveX = false; // フラグ変更
	}

	if (x < C_Common::TILE_SIZE)	tileNoX = (isPositiveX) ? 4 : 3;
	else if (x < C_Common::TILE_SIZE * 2) tileNoX = (isPositiveX) ? 5 : 2;
	else tileNoX = (isPositiveX) ? 6 : 1;
	// ***

	// *** タイルNo取得 ***
	// ( (タイル縦(x) - 1個) * フィールドの横のタイル個数 ) + タイル横(y)
	int _tileNo = ((tileNoX - 1) * C_Common::TILE_NUM_Y) + tileNoY;


	return _tileNo;
}

// コマ移動マテリアルをセット
void AC_Tile::SetMaterial()
{
	if (mesh == nullptr) return; // mesh確認
	if (pieceMoveMaterial == nullptr) return; // マテリアル確認
	
	if (pieceMoveMaterial != nullptr) mesh->SetOverlayMaterial(pieceMoveMaterial);
}

// メインのオーバーレイマテリアルを削除する
void AC_Tile::RemoveMainMaterial()
{
	if (mesh == nullptr) return; // mesh確認

	mesh->SetOverlayMaterial(nullptr);
}

// サブのオーバーレイマテリアルを削除する
void AC_Tile::RemoveSubMaterial()
{
	//if (subMesh == nullptr) return; // sub meshのnullチェック

	// マテリアルを削除
	//subMesh->SetOverlayMaterial(nullptr);
}

// パスレンジマテリアルをセット
void AC_Tile::SetPassRangeMaterial()
{
	// マテリアルをセット
	//if (passRangeMaterial != nullptr) mainMesh->SetOverlayMaterial(passRangeMaterial);
}

// マークレンジマテリアルをセット
void AC_Tile::SetMarkRangeMaterial()
{
	// マテリアルをセット
	//if (markRangeMaterial != nullptr) mainMesh->SetOverlayMaterial(markRangeMaterial);
}

// プレイヤー配置レンジマテリアルをセット
// - FW -
void AC_Tile::SetFWPlayerPlaceRangeMaterial()
{
	// マテリアルをセット
	//if (FWplayerPlaceMaterial != nullptr) subMesh->SetOverlayMaterial(FWplayerPlaceMaterial);
}
// - MF -
void AC_Tile::SetMFPlayerPlaceRangeMaterial()
{
	// マテリアルをセット
	//if (MFplayerPlaceMaterial != nullptr) subMesh->SetOverlayMaterial(MFplayerPlaceMaterial);
}
// - DF -
void AC_Tile::SetDFPlayerPlaceRangeMaterial()
{
	// マテリアルをセット
	//if (DFplayerPlaceMaterial != nullptr) subMesh->SetOverlayMaterial(DFplayerPlaceMaterial);
}

// ハーフレーンマテリアルをセット
void AC_Tile::SetHalfLernMaterial()
{
	//if (halfLernMaterial != nullptr) mainMesh->SetOverlayMaterial(halfLernMaterial);
}