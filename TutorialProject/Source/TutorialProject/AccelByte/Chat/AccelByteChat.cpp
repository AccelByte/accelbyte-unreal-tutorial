// Copyright (c) 2022 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.

#include "AccelByteChat.h"

#include "AccelByteChatEntry.h"
#include "AccelByteChatTab.h"
#include "Api/AccelByteLobbyApi.h"
#include "Api/AccelByteUserApi.h"
#include "Components/Button.h"
#include "Components/EditableText.h"
#include "Components/ScrollBox.h"
#include "Components/VerticalBox.h"
#include "Core/AccelByteRegistry.h"
#include "TutorialProject/TutorialProjectUtilities.h"

#pragma region Initialization

void UAccelByteChat::NativeConstruct()
{
	Super::NativeConstruct();
	
	SetWidgetCallbacks();
	SetNotificationDelegates();
	
	ResetChatComponents();
}

void UAccelByteChat::SetWidgetCallbacks()
{
	Btn_SendChat->OnClicked.AddUniqueDynamic(this, &UAccelByteChat::OnClickedSendChatButton);
	Btn_LeftTab->OnClicked.AddUniqueDynamic(this, &UAccelByteChat::OnClickedLeftTabButton);
	Btn_RightTab->OnClicked.AddUniqueDynamic(this, &UAccelByteChat::OnClickedRightTabButton);
	
	Et_Chat->OnTextCommitted.AddUniqueDynamic(this, &UAccelByteChat::OnTextCommittedChat);
}

void UAccelByteChat::SetNotificationDelegates()
{
	// On Joined Global Chat
	FRegistry::Lobby.SetJoinChannelChatResponseDelegate(
		Lobby::FJoinDefaultChannelChatResponse::CreateLambda([this](const FAccelByteModelsJoinDefaultChannelResponse& Result)
		{
			TutorialProjectUtilities::ShowLog(ELogVerbosity::Log, "Chat| Successfully joined Global Channel!");
			
			CurrentChatTab = EChatTabType::Global;
			CreateTabButtonWidget(EChatTabType::Global);
			TabButtonsArray[CurrentTabIndex]->OnClickedChatTabButton();
			
			const FString JoinInfo = FString::Printf(TEXT("You have joined [%s]."), *Result.ChannelSlug);
			AddMessageToArray(EChatTabType::Global, JoinInfo);
			CreateChatEntryWidget(JoinInfo);
		}));
	
	// Receive global chat notification
	FRegistry::Lobby.SetChannelMessageNotifDelegate(
		Lobby::FChannelChatNotif::CreateLambda([this](const FAccelByteModelsChannelMessageNotice& Result)
		{
			ReceiveChatMessage(EChatTabType::Global, Result.From, Result.Payload, Result.ChannelSlug);
		}));
	
	// Receive party chat notification
	FRegistry::Lobby.SetPartyChatNotifDelegate(
		Lobby::FPartyChatNotif::CreateLambda([this](const FAccelByteModelsPartyMessageNotice& Result)
		{
			ReceiveChatMessage(EChatTabType::Party, Result.From, Result.Payload);
		}));
	
	// Receive private chat notification
	FRegistry::Lobby.SetPrivateMessageNotifDelegate(
		Lobby::FPersonalChatNotif::CreateLambda([this](const FAccelByteModelsPersonalMessageNotice& Result)
		{
			TutorialProjectUtilities::ShowLog(ELogVerbosity::Type::Log, FString("Chat| Private Message Received"));
			if (PrivateTabButtonsMap.Contains(FName(Result.From)))
			{
				ReceiveChatMessage(EChatTabType::Private, Result.From, Result.Payload);
			}
			else
			{
				FRegistry::User.GetUserByUserId(
					Result.From,
					THandler<FSimpleUserData>::CreateLambda([this, Result](const FSimpleUserData& UserData)
					{
						CreateTabButtonWidget(EChatTabType::Private, UserData);
						ReceiveChatMessage(EChatTabType::Private, Result.From, Result.Payload);
					}),
					FErrorHandler::CreateLambda([](int32 ErrorCode, const FString& ErrorMessage)
					{
						TutorialProjectUtilities::ShowLog(ELogVerbosity::Error,
							FString::Printf(TEXT("Chat| Failed Getting User Data! %d: %s"), ErrorCode, *ErrorMessage));
					}));
			}
		}));
}

#pragma endregion

#pragma region Widget Creation

