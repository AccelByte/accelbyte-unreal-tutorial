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

#define FULLSCREEN_COMBO_BOX_INDEX 0
#define FULLSCREEN_COMBO_BOX_ITEM "Fullscreen"
#define WINDOWED_COMBO_BOX_INDEX 1
#define WINDOWED_COMBO_BOX_ITEM "Windowed"

void UAccelByteSettingsMenu::NativeConstruct()
{
	Super::NativeConstruct();

	Cbs_DisplayMode->ClearOptions();
	Cbs_DisplayMode->AddOption(FULLSCREEN_COMBO_BOX_ITEM);
	Cbs_DisplayMode->AddOption(WINDOWED_COMBO_BOX_ITEM);

	Cbs_DisplayMode->SetSelectedIndex(LatestGameSettingsState.DisplayMode == EWindowMode::Fullscreen ? FULLSCREEN_COMBO_BOX_INDEX : WINDOWED_COMBO_BOX_INDEX);
	Cbs_CloudSaveState->SetSelectedIndex(LatestGameSettingsState.bCloudSaveState);

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
		if (SelectedItem == FULLSCREEN_COMBO_BOX_ITEM)
		{
			CurrentGameSettingsState.DisplayMode = EWindowMode::Type::Fullscreen;
		}
		else if (SelectedItem == WINDOWED_COMBO_BOX_ITEM)
		{
			CurrentGameSettingsState.DisplayMode = EWindowMode::Type::Windowed;
		}
	}
}

void UAccelByteSettingsMenu::OnCloudSaveStateChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	if (SelectionType == ESelectInfo::Type::OnMouseClick)
	{
		CurrentGameSettingsState.bCloudSaveState = static_cast<bool>(Cbs_CloudSaveState->GetSelectedIndex());
	}
}

void UAccelByteSettingsMenu::OnSaveChangesButtonClicked()
{
	ShowPopUp(EPopUpType::Loading);
		
	FJsonObject RecordRequest;
	RecordRequest.SetNumberField(TutorialProjectUtilities::CloudSaveJsonKeyA, static_cast<uint8>(CurrentGameSettingsState.DisplayMode));
	RecordRequest.SetNumberField(TutorialProjectUtilities::CloudSaveJsonKeyB, CurrentGameSettingsState.bCloudSaveState);
	
	FRegistry::CloudSave.SaveUserRecord(TutorialProjectUtilities::GameSettingCloudSaveKey, false, RecordRequest, FVoidHandler::CreateWeakLambda(this, [this]()
	{
		UE_LOG(LogTemp, Log, TEXT("Success Save User Record"));

		LatestGameSettingsState.DisplayMode = CurrentGameSettingsState.DisplayMode;
		LatestGameSettingsState.bCloudSaveState = CurrentGameSettingsState.bCloudSaveState;

		InitDisplaySettings(LatestGameSettingsState.DisplayMode);
		
		ShowPopUp(EPopUpType::Saved);
	}),
	FErrorHandler::CreateWeakLambda(this, [this](int32 ErrorCode, const FString& ErrorMessage)
	{
		UE_LOG(LogTemp, Error, TEXT("Save User Record Failed, Error Code: %d Error Message: %s"), ErrorCode, *ErrorMessage);
		
		ShowPopUp(EPopUpType::None);
	}));
}

void UAccelByteSettingsMenu::OnBackButtonClicked()
{
	ShowPopUp(EPopUpType::None);
}

