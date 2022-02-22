// Copyright (c) 2021 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "TutGameModeMainMenu.generated.h"

class ATutorialMenuHUD;

/**
 * This game mode is to handle leave party when ever the game is destroyed
 */
UCLASS()
class TUTORIALPROJECT_API ATutGameModeMainMenu : public AGameMode
{
	GENERATED_BODY()

protected:

	class ATutorialMenuHUD* TutorialMenuHUD;

	ATutGameModeMainMenu();
};
