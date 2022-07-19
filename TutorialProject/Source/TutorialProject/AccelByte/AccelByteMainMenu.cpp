// Copyright (c) 2021 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.

#include "AccelByteMainMenu.h"
#include "Authentication/AccelByteAuth.h"
#include "Api/AccelByteUserProfileApi.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "TutorialProject/TutorialMenuHUD.h"
#include "TutorialProject/TutorialProjectGameInstance.h"
#include "TutorialProject/TutorialProjectUtilities.h"
#include "UserProfile/AccelByteUserProfile.h"

void UAccelByteMainMenu::NativeConstruct()
{
	Super::NativeConstruct();
	
	TutorialMenuHUD = Cast<ATutorialMenuHUD>(GetOwningPlayer()->GetHUD());
	
	check(TutorialMenuHUD);
	
	Btn_Lobby->OnClicked.AddUniqueDynamic(TutorialMenuHUD, &ATutorialMenuHUD::OpenLobbyMenu);
	Btn_Store->OnClicked.AddUniqueDynamic(TutorialMenuHUD, &ATutorialMenuHUD::OpenStoreMenu);
	Btn_Inventory->OnClicked.AddUniqueDynamic(TutorialMenuHUD, &ATutorialMenuHUD::OpenInventoryMenu);
	Btn_Gallery->OnClicked.AddUniqueDynamic(TutorialMenuHUD, &ATutorialMenuHUD::OpenGalleryMenu);
	Btn_Settings->OnClicked.AddUniqueDynamic(TutorialMenuHUD, &ATutorialMenuHUD::OpenSettingsMenu);
	Btn_Leaderboard->OnClicked.AddUniqueDynamic(TutorialMenuHUD, &ATutorialMenuHUD::OpenLeaderboardMenu);
	Btn_Achievement->OnClicked.AddUniqueDynamic(TutorialMenuHUD, &ATutorialMenuHUD::OpenAchievementMenu);
	
	if (FParse::Param(FCommandLine::Get(), *TutorialProjectUtilities::LaunchArgsSteam))
	{
		Sb_Logout->SetVisibility(ESlateVisibility::Collapsed);
	}	
	else
	{
		Btn_Logout->OnClicked.AddUniqueDynamic(this, &UAccelByteMainMenu::LogOut);
	}

	const FString FirstName = TutorialMenuHUD->GetUserProfileMenu()->GetUserInfo().FirstName;
	Tb_PlayerName->SetText(FText::FromString(FirstName));
}

void UAccelByteMainMenu::LogOut()
{
	TutorialMenuHUD->GetLoginMenu()->OnLogoutButtonClicked();
}
