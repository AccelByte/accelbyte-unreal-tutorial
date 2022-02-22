// Copyright (c) 2022 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.

#include "AccelByteSettingsMenu.h"
#include "Api/AccelByteCloudSaveApi.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/ComboBoxString.h"
#include "Components/SizeBox.h"
#include "Components/WidgetSwitcher.h"
#include "Components/CircularThrobber.h"
#include "Components/VerticalBox.h"
#include "Core/AccelByteRegistry.h"
#include "GameFramework/GameUserSettings.h"
#include "TutorialProject/TutorialMenuHUD.h"
#include "GenericPlatform/GenericWindow.h"
#include "TutorialProject/TutorialProjectUtilities.h"

void UAccelByteSettingsMenu::NativeConstruct()
{
	Super::NativeConstruct();

	Cbs_DisplayMode->SetSelectedIndex(LatestGameSettingsState.DisplayMode);
	Cbs_CloudSaveState->SetSelectedIndex(LatestGameSettingsState.CloudSaveState);

	Cbs_DisplayMode->OnSelectionChanged.AddUniqueDynamic(this, &UAccelByteSettingsMenu::OnDisplayModeChanged);
	Cbs_CloudSaveState->OnSelectionChanged.AddUniqueDynamic(this, &UAccelByteSettingsMenu::OnCloudSaveStateChanged);

	Btn_SaveChanges->OnClicked.AddUniqueDynamic(this, &UAccelByteSettingsMenu::OnSaveChangesButtonClicked);
	Btn_BackToMainMenu->OnClicked.AddUniqueDynamic(this, &UAccelByteSettingsMenu::OnBackToMainMenuButtonClicked);
	Btn_ProceedToMainMenu->OnClicked.AddUniqueDynamic(this, &UAccelByteSettingsMenu::OnProceedToMainMenuButtonClicked);
	Btn_KeepEditing->OnClicked.AddUniqueDynamic(this, &UAccelByteSettingsMenu::OnKeepEditingButtonClicked);
	Btn_Back->OnClicked.AddUniqueDynamic(this, &UAccelByteSettingsMenu::OnBackButtonClicked);
}

void UAccelByteSettingsMenu::OnDisplayModeChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	if (SelectionType == ESelectInfo::Type::OnMouseClick)
	{
		CurrentGameSettingsState.DisplayMode = Cbs_DisplayMode->GetSelectedIndex();
	}
}

void UAccelByteSettingsMenu::OnCloudSaveStateChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	if (SelectionType == ESelectInfo::Type::OnMouseClick)
	{
		CurrentGameSettingsState.CloudSaveState = Cbs_CloudSaveState->GetSelectedIndex();
	}
}

void UAccelByteSettingsMenu::OnSaveChangesButtonClicked()
{
	ShowPopUp(EPopUpType::Loading);
		
	FJsonObject RecordRequest;
	RecordRequest.SetNumberField(TutorialProjectUtilities::CloudSaveJsonKeyA, CurrentGameSettingsState.DisplayMode);
	RecordRequest.SetNumberField(TutorialProjectUtilities::CloudSaveJsonKeyB, CurrentGameSettingsState.CloudSaveState);
	
	FRegistry::CloudSave.SaveUserRecord(TutorialProjectUtilities::GameSettingCloudSaveKey, RecordRequest, false, FVoidHandler::CreateLambda([this]()
	{
		TutorialProjectUtilities::ShowLog(ELogVerbosity::Log, TEXT("Success Save User Record"));

		LatestGameSettingsState.DisplayMode = CurrentGameSettingsState.DisplayMode;
		LatestGameSettingsState.CloudSaveState = CurrentGameSettingsState.CloudSaveState;

		InitDisplaySettings(LatestGameSettingsState.DisplayMode);
		
		ShowPopUp(EPopUpType::Saved);
	}),
	FErrorHandler::CreateLambda([this](int32 ErrorCode, const FString& ErrorMessage)
	{
		TutorialProjectUtilities::ShowLog(ELogVerbosity::Error, FString::Printf(TEXT("Save User Record Failed, Error Code: %d Error Message: %s"), ErrorCode, *ErrorMessage));
		
		ShowPopUp(EPopUpType::None);
	}));
}

