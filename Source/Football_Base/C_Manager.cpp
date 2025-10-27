// Fill out your copyright notice in the Description page of Project Settings.


#include "C_Manager.h"

// Sets default values
AC_Manager::AC_Manager()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AC_Manager::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AC_Manager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AC_Manager::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

