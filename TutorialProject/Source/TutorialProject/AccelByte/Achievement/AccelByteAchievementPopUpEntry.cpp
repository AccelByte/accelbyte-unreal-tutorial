 // Copyright (c) 2022 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.

#include "AccelByteAchievementPopUpEntry.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Core/AccelByteRegistry.h"
#include "TutorialProject/TutorialProjectUtilities.h"

void UAccelByteAchievementPopUpEntry::InitData(const FAchievementInfo& AchievementInfo)
{
	UE_LOG(LogTemp, Warning, TEXT("[Pop Up] Pop Up Notification Update!"));

	// Update Achievement's Name and Description
	T_AchievementName->SetText(FText::FromString(AchievementInfo.AchievementName));
	T_Requirements->SetText(FText::FromString(AchievementInfo.AchievementDescription));
	
	// Set Reward's Text
	if (AchievementInfo.AchievementRewardNames.Num() > 0)
	{
		FString RewardsName;
		for (auto AchievementRewardName : AchievementInfo.AchievementRewardNames)
		{
			RewardsName += AchievementRewardName + ", ";
		}
		// Removing "," and " " of the rewards name
		RewardsName.RemoveAt(RewardsName.Len() -1);
		RewardsName.RemoveAt(RewardsName.Len() -1);

		T_Rewards->SetText(FText::FromString(RewardsName));
	}
	else
	{
		T_Rewards->SetVisibility(ESlateVisibility::Collapsed);
	}
	
	// Display Achievement Icon
	if (AchievementInfo.AchievementUnlockedIcons.Num() > 0)
	{
		const FString& ImageUrl = AchievementInfo.AchievementUnlockedIcons[0].Url;
		if (!ImageUrl.IsEmpty())
		{
			const FString ImageId = FBase64::Encode(ImageUrl);
			const FCacheBrush Brush = TutorialProjectUtilities::GetImageFromCache(ImageId);

			if (Brush.IsValid())
			{
				Img_AchievementIcon->SetBrush(*Brush.Get());
			}
			else
			{
				TutorialProjectUtilities::GetImageFromURL(ImageUrl, ImageId, FOnImageReceived::CreateWeakLambda(this, [this](const FCacheBrush Result)
				{
					Img_AchievementIcon->SetBrush(*Result.Get());
				}));
			}
		}
		else
		{
			const FCacheBrush DefaultBrush = TutorialProjectUtilities::GetImageFromCache(TutorialProjectUtilities::DefaultImage);
			if (DefaultBrush.IsValid())
			{
				Img_AchievementIcon->SetBrush(*DefaultBrush.Get());
			}
		}
	}
}
