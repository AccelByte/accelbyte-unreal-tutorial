// Copyright (c) 2022 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.

#pragma once

#include "CoreMinimal.h"
#include "Models/AccelByteAchievementModels.h"
#include "Blueprint/UserWidget.h"
#include "AccelByteAchievementPopUp.generated.h"

/**
 * @brief All Achievement Information
 */
USTRUCT(BlueprintType)
struct FAchievementInfo
{
	GENERATED_BODY()

	FString AchievementName;
	FString AchievementDescription;
	TArray<FAccelByteModelsAchievementIcon> AchievementUnlockedIcons;
	TArray<FString> AchievementRewardNames;
};

class UVerticalBox;
class UAccelByteAchievementPopUpEntry;

/**
 * Widget Class to display Achievements Pop Up Notification.
 * This class provide these functionality:
 * 
 * - Initialize Achievement Pop Up Notification
 */
UCLASS()
class TUTORIALPROJECT_API UAccelByteAchievementPopUp : public UUserWidget
{
	GENERATED_BODY()

protected:
	
	/**
	 * @brief Achievement Pop Up Entry Widget Class
	 */
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UAccelByteAchievementPopUpEntry> AchievementPopUpEntryClass;

	/**
	 * @brief Virtual Box for Achievement's Pop Up Notification Box
	 */
	UPROPERTY(meta = (BindWidget))
	UVerticalBox* VB_AchievementEntry;

	/**
	 * @brief 
	 */
	struct CacheRewardInfo
	{
		FAchievementInfo AchievementInfo;
		int32 TotalCount = 0;
	};
	
	/**
	 * @brief Cache all reward items into the map
	 */
	TMap<FString, CacheRewardInfo> CacheRewardItems;
	
	/**
	 * @brief Initiate the pop up achievement notification
	 * @param AchievementInfo Achievment Information that gonna shown to pop up notification
	 */
	void InitPopUpAchievementEntry(const FAchievementInfo& AchievementInfo);

	/**
	 * @brief Caching the reward items and push to the achievement information
	 * @param TempCacheId Temporary Cache Id for the unique map id
	 * @param RewardName The Reward Name that the data is gonna push to the achievement information
	 */
	void CacheRewardItem(const FString& TempCacheId, const FString& RewardName);

public:

	/**
	 * @brief Initialize Achievements' Pop Up Notification widget
	 * @param AchievementCode Achievement Code of the unlocked Achievement
	 * @param RewardItemIds All Achievement Reward Item Ids
	 * @param CurrencyAmounts All Currency Reward
	 */
	void InitPopUpAchievement(const FString& AchievementCode, const TArray<FString>& RewardItemIds = {}, const TArray<FString>& CurrencyAmounts = {});
};
