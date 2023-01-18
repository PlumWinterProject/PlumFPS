// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once 

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "PlumFPSHUD.generated.h"

UCLASS()
class APlumFPSHUD : public AHUD
{
	GENERATED_BODY()

public:
	APlumFPSHUD();

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;
	void setNormal();
	void setAds();

private:
	/** Crosshair asset pointer */
	class UTexture2D* CrosshairTex;
	float position;
	bool isAim;
};

