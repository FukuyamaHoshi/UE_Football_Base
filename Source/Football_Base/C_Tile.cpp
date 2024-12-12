// Fill out your copyright notice in the Description page of Project Settings.


#include "C_Tile.h"

// Sets default values
AC_Tile::AC_Tile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

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

void AC_Tile::SetMaterial()
{
	TArray<UStaticMeshComponent*> Components; // component配列

	// stacic componentを取得
	GetComponents<UStaticMeshComponent>(Components);
	UStaticMeshComponent* StaticMeshComponent = Components[0];

	// マテリアルをセット
	if (liteMaterial != nullptr) StaticMeshComponent->SetOverlayMaterial(liteMaterial);
}

void AC_Tile::RemoveMaterial()
{
	TArray<UStaticMeshComponent*> Components; // component配列

	// stacic componentを取得
	GetComponents<UStaticMeshComponent>(Components);
	UStaticMeshComponent* StaticMeshComponent = Components[0];

	// マテリアルを削除
	StaticMeshComponent->SetOverlayMaterial(nullptr);
}

