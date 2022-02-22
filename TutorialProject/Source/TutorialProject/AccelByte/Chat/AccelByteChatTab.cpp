// Copyright (c) 2022 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.

#include "AccelByteChatTab.h"
#include "AccelByteChat.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "TutorialProject/TutorialProjectUtilities.h"

#define GLOBAL_TAB_TEXT NSLOCTEXT("ChatTab","GlobalTabText","Global")
#define PARTY_TAB_TEXT NSLOCTEXT("ChatTab","PartyTabText","Party")

#pragma region Initialization

void UAccelByteChatTab::InitData(UAccelByteChat* InAccelByteChat, const EChatTabType& ChatTab, const FSimpleUserData& InUserData)
{
	AccelByteChat = InAccelByteChat;
	TabButtonType = ChatTab;
	
	switch(ChatTab)
	{
	case EChatTabType::Global:
		Tb_TabName->SetText(GLOBAL_TAB_TEXT);
		Btn_Close->SetVisibility(ESlateVisibility::Collapsed);
		break;
		
	case EChatTabType::Party:
		Tb_TabName->SetText(PARTY_TAB_TEXT);
		Btn_Close->SetVisibility(ESlateVisibility::Collapsed);
		break;
		
	case EChatTabType::Private:
		UserData = InUserData;
		Tb_TabName->SetText(FText::FromString(UserData.DisplayName));
		Btn_Close->SetVisibility(ESlateVisibility::Visible);
		Btn_Close->OnClicked.AddUniqueDynamic(this, &UAccelByteChatTab::OnClickedCloseButton);
		break;
		
	default:
		break;
	}
	
	SetButtonColorState(EChatTabState::Normal);
	Btn_ChatTab->OnClicked.AddUniqueDynamic(this, &UAccelByteChatTab::OnClickedChatTabButton);
}

#pragma endregion 

#pragma region Button Callbacks

void UAccelByteChatTab::OnClickedChatTabButton()
{
	SetButtonColorState(EChatTabState::Selected);
	
	if (TabButtonType == EChatTabType::Private)
	{
		AccelByteChat->OnClickedTabButton(TabButtonType, UserData.UserId);
	}
	else
	{
		AccelByteChat->OnClickedTabButton(TabButtonType);
	}
}

void UAccelByteChatTab::OnClickedCloseButton()
{
	AccelByteChat->DeleteTabButtonWidget(TabButtonType, UserData.UserId);
}

void UAccelByteChatTab::SetButtonColorState(const EChatTabState& TabState) const
{
	switch (TabState)
	{
	case EChatTabState::Normal:
		Btn_ChatTab->SetBackgroundColor(FLinearColor::Gray);
		break;
		
	case EChatTabState::Unread: 
		Btn_ChatTab->SetBackgroundColor(FLinearColor::Green);
		break;
		
	case EChatTabState::Selected:
		Btn_ChatTab->SetBackgroundColor(FLinearColor::Blue);
		break;
		
	default:
		break;
	}
}

#pragma endregion 

#undef GLOBAL_TAB_TEXT
#undef PARTY_TAB_TEXT