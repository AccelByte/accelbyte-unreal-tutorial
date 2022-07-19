// Copyright (c) 2022 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.

#include "AccelByteNotificationObject.h"

void UAccelByteNotificationObject::InitRewardNotification(const FAccelByteModelsNotificationMessage& Result, const FOnInitRewardSuccess& OnInitRewardSuccess, const FOnInitRewardError& OnInitRewardError) const
{
	UE_LOG(LogTemp, Log, TEXT("Message Notification From: %s"), *Result.From);

	TSharedPtr<FJsonObject> JsonObject;
	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Result.Payload);

	if (FJsonSerializer::Deserialize(Reader, JsonObject))
	{
		if (JsonObject->HasField(TutorialProjectUtilities::RewardTopic))
		{
			if (JsonObject->GetStringField(TutorialProjectUtilities::RewardTopic) == TutorialProjectUtilities::AchievementEventTopic)
			{
				FString AchievementCode;
				if (JsonObject->HasField(TutorialProjectUtilities::RewardCondition))
				{
					const FString rewardCondition = JsonObject->GetStringField(TutorialProjectUtilities::RewardCondition);

					FString LeftS, RightS;
					rewardCondition.Split(TutorialProjectUtilities::AchievementCodeSplitString, &LeftS, &RightS);
					// Split the achievement code and get the achievement code from the notification
					RightS = RightS.Replace(TEXT(" == \""), TEXT(""));
					RightS = RightS.Replace(TEXT("\")]"), TEXT(""));

					AchievementCode = RightS;
					
					UE_LOG(LogTemp, Log, TEXT("Achievement Code: %s"), *AchievementCode);
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("rewardCondition is not valid"));
				}

				//Item Id to get entitlement fulfillment
				TArray<FString> RewardItemIds;
				if (JsonObject->HasField(TutorialProjectUtilities::EntitlementSummariesObjectField))
				{
					TArray<TSharedPtr<FJsonValue>> entitlementSummaries = JsonObject->GetArrayField(TutorialProjectUtilities::EntitlementSummariesObjectField);
					for (const TSharedPtr<FJsonValue>& entitlementSummary : entitlementSummaries)
					{
						const FString ItemId = entitlementSummary->AsObject()->GetStringField(TutorialProjectUtilities::ItemIdStringField);
						RewardItemIds.Add(ItemId);
						UE_LOG(LogTemp, Log, TEXT("Reward Item Id: %s"), *ItemId);
					}
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("entitlementSummaries is not valid"));
				}

				//Currency amount and get from the credit fulfillment
				TArray<FString> CurrencyAmounts;
				if (JsonObject->HasField(TutorialProjectUtilities::CreditSummariesObjectField))
				{
					TArray<TSharedPtr<FJsonValue>> creditSummaries = JsonObject->GetArrayField(TutorialProjectUtilities::CreditSummariesObjectField);
					for (const TSharedPtr<FJsonValue>& creditSummary : creditSummaries)
					{
						// get the amount of the currency fulfillment
						const FString CurrencyAmount = creditSummary->AsObject()->GetStringField(TutorialProjectUtilities::AmountStringField);
						CurrencyAmounts.Add(CurrencyAmount);
						UE_LOG(LogTemp, Log, TEXT("Currency Amount: %s"), *CurrencyAmount);
					}
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("entitlementSummaries is not valid"));
				}

				if (!AchievementCode.IsEmpty())
				{
					FRewardNotifInfo RewardNotifInfo;
					RewardNotifInfo.AchievementCode = AchievementCode;
					RewardNotifInfo.RewardItemIds = RewardItemIds;
					RewardNotifInfo.CurrencyAmounts = CurrencyAmounts;
					OnInitRewardSuccess.ExecuteIfBound(RewardNotifInfo);
				}
				else
				{
					const FString ErrorMessage = TEXT("AchievementCode or RewardItemIds is empty");
					UE_LOG(LogTemp, Error, TEXT("Error Message: %s"), *ErrorMessage);

					OnInitRewardError.ExecuteIfBound(ErrorMessage);
				}
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("rewardTopic achievement is not valid"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("rewardTopic is not valid"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Deserialize object is not valid"));
	}
}
