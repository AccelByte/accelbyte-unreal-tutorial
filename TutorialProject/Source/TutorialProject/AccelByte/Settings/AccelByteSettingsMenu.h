// Copyright (c) 2022 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AccelByteSettingsMenu.generated.h"

/**
 * @brief This is the struct to handle all current and latest game settings.
 */
USTRUCT()
struct FGameSettings
{
	GENERATED_BODY()

	int32 DisplayMode;
	int32 CloudSaveState;
};

/**
 * @brief Enumerator to handle popup type.
 */
UENUM()
enum class EPopUpType : uint8
{
	Loading,
	Saved,
	Unsaved,
	None
};

class ATutorialMenuHUD;
class UButton;
class UComboBoxString;
class UBorder;
class UWidgetSwitcher;
class USizeBox;
class UCircularThrobber;
class UVerticalBox;

/**
 * Settings Menu controller.
 */

UCLASS()
class TUTORIALPROJECT_API UAccelByteSettingsMenu : public UUserWidget
{
	GENERATED_BODY()
	
	virtual void NativeConstruct() override;
	
protected:
	/**
	 * @brief Game user setting object.
	 */
	UGameUserSettings* GameUserSettings;

	/**
	 * @brief Tutorial Menu HUD reference class.
	 */
	ATutorialMenuHUD* TutorialMenuHUD;

	/**
	 * @brief Combo Box to change fullscreen type.
	 */
	UPROPERTY(meta = (BindWidget))
	UComboBoxString* Cbs_DisplayMode;

	/**
	* @brief Combo Box to change cloud save state type.
	*/
	UPROPERTY(meta = (BindWidget))
	UComboBoxString* Cbs_CloudSaveState;

	/**
	 * @brief Button to save and apply settings.
	 */
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_SaveChanges;

	/**
	* @brief Button to go back to game settings menu.
	*/
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Back;

	/**
	 * @brief Button to go back to main menu.
	 */
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_BackToMainMenu;

	/**
	* @brief Button to go proceed to main menu.
	*/
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_ProceedToMainMenu;
	
	/**
	* @brief Button to keep editing the settings.
	*/
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_KeepEditing;

	/**
	 * @brief This to change the state to the loading and saving state.
	 */
	UPROPERTY(meta = (BindWidget))
	UBorder* B_SaveState;

	/**
	* @brief Widget switcher to change the loading screen into successful response state.
	*/
	UPROPERTY(meta = (BindWidget))
	UWidgetSwitcher* WS_SaveState;

	/**
	 * @brief Overlay to prevent the players to click widget buttons behind popup.
	 */
	UPROPERTY(meta = (BindWicget))
	USizeBox* Sb_GameSettingsContainer;

	/**
	 * @brief Loading State Widget.
	 */
	UPROPERTY(meta = (BindWidget))
	UCircularThrobber* CT_LoadingThrobber;

	/**
	 * @brief Save Success State Widget.
	 */
	UPROPERTY(meta = (BindWidget))
	UVerticalBox* VB_SaveSuccess;

	/**
	 * @brief Prompt Unsaved State Widget.
	 */
	UPROPERTY(meta = (BindWidget))
	UVerticalBox* VB_PromptUnsaved;

	/**
	 * @brief Called when the display mode is changed.
	 */
	UFUNCTION()
	void OnDisplayModeChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

	/**
	 * @brief Called when the cloud save state is changed. 
	 */
	UFUNCTION()
	void OnCloudSaveStateChanged(FString SelectedItem, ESelectInfo::Type SelectionType);
	
	/**
	* @brief Button callback on Confirm.
	*/
	UFUNCTION()
	void OnSaveChangesButtonClicked();

	/**
	* @brief Button callback on Confirm.
	*/
	UFUNCTION()
	void OnBackButtonClicked();
	
	/**
	* @brief Button callback on Cancel.
	*/
	UFUNCTION()
	void OnBackToMainMenuButtonClicked();

	/**
	* @brief Button callback on proceed to main menu.
	*/
	UFUNCTION()
	void OnProceedToMainMenuButtonClicked();

	/**
	* @brief Button callback on keep editing.
	*/
	UFUNCTION()
	void OnKeepEditingButtonClicked();

	/**
	 * @brief Init current display settings.
	 */
	void InitDisplaySettings(const int DisplayModeIndex);

	/**
	 * @brief Show or hide popup based on Pop up type.
	 * @param PopUpType The type of popup from EPopUpType.
	 */
	void ShowPopUp(const EPopUpType& PopUpType) const;

	/**
	 * @brief Saving default game settings when the first time open the game.
	 */
	void SaveDefaultGameSettings();

	/**
	 * @brief Check if game settings and HUD is valid or not
	 */
	void CheckGameSettingsAndHUDIsValid();
	
	/**
	 * @brief Current Game Settings State from player's mode perspective change.
	 */
	FGameSettings CurrentGameSettingsState;

	/**
	 * @brief Latest Game Settings State from cloud save.
	 */
	FGameSettings LatestGameSettingsState;
	
public:
	
	/**
	* @brief Initialize game settings data from cloud save.
	*/
	void InitGameSettings();

	/**
	* @brief Initialize game settings data at the first time open the game.
	*/
	void InitDefaultGameSettings();
};
