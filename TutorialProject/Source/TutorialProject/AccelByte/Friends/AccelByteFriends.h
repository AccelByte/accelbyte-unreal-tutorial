// Copyright (c) 2021 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.

#pragma once

#include "CoreMinimal.h"
#include "AccelByteFriendEntry.h"
#include "Blueprint/UserWidget.h"
#include "Models/AccelByteLobbyModels.h"
#include "TutorialProject/TutorialMenuHUD.h"
#include "AccelByteFriends.generated.h"

/**
 * Enumerator to determine the current state
 * Available options:
 * - FRIEND_LIST
 * - INCOMING_LIST
 * - OUTGOING_LIST
 * - BLOCKED_LIST
 */

UENUM()
enum class ECurrentStateFriend: uint8
{
	FRIEND_LIST		UMETA(DisplayName = "Friends List"),
	INCOMING_LIST	UMETA(DisplayName = "Incoming Requests List"),
	OUTGOING_LIST	UMETA(DisplayName = "Outgoing Requests List"),
	BLOCKED_LIST	UMETA(DisplayName = "Blocked Users List")
};

class UScrollBox;

/**
* Component for to use AccelByte Friend services.
* This code covers AccelByte services including :
*
* - Friend List
* - List Outgoing Friend Request
* - List Incoming Friend Request
* - Block Friend List
* 
*/

UCLASS()
class TUTORIALPROJECT_API UAccelByteFriends : public UUserWidget
{
	GENERATED_BODY()
	
	virtual void NativeConstruct() override;

public:
	
	/**
	 * @brief Set Friend services notification delegates.
	 */
	void SetNotificationDelegate();

private:
	
	/**
	 * @brief Button for opening Find Friend Menu pop up.
	 */
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_FindFriend;
	/**
	 * @brief Button for Friend List displaying.
	 */
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_FriendList;
	/**
	 * @brief Button for Pending Incoming Friend List.
	 */
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_PendingIncomingList;
	/**
	 * @brief Button for Pending Outgoing Friend List.
	 */
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_PendingSentList;
	/**
	 * @brief Button for Block Friend List.
	 */
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_BlockList;
	/**
	 * @brief Button for Back to Main Menu.
	 */
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Back;
	/**
	 * @brief Scroll Box for content display of four functionality (FriendList, Incoming, Outgoing, Block).
	 */
	UPROPERTY(meta = (BindWidget))
	UScrollBox* Sb_ContentList;

	/**
	 * @brief Callback for Find Friend.
	 */
	UFUNCTION()
	void OnClickedFindFriend();
	/**
	 * @brief Callback for Friend List.
	 */
	UFUNCTION()
	void OnClickedFriendList();
	/**
	 * @brief Callback for Pending Incoming Friend List.
	 */
	UFUNCTION()
	void OnClickedPendingIncomingList();
	/**
	 * @brief Callback for Pending Outgoing Friend List.
	 */
	UFUNCTION()
	void OnClickedPendingOutgoingList();
	/**
	 * @brief Callback for Block Friend List.
	 */
	UFUNCTION()
	void OnClickedBlockList();
	/**
	 * @brief Callback for Back button to lobby.
	 */
	UFUNCTION()
	void OnClickedBackToLobby();

	/**
	 * @brief Callback to get friends presence response.
	 * @param Result Model for user presence.
	 */
	void OnGetFriendsPresenceResponse(const FAccelByteModelsGetOnlineUsersResponse& Result);
	/**
	 * @brief Get all the friend list data response here.
	 */
	void OnLoadFriendListResponse(const FAccelByteModelsLoadFriendListResponse& Result);
	/**
	 * @brief Callback for list Incoming Friend response.
	 */
	void OnListIncomingFriendResponse(const FAccelByteModelsListIncomingFriendsResponse& Result);
	/**
	 * @brief Callback for list Outgoing Friend response.
	 */
	void OnListOutgoingFriendResponse(const FAccelByteModelsListOutgoingFriendsResponse& Result);
	/**
	 * @brief Callback for Successfully Get List Blocked User.
	 */
	void OnSuccessRetrieveListBlockedUser(const FAccelByteModelsListBlockedUserResponse& Result);
	/**
	 * @brief Callback for Failed Get List Blocked User.
	 */
	void OnFailedRetrieveListBlockedUser(int32 ErrorCode, const FString& ErrorMessage);

	/**
	 * @brief Creates Friend Entry Widget through weak object pointer.
	 * @param EntryMode Entry mode for the friend widget.
	 * @param FriendId Targeted Friend Id for the entry.
	 */
	void CreateEntryWidget(const EFriendEntryMode& EntryMode, const FString& FriendId);
	/**
	 * @brief Sets the user presence of the entry.
	 * @param FriendEntry Pointer for Friend Entry Class.
	 * @param Availability User Presence Availability Code.
	 */
	void SetUserPresence(const TWeakObjectPtr<UAccelByteFriendEntry> FriendEntry, const FString& Availability) const;
	
	/**
	 * @brief Reference to Friend Entry Class.
	 */
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UAccelByteFriendEntry> FriendEntryClass;
	/**
	 * @brief Tutorial Menu HUD pointer reference.
	 */
	UPROPERTY()
	ATutorialMenuHUD* TutorialMenuHUD;
	/**
	 * @brief Array for Entry Widgets.
	 */
	TArray<TWeakObjectPtr<UAccelByteFriendEntry>> FriendListWidgets;
	/**
	 * @brief Current state of the menu.
	 */
	ECurrentStateFriend CurrentState;
};

