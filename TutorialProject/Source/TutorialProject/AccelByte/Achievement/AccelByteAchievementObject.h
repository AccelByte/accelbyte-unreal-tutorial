// Copyright (c) 2022 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.

#pragma once

#include "CoreMinimal.h"
#include "Core/AccelByteError.h"
#include "Models/AccelByteAchievementModels.h"
#include "UObject/Object.h"
#include "AccelByteAchievementObject.generated.h"

DECLARE_DELEGATE_OneParam(FGetPlayerAchievementsData, const TArray<FAccelByteModelsUserAchievement>& PlayerAchievementData);
DECLARE_DELEGATE_OneParam(FGetPublicAchievementsData, const TArray<FAccelByteModelsPublicAchievement>& PublicAchievementData);

/**
 * UObject class to handle Achievement services, used in different levels.
 * This class provide these functionality:
 * 
 * - Unlock Achievement from Client-side
 */
UCLASS()
class TUTORIALPROJECT_API UAccelByteAchievementObject : public UObject
{
	GENERATED_BODY()

public:
	
	/**
	 * @brief Unlock Specific Achievement
	 * @param AchievementCode achievement code of the specific achievement that will be unlocked
	 * @param OnSuccess 
	 */
	void UnlockAchievement(const FString& AchievementCode, const AccelByte::FVoidHandler& OnSuccess);

	/**
	 * @brief Get Player's Achievements data
	 * @param SortBy sort method for the query result
	 * @param Offset offset value of the query result
	 * @param Limit limit value of the query result
	 * @param PreferUnlocked bool value of list order method with Unlocked Achievements as priority
	 * @param OnSuccess Delegate on the Player Achievements query success
	 */
	void QueryUserAchievements(const EAccelByteAchievementListSortBy& SortBy, const int32& Offset, const int32& Limit, bool bPreferUnlocked, const FGetPlayerAchievementsData& OnSuccess);

	/**
	 * @brief Get All Achievements basic data
	 * @param Language default language setting
	 * @param SortBy sort method for the query result
	 * @param Offset offset value of the query result
	 * @param Limit limit value of the query result
	 * @param OnSuccess Delegate on the Public Achievements query success
	 */
	void QueryAllAchievements(const FString& Language, const EAccelByteAchievementListSortBy& SortBy, const int32& Offset, const int32& Limit, const FGetPublicAchievementsData& OnSuccess);
};
