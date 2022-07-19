// Copyright (c) 2022 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.

#include "AccelByteAchievement.h"
#include "AccelByteAchievementEntry.h"

#include "Api/AccelByteAchievementApi.h"
#include "Components/Button.h"
#include "Components/GridPanel.h"
#include "Components/WidgetSwitcher.h"
#include "Core/AccelByteRegistry.h"
#include "Datasmith/DirectLink/Public/DirectLinkCommon.h"
#include "TutorialProject/TutorialMenuHUD.h"

void UAccelByteAchievement::NativeConstruct()
{
	Super::NativeConstruct();

	TutorialMenuHUD = Cast<ATutorialMenuHUD>(GetOwningPlayer()->GetHUD());
	check(TutorialMenuHUD);

	// create Achievement object
	if (!AccelByteAchievementObject)
	{
		AccelByteAchievementObject = NewObject<UAccelByteAchievementObject>();
	}
	check(AccelByteAchievementObject);

	// setup button
	Btn_BackToMainMenu->OnClicked.AddUniqueDynamic(TutorialMenuHUD, &ATutorialMenuHUD::CloseAchievementMenu);

	DisplayAchievement();
}

void UAccelByteAchievement::DisplayAchievement()
{
	// Refresh widgets state
	// WS_AchievementMenu->SetActiveWidgetIndex(0);
	WS_AchievementMenu->SetActiveWidget(CT_Loading);
	GP_Achievements->ClearChildren();

	// Local variable for Query Parameters' value
	const EAccelByteAchievementListSortBy SortBy = EAccelByteAchievementListSortBy::UPDATED_AT_ASC;
	const uint8 Offset = 0;
	const uint8 Limit = 99;
	const bool bPreferUnlocked = true;
	
	// QueryUserAchievements(SortBy, Offset, Limit, PreferUnlocked);
	AccelByteAchievementObject->QueryUserAchievements(
		SortBy,
		Offset,
		Limit,
		bPreferUnlocked,
		FGetPlayerAchievementsData::CreateWeakLambda(this, 
			[this, SortBy, Offset, Limit](const TArray<FAccelByteModelsUserAchievement>& PlayerAchievements)
			{
				const FString Language = "en";
				AccelByteAchievementObject->QueryAllAchievements(
					Language,
					SortBy,
					Offset,
					Limit,
					FGetPublicAchievementsData::CreateWeakLambda(this, 
						[this, PlayerAchievements](const TArray<FAccelByteModelsPublicAchievement>& PublicAchievements)
						{
							InitAchievementEntry(PlayerAchievements, PublicAchievements);
						}));
			}));
}

void UAccelByteAchievement::InitAchievementEntry(const TArray<FAccelByteModelsUserAchievement>& PlayerAchievements, const TArray<FAccelByteModelsPublicAchievement>& PublicAchievements)
{
	int32 RowCount = 0;
	int32 ColumnCount = 0;

	for (const FAccelByteModelsPublicAchievement& PublicAchievementData : PublicAchievements)
	{
		UAccelByteAchievementEntry* AchievementEntry = CreateWidget<UAccelByteAchievementEntry>(this, AchievementEntryClass);

		GP_Achievements->AddChildToGrid(AchievementEntry, RowCount, ColumnCount);

		if (ColumnCount == MaximumGridColumn - 1)
		{
			RowCount++;
			ColumnCount = 0;
		}
		else
		{
			ColumnCount++;
		}

		// Prepare Current Achievement Data
		FAccelByteModelsUserAchievement UserAchievementData;
		// check User Achievements size to assign to find the current UserAchievement data
		for (const FAccelByteModelsUserAchievement& Data: PlayerAchievements)
		{
			if (Data.AchievementCode == PublicAchievementData.AchievementCode)
			{
				UserAchievementData = Data;
				break;
			}
		}
					
		AchievementEntry->InitData(PublicAchievementData, UserAchievementData);
	}

	// WS_AchievementMenu->SetActiveWidgetIndex(1);
	WS_AchievementMenu->SetActiveWidget(ScB_AchievementsList);
}