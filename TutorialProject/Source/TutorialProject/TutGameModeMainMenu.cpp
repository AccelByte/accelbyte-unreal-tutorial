// Copyright (c) 2021 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.

#include "TutGameModeMainMenu.h"
#include "TutorialMenuHUD.h"
#include "Core/AccelByteRegistry.h"

ATutGameModeMainMenu::ATutGameModeMainMenu()
{
	HUDClass = ATutorialMenuHUD::StaticClass();
}