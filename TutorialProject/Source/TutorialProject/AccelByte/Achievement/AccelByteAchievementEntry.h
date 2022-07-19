// Copyright (c) 2022 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Models/AccelByteAchievementModels.h"
#include "AccelByteAchievementEntry.generated.h"

class UWidgetSwitcher;
class UWidget;
class UImage;
class UTextBlock;
class UProgressBar;

/**
 * Achievement Entry Widget. This will hold each Achievement individual data.
 * This class provide these functionality:
 * 
 * - Initialize Achievement Entry data
 */
UCLASS()
class TUTORIALPROJECT_API UAccelByteAchievementEntry : public UUserWidget
{
	GENERATED_BODY()
	
#pragma region UI Binding

	/**
	 * @brief Widget Switcher for Achievement Entry
	 */
	UPROPERTY(meta = (BindWidget))
	UWidgetSwitcher* WS_AchievementEntry;

	/**
	 * @brief Loading widget for Achievement Entry
	 */
	UPROPERTY(meta = (BindWidget))
	UWidget* CT_Loading;

	/**
	 * @brief Content widget for Achievement Entry
	 */
	UPROPERTY(meta = (BindWidget))
	UWidget* HB_AchievementItem;
	
	/**
	 * @brief Image for Achievement's Icon image
	 */
	UPROPERTY(meta = (BindWidget))
	UImage* Img_AchievementIcon;

	/**
	 * @brief Text for Achievement's Name
	 */
	UPROPERTY(meta = (BindWidget))
	UTextBlock* T_AchievementName;

	/**
	 * @brief Text for Achievement's description
	 */
	UPROPERTY(meta = (BindWidget))
	UTextBlock* T_Requirements;

	/**
	 * @brief Progress Bar for Achievement's current progress
	 */
	UPROPERTY(meta = (BindWidget))
	UProgressBar* PB_AchievementProgress;
	
#pragma endregion

public:
	
	/**
	 * @brief Initialize Achievement Entry data based on Player's Achievements and Basic Achievements data
	 * @param PublicAchievementData Basic Achievements data that registered in Admin Portal
	 * @param UserAchievementData Player's Achievements data
	 */
	void InitData(const FAccelByteModelsPublicAchievement& PublicAchievementData, const FAccelByteModelsUserAchievement& UserAchievementData);
};
