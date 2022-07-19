// Copyright (c) 2022 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Models/AccelByteUserProfileModels.h"
#include "AccelByteUserProfile.generated.h"

class UButton;
class UComboBoxString;
class UEditableTextBox;

/**
 * Widget to handle user profile services
 */
UCLASS()
class TUTORIALPROJECT_API UAccelByteUserProfile : public UUserWidget
{
	GENERATED_BODY()

	TArray<FString> Languages = {
		"en-US",
		"id",
		"de",
		"en",
		"es",
		"fr",
		"it",
		"pl",
		"pt",
		"tr"
	};

	TArray<FString> Timezones = {
		"Asia/Jakarta",
		"Asia/Anadyr",
		"Asia/Aqtau",
		"Asia/Aqtobe",
		"Asia/Ashgabat",
		"Asia/Ashkhabad",
		"Asia/Istanbul",
		"Asia/Jayapura",
		"Asia/Jerusalem",
		"Asia/Kabul",
		"Asia/Kamchatka",
		"Asia/Karachi",
		"Asia/Kashgar",
		"Asia/Thimbu",
		"Asia/Thimphu",
		"Asia/Tokyo"};

protected:

	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* Etb_FirstName;

	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* Etb_LastName;

	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* Etb_Year;

	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* Etb_Month;
	
	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* Etb_Day;

	UPROPERTY(meta = (BindWidget))
	UComboBoxString* Cbs_Language;

	UPROPERTY(meta = (BindWidget))
	UComboBoxString* Cbs_Timezone;

	UPROPERTY(meta = (BindWidget))
	UButton* Btn_CreateProfile;

	FAccelByteModelsUserProfileInfo UserInfo;

	void ShowCreateProfileWidget();

	UFUNCTION()
	void OnCreateProfileButtonClicked();

	void ShowMainMenu();

public:

	void InitUserProfile();

	FAccelByteModelsUserProfileInfo GetUserInfo() const;
};
