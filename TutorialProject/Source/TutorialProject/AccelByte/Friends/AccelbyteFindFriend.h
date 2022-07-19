// Copyright (c) 2021 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.

#pragma once

#include "CoreMinimal.h"
#include "AccelByteFriendEntry.h"
#include "Blueprint/UserWidget.h"
#include "Api/AccelByteLobbyApi.h"
#include "Core/AccelByteRegistry.h"
#include "AccelbyteFindFriend.generated.h"

class UEditableTextBox;
class UButton;
class UScrollBox;
class ATutorialMenuHUD;

/**
* Component for Finding Friend using AccelByte SDK.
* This code covers AccelByte services including :
*
* - List Outgoing Friends
*/

UCLASS()
class TUTORIALPROJECT_API UAccelByteFindFriend : public UUserWidget
{
	GENERATED_BODY()
	
	virtual void NativeConstruct() override;

protected:
	
	/**
	* @brief Editable Text Box for Find Friend.
	*/
	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* Etb_FindFriend;
	
	/**
	* @brief Button to start searching Friend.
	*/
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_FindFriend;
	
	/**
	* @brief Button to close Find Friend Widget.
	*/
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Close;
	
	/**
	* @brief Scrollable Box for Friends List result.
	*/
	UPROPERTY(meta = (BindWidget))
	UScrollBox* Sb_FriendList;
	
	/**
	* @brief Find Friends button.
	*/
	UFUNCTION()
	void OnClickFindFriends();
	
	/**
	* @brief Close Find Friend Widget.
	*/
	UFUNCTION()
	void OnClickCloseFindFriendWidget();

	/**
	* @brief Filter search entry to disable the add button for users who are already 
	* in their friends list and outgoing requests list.
	*
	* The flow goes like this:
	* 1. Add data to Array from Friends List.
	* 2. Add data to Array from Outgoing Request List.
	* 3. Search Users with the result that matches the array having their buttons disabled.
	*/
	void FilterSearchEntryList();
	
	/**
	 * @brief Callback function from LoadFriendsList.
	 * @param Result Array containing Friends Id.
	 */
	void OnLoadFriendListResponse(const FAccelByteModelsLoadFriendListResponse& Result);
	
	/**
	* @brief List Outgoing Friend result.
	*/
	void OnListOutgoingFriendResponse(const FAccelByteModelsListOutgoingFriendsResponse& Result);
	
	/**
	* @brief Find Users functionality inside the callback.
	*/
	void FindUsers(const FString& FriendName);
	
	/**
	* @brief Callback for Success Find Friends.
	*/
	void OnSuccessFindFriends(const FPagedPublicUsersInfo& Result);
	
	/**
	* @brief Callback for Failing Find Friends.
	*/
	void OnFailedFindFriends(int32 ErrorCode, const FString& ErrorMessage);
	
	/**
	* @brief Widget to spawn for friend Entry.
	*/
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UAccelByteFriendEntry> FriendEntryClass;
	
	/**
	 * @brief Tutorial Menu HUD pointer reference.
	 */
	UPROPERTY()
	ATutorialMenuHUD* TutorialMenuHUD;
	
	/**
	* @brief Container for all friend names copied from outgoing friend list.
	*/
	UPROPERTY()
	TArray<FString> FriendSentRequestArray;
};
