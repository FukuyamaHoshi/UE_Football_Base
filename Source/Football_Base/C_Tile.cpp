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
}

// Called when the game starts or when spawned
void AC_Tile::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AC_Tile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// コマ移動マテリアルをセット
void AC_Tile::SetMaterial()
{
	TArray<UStaticMeshComponent*> Components; // component配列

	// stacic componentを取得
	GetComponents<UStaticMeshComponent>(Components);
	UStaticMeshComponent* StaticMeshComponent = Components[0];

	// マテリアルをセット
	if (pieceMoveMaterial != nullptr) StaticMeshComponent->SetOverlayMaterial(pieceMoveMaterial);
}

// オーバーレイマテリアルを削除する
void AC_Tile::RemoveMaterial()
{
	TArray<UStaticMeshComponent*> Components; // component配列

	// stacic componentを取得
	GetComponents<UStaticMeshComponent>(Components);
	UStaticMeshComponent* StaticMeshComponent = Components[0];

	// マテリアルを削除
	StaticMeshComponent->SetOverlayMaterial(nullptr);
}

// パスレンジマテリアルをセット
void AC_Tile::SetPassRangeMaterial()
{
	TArray<UStaticMeshComponent*> Components; // component配列

	// stacic componentを取得
	GetComponents<UStaticMeshComponent>(Components);
	UStaticMeshComponent* StaticMeshComponent = Components[0];

	// マテリアルをセット
	if (passRangeMaterial != nullptr) StaticMeshComponent->SetOverlayMaterial(passRangeMaterial);
}

// マークレンジマテリアルをセット
void AC_Tile::SetMarkRangeMaterial()
{
	TArray<UStaticMeshComponent*> Components; // component配列

	// stacic componentを取得
	GetComponents<UStaticMeshComponent>(Components);
	UStaticMeshComponent* StaticMeshComponent = Components[0];

	// マテリアルをセット
	if (markRangeMaterial != nullptr) StaticMeshComponent->SetOverlayMaterial(markRangeMaterial);
}

