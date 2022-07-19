// Copyright (c) 2022 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TutPlayerControllerMainMenu.generated.h"

/**
 * 
 */
UCLASS()
class TUTORIALPROJECT_API ATutPlayerControllerMainMenu : public APlayerController
{
	GENERATED_BODY()

protected:

	virtual void BeginPlay() override;
	
	virtual void SetupInputComponent() override;

	void OnScreenshotActionPressed();
};