void UAccelByteChat::CreateTabButtonWidget(const EChatTabType& ChatTabType, const FSimpleUserData& UserData)
{
	const FName UserId = FName(UserData.UserId);
	
	// Checks if tab button already exists for private chats.
	if (ChatTabType == EChatTabType::Private && PrivateTabButtonsMap.Contains(UserId))
	{
		return;
	}
	
	UAccelByteChatTab* ChatTabWidget = CreateWidget<UAccelByteChatTab>(this, *AccelByteChatTabClass);
	
	if (ChatTabType == EChatTabType::Private)
	{
		ChatTabWidget->InitData(this, ChatTabType, UserData);
		ChatTabWidget->SetButtonColorState(EChatTabState::Unread);
		
		// Add Widget to Array in Private Buttons Map.
		TArray<UAccelByteChatTab*>* PrivateTabs =
			PrivateTabButtonsMap.Contains(UserId) ? PrivateTabButtonsMap.Find(UserId) : new TArray<UAccelByteChatTab*>;
		PrivateTabs->Add(ChatTabWidget);
		PrivateTabButtonsMap.Emplace(UserData.UserId, *PrivateTabs);
	}
	else
	{
		ChatTabWidget->InitData(this, ChatTabType);
		ChatTabWidget->SetButtonColorState(EChatTabState::Normal);
	}
	
	Sb_TabButtons->AddChild(ChatTabWidget);
	TabButtonsArray.Add(ChatTabWidget);
}

void UAccelByteChat::DeleteTabButtonWidget(const EChatTabType& ChatTabType, const FString& UserId)
{
	for (int32 Index = 0; Index < TabButtonsArray.Num(); ++Index)
	{
		if (TabButtonsArray[Index]->TabButtonType == ChatTabType)
		{
			if (ChatTabType == EChatTabType::Private)
			{
				if (UserId == TabButtonsArray[Index]->UserData.UserId)
				{
					const FName PrivateUserId = FName(UserId);
					
					check(PrivateTabButtonsMap.Contains(PrivateUserId));
					PrivateTabButtonsMap.Remove(PrivateUserId);
					
					Sb_TabButtons->RemoveChild(TabButtonsArray[Index]);
					TabButtonsArray.RemoveAt(Index);
					break;
				}
			}
			else
			{
				ChatContentMap.Remove(TabButtonsArray[Index]->TabButtonType);
				Sb_TabButtons->RemoveChild(TabButtonsArray[Index]);
				TabButtonsArray.RemoveAt(Index);
				break;
			}
		}
	}
}

void UAccelByteChat::SetTabButtonState(const EChatTabState& ChatTabState, const EChatTabType& ChatTabType,
	const FString& UserId)
{
	for (const UAccelByteChatTab* TabButtonWidget : TabButtonsArray)
	{
		if (TabButtonWidget->TabButtonType == ChatTabType)
		{
			if (ChatTabType == EChatTabType::Private)
			{
				if (UserId == TabButtonWidget->UserData.UserId)
				{
					TabButtonWidget->SetButtonColorState(ChatTabState);
					break;
				}
			}
			else
			{
				TabButtonWidget->SetButtonColorState(ChatTabState);
				break;
			}
		}
	}
}

void UAccelByteChat::CreateChatEntryWidget(const FString& ChatContent)
{
	UAccelByteChatEntry* ChatEntryWidget = CreateWidget<UAccelByteChatEntry>(this, AccelByteChatEntryClass.Get());
	
	ChatEntryWidget->InitData(ChatContent);
	
	ChatEntriesArray.Add(ChatEntryWidget);
	Vb_ChatContent->AddChild(ChatEntryWidget);
}

#pragma endregion 

#pragma region Widget Callbacks

void UAccelByteChat::OnClickedTabButton(const EChatTabType& ChatTabType, const FString& UserId)
{
	SwitchChatTab(ChatTabType, UserId);
}

void UAccelByteChat::OnClickedSendChatButton()
{
	SendChat(Et_Chat->GetText().ToString());

	Et_Chat->SetFocus();
}

void UAccelByteChat::OnClickedLeftTabButton()
{
	if (CurrentTabIndex > 0)
	{
		// Set previous tab button color to normal.
		TabButtonsArray[CurrentTabIndex]->SetButtonColorState(EChatTabState::Normal);
		
		--CurrentTabIndex;

		check(TabButtonsArray[CurrentTabIndex] != nullptr);
		TabButtonsArray[CurrentTabIndex]->OnClickedChatTabButton();
	}
}