void UAccelByteSettingsMenu::OnBackToMainMenuButtonClicked()
{
	if (CurrentGameSettingsState.DisplayMode != LatestGameSettingsState.DisplayMode || CurrentGameSettingsState.bCloudSaveState != LatestGameSettingsState.bCloudSaveState)
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
	CurrentGameSettingsState.bCloudSaveState = LatestGameSettingsState.bCloudSaveState;
	
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

void UAccelByteSettingsMenu::InitDisplaySettings(const EWindowMode::Type& WindowMode)
{
	CheckGameSettingsAndHUDIsValid();
	
	GameUserSettings->SetFullscreenMode(WindowMode);

	const FIntPoint NewResolution = WindowMode == EWindowMode::Type::Fullscreen ? GameUserSettings->GetDesktopResolution() : TutorialProjectUtilities::DefaultResolutionSettings;
	GameUserSettings->SetScreenResolution(NewResolution);

	GameUserSettings->ValidateSettings();
	GameUserSettings->ConfirmVideoMode();
	GameUserSettings->ApplySettings(false);
		
	UE_LOG(LogTemp, Log, TEXT("res x: %d | res y: %d"), GameUserSettings->GetScreenResolution().X, GameUserSettings->GetScreenResolution().Y);
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
	CurrentGameSettingsState.DisplayMode = TutorialProjectUtilities::DefaultDisplaySettings;
	CurrentGameSettingsState.bCloudSaveState = static_cast<bool>(TutorialProjectUtilities::DefaultCloudSaveStateIndex);
	
	FJsonObject RecordRequest;
	RecordRequest.SetNumberField(TutorialProjectUtilities::CloudSaveJsonKeyA, static_cast<uint8>(CurrentGameSettingsState.DisplayMode));
	RecordRequest.SetNumberField(TutorialProjectUtilities::CloudSaveJsonKeyB, CurrentGameSettingsState.bCloudSaveState);
			
	FRegistry::CloudSave.SaveUserRecord(TutorialProjectUtilities::GameSettingCloudSaveKey, false, RecordRequest, FVoidHandler::CreateWeakLambda(this, [this]()
	{
		UE_LOG(LogTemp, Log, TEXT("Success Save User Record"));

		LatestGameSettingsState.DisplayMode = CurrentGameSettingsState.DisplayMode;
		LatestGameSettingsState.bCloudSaveState = CurrentGameSettingsState.bCloudSaveState;

		InitDisplaySettings(LatestGameSettingsState.DisplayMode);
		
		TutorialMenuHUD->InitUserProfile();
		this->RemoveFromParent();
	}),
	FErrorHandler::CreateWeakLambda(this, [](int32 ErrorCode, const FString& ErrorMessage)
	{
		UE_LOG(LogTemp, Error, TEXT("Save User Record Failed, Error Code: %d Error Message: %s"), ErrorCode, *ErrorMessage);
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
	FRegistry::CloudSave.GetUserRecord(TutorialProjectUtilities::GameSettingCloudSaveKey, THandler<FAccelByteModelsUserRecord>::CreateWeakLambda(this, [this](const FAccelByteModelsUserRecord& Result)
	{
		UE_LOG(LogTemp, Log, TEXT("Get user record Success"));

		bool bIsCloudSaveFound = false;
		int32 DisplayModeIndex;
		if (Result.Value.JsonObject->TryGetNumberField(TutorialProjectUtilities::CloudSaveJsonKeyA, DisplayModeIndex))
		{
			int32 CloudSaveState;
			if (Result.Value.JsonObject->TryGetNumberField(TutorialProjectUtilities::CloudSaveJsonKeyB, CloudSaveState))
			{
				bIsCloudSaveFound = true;
				InitDisplaySettings(static_cast<EWindowMode::Type>(DisplayModeIndex));

				CurrentGameSettingsState.DisplayMode = DisplayModeIndex == static_cast<uint8>(EWindowMode::Type::Fullscreen) ? EWindowMode::Type::Fullscreen : EWindowMode::Type::Windowed;
				CurrentGameSettingsState.bCloudSaveState = static_cast<bool>(CloudSaveState);
				LatestGameSettingsState.DisplayMode = DisplayModeIndex == static_cast<uint8>(EWindowMode::Type::Fullscreen) ? EWindowMode::Type::Fullscreen : EWindowMode::Type::Windowed;
				LatestGameSettingsState.bCloudSaveState = static_cast<bool>(CloudSaveState);

				TutorialMenuHUD->InitUserProfile();
				this->RemoveFromParent();
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("There is no get value from result of number field"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("There is no get value from result of number field"));
		}

		if (!bIsCloudSaveFound)
		{
			UE_LOG(LogTemp, Warning, TEXT("Cloud save found with no entry, saving initial data"));
			
			SaveDefaultGameSettings();
		}
	}),
	FErrorHandler::CreateWeakLambda(this, [this](int32 ErrorCode, const FString& ErrorMessage)
	{
		UE_LOG(LogTemp, Error, TEXT("Get User Record Failed, Error Code: %d Error Message: %s"), ErrorCode, *ErrorMessage);

		SaveDefaultGameSettings();
	}));
}

void UAccelByteSettingsMenu::InitDefaultGameSettings()
{
	CheckGameSettingsAndHUDIsValid();

	InitDisplaySettings(TutorialProjectUtilities::DefaultDisplaySettings);
}

#undef FULLSCREEN_COMBO_BOX_INDEX
#undef FULLSCREEN_COMBO_BOX_ITEM
#undef WINDOWED_COMBO_BOX_INDEX
#undef WINDOWED_COMBO_BOX_ITEM