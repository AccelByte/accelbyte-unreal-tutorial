// Copyright (c) 2022 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.

#include "AccelByteUserProfile.h"

#include "Api/AccelByteUserProfileApi.h"
#include "Components/Button.h"
#include "Components/ComboBoxString.h"
#include "Components/EditableTextBox.h"
#include "Core/AccelByteRegistry.h"
#include "TutorialProject/TutorialMenuHUD.h"

void UAccelByteUserProfile::InitUserProfile()
{
	FRegistry::UserProfile.GetUserProfile(THandler<FAccelByteModelsUserProfileInfo>::CreateWeakLambda(this, [this](const FAccelByteModelsUserProfileInfo& Result)
	{
		UE_LOG(LogTemp, Log, TEXT("GetUserProfile Success. Status: %s"), *Result.Status);

		UserInfo = Result;

		ShowMainMenu();
	}), FErrorHandler::CreateWeakLambda(this, [this](int32 ErrorCode, const FString& ErrorMessage)
	{
		UE_LOG(LogTemp, Error, TEXT("GetUserProfile Error. Code: %d, Message: %s"), ErrorCode, *ErrorMessage);

		ShowCreateProfileWidget();
	}));
}

FAccelByteModelsUserProfileInfo UAccelByteUserProfile::GetUserInfo() const
{
	return UserInfo;
}

void UAccelByteUserProfile::ShowCreateProfileWidget()
{
	if (!IsInViewport())
	{
		Etb_FirstName->SetText(FText::GetEmpty());
		Etb_LastName->SetText(FText::GetEmpty());
		Etb_Year->SetText(FText::GetEmpty());
		Etb_Month->SetText(FText::GetEmpty());
		Etb_Day->SetText(FText::GetEmpty());

		Cbs_Language->ClearOptions();
		for (const FString& Language : Languages)
		{
			Cbs_Language->AddOption(Language);
		}
		Cbs_Language->SetSelectedOption(Cbs_Language->GetOptionAtIndex(0));
		
		Cbs_Timezone->ClearOptions();
		for (const FString& Timezone : Timezones)
		{
			Cbs_Timezone->AddOption(Timezone);
		}
		Cbs_Timezone->SetSelectedOption(Cbs_Timezone->GetOptionAtIndex(0));
		
		AddToViewport();

		Btn_CreateProfile->OnClicked.AddUniqueDynamic(this, &UAccelByteUserProfile::OnCreateProfileButtonClicked);
	}
}

void UAccelByteUserProfile::OnCreateProfileButtonClicked()
{
	FAccelByteModelsUserProfileCreateRequest UserProfileCreateRequest;
	UserProfileCreateRequest.FirstName = Etb_FirstName->GetText().ToString();
	UserProfileCreateRequest.LastName = Etb_LastName->GetText().ToString();
	UserProfileCreateRequest.DateOfBirth = FString::Printf(TEXT("%s-%s-%s"), *Etb_Year->GetText().ToString(),
		*Etb_Month->GetText().ToString(), *Etb_Day->GetText().ToString());
	UserProfileCreateRequest.Language = Cbs_Language->GetSelectedOption();
	UserProfileCreateRequest.Timezone = Cbs_Timezone->GetSelectedOption();
	UserProfileCreateRequest.AvatarSmallUrl = TEXT("https://accelbyte.io");
	UserProfileCreateRequest.AvatarLargeUrl = TEXT("https://accelbyte.io");
	UserProfileCreateRequest.AvatarUrl = TEXT("https://accelbyte.io");
	FRegistry::UserProfile.CreateUserProfile(UserProfileCreateRequest, THandler<FAccelByteModelsUserProfileInfo>::CreateWeakLambda(this, [this](const FAccelByteModelsUserProfileInfo& Result)
	{
		UE_LOG(LogTemp, Log, TEXT("CreateUserProfile Success. Status: %s"), *Result.Status);

		UserInfo = Result;

		ShowMainMenu();
	}), FErrorHandler::CreateWeakLambda(this, [](int32 ErrorCode, const FString& ErrorMessage)
	{
		UE_LOG(LogTemp, Error, TEXT("CreateUserProfile Error. Code: %d, Message: %s"), ErrorCode, *ErrorMessage);
	}));
}

void UAccelByteUserProfile::ShowMainMenu()
{
	ATutorialMenuHUD* TutorialMenuHUD = Cast<ATutorialMenuHUD>(GetOwningPlayer()->GetHUD());

	ensure(TutorialMenuHUD);
	
	TutorialMenuHUD->InitMainMenu();
	RemoveFromParent();
}

