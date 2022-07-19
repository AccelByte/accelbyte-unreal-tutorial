// Copyright (c) 2022 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.

#pragma once

#include "CoreMinimal.h"
#include "AccelByteAchievementPopUp.h"
#include "Blueprint/UserWidget.h"
#include "Core/AccelByteError.h"
#include "AccelByteAchievementPopUpEntry.generated.h"

class ATutorialMenuHUD;
class UImage;
class UTextBlock;

/**
 * Achievement Pop Up Notification Entry widget that hold the specific unlocked Achievement data
 * This class provide these functionality:
 * 
 * - Initialize Achievement Pop Up Entry data
 */
UCLASS()
class TUTORIALPROJECT_API UAccelByteAchievementPopUpEntry : public UUserWidget
{
	GENERATED_BODY()

#pragma region UI Binding

	/**
	 * @brief Image for Achievement's Icon Image
	 */
	UPROPERTY(meta = (BindWidget))
	UImage* Img_AchievementIcon;

	/**
	 * @brief Text for Achievement's Name
	 */
	UPROPERTY(meta = (BindWidget))
	UTextBlock* T_AchievementName;

	/**
	 * @brief Text for Achievement's Description
	 */
	UPROPERTY(meta = (BindWidget))
	UTextBlock* T_Requirements;

	/**
	 * @brief Text for Achievement's Rewards description
	 */
	UPROPERTY(meta = (BindWidget))
	UTextBlock* T_Rewards;
	
#pragma endregion

public:
	
	/**
	 * @brief Initialize Achievement's Pop Up Entry data
	 * @param AchievementInfo Achievement detail information
	 */
	void InitData(const FAchievementInfo& AchievementInfo);
};
