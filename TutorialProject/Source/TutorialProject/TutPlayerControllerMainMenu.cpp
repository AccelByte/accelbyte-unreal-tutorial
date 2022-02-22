// Copyright (c) 2022 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.

#include "TutPlayerControllerMainMenu.h"
#include "TutorialMenuHUD.h"
#include "Core/AccelByteRegistry.h"

void ATutPlayerControllerMainMenu::BeginPlay()
{
	Super::BeginPlay();

	this->bShowMouseCursor = true;
}

void ATutPlayerControllerMainMenu::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction(FName("Screenshot"), IE_Pressed, this, &ATutPlayerControllerMainMenu::OnScreenshotActionPressed);
}

void ATutPlayerControllerMainMenu::OnScreenshotActionPressed()
{
	Cast<ATutorialMenuHUD>(this->GetHUD())->GetGalleryMenu()->TakeScreenshot();
}
