// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "C_Tile.generated.h"

UCLASS()
class FOOTBALL_BASE_API AC_Tile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AC_Tile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	UMaterial* liteMaterial = nullptr; // 光るマテリアル
	int tileNo = 0; // タイルNo

	// オーバーレイマテリアルをセットし、タイルを光らせる
	void SetMaterial();
	// オーバーレイマテリアルを削除する
	void RemoveMaterial();
};
