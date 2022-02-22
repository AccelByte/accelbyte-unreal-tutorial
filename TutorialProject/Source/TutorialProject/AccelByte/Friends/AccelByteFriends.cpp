// Copyright (c) 2021 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.

#include "AccelByteFriends.h"
#include "Api/AccelByteLobbyApi.h"
#include "Api/AccelByteUserApi.h"
#include "Core/AccelByteRegistry.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/ScrollBox.h"
#include "TutorialProject/TutorialMenuHUD.h"
#include "TutorialProject/TutorialProjectUtilities.h"
#include "TutorialProject/AccelByte/Chat/AccelByteChat.h"
#include "TutorialProject/AccelByte/Chat/AccelByteChatTab.h"

void UAccelByteFriends::NativeConstruct()
{
	Super::NativeConstruct();
	
	check (GetOwningPlayer() != nullptr);
	check (GetOwningPlayer()->GetHUD() != nullptr);
	TutorialMenuHUD = Cast<ATutorialMenuHUD>(GetOwningPlayer()->GetHUD());
	
	Btn_FindFriend->OnClicked.AddUniqueDynamic(this, &UAccelByteFriends::OnClickedFindFriend);
	Btn_FriendList->OnClicked.AddUniqueDynamic(this, &UAccelByteFriends::OnClickedFriendList);
	Btn_PendingIncomingList->OnClicked.AddUniqueDynamic(this, &UAccelByteFriends::OnClickedPendingIncomingList);
	Btn_PendingSentList->OnClicked.AddUniqueDynamic(this, &UAccelByteFriends::OnClickedPendingOutgoingList);
	Btn_BlockList->OnClicked.AddUniqueDynamic(this, &UAccelByteFriends::OnClickedBlockList);
	Btn_Back->OnClicked.AddUniqueDynamic(this, &UAccelByteFriends::OnClickedBackToLobby);

	SetNotificationDelegate();
}

void UAccelByteFriends::SetNotificationDelegate()
{
	FRegistry::Lobby.SetOnFriendRequestAcceptedNotifDelegate(
		Lobby::FAcceptFriendsNotif::CreateLambda([this](const FAccelByteModelsAcceptFriendsNotif& Result)
		{
			if (CurrentState == ECurrentStateFriend::FRIEND_LIST)
			{
				CreateEntryWidget(EFriendEntryMode::Friend, Result.friendId);
			}
			else if (CurrentState == ECurrentStateFriend::OUTGOING_LIST)
			{
				FRegistry::Lobby.ListOutgoingFriends();
			}
		}));
		
	FRegistry::Lobby.SetOnUnfriendNotifDelegate(
		Lobby::FUnfriendNotif::CreateLambda([this](const FAccelByteModelsUnfriendNotif& Result)
		{
			if (CurrentState == ECurrentStateFriend::FRIEND_LIST)
			{
				FRegistry::Lobby.LoadFriendsList();
			}
			
			FRegistry::User.GetUserByUserId(
				Result.friendId,
				THandler<FSimpleUserData>::CreateLambda([this](const FSimpleUserData& UserData)
				{
					TutorialMenuHUD->GetChatMenu()->DeleteTabButtonWidget(EChatTabType::Private, UserData.UserId);
				}),
				FErrorHandler::CreateLambda([](int32 ErrorCode, const FString& ErrorMessage)
				{
					TutorialProjectUtilities::ShowLog(ELogVerbosity::Error, FString::Printf(TEXT("Friend| Failed Getting User Id. %d: %s"), ErrorCode, *ErrorMessage));
				}));
		}));
	
	FRegistry::Lobby.SetOnCancelFriendsNotifDelegate(
		Lobby::FCancelFriendsNotif::CreateLambda([this](const FAccelByteModelsCancelFriendsNotif& Result)
		{
			if (CurrentState == ECurrentStateFriend::INCOMING_LIST)
			{
				FRegistry::Lobby.ListIncomingFriends();
			}
		}));
	
	FRegistry::Lobby.SetOnRejectFriendsNotifDelegate(
		Lobby::FRejectFriendsNotif::CreateLambda([this](const FAccelByteModelsRejectFriendsNotif& Result)
		{
			if (CurrentState == ECurrentStateFriend::OUTGOING_LIST)
			{
				FRegistry::Lobby.ListOutgoingFriends();
			}
		}));
	
	FRegistry::Lobby.SetOnIncomingRequestFriendsNotifDelegate(
		Lobby::FRequestFriendsNotif::CreateLambda([this](const FAccelByteModelsRequestFriendsNotif& Result)
		{
			if (CurrentState == ECurrentStateFriend::INCOMING_LIST)
			{
				FRegistry::Lobby.ListIncomingFriends();
			}
		}));

	FRegistry::Lobby.SetBlockPlayerNotifDelegate(
		Lobby::FBlockPlayerNotif::CreateLambda([this](const FAccelByteModelsBlockPlayerNotif& Result)
		{
			if (CurrentState == ECurrentStateFriend::FRIEND_LIST)
			{
				FRegistry::Lobby.LoadFriendsList();
			}
		}));

	FRegistry::Lobby.SetUserPresenceNotifDelegate(
		Lobby::FFriendStatusNotif::CreateLambda([this](const FAccelByteModelsUsersPresenceNotice& Result)
		{
			for (const TWeakObjectPtr<UAccelByteFriendEntry>& WidgetEntry : FriendListWidgets)
			{
				if (Result.UserID == WidgetEntry.Get()->UserData.UserId)
				{
					SetUserPresence(WidgetEntry, Result.Availability);
				}
			}
		}));
	
	FRegistry::Lobby.SetGetOnlineFriendsPresenceResponseDelegate(Lobby::FGetAllFriendsStatusResponse::CreateUObject(this, &UAccelByteFriends::OnGetFriendsPresenceResponse));
}

