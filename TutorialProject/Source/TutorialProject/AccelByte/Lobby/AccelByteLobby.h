// Copyright (c) 2021 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../Matchmaking/AccelByteMatchmaking.h"
#include "../Party/AccelByteParty.h"
#include "TutorialProject/AccelByte/Chat/AccelByteChat.h"
#include "AccelByteLobby.generated.h"

class ATutorialMenuHUD;
class UButton;
class UComboBoxString;
class UScrollBox;
class USizeBox;
class UTutorialProjectGameMode;

/**
* Component for Join to AccelByte Lobby.
* This code covers AccelByte services including :
*
* - Join Lobby
* - Leave Lobby
*/
UCLASS()
class TUTORIALPROJECT_API UAccelByteLobby : public UUserWidget
{
	GENERATED_BODY()

#pragma region Initialization

protected:

	virtual void NativeConstruct() override;

#pragma endregion

#pragma region Widget Components

protected:

	/**
	* @brief Button for go to Friend Management Menu.
	*/
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_FriendsManagement;

	/**
	* @brief Button for Start Finding Match.
	*/
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_FindMatch;

	/**
	* @brief Button for Leave Lobby and back to Main Menu.
	*/
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_LeaveLobby;

	/**
	* @brief Scroll Box to spawn Party Widget.
	*/
	UPROPERTY(meta = (BindWidget))
	UScrollBox* Sb_Party;

	/**
	* @brief Size Box to spawn Chat Widget.
	*/
	UPROPERTY(meta = (BindWidget))
	USizeBox* Sb_Chat;

	/**
	 * @brief Combo Box to handle all Game Mode that we specify
	 */
	UPROPERTY(meta = (BindWidget))
	UComboBoxString* Cbs_Gamemode;

	/**
	 * @brief Text Block to handle the current error state if the current game mode is not match with the current total number of player in a party.
	 */
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Tb_FindMatchError;

	/**
	 * @brief Combo Box to handle all Region that we specify
	 */
	UPROPERTY(meta = (BindWidget))
	UComboBoxString* Cbs_Region;

#pragma endregion

#pragma region Widget Callbacks

private:

	/**
	* @brief Button callback on Open Friends Management.
	*/
	UFUNCTION()
	void OnClickedOpenFriendsManagement();

	/**
	* @brief Button callback on Find Match.
	*/
	UFUNCTION()
	void OnClickedOpenFindMatch();

	/**
	* @brief ComboBox callback for Game Mode selection.
	*/
	UFUNCTION()
	void OnGameModeChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

#pragma endregion

#pragma region Responses

private:

	/**
	* @brief Callback when button Region selector pressed.
	*/
	void GetServerLatencies(const FGetServerLatencies& OnSuccess, const AccelByte::FErrorHandler& OnError);

#pragma endregion

#pragma region Utilities

public:

	/**
	* @brief Connect to AccelByte lobby.
	*/
	UFUNCTION()
	void ConnectToLobby();

	/**
	 * @brief Add new Party Menu to the party scroll box widget child.
	 * @param PartyMenu Party Menu that gonna add to party scroll box widget child.
	 */
	UFUNCTION()
	void AddPartyChild(UAccelByteParty* PartyMenu);

	/**
	 * @brief Spawn Chat Menu inside party size box widget as child.
	 * @param ChatMenu Chat Menu reference that is going to be spawned.
	 */
	UFUNCTION()
	void AddChatChild(UAccelByteChat* ChatMenu);

	/**
	* @brief Set Find Match Button to Enabled or not.
	* @param bIsFindMatchEnabled Is Find Match Enabled.
	*/
	UFUNCTION()
	void SetFindMatchEnabled(bool bIsFindMatchEnabled) const;

	/**
	* @brief Showing the error text block if the current state of game mode is not matched.
	* @param TotalMemberToKick Total member that need to inform to the player that need to be kicked.
	*/
	UFUNCTION()
	void ShowErrorFindMatch(int TotalMemberToKick) const;

	/**
	* @brief Removing the error of the find match if the current state of game mode is matched.
	*/
	UFUNCTION()
	void RemoveErrorFindMatch() const;

	/**
	 * @brief Reset all party component back to default
	 */
	UFUNCTION()
	void DisableMatchmakingComponents();

	/**
	* @brief Set The Game Mode Combo Box to enable or not.
	* @param bIsComboBoxEnabled Is Game Mode Combo Box Enabled.
	*/
	UFUNCTION()
	void SetGameModeComboBoxEnabled(bool bIsComboBoxEnabled);

	/**
	 * @brief Get Current Game Mode Combo Box Selected Option.
	 * @return The String of Current Selected Option of the Game Mode Combo Box
	 */
	UFUNCTION()
	FString GetGameModeComboBoxSelectedOption();

	/**
	* @brief Get Current Maximum Party Member from the Game Mode that has selected.
	* @return Total Maximum Party Member that allowed to join.
	*/
	UFUNCTION()
	int32 GetMaximumPartyMemberFromGameMode();

	/**
	 * @brief Get Currently selected latency from combo box index.
	 * @return the currently selected latency.
	 */
	TPair<FString, float> GetSelectedLatency() const;

	/**
	 * @brief Set Lobby services notification delegates.
	 */
	void SetLobbyNotificationDelegate();

private:

	/**
	* @brief Reset the region
	*/
	UFUNCTION()
	void ResetRegion();

	/**
	* @brief Set Visibility of Find Match Error Text Block.
	* @param IsVisibility The Visibility of Find Match Error.
	*/
	UFUNCTION()
	void SetFindMatchErrorVisibility(const ESlateVisibility& IsVisibility) const;

	/**
	* @brief Set Game Mode Combo Box selected Index.
	* @param SelectedIndex The current Selected Index of the Combo Box that you want to set.
	*/
	UFUNCTION()
	void SetGameModeComboBoxSelectedIndex(int32 SelectedIndex) const;

	/**
	 * @brief Tutorial Menu HUD pointer reference.
	 */
	UPROPERTY()
	ATutorialMenuHUD* TutorialMenuHUD;

	/**
	 * @brief Matchmaking pointer reference.
	 */
	UPROPERTY()
	UAccelByteMatchmaking* MatchmakingMenu;

	/**
	 * @brief Check if running on first time.
	 */
	bool bIsFirstTimeRun;

	/**
	 * @brief Array of available latencies.
	 */
	TArray<TPair<FString, float>> AvailableLatencies;

	/**
	 * @brief Previous selected latency.
	 */
	TPair<FString, float> PreviousLatency;

#pragma endregion
};
