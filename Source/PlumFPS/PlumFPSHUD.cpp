// Copyright Epic Games, Inc. All Rights Reserved.

#include "PlumFPSHUD.h"
#include "Engine/Canvas.h"
#include "Engine/Texture2D.h"
#include "TextureResource.h"
#include "CanvasItem.h"
#include "UObject/ConstructorHelpers.h"

APlumFPSHUD::APlumFPSHUD()
{
	// Set the crosshair texture
	static ConstructorHelpers::FObjectFinder<UTexture2D> CrosshairTexObj(TEXT("/Game/FirstPerson/Textures/FirstPersonCrosshair"));
	CrosshairTex = CrosshairTexObj.Object;
	position = 8.0f;
}

void APlumFPSHUD::setNormal()
{
	isAim = false;
}

void APlumFPSHUD::setAds()
{
	isAim = true;
}

void APlumFPSHUD::DrawHUD()
{
	Super::DrawHUD();

	if (isAim == true)
	{
		position = 256.0f;
		UTexture2D* NewTexObj = Cast<UTexture2D>
			(StaticLoadObject(UTexture2D::StaticClass(), NULL, TEXT("/Game/FirstPerson/Textures/aiming")));
		if (NewTexObj)
		{
			CrosshairTex = NewTexObj;
		}
	}
	else
	{
		position = 8.0f;
		UTexture2D* NewTexObj = Cast<UTexture2D>
			(StaticLoadObject(UTexture2D::StaticClass(), NULL, TEXT("/Game/FirstPerson/Textures/FirstPersonCrosshair")));
		if (NewTexObj)
		{
			CrosshairTex = NewTexObj;
		}
	}

	// find center of the Canvas
	const FVector2D Center(Canvas->ClipX * 0.5f, Canvas->ClipY * 0.5f);

	// offset by half the texture's dimensions so that the center of the texture aligns with the center of the Canvas
	const FVector2D CrosshairDrawPosition((Center.X - position), (Center.Y - position));

	// draw the crosshair
	FCanvasTileItem TileItem(CrosshairDrawPosition, CrosshairTex->Resource, FLinearColor::White);
	TileItem.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem(TileItem);
}