void UAccelByteFriends::OnClickedFindFriend()
{
	TutorialMenuHUD->OpenFindFriendsMenu();
}

void UAccelByteFriends::OnClickedFriendList()
{	
	Sb_ContentList->ClearChildren();
	CurrentState = ECurrentStateFriend::FRIEND_LIST;
	
	FRegistry::Lobby.SetLoadFriendListResponseDelegate(Lobby::FLoadFriendListResponse::CreateUObject(this, &UAccelByteFriends::OnLoadFriendListResponse));
	FRegistry::Lobby.LoadFriendsList();
}

void UAccelByteFriends::OnClickedPendingIncomingList()
{	
	Sb_ContentList->ClearChildren();
	CurrentState = ECurrentStateFriend::INCOMING_LIST;
	
	FRegistry::Lobby.SetListIncomingFriendsResponseDelegate(Lobby::FListIncomingFriendsResponse::CreateUObject(this, &UAccelByteFriends::OnListIncomingFriendResponse));
	FRegistry::Lobby.ListIncomingFriends();
}

void UAccelByteFriends::OnClickedPendingOutgoingList()
{	
	Sb_ContentList->ClearChildren();
	CurrentState = ECurrentStateFriend::OUTGOING_LIST;
	
	FRegistry::Lobby.SetListOutgoingFriendsResponseDelegate(Lobby::FListOutgoingFriendsResponse::CreateUObject(this, &UAccelByteFriends::OnListOutgoingFriendResponse));
	FRegistry::Lobby.ListOutgoingFriends();
}

void UAccelByteFriends::OnClickedBlockList()
{	
	Sb_ContentList->ClearChildren();
	CurrentState = ECurrentStateFriend::BLOCKED_LIST;
	
	FRegistry::Lobby.GetListOfBlockedUsers(Lobby::FListBlockedUserResponse::CreateUObject(this, &UAccelByteFriends::OnSuccessRetrieveListBlockedUser),
		FErrorHandler::CreateUObject(this, &UAccelByteFriends::OnFailedRetrieveListBlockedUser));
}

void UAccelByteFriends::OnClickedBackToLobby()
{
	TutorialMenuHUD->CloseFriendMenu();
}

void UAccelByteFriends::OnGetFriendsPresenceResponse(const FAccelByteModelsGetOnlineUsersResponse& Result)
{
	if (Result.Code == "0")
	{
		TutorialProjectUtilities::ShowLog(ELogVerbosity::Log, TEXT("Success retrieve user presences!"));

		for (int Index = 0 ; Index < FriendListWidgets.Num()-1 ; ++Index)
		{
			SetUserPresence(FriendListWidgets[Index], Result.availability[Index]);
		}
	}
	else
	{
		TutorialProjectUtilities::ShowLog(ELogVerbosity::Error, TEXT("Unable to retrieve user presences!"));
	}
}

void UAccelByteFriends::OnLoadFriendListResponse(const FAccelByteModelsLoadFriendListResponse& Result)
{
	Sb_ContentList->ClearChildren();
	FriendListWidgets.Empty();
	
	if (Result.Code == "0")  
	{
		TutorialProjectUtilities::ShowLog(ELogVerbosity::Log, TEXT("Success Retrieve Load Friend List Response!"));

		for (const FString& FriendId : Result.friendsId)  
		{
			CreateEntryWidget(EFriendEntryMode::Friend, FriendId);
		}
		
		FRegistry::Lobby.SendGetOnlineFriendPresenceRequest();
	}
	else
	{
		TutorialProjectUtilities::ShowLog(ELogVerbosity::Error, TEXT("Unable to retrieve the your friends list!"));
	}
}


void UAccelByteFriends::OnListIncomingFriendResponse(const FAccelByteModelsListIncomingFriendsResponse& Result)
{
	Sb_ContentList->ClearChildren();
	FriendListWidgets.Empty();

	if (Result.Code == "0")  
	{
		TutorialProjectUtilities::ShowLog(ELogVerbosity::Log, TEXT("Success Retrieve List Incoming Friend Response!"));
		
		for (const FString& FriendId : Result.friendsId)  
		{
			CreateEntryWidget(EFriendEntryMode::Incoming, FriendId);
		}
	}
	else  
	{
		TutorialProjectUtilities::ShowLog(ELogVerbosity::Error, TEXT("Unable to retrieve the list of incoming friend requests!"));
	}
}

void UAccelByteFriends::OnListOutgoingFriendResponse(const FAccelByteModelsListOutgoingFriendsResponse& Result)
{
	Sb_ContentList->ClearChildren();
	FriendListWidgets.Empty();

	if (Result.Code == "0")  
	{
		TutorialProjectUtilities::ShowLog(ELogVerbosity::Log, TEXT("Success Retrieve List Outgoing Friend Response!"));
		
		for (const FString& FriendId : Result.friendsId)  
		{
			CreateEntryWidget(EFriendEntryMode::Outgoing, FriendId);
		}
	}
	else
	{
		TutorialProjectUtilities::ShowLog(ELogVerbosity::Error, TEXT("Unable to retrieve the list of outgoing friend requests!"));
	}  
}

void UAccelByteFriends::OnSuccessRetrieveListBlockedUser(const FAccelByteModelsListBlockedUserResponse& Result)
{
	Sb_ContentList->ClearChildren();
	FriendListWidgets.Empty();

	TutorialProjectUtilities::ShowLog(ELogVerbosity::Log, TEXT("Success Retrieve List Blocked User!"));

	for (const FBlockedData& BlockedData : Result.Data)  
	{
		CreateEntryWidget(EFriendEntryMode::Blocked, BlockedData.BlockedUserId);
	}
}

void UAccelByteFriends::OnFailedRetrieveListBlockedUser(int32 ErrorCode, const FString& ErrorMessage)
{
	TutorialProjectUtilities::ShowLog(ELogVerbosity::Error, FString::Printf(TEXT("Failed to Retrieve Blocked Users List : %d , %s"), ErrorCode, *ErrorMessage));
}

void UAccelByteFriends::CreateEntryWidget(const EFriendEntryMode& EntryMode, const FString& FriendId)
{
	const TWeakObjectPtr<UAccelByteFriendEntry> FriendEntryWidget = MakeWeakObjectPtr<UAccelByteFriendEntry>(CreateWidget<UAccelByteFriendEntry>(this, FriendEntryClass.Get()));
	
	FriendListWidgets.Add(FriendEntryWidget);
	
	FriendEntryWidget->InitData(EntryMode, FriendId);
	
	Sb_ContentList->AddChild(FriendEntryWidget.Get());
}

void UAccelByteFriends::SetUserPresence(const TWeakObjectPtr<UAccelByteFriendEntry> FriendEntry, const FString& Availability) const
{
	if (Availability == "1")
	{
		FriendEntry.Get()->SetPresenceText("Online");
	}
	else if (Availability == "2")
	{
		FriendEntry.Get()->SetPresenceText("Busy");
	}
	else
	{
		FriendEntry.Get()->SetPresenceText("Offline");
	}
}