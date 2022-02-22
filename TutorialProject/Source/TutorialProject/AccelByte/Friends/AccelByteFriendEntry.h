// Copyright (c) 2021 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Models/AccelByteUserModels.h"
#include "Models/AccelByteLobbyModels.h"
#include "TutorialProject/TutorialMenuHUD.h"
#include "AccelByteFriendEntry.generated.h"

class UTextBlock;
class UButton;
class UScaleBox;
class UHorizontalBox;
class UWidgetSwitcher;

/**
* Enumerator for Friend Entry widget modes.
* Available options:
* - Friend
* - Incoming Request
* - Outgoing Request
* - Blocked Users
* - Search User Entry
*/
UENUM()
enum class EFriendEntryMode : uint8
{
	Friend		UMETA(DisplayName = "Friend"),
	Incoming	UMETA(DisplayName = "Incoming Request"),
	Outgoing	UMETA(DisplayName = "Outgoing Request"),
	Blocked		UMETA(DisplayName = "Blocked"),
	Search		UMETA(DisplayName = "Search")
};

/**
* Entry for Friend data.
* In this code we will use :
* 
* - Get User by User ID
* - Chat with specific person
* - Unfriend specific person
* - Block specific person
*/
UCLASS()
class TUTORIALPROJECT_API UAccelByteFriendEntry : public UUserWidget
{
	GENERATED_BODY()

#pragma region Initilization
	
public:
	
	/**
	* @brief Initialize Entry Data by getting User Data by User Id and selecting its Entry Mode
	* @param EntryMode Select Entry Mode. Available modes: FRIEND, INCOMING, OUTGOING, BLOCKED, SEARCH.
	* @param UserId User Id needed to get User Data.
	*/
	void InitData(const EFriendEntryMode& EntryMode, const FString& UserId);

	/**
	 * @brief Set user presence text block.
	 * @param FriendPresence Replace this value with available user presences.
	 */
	void SetPresenceText(const FString& FriendPresence) const;

	/**
	 * @brief Enable or Disable Add Friend button for search entry.
	 * @param bEnable Value to enable or disable the button.
	 */
	void EnableAddFriendButton(bool bEnable) const;
	
#pragma endregion

#pragma region Widget Components
	
protected:
	
	// Friend Entry Type
	/**
	* @brief Button for Chat Friend.
	*/
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Chat;
	/**
	* @brief Button for Invite Friend to Party.
	*/
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_InviteParty;
	/**
	* @brief Button for Unfriend.
	*/
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Unfriend;
	/**
	* @brief Button for Block Friend.
	*/
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Block;

	// Block Entry Type
	/**
	* @brief Button to Unblock User.
	*/
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Unblock;

	// Incoming Entry Type
	/**
	* @brief Button for Accepting Friend Request.
	*/
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Accept;
	/**
	* @brief Button for Decline Friend Request.
	*/
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Decline;
	/**
	* @brief Button for Block User by Incoming Request.
	*/
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Block_Request;

	// Outgoing Entry Type
	/**
	* @brief Button for canceling outgoing friend request.
	*/
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_CancelRequest;
	
	// Search Entry Type
	/**
	* @brief Button for Add Friend.
	*/
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_AddFriend;

	/**
	* @brief Text Box for Friend Name (Will be replaced with friend name).
	*/
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Tb_FriendName;
	/**
	* @brief Text Box for Friend Status i.e(Online, Offline, Away).
	*/
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Tb_Status;
	/**
	* @brief Widget Switcher to switch Entry Mode.
	*/
	UPROPERTY(meta = (BindWidget))
	UWidgetSwitcher* Ws_EntryMode;
	/**
	* @brief Scale Box for User Image.
	*/
	UPROPERTY(meta = (BindWidget))
	UScaleBox* Sb_Friend;