void UAccelByteSettingsMenu::OnBackButtonClicked()
{
	ShowPopUp(EPopUpType::None);
}

void UAccelByteSettingsMenu::OnBackToMainMenuButtonClicked()
{
	if (CurrentGameSettingsState.DisplayMode != LatestGameSettingsState.DisplayMode || CurrentGameSettingsState.CloudSaveState != LatestGameSettingsState.CloudSaveState)
	{
		ShowPopUp(EPopUpType::Unsaved);
	}
	else
	{
		ShowPopUp(EPopUpType::None);
		TutorialMenuHUD->OpenMainMenu();
		this->RemoveFromParent();
	}
}

void UAccelByteSettingsMenu::OnProceedToMainMenuButtonClicked()
{
	CurrentGameSettingsState.DisplayMode = LatestGameSettingsState.DisplayMode;
	CurrentGameSettingsState.CloudSaveState = LatestGameSettingsState.CloudSaveState;
	
	B_SaveState->SetVisibility(ESlateVisibility::Collapsed);
	WS_SaveState->SetActiveWidget(Cast<UWidget>(CT_LoadingThrobber));
	Sb_GameSettingsContainer->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	
	TutorialMenuHUD->OpenMainMenu();
	this->RemoveFromParent();
}

void UAccelByteSettingsMenu::OnKeepEditingButtonClicked()
{
	ShowPopUp(EPopUpType::None);
}

void UAccelByteSettingsMenu::InitDisplaySettings(int DisplayModeIndex)
{
	CheckGameSettingsAndHUDIsValid();
	
	const EWindowMode::Type ScreenMode = static_cast<EWindowMode::Type>(DisplayModeIndex);
	GameUserSettings->SetFullscreenMode(ScreenMode);

	const FIntPoint NewResolution = ScreenMode == EWindowMode::Type::Fullscreen ? GameUserSettings->GetDesktopResolution() : TutorialProjectUtilities::DefaultResolutionSettings;
	GameUserSettings->SetScreenResolution(NewResolution);

	GameUserSettings->ValidateSettings();
	GameUserSettings->ConfirmVideoMode();
	GameUserSettings->ApplySettings(false);
		
	TutorialProjectUtilities::ShowLog(ELogVerbosity::Log, FString::Printf(TEXT("res x: %d | res y: %d"), GameUserSettings->GetScreenResolution().X, GameUserSettings->GetScreenResolution().Y));
}

void UAccelByteSettingsMenu::ShowPopUp(const EPopUpType& PopUpType) const
{
	switch (PopUpType)
	{
		case EPopUpType::Loading:
			WS_SaveState->SetActiveWidget(CT_LoadingThrobber);
			break;
		case EPopUpType::Saved:
			WS_SaveState->SetActiveWidget(VB_SaveSuccess);
			break;
		case EPopUpType::Unsaved:
			WS_SaveState->SetActiveWidget(VB_PromptUnsaved);
			break;
		default:
			break;
	}
	B_SaveState->SetVisibility(PopUpType == EPopUpType::None ? ESlateVisibility::Collapsed : ESlateVisibility::SelfHitTestInvisible);
	Sb_GameSettingsContainer->SetVisibility(PopUpType == EPopUpType::None ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::HitTestInvisible);
}

