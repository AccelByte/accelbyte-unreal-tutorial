// Copyright (c) 2022 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.

#pragma once

#include "CoreMinimal.h"
#include "Models/AccelByteLobbyModels.h"
#include "UObject/Object.h"
#include "AccelByteNotificationObject.generated.h"

/**
 * @brief All Reward Notification Info
 */
USTRUCT(BlueprintType)
struct FRewardNotifInfo
{
	GENERATED_BODY()

	FString AchievementCode;
	TArray<FString> RewardItemIds;
	TArray<FString> CurrencyAmounts;
};

DECLARE_DELEGATE_OneParam(FOnInitRewardSuccess, const FRewardNotifInfo&);
DECLARE_DELEGATE_OneParam(FOnInitRewardError, const FString&);

/**
 * Reward Notification Object to handle Pop Up Notification
 */
UCLASS()
class TUTORIALPROJECT_API UAccelByteNotificationObject : public UObject
{
	GENERATED_BODY()

public:

	/**
	 * @brief Initiate and get all Reward Information
	 * @param Result The Payload that got from the notification
	 * @param OnInitRewardSuccess Delegate when Initiate Reward is Success
	 * @param OnInitRewardError Delegate when Initiate Reward is Error
	 */
	void InitRewardNotification(const FAccelByteModelsNotificationMessage& Result, const FOnInitRewardSuccess& OnInitRewardSuccess, const FOnInitRewardError& OnInitRewardError) const;
};
