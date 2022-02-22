// Copyright (c) 2022 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.

#pragma once

#include "CoreMinimal.h"
#include "AccelByteChatTab.h"
#include "Api/AccelByteLobbyApi.h"
#include "Blueprint/UserWidget.h"
#include "AccelByteChat.generated.h"

class UAccelByteChatTab;
class UAccelByteChatEntry;
class UButton;
class UVerticalBox;
class UEditableText;
class UScrollBox;

using namespace AccelByte::Api;

/**
 * UserWidget class to handle general chat interactions.
 */
UCLASS()
class TUTORIALPROJECT_API UAccelByteChat : public UUserWidget
{
	GENERATED_BODY()

#pragma region Initialization
	
protected:
	
	/**
	 * @brief Native Construct override for AccelByte Chat.
	 */
	virtual void NativeConstruct() override;

private:
	
	/**
	 * @brief Set callback functions for widgets.
	 */
	void SetWidgetCallbacks();
	
	/**
	 * @brief Set notification delegates for the chat service.
	 */
	void SetNotificationDelegates();

#pragma endregion

#pragma region Widget Components
	
protected:
	
	/**
	 * @brief Reference for AccelByteChatTab.
	 */
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UAccelByteChatTab> AccelByteChatTabClass;
	
	/**
	 * @brief Reference for AccelByteChatEntry.
	 */
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UAccelByteChatEntry> AccelByteChatEntryClass;
	
	/**
	 * @brief Button to Send Chat.
	 */
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_SendChat;
	
	/**
	 * @brief The left arrow button.
	 */
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_LeftTab;
	
	/**
	 * @brief The right arrow button.
	 */
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_RightTab;
	
	/**
	 * @brief Editable Text input for chat.
	 */
	UPROPERTY(meta = (BindWidget))
	UEditableText* Et_Chat;

	/**
	 * @brief Scrollbox containing Tab buttons.
	 */
	UPROPERTY(meta = (BindWidget))
	UScrollBox* Sb_TabButtons;
	
	/**
	 * @brief Scrollbox containing chat vertical box.
	 */
	UPROPERTY(meta = (BindWidget))
	UScrollBox* Sb_Chat;

	/**
	 * @brief Vertical box containing chat message entries.
	 */
	UPROPERTY(meta = (BindWidget))
	UVerticalBox* Vb_ChatContent;

#pragma endregion 

#pragma region Widget Creation

public:
	
	/**
	 * @brief Create Tab Button Widget.
	 * @param ChatTabType Chat type for tab button.
	 * @param UserData Target user data for private message.
	 */
	void CreateTabButtonWidget(const EChatTabType& ChatTabType, const FSimpleUserData& UserData = {});
	
	/**
	 * @brief Delete a tab button by their type.
	 * @param ChatTabType chat tab type to delete.
	 * @param UserId target user Id when tab button is private type.
	 */
	void DeleteTabButtonWidget(const EChatTabType& ChatTabType, const FString& UserId = {});

private:

	/**
	 * @brief Set Tab Button State.
	 * @param ChatTabState The tab state to change.
	 * @param ChatTabType chat tab type to determine the target button.
	 * @param UserId target user Id when tab button is private.
	 */
	void SetTabButtonState(const EChatTabState& ChatTabState, const EChatTabType& ChatTabType, const FString& UserId = "");
	
	/**
	 * @brief Create chat entry message.
	 * @param ChatContent the content of the message.
	 */
	void CreateChatEntryWidget(const FString& ChatContent);
	
#pragma endregion 
	
#pragma region Widget Callbacks

public:
	
	/**
	 * @brief Execute function on tab button clicked from UAccelByteChatTab.
	 * @param ChatTabType The category for chat.
	 * @param UserId Target user id for private message.
	 */
	void OnClickedTabButton(const EChatTabType& ChatTabType, const FString& UserId = "");

protected:
	
	/**
	 * @brief Callback for send chat button
	 */
	UFUNCTION()
	void OnClickedSendChatButton();

	/**
	 * @brief Callback for left arrow tab button (Switch tab to left)
	 */
	UFUNCTION()
	void OnClickedLeftTabButton();

	/**
	 * @brief Callback for right arrow tab button (Switch tab to right)
	 */
	UFUNCTION()
	void OnClickedRightTabButton();

	/**
	 * @brief To send text when editable text box is focused and user presses the enter key.
	 * @param ChatContent the content of the chat.
	 * @param TextCommitType text commit type, currently using the enter key.
	 */
	UFUNCTION()
	void OnTextCommittedChat(const FText& ChatContent, ETextCommit::Type TextCommitType);

#pragma endregion 
	
#pragma region Utilities

public:
	
	/**
	 * @brief Join default global channel.
	 */
	void JoinDefaultChannel() const;
	
private:
	
	/**
	 * @brief Refresh chat tab by their type.
	 * @param ChatTabType The category for chat.
	 * @param UserId Target user id for private message
	 */
	void SwitchChatTab(const EChatTabType& ChatTabType, const FString& UserId = "");
	
	/**
	 * @brief Send a chat message.
	 * @param ChatMessage message to send.
	 */
	void SendChat(const FString& ChatMessage);
	
	/**
	 * @brief Receive incoming chat message.
	 * @param ChatTabType tab type for the message.
	 * @param UserId user id who sends the message.
	 * @param Payload chat message content.
	 * @param ChannelSlug currently joined channel.
	 */
	void ReceiveChatMessage(const EChatTabType& ChatTabType, const FString& UserId, const FString& Payload, const FString& ChannelSlug = "");
	
	/**
	 * @brief Add incoming messages to their array map.
	 * @param ChatTabType tab type to specify map.
	 * @param ChatMessage chat message content.
	 * @param UserId target user Id for private message.
	 */
	void AddMessageToArray(const EChatTabType& ChatTabType, const FString& ChatMessage, const FString& UserId = "");
	
public:
	
	/**
	 * @brief Clear all chat messages.
	 */
	void ResetChatComponents();
	
	/**
	 * @brief The current active chat tab.
	 */
	EChatTabType CurrentChatTab;
	
	/**
	 * @brief Current chat tab user id for private message.
	 */
	FString CurrentPrivateChatId;
	
	/**
	 * @brief Current Tab index for switching with arrow tabs.
	 */
	int32 CurrentTabIndex;
	
	/**
	 * @brief Currently present tab buttons.
	 */
	TArray<UAccelByteChatTab*> TabButtonsArray;
	
	/**
	 * @brief This will be used to check out existing private chat tabs by checking their user id.
	 */
	TMap<FName, TArray<UAccelByteChatTab*>> PrivateTabButtonsMap;
	
	/**
	 * @brief Currently present chat entries array.
	 */
	TArray<UAccelByteChatEntry*> ChatEntriesArray;
	
	/**
	 * @brief Map to store chat messages by their type.
	 * @relates TabButtonsArray
	 */
	TMap<EChatTabType, TArray<FString>> ChatContentMap;
	
	/**
	 * @brief Store chat messages by User Id.
	 * @relates PrivateTabButtonsMap
	 */
	TMap<FName, TArray<FString>> PrivateChatContentMap;
	
	/**
	 * @brief Store user id and display name.
	 */
	TMap<FName, FString> DisplayNameMap;
	
#pragma endregion 
	
};