void UAccelByteSettingsMenu::SaveDefaultGameSettings()
{
	CurrentGameSettingsState.DisplayMode = 2;
	CurrentGameSettingsState.CloudSaveState = 0;
	
	FJsonObject RecordRequest;
	RecordRequest.SetNumberField(TutorialProjectUtilities::CloudSaveJsonKeyA, CurrentGameSettingsState.DisplayMode);
	RecordRequest.SetNumberField(TutorialProjectUtilities::CloudSaveJsonKeyB, CurrentGameSettingsState.CloudSaveState);
			
	FRegistry::CloudSave.SaveUserRecord(TutorialProjectUtilities::GameSettingCloudSaveKey, RecordRequest, false, FVoidHandler::CreateLambda([this]()
	{
		TutorialProjectUtilities::ShowLog(ELogVerbosity::Log, TEXT("Success Save User Record"));

		LatestGameSettingsState.DisplayMode = CurrentGameSettingsState.DisplayMode;
		LatestGameSettingsState.CloudSaveState = CurrentGameSettingsState.CloudSaveState;

		InitDisplaySettings(LatestGameSettingsState.DisplayMode);
		
		TutorialMenuHUD->OpenMainMenu();
		this->RemoveFromParent();
	}),
	FErrorHandler::CreateLambda([](int32 ErrorCode, const FString& ErrorMessage)
	{
		TutorialProjectUtilities::ShowLog(ELogVerbosity::Error, FString::Printf(TEXT("Save User Record Failed, Error Code: %d Error Message: %s"), ErrorCode, *ErrorMessage));
	}));
}

void UAccelByteSettingsMenu::CheckGameSettingsAndHUDIsValid()
{
	if (GameUserSettings == nullptr || !GameUserSettings->IsValidLowLevel())
	{
		GameUserSettings = UGameUserSettings::GetGameUserSettings();
		check(GameUserSettings);
	}
	if (!TutorialMenuHUD->IsValidLowLevel() || TutorialMenuHUD == nullptr)
	{
		TutorialMenuHUD = Cast<ATutorialMenuHUD>(GetOwningPlayer()->GetHUD());
		check(TutorialMenuHUD);
	}
}

void UAccelByteSettingsMenu::InitGameSettings()
{	
	FRegistry::CloudSave.GetUserRecord(TutorialProjectUtilities::GameSettingCloudSaveKey, THandler<FAccelByteModelsUserRecord>::CreateLambda([this](const FAccelByteModelsUserRecord& Result)
	{
		TutorialProjectUtilities::ShowLog(ELogVerbosity::Log, TEXT("Get user record Success"));

		bool bIsCloudSaveFound = false;
		int32 DisplayMode;
		if (Result.Value.TryGetNumberField(TutorialProjectUtilities::CloudSaveJsonKeyA, DisplayMode))
		{
			int32 CloudSaveState;
			if (Result.Value.TryGetNumberField(TutorialProjectUtilities::CloudSaveJsonKeyB, CloudSaveState))
			{
				bIsCloudSaveFound = true;
				InitDisplaySettings(DisplayMode);

				CurrentGameSettingsState.DisplayMode = DisplayMode;
				CurrentGameSettingsState.CloudSaveState = CloudSaveState;
				LatestGameSettingsState.DisplayMode = DisplayMode;
				LatestGameSettingsState.CloudSaveState = CloudSaveState;

				TutorialMenuHUD->InitMainMenu();
				this->RemoveFromParent();
			}
			else
			{
				TutorialProjectUtilities::ShowLog(ELogVerbosity::Error, TEXT("There is no get value from result of number field"));
			}
		}
		else
		{
			TutorialProjectUtilities::ShowLog(ELogVerbosity::Error, TEXT("There is no get value from result of number field"));
		}

		if (!bIsCloudSaveFound)
		{
			TutorialProjectUtilities::ShowLog(ELogVerbosity::Warning, TEXT("Cloud save found with no entry, saving initial data"));
			
			SaveDefaultGameSettings();
		}
	}),
	FErrorHandler::CreateLambda([this](int32 ErrorCode, const FString& ErrorMessage)
	{
		TutorialProjectUtilities::ShowLog(ELogVerbosity::Error, FString::Printf(TEXT("Get User Record Failed, Error Code: %d Error Message: %s"), ErrorCode, *ErrorMessage));

		SaveDefaultGameSettings();
	}));
}

void UAccelByteSettingsMenu::InitDefaultGameSettings()
{
	CheckGameSettingsAndHUDIsValid();

	InitDisplaySettings(TutorialProjectUtilities::DefaultDisplaySettings);
}