// Copyright (c) 2022 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.

#include "AccelByteAchievementPopUp.h"

#include "AccelByteAchievementPopUpEntry.h"
#include "Api/AccelByteAchievementApi.h"
#include "Api/AccelByteItemApi.h"
#include "Components/VerticalBox.h"
#include "Core/AccelByteRegistry.h"

#define DEFAULT_LANGUAGE "en"

void UAccelByteAchievementPopUp::InitPopUpAchievementEntry(const FAchievementInfo& AchievementInfo)
{
	UAccelByteAchievementPopUpEntry* AccelByteAchievementPopUpEntry = CreateWidget<UAccelByteAchievementPopUpEntry>(this, AchievementPopUpEntryClass.Get());
	check(AccelByteAchievementPopUpEntry);

	AccelByteAchievementPopUpEntry->InitData(AchievementInfo);
	
	AccelByteAchievementPopUpEntry->SetPadding(FMargin(FVector2D(10.f, 10.f)));
	VB_AchievementEntry->AddChild(AccelByteAchievementPopUpEntry);

	// Remove the pop up after 3 seconds
	const TSharedPtr<FTimerHandle> Timer = MakeShared<FTimerHandle>();
	if (Timer)
	{
		GetWorld()->GetTimerManager().SetTimer(*Timer, FTimerDelegate::CreateWeakLambda(this, [AccelByteAchievementPopUpEntry]()
		{
			AccelByteAchievementPopUpEntry->RemoveFromParent();
		}), 3.0f, false);
	}
}

void UAccelByteAchievementPopUp::InitPopUpAchievement(const FString& AchievementCode, const TArray<FString>& RewardItemIds, const TArray<FString>& CurrencyAmounts)
{
	if (!IsInViewport()) AddToViewport();
	
	FRegistry::Achievement.GetAchievement(
		AchievementCode,
		THandler<FAccelByteModelsMultiLanguageAchievement>::CreateWeakLambda(this, 
			[this, RewardItemIds, CurrencyAmounts](const FAccelByteModelsMultiLanguageAchievement& Result)
			{
				FAchievementInfo AchievementInfo;
				AchievementInfo.AchievementName = Result.Name[DEFAULT_LANGUAGE];
				AchievementInfo.AchievementDescription = Result.Description[DEFAULT_LANGUAGE];
				AchievementInfo.AchievementUnlockedIcons = Result.UnlockedIcons;

				const FString& TempCacheId = FGuid().NewGuid().ToString();
				CacheRewardItems.Emplace(TempCacheId);
				CacheRewardItems[TempCacheId].AchievementInfo = AchievementInfo;
				CacheRewardItems[TempCacheId].TotalCount = RewardItemIds.Num() + CurrencyAmounts.Num();

				if (CacheRewardItems[TempCacheId].TotalCount > 0)
				{
					if (CurrencyAmounts.Num() > 0)
					{
						for (auto CurrencyAmount : CurrencyAmounts)
						{
							CacheRewardItem(TempCacheId, FString::Printf(TEXT("Coin %s"), *CurrencyAmount));
						}
					}
				
					if (RewardItemIds.Num() > 0)
					{
						UE_LOG(LogTemp, Warning, TEXT("Temp Cache Id: %s, Reward Item Ids: %d"), *TempCacheId, RewardItemIds.Num());
						for (auto RewardItemId : RewardItemIds)
						{
							FRegistry::Item.GetItemById(RewardItemId, "en", "US", THandler<FAccelByteModelsPopulatedItemInfo>::CreateWeakLambda(this, [this, TempCacheId](const FAccelByteModelsPopulatedItemInfo& Result)
							{
								CacheRewardItem(TempCacheId, Result.Name);
							}), FErrorHandler::CreateWeakLambda(this, [](int32 ErrorCode, const FString& ErrorMessage)
							{
								UE_LOG(LogTemp, Error, TEXT("GetItemById Error, Code: %d Message: %s"), ErrorCode, *ErrorMessage);
							}));
						}
					}
				}
				else
				{
					InitPopUpAchievementEntry(AchievementInfo);
				}
			}),
		FErrorHandler::CreateWeakLambda(this, 
			[](int32 ErrorCode, const FString& ErrorMessage)
			{
				UE_LOG(LogTemp, Error, TEXT("GetAchievement Error, Code: %d Message: %s"), ErrorCode, *ErrorMessage);
			}));
}

void UAccelByteAchievementPopUp::CacheRewardItem(const FString& TempCacheId, const FString& RewardName)
{
	UE_LOG(LogTemp, Warning, TEXT("Temp Cache Reward Id: %s"), *TempCacheId);
	
	CacheRewardItems[TempCacheId].AchievementInfo.AchievementRewardNames.Push(RewardName);
	UE_LOG(LogTemp, Warning, TEXT("Total Reward Cache: %d, Total Reward Count: %d"), CacheRewardItems[TempCacheId].AchievementInfo.AchievementRewardNames.Num(), CacheRewardItems[TempCacheId].TotalCount);
	if (CacheRewardItems[TempCacheId].AchievementInfo.AchievementRewardNames.Num() == CacheRewardItems[TempCacheId].TotalCount)
	{
		InitPopUpAchievementEntry(CacheRewardItems[TempCacheId].AchievementInfo);
		CacheRewardItems.Remove(TempCacheId);
	}
}

#undef DEFAULT_LANGUAGE