void UAccelByteChat::OnClickedRightTabButton()
{
	if (CurrentTabIndex < TabButtonsArray.Num() - 1)
	{
		// Set previous tab button color to normal.
		TabButtonsArray[CurrentTabIndex]->SetButtonColorState(EChatTabState::Normal);
		
		++CurrentTabIndex;
		
		check(TabButtonsArray[CurrentTabIndex] != nullptr);
		TabButtonsArray[CurrentTabIndex]->OnClickedChatTabButton();
	}
}

void UAccelByteChat::OnTextCommittedChat(const FText& ChatContent, ETextCommit::Type TextCommitType)
{
	if (TextCommitType == ETextCommit::OnEnter)
	{
		SendChat(ChatContent.ToString());

		Et_Chat->SetFocus();
	}
}

#pragma endregion

#pragma region Utilities

void UAccelByteChat::JoinDefaultChannel() const
{
	FRegistry::Lobby.SendJoinDefaultChannelChatRequest();
}

void UAccelByteChat::SwitchChatTab(const EChatTabType& ChatTabType, const FString& UserId)
{
	CurrentChatTab = ChatTabType;
	
	ChatEntriesArray.Empty();
	Vb_ChatContent->ClearChildren();
	
	TabButtonsArray[CurrentTabIndex]->SetButtonColorState(EChatTabState::Normal);
	for (int32 TabIndex = 0; TabIndex < TabButtonsArray.Num(); ++TabIndex)
	{
		if (ChatTabType == EChatTabType::Private)
		{
			if (UserId == TabButtonsArray[TabIndex]->UserData.UserId)
			{
				CurrentTabIndex = TabIndex;
				TabButtonsArray[TabIndex]->SetButtonColorState(EChatTabState::Selected);
			}
		}
		else if (ChatTabType == TabButtonsArray[TabIndex]->TabButtonType)
		{
			CurrentTabIndex = TabIndex;
			TabButtonsArray[TabIndex]->SetButtonColorState(EChatTabState::Selected);
		}
	}
	
	switch (ChatTabType)
	{
	case EChatTabType::Global:
		TutorialProjectUtilities::ShowLog(ELogVerbosity::Type::Log, FString("Chat| Switched to Global chat"));
		
		if (ChatContentMap.Contains(EChatTabType::Global))
		{
			for (const FString& ChatMessage : ChatContentMap[EChatTabType::Global])
			{
				CreateChatEntryWidget(ChatMessage);
			}
		}
		break;
		
	case EChatTabType::Party:
		TutorialProjectUtilities::ShowLog(ELogVerbosity::Type::Log, FString("Chat| Switched to Party chat"));
		
		if (ChatContentMap.Contains(EChatTabType::Party))
		{
			for (const FString& ChatMessage : ChatContentMap[EChatTabType::Party])
			{
				CreateChatEntryWidget(ChatMessage);
			}
		}
		break;
		
	case EChatTabType::Private:
		TutorialProjectUtilities::ShowLog(ELogVerbosity::Type::Log, FString::Printf(TEXT("Chat| Switched to Private chat: %s"), *UserId));
		
		CurrentPrivateChatId = UserId;
		
		if (PrivateChatContentMap.Contains(FName(UserId)))
		{
			for (const FString& ChatMessage : PrivateChatContentMap[FName(UserId)])
			{
				CreateChatEntryWidget(ChatMessage);
			}
		}
		break;
		
	default:
		break;
	}
}

void UAccelByteChat::SendChat(const FString& ChatMessage)
{
	// Returns if Editable text box is empty.
	if (ChatMessage.IsEmpty())
	{
		return;
	}
	
	switch (CurrentChatTab)
	{
	case EChatTabType::Global: 
		TutorialProjectUtilities::ShowLog(ELogVerbosity::Type::Log, FString("Chat| Message sent to Global Chat"));
		
		FRegistry::Lobby.SendChannelMessage(ChatMessage);
		break;
		
	case EChatTabType::Party:
		{
			TutorialProjectUtilities::ShowLog(ELogVerbosity::Type::Log, FString("Chat| Message sent to Party Chat"));
			
			const FString PartyMessage = FString::Printf(TEXT("[Party] Me> %s"), *ChatMessage);
			AddMessageToArray(EChatTabType::Party, PartyMessage);
			
			CreateChatEntryWidget(PartyMessage);
			
			FRegistry::Lobby.SendPartyMessage(ChatMessage);
		}
		break;
		
	case EChatTabType::Private:
		{
			TutorialProjectUtilities::ShowLog(ELogVerbosity::Type::Log, FString::Printf(TEXT("Message sent to Private Chat: %s"), *CurrentPrivateChatId));
			
			const FString PrivateMessage = FString::Printf(TEXT("[Private] Me> %s"), *ChatMessage);
			AddMessageToArray(EChatTabType::Private, PrivateMessage, CurrentPrivateChatId);
			
			CreateChatEntryWidget(PrivateMessage);
			
			FRegistry::Lobby.SendPrivateMessage(CurrentPrivateChatId, ChatMessage);
		}
		break;
		
	default:
		TutorialProjectUtilities::ShowLog(ELogVerbosity::Type::Error, FString("Chat| Unable to send message"));
		break;
	}
		
	Et_Chat->SetText(FText::FromString(""));
}

