// Copyright (c) 2021 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.

#pragma once

#include "CoreMinimal.h"
#include "AccelBytePartyInvitationPopUp.h"
#include "Blueprint/UserWidget.h"
#include "Models/AccelByteLobbyModels.h"
#include "AccelByteParty.generated.h"

class UAccelBytePartyPlayerEntry;
class UScrollBox;
class UButton;
class UTextBlock;
class ATutorialMenuHUD;
class UAccelByteLobby;

/**
* Party setup
* This code covers AccelByte services including :
*
* - Create Party
* - Leave Party
* - Invite Party
* - Get Notification when leave party
* - Get Notification when join party
* - Get Notification when someone kicked from party
* - Get Notification when someone promoted to be a party leader
* - Update party data
 */
UCLASS()
class TUTORIALPROJECT_API UAccelByteParty : public UUserWidget
{
	GENERATED_BODY()

	virtual void NativeConstruct() override;
	
	/**
	* @brief Scroll Box to spawn the player entry.
	*/
	UPROPERTY(meta = (BindWidget))
	UScrollBox* Sb_Player;
	
	/**
	* @brief Button for Create Party.
	*/
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_CreateParty;
	
	/**
	* @brief Button for Leave Party.
	*/
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_LeaveParty;
	
	/**
	* @brief Text Block for displaying Party ID.
	*/
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Tb_PartyID;

	/**
	* @brief Tutorial Menu HUD to handle all functionality of the UI.
	*/
	ATutorialMenuHUD* TutorialMenuHUD;

	/**
	 * @brief Local variable to handle lobby functionality
	 */
	UAccelByteLobby* AccelByteLobby;

public:
	
	/**
	 * @brief Current Party Information
	 */
	FAccelByteModelsPartyDataNotif PartyInfo;

	/**
	* @brief Callback when creating party.
	*/
	UFUNCTION()
	void OnClickedCreateParty();
	
	/**
	* @brief Callback when leaving party.
	*/
	UFUNCTION()
	void OnClickedLeaveParty();
	
	/**
	* @brief Callback when inviting to party.
	* @param UserId user ID to invited.
	*/
	UFUNCTION()
	void OnClickedInviteParty(const FString& UserId);

private:
	
	/**
	* @brief Response for creating party.
	*/
	void OnCreatePartyResponse(const FAccelByteModelsCreatePartyResponse& Result);
	
	/**
	* @brief Response for failing create party.
	*/
	void OnCreatePartyFailed(int32 ErrorCode, const FString& ErrorMessage);
	
	/**
	* @brief Response for leaving party.
	*/
	void OnLeavePartyResponse(const FAccelByteModelsLeavePartyResponse& Result);
	
	/**
	* @brief Response for failing leaving party.
	*/
	void OnLeavePartyFailed(int32 ErrorCode, const FString& ErrorMessage);
	
	/**
	* @brief Response for inviting friend to party.
	*/
	void OnInvitePartyResponse(const FAccelByteModelsPartyInviteResponse& Result);
	
	/**
	* @brief Response for failing inviting friend to party.
	*/
	void OnInvitePartyFailed(int32 ErrorCode, const FString& ErrorMessage);
	
	/**
	* @brief Response for get notification for invitation party.
	*/
	void OnInvitePartyGetInviteNotice(const FAccelByteModelsPartyGetInvitedNotice& Result);
	
	/**
	* @brief Response for notification when someone kicked in the party.
	*/
	void OnInvitePartyKickedNotice(const FAccelByteModelsGotKickedFromPartyNotice& Result);
	
	/**
	* @brief Response for notification when someone leave party.
	*/
	void OnLeavePartyNotice(const FAccelByteModelsLeavePartyNotice& Result);
	
	/**
	* @brief Response for updating party data when someone do some action regarding to party.
	*/
	void OnPartyDataUpdateResponse(const FAccelByteModelsPartyDataNotif& Result);

public:

	/**
	* @brief Create an Empty Party List with UMG.
	*/
	void RefreshPartyList();
	
	/**
	* @brief Function where all the notifications are being set.
	*/
	void SetNotificationDelegate();
	
	/**
	* @brief Refresh party entries.
	*/
	void RefreshPartyEntries();
	
	/**
	* @brief Update the leader icon.
	* @param LeaderId The ID of the current party leader.
	*/
	void SetUpdateLeaderIcon(const FString& LeaderId) const;
	
	/**
	* @brief Set enabled / disabled state of the Create Party & Leave Party button.
	*/
	void SetCreatePartyButtonEnabled(const bool& bIsCreateParty) const;
	
	/**
	 * @brief Check total member from game mode that has selected.
	 */
	void CheckTotalMemberGameMode();

private:
	
	/**
	* @brief Copy Array of player entry.
	*/
	TArray<TWeakObjectPtr<UAccelBytePartyPlayerEntry>> PartyPlayerEntryWidgetArray;
	
	/**
	* @brief Reference to Party Player Entry Class.
	*/
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UAccelBytePartyPlayerEntry> PartyPlayerEntryClass;
	
	/**
	* @brief Reference to Party Invitation Pop Up Class.
	*/
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UAccelBytePartyInvitationPopUp> PartyInvitationPopUpClass;

	/**
	* @brief Define maximum party member allowed.
	*/
	UPROPERTY()
	int TotalPartyMember;
};