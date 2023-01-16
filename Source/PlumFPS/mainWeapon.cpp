// Fill out your copyright notice in the Description page of Project Settings.


#include "mainWeapon.h"

// Sets default values
AmainWeapon::AmainWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;

	magazine = 30;
	remainAmmo = 120;
	coefYawRecoil = 1000;
	coefPitchRecoil = 500;
	zoomScale = 0.5f;
	fireRate = 0.2f;
	bulletSpread = 2000;
	damage = 40;
	canFullAutoFire = true;
}

// Called when the game starts or when spawned
void AmainWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AmainWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

