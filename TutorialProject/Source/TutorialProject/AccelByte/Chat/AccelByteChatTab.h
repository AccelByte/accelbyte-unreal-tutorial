// Copyright (c) 2022 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Models/AccelByteUserModels.h"
#include "AccelByteChatTab.generated.h"

class UAccelByteChat;
class UButton;
class UTextBlock;

UENUM()
enum class EChatTabType : uint8
{
	None,
	Global,
	Party,
	Private
};

UENUM()
enum class EChatTabState : uint8
{
	Normal,
	Unread,
	Selected
};

/**
 * UserWidget class to handle Tab Buttons on Chat.
 */
UCLASS()
class TUTORIALPROJECT_API UAccelByteChatTab : public UUserWidget
{
	GENERATED_BODY()

#pragma region Initialization
	
public:
	
	/**
	 * @brief Initialize tab button widget.
	 * @param InAccelByteChat pointer of UAccelByteChat.
	 * @param ChatTab Tab Button Type.
	 * @param InUserData User data if tab button type is private.
	 */
	void InitData(UAccelByteChat* InAccelByteChat, const EChatTabType& ChatTab, const FSimpleUserData& InUserData = {});

#pragma endregion
	
#pragma region Widget Components

protected:

	/**
	 * @brief The chat tab button.
	 */
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_ChatTab;

	/**
	 * @brief Close button for private chats.
	 */
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Close;
	
	/**
	 * @brief Text block inside Btn_ChatTab.
	 */
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Tb_TabName;
	
	/**
	 * @brief Reference of AccelByteChat.
	 */
	UPROPERTY()
	UAccelByteChat* AccelByteChat = nullptr;

#pragma endregion

#pragma region Widget Callbacks

public:
	
	/**
	 * @brief On tab button clicked used in UAccelByteChatTab.
	 */
	UFUNCTION()
	void OnClickedChatTabButton();
	
	/**
	 * @brief Remove button on close button clicked.
	 */
	UFUNCTION()
	void OnClickedCloseButton();

#pragma endregion

#pragma region Utilities
	
public:

	/**
	 * @brief Set button color according to tab state.
	 * @param TabState needed to determine button color.
	 */
	void SetButtonColorState(const EChatTabState& TabState) const;
	
	/**
	 * @brief Store Tab button type.
	 */
	EChatTabType TabButtonType;
	
	/**
	 * @brief Store user data in private tab button type.
	 */
	FSimpleUserData UserData;

#pragma endregion 
	
};
