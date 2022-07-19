// Copyright (c) 2022 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.

#include "AccelByteAchievementEntry.h"

#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"
#include "TutorialProject/TutorialProjectUtilities.h"

void UAccelByteAchievementEntry::InitData(const FAccelByteModelsPublicAchievement& PublicAchievementData, const FAccelByteModelsUserAchievement& UserAchievementData)
{
	// WS_AchievementEntry->SetActiveWidgetIndex(0);
	WS_AchievementEntry->SetActiveWidget(CT_Loading);

	// default progress bar visibility	
	PB_AchievementProgress->SetVisibility(ESlateVisibility::Hidden);

	// get Achievement's Status based on query,
	// 0 = locked(empty), 1 = in-progress, 2 = unlocked
	int AchievementStatus = 0;
	if (!UserAchievementData.Id.IsEmpty())
	{
		AchievementStatus = UserAchievementData.Status;
	}

	TArray<FAccelByteModelsAchievementIcon> AchievementIcon = PublicAchievementData.LockedIcons;
	// If Achievement Status is unlocked
	if (AchievementStatus == 2)
	{
		AchievementIcon = PublicAchievementData.UnlockedIcons;
	}

	if (PublicAchievementData.Hidden && AchievementStatus != 2)
	{
		T_AchievementName->SetText(FText::FromString("Hidden Achievement"));
		T_Requirements->SetText(FText::FromString("Keep playing to find out :)"));
	}
	else
	{
		T_AchievementName->SetText(FText::FromString(PublicAchievementData.Name));
		T_Requirements->SetText(FText::FromString(PublicAchievementData.Description));		
	}

	// If Achievement is incremental and have progress
	if (PublicAchievementData.Incremental)
	{
		PB_AchievementProgress->SetVisibility(ESlateVisibility::Visible);
		
		float ProgressPercent = 0;
		// check if Achievement's is in-progress or unlocked
		if (AchievementStatus != 0)
		{
			// count percentage for progress bar then set the percent value
			ProgressPercent = UserAchievementData.LatestValue / PublicAchievementData.GoalValue;
		}
		PB_AchievementProgress->SetPercent(ProgressPercent);
	}

	// Display Achievement Icon
	if (AchievementIcon.Num() > 0)
	{
		const FString ImageId = FBase64::Encode(AchievementIcon[0].Url);
		const FCacheBrush Brush = TutorialProjectUtilities::GetImageFromCache(ImageId);

		if (Brush.IsValid())
		{
			Img_AchievementIcon->SetBrush(*Brush.Get());
			// WS_AchievementEntry->SetActiveWidgetIndex(1);
			WS_AchievementEntry->SetActiveWidget(HB_AchievementItem);
		}
		else
		{
			TutorialProjectUtilities::GetImageFromURL(AchievementIcon[0].Url, ImageId, FOnImageReceived::CreateWeakLambda(this, [this](const FCacheBrush Result)
			{
				Img_AchievementIcon->SetBrush(*Result.Get());
				// WS_AchievementEntry->SetActiveWidgetIndex(1);
				WS_AchievementEntry->SetActiveWidget(HB_AchievementItem);
			}));
		}
	}
	else
	{
		const FCacheBrush DefaultBrush = TutorialProjectUtilities::GetImageFromCache(TutorialProjectUtilities::DefaultImage);
		if (DefaultBrush.IsValid())
		{
			Img_AchievementIcon->SetBrush(*DefaultBrush.Get());
			// WS_AchievementEntry->SetActiveWidgetIndex(1);
			WS_AchievementEntry->SetActiveWidget(HB_AchievementItem);
		}
	}
}