void UAccelByteChat::ReceiveChatMessage(const EChatTabType& ChatTabType, const FString& UserId, const FString& Payload,
	const FString& ChannelSlug)
{
	const FName SenderId = FName(UserId);
	if (DisplayNameMap.Contains(SenderId))
	{
		FString MessageToShow;
		switch (ChatTabType)
		{
		case EChatTabType::Global:
			MessageToShow = FString::Printf(TEXT("[%s] %s> %s"), *ChannelSlug, *DisplayNameMap[SenderId], *Payload);
			AddMessageToArray(EChatTabType::Global, MessageToShow);
			break;
			
		case EChatTabType::Party:
			MessageToShow = FString::Printf(TEXT("[Party] %s> %s"), *DisplayNameMap[SenderId], *Payload);
			AddMessageToArray(EChatTabType::Party, MessageToShow);
			break;
			
		case EChatTabType::Private:
			MessageToShow = FString::Printf(TEXT("[Private] %s> %s"), *DisplayNameMap[SenderId], *Payload);
			AddMessageToArray(EChatTabType::Private, MessageToShow, UserId);
			break;
			
		default:
			break;
		}

		// Create Widget when tab is opened.
		if (CurrentChatTab == ChatTabType)
		{
			if (ChatTabType == EChatTabType::Private)
			{
				if (UserId == CurrentPrivateChatId)
				{
					CreateChatEntryWidget(MessageToShow);
				}
			}
			else
			{
				CreateChatEntryWidget(MessageToShow);
			}
		}
		else
		{
			SetTabButtonState(EChatTabState::Unread, ChatTabType, UserId);
		}
		
		Sb_Chat->ScrollToEnd();
	}
	else
	{
		FRegistry::User.GetUserByUserId(
			UserId,
			THandler<FSimpleUserData>::CreateLambda([this, ChatTabType, Payload, ChannelSlug](const FSimpleUserData& UserData)
			{
				DisplayNameMap.Emplace(UserData.UserId, UserData.DisplayName);
				
				ReceiveChatMessage(ChatTabType, UserData.UserId, Payload, ChannelSlug);
			}),
			FErrorHandler::CreateLambda([](int32 ErrorCode, const FString& ErrorMessage)
			{
				TutorialProjectUtilities::ShowLog(ELogVerbosity::Error,
					FString::Printf(TEXT("Chat| Failed Getting User Data! %d: %s"), ErrorCode, *ErrorMessage));
			}));
	}
}

void UAccelByteChat::AddMessageToArray(const EChatTabType& ChatTabType, const FString& ChatMessage, const FString& UserId)
{
	if (ChatTabType == EChatTabType::Private)
	{
		TArray<FString>* PrivateMessageArray = PrivateChatContentMap.Contains(FName(UserId)) ? PrivateChatContentMap.Find(FName(UserId)) : new TArray<FString>;
		PrivateMessageArray->Add(ChatMessage);
		PrivateChatContentMap.Emplace(UserId, *PrivateMessageArray);
	}
	else
	{
		TArray<FString>* MessageArray = ChatContentMap.Contains(ChatTabType) ? ChatContentMap.Find(ChatTabType) : new TArray<FString>;
		MessageArray->Add(ChatMessage);
		ChatContentMap.Emplace(ChatTabType, *MessageArray);
	}
}

void UAccelByteChat::ResetChatComponents()
{
	CurrentTabIndex = 0;
	
	ChatEntriesArray.Empty();
	Vb_ChatContent->ClearChildren();
	
	ChatContentMap.Empty();
	PrivateChatContentMap.Empty();
	
	TabButtonsArray.Empty();
	PrivateChatContentMap.Empty();
	PrivateTabButtonsMap.Empty();
	Sb_TabButtons->ClearChildren();
}

#pragma endregion 