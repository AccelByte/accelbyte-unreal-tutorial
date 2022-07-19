// Copyright (c) 2021 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.

#include "AccelByteFriendEntry.h"
#include "../Party/AccelByteParty.h"
#include "Api/AccelByteLobbyApi.h"
#include "Api/AccelByteUserApi.h"
#include "Core/AccelByteRegistry.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"
#include "Components/Image.h"
#include "Components/ScaleBox.h"
#include "Components/HorizontalBox.h"
#include "TutorialProject/TutorialMenuHUD.h"
#include "TutorialProject/AccelByte/Lobby/AccelByteLobby.h"

#pragma region Initilization

void UAccelByteFriendEntry::InitData(const EFriendEntryMode& EntryMode, const FString& UserId)
{
	TutorialMenuHUD = Cast<ATutorialMenuHUD>(GetOwningPlayer()->GetHUD());
	check(TutorialMenuHUD);
	
	FRegistry::User.GetUserByUserId(
		UserId,
		THandler<FSimpleUserData>::CreateUObject(this, &UAccelByteFriendEntry::OnSuccessGetUserId),
		FErrorHandler::CreateUObject(this, &UAccelByteFriendEntry::OnFailedGetUserId));

	SetEntryMode(EntryMode);
}

void UAccelByteFriendEntry::SetPresenceText(const FString& FriendPresence) const
{
	Tb_Status->SetText(FText::FromString(FriendPresence));
}

void UAccelByteFriendEntry::EnableAddFriendButton(bool bEnable) const
{
	Btn_AddFriend->SetIsEnabled(bEnable);
}

#pragma endregion

#pragma region Button Callbacks

void UAccelByteFriendEntry::OnClickedChat()
{
	TutorialMenuHUD->GetChatMenu()->CreateTabButtonWidget(EChatTabType::Private, UserData);
	TutorialMenuHUD->GetChatMenu()->SwitchChatTab(EChatTabType::Private, UserData.UserId);
	TutorialMenuHUD->CloseFriendMenu();
}

void UAccelByteFriendEntry::OnClickedInviteParty()
{
	TutorialMenuHUD->GetPartyMenu()->OnClickedInviteParty(UserData.UserId);
}

void UAccelByteFriendEntry::OnClickedUnfriend()
{
	FRegistry::Lobby.SetUnfriendResponseDelegate(Api::Lobby::FUnfriendResponse::CreateWeakLambda(this, [this](const FAccelByteModelsUnfriendResponse& Result)
	{
		UE_LOG(LogTemp, Log, TEXT("Successfully unfriend a friend!"));
		
		TutorialMenuHUD->GetChatMenu()->DeleteTabButtonWidget(EChatTabType::Private, UserData.UserId);
		
		this->RemoveFromParent();
	}),
	FErrorHandler::CreateWeakLambda(this, [](int32 ErrorCode, const FString& ErrorMessage)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to unfriend a friend! %d: %s"), ErrorCode, *ErrorMessage);
	}));
	
	FRegistry::Lobby.Unfriend(UserData.UserId);
}

void UAccelByteFriendEntry::OnClickedBlock()
{
	FRegistry::Lobby.SetBlockPlayerResponseDelegate(Api::Lobby::FBlockPlayerResponse::CreateUObject(this, &UAccelByteFriendEntry::OnBlockPlayerResponse));
	FRegistry::Lobby.BlockPlayer(UserData.UserId);  
}

void UAccelByteFriendEntry::OnClickedUnblock()
{
	FRegistry::Lobby.SetUnblockPlayerResponseDelegate(Api::Lobby::FUnblockPlayerResponse::CreateUObject(this, &UAccelByteFriendEntry::OnUnblockPlayerResponse));
	FRegistry::Lobby.UnblockPlayer(UserData.UserId);
}

void UAccelByteFriendEntry::OnClickedAccept()
{
	FRegistry::Lobby.SetAcceptFriendsResponseDelegate(Api::Lobby::FAcceptFriendsResponse::CreateUObject(this, &UAccelByteFriendEntry::OnAcceptFriendResponse));
	FRegistry::Lobby.AcceptFriend(UserData.UserId);
}

void UAccelByteFriendEntry::OnClickedDecline()
{
	FRegistry::Lobby.SetRejectFriendsResponseDelegate(Api::Lobby::FRejectFriendsResponse::CreateUObject(this, &UAccelByteFriendEntry::OnRejectFriendResponse));
	FRegistry::Lobby.RejectFriend(UserData.UserId);
}

void UAccelByteFriendEntry::OnClickedCancelRequest()
{
	FRegistry::Lobby.CancelFriendRequest(UserData.UserId);
	this->RemoveFromParent();
}

void UAccelByteFriendEntry::OnClickedAddFriend()
{
	FRegistry::Lobby.SetRequestFriendsResponseDelegate(Api::Lobby::FRequestFriendsResponse::CreateUObject(this, &UAccelByteFriendEntry::OnRequestFriendResponse));
	FRegistry::Lobby.RequestFriend(UserData.UserId);
	Btn_AddFriend->SetIsEnabled(false);
}

#pragma endregion

#pragma region Responses

void UAccelByteFriendEntry::OnSuccessGetUserId(const FSimpleUserData& Data)
{
	UE_LOG(LogTemp, Log, TEXT("Success Get User By User ID!"));

	UserData = Data;
	Tb_FriendName->SetText(FText::FromString(UserData.DisplayName));
}

void UAccelByteFriendEntry::OnFailedGetUserId(int32 ErrorCode, const FString& ErrorMessage)
{
	UE_LOG(LogTemp, Error, TEXT(" Get User Id Failed : Code: %d, Reason: %s"), ErrorCode, *ErrorMessage);
}

void UAccelByteFriendEntry::OnUnfriendResponse(const FAccelByteModelsUnfriendResponse& Result)
{
	if (Result.Code == "0")
	{
		UE_LOG(LogTemp, Log, TEXT("Successfully unfriend a friend!"));

		this->RemoveFromParent();
	}  
	else  
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to unfriend a friend! Code: %s"), *Result.Code);
	}  
}

void UAccelByteFriendEntry::OnBlockPlayerResponse(const FAccelByteModelsBlockPlayerResponse& Result)
{
	if (Result.Code == "0")
	{
		UE_LOG(LogTemp, Log, TEXT("Successfully block a friend!"));
		
		TutorialMenuHUD->GetChatMenu()->DeleteTabButtonWidget(EChatTabType::Private, UserData.UserId);
		
		this->RemoveFromParent();
	}  
	else  
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to block a friend! Code: %s"), *Result.Code);
	}  
}

void UAccelByteFriendEntry::OnUnblockPlayerResponse(const FAccelByteModelsUnblockPlayerResponse& Result)
{
	if (Result.Code == "0")
	{
		UE_LOG(LogTemp, Log, TEXT("Successfully Unblock User!"));
		
		this->RemoveFromParent();
	}  
	else  
	{
		UE_LOG(LogTemp, Error, TEXT("Cannot retrieve the list of outgoing/pending friend request!"));
	}
}

void UAccelByteFriendEntry::OnAcceptFriendResponse(const FAccelByteModelsAcceptFriendsResponse& Result)
{
	if (Result.Code == "0")
	{
		UE_LOG(LogTemp, Log, TEXT("Successfully accept a friend request!"));

		this->RemoveFromParent();
	}  
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to accept a friend request! Code: %s"), *Result.Code);
	}
}

void UAccelByteFriendEntry::OnRejectFriendResponse(const FAccelByteModelsRejectFriendsResponse& Result)
{
	if (Result.Code == "0")
	{
		UE_LOG(LogTemp, Log, TEXT("Successfully reject a friend request!"));

		this->RemoveFromParent();
	}  
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to reject a friend request! Code: %s"), *Result.Code);
	}
}

void UAccelByteFriendEntry::OnRequestFriendResponse(const FAccelByteModelsRequestFriendsResponse& Result)
{
	if (Result.Code == "0")
	{
		UE_LOG(LogTemp, Log, TEXT("Successfully send a friend request!"));
	}  
	else  
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to send a friend request! Code: %s"), *Result.Code);
	}
}

#pragma endregion

#pragma region Utilities

void UAccelByteFriendEntry::SetEntryMode(const EFriendEntryMode& EntryMode) const
{
	switch(EntryMode)
	{
	case EFriendEntryMode::Friend:
		Sb_Friend->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		Tb_Status->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		Ws_EntryMode->SetActiveWidget(Hb_Friend);
		
		Btn_Chat->OnClicked.AddUniqueDynamic(this, &UAccelByteFriendEntry::OnClickedChat);
		Btn_InviteParty->OnClicked.AddUniqueDynamic(this, &UAccelByteFriendEntry::OnClickedInviteParty);
		Btn_Unfriend->OnClicked.AddUniqueDynamic(this, &UAccelByteFriendEntry::OnClickedUnfriend);
		Btn_Block->OnClicked.AddUniqueDynamic(this, &UAccelByteFriendEntry::OnClickedBlock);
		return;
		
	case EFriendEntryMode::Incoming:
		Sb_Friend->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		Tb_Status->SetVisibility(ESlateVisibility::Collapsed);
		Ws_EntryMode->SetActiveWidget(Hb_IncomingRequest);
		
		Btn_Accept->OnClicked.AddUniqueDynamic(this, &UAccelByteFriendEntry::OnClickedAccept);
		Btn_Decline->OnClicked.AddUniqueDynamic(this, &UAccelByteFriendEntry::OnClickedDecline);
		Btn_Block_Request->OnClicked.AddUniqueDynamic(this, &UAccelByteFriendEntry::OnClickedBlock);
		return;
		
	case EFriendEntryMode::Outgoing:
		Sb_Friend->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		Tb_Status->SetVisibility(ESlateVisibility::Collapsed);
		Ws_EntryMode->SetActiveWidget(Hb_OutgoingRequest);
		
		Btn_CancelRequest->OnClicked.AddUniqueDynamic(this, &UAccelByteFriendEntry::OnClickedCancelRequest);
		return;
		
	case EFriendEntryMode::Blocked:
		Sb_Friend->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		Tb_Status->SetVisibility(ESlateVisibility::Collapsed);
		Ws_EntryMode->SetActiveWidget(Hb_Blocked);
		
		Btn_Unblock->OnClicked.AddUniqueDynamic(this, &UAccelByteFriendEntry::OnClickedUnblock);
		return;
		
	case EFriendEntryMode::Search:
		Sb_Friend->SetVisibility(ESlateVisibility::Collapsed);
		Tb_Status->SetVisibility(ESlateVisibility::Collapsed);
		Ws_EntryMode->SetActiveWidget(Hb_Search);
		
		Btn_AddFriend->OnClicked.AddUniqueDynamic(this, &UAccelByteFriendEntry::OnClickedAddFriend);
		return;
		
	default:
		Sb_Friend->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		Tb_Status->SetVisibility(ESlateVisibility::Hidden);
		Ws_EntryMode->SetActiveWidget(Hb_Friend);
		
		Btn_Chat->OnClicked.AddUniqueDynamic(this, &UAccelByteFriendEntry::OnClickedChat);
		Btn_InviteParty->OnClicked.AddUniqueDynamic(this, &UAccelByteFriendEntry::OnClickedInviteParty);
		Btn_Unfriend->OnClicked.AddUniqueDynamic(this, &UAccelByteFriendEntry::OnClickedUnfriend);
		Btn_Block->OnClicked.AddUniqueDynamic(this, &UAccelByteFriendEntry::OnClickedBlock);
	}
}

#pragma endregion 