	// Horizontal Boxes for Widget Switcher.
	/**
	 * @brief Horizontal Box for Friend component.
	 */
	UPROPERTY(meta = (BindWidget))
	UHorizontalBox* Hb_Friend;
	/**
	 * @brief Horizontal Box for Incoming component.
	 */
	UPROPERTY(meta = (BindWidget))
	UHorizontalBox* Hb_IncomingRequest;
	/**
	 * @brief Horizontal Box for Outgoing component.
	 */
	UPROPERTY(meta = (BindWidget))
	UHorizontalBox* Hb_OutgoingRequest;
	/**
	 * @brief Horizontal Box for Blocked component.
	 */
	UPROPERTY(meta = (BindWidget))
	UHorizontalBox* Hb_Blocked;
	/**
	 * @brief Horizontal Box for Search component.
	 */
	UPROPERTY(meta = (BindWidget))
	UHorizontalBox* Hb_Search;

#pragma endregion 

#pragma region Button Callbacks
	
	/**
	* @brief Functionality for Chat Button.
	*/
	UFUNCTION()
	void OnClickedChat();
	
	/**
	* @brief Functionality for Invite Party Button.
	*/
	UFUNCTION()
	void OnClickedInviteParty();
	
	/**
	* @brief Functionality for Unfriend Button.
	*/
	UFUNCTION()
	void OnClickedUnfriend();
	
	/**
	* @brief Functionality for Block Friend Button.
	*/
	UFUNCTION()
	void OnClickedBlock();
	
	/**
	* @brief Callback for Clicking Unblock button.
	*/
	UFUNCTION()
	void OnClickedUnblock();
	
	/**
	* @brief Callback for Accept Button.
	*/
	UFUNCTION()
	void OnClickedAccept();
	
	/**
	* @brief Callback for Decline Button.
	*/
	UFUNCTION()
	void OnClickedDecline();
	
	/**
	* @brief Click callback to cancel request.
	*/
	UFUNCTION()
	void OnClickedCancelRequest();
	
	/**
	* @brief Functionality for Add Friend Button.
	*/
	UFUNCTION()
	void OnClickedAddFriend();
	
#pragma endregion
	
#pragma region Responses
	
	/**
	* @brief Callback for successfully get the user ID.
	*/
	void OnSuccessGetUserId(const FSimpleUserData& Data);
	
	/**
	* @brief Callback for failed getting user ID.
	*/
	void OnFailedGetUserId(int32 ErrorCode, const FString& ErrorMessage);
	
	/**
	* @brief Callback for Unfriend.
	*/
	void OnUnfriendResponse(const FAccelByteModelsUnfriendResponse& Result);
	
	/**
	* @brief Callback for Block.
	*/
	void OnBlockPlayerResponse(const FAccelByteModelsBlockPlayerResponse& Result);
	
	/**
	* @brief Callback for when successfully unblock user.
	*/
	void OnUnblockPlayerResponse(const FAccelByteModelsUnblockPlayerResponse& Result);
	
	/**
	* @brief Response for Accepting Friend Request.
	*/
	void OnAcceptFriendResponse(const FAccelByteModelsAcceptFriendsResponse& Result);
	
	/**
	* @brief Response for Rejecting Friend Request.
	*/
	void OnRejectFriendResponse(const FAccelByteModelsRejectFriendsResponse& Result);
	
	/**
	* @brief Response for Friend Request.
	*/
	void OnRequestFriendResponse(const FAccelByteModelsRequestFriendsResponse& Result);
	
#pragma endregion

#pragma region Utilities
	
	/**
	* @brief Set Mode settings for selected entry enumerator.
	*/
	void SetEntryMode(const EFriendEntryMode& EntryMode) const;
	
private:

	UPROPERTY()
	ATutorialMenuHUD* TutorialMenuHUD;
	
public:
	
	/**
	* @brief Models to store User Data contained in this entry.
	*/
	FSimpleUserData UserData;
	
#pragma endregion 
	
};
