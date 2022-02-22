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
	
	if (FParse::Param(FCommandLine::Get(), *TutorialProjectUtilities::LaunchArgsSteam))
	{
		Sb_Logout->SetVisibility(ESlateVisibility::Collapsed);
	}	
	else
	{
		Btn_Logout->OnClicked.AddUniqueDynamic(this, &UAccelByteMainMenu::LogOut);
	}

	FRegistry::UserProfile.GetUserProfile(
		THandler<FAccelByteModelsUserProfileInfo>::CreateUObject(this, &UAccelByteMainMenu::OnSuccessUserProfile),
		FErrorHandler::CreateUObject(this, &UAccelByteMainMenu::OnFailedUserProfile));
}

void UAccelByteMainMenu::LogOut()
{
	TutorialMenuHUD->GetLoginMenu()->OnClickLogoutButton();
}

void UAccelByteMainMenu::OnSuccessUserProfile(const FAccelByteModelsUserProfileInfo& Data)
{
	TutorialProjectUtilities::ShowLog(ELogVerbosity::Log, FString::Printf(TEXT("Success Get User Profile!")));
	
	Tb_PlayerName->SetText(FText::FromString(Data.FirstName));
}

void UAccelByteMainMenu::OnFailedUserProfile(int32 ErrorCode, const FString& ErrorMessage)
{
	TutorialProjectUtilities::ShowLog(ELogVerbosity::Error, FString::Printf(TEXT("Failed Get User Profile : %d , %s"), ErrorCode, *ErrorMessage));
}
