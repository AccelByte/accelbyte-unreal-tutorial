// Copyright (c) 2021 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.

#include "AccelbyteFindFriend.h"
#include "AccelByteFriends.h"
#include "Api/AccelByteLobbyApi.h"
#include "Api/AccelByteUserApi.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/ScrollBox.h"
#include "TutorialProject/TutorialMenuHUD.h"
#include "TutorialProject/TutorialProjectUtilities.h"

void UAccelByteFindFriend::NativeConstruct()
{
	Super::NativeConstruct();
	
	check (GetOwningPlayer() != nullptr);
	check (GetOwningPlayer()->GetHUD() != nullptr);

	TutorialMenuHUD = Cast<ATutorialMenuHUD>(GetOwningPlayer()->GetHUD());
	
	Btn_FindFriend->OnClicked.AddUniqueDynamic(this, &UAccelByteFindFriend::OnClickFindFriends);
	Btn_Close->OnClicked.AddUniqueDynamic(this, &UAccelByteFindFriend::OnClickCloseFindFriendWidget);
}

void UAccelByteFindFriend::OnClickFindFriends()
{
	Sb_FriendList->ClearChildren();
	
	FilterSearchEntryList();
}

void UAccelByteFindFriend::OnClickCloseFindFriendWidget()
{
	TutorialMenuHUD->CloseFindFriendsMenu();
}

void UAccelByteFindFriend::FilterSearchEntryList()
{
	FRegistry::Lobby.SetLoadFriendListResponseDelegate(Api::Lobby::FLoadFriendListResponse::CreateUObject(this, &UAccelByteFindFriend::OnLoadFriendListResponse));
	FRegistry::Lobby.SetListOutgoingFriendsResponseDelegate(Api::Lobby::FListOutgoingFriendsResponse::CreateUObject(this, &UAccelByteFindFriend::OnListOutgoingFriendResponse));

	FRegistry::Lobby.LoadFriendsList();
}

void UAccelByteFindFriend::OnLoadFriendListResponse(const FAccelByteModelsLoadFriendListResponse& Result)
{
	FriendSentRequestArray.Empty();
	
	if (Result.Code == "0")
	{
		TutorialProjectUtilities::ShowLog(ELogVerbosity::Log, TEXT("On Load Friend List Success!"));
		
		FriendSentRequestArray = Result.friendsId;

		FRegistry::Lobby.ListOutgoingFriends();
	}
	else
	{
		TutorialProjectUtilities::ShowLog(ELogVerbosity::Error, TEXT("Unable to retrieve friends list!"));
	}
}

void UAccelByteFindFriend::OnListOutgoingFriendResponse(const FAccelByteModelsListOutgoingFriendsResponse& Result)
{
	if (Result.Code == "0")  
	{
		TutorialProjectUtilities::ShowLog(ELogVerbosity::Log, TEXT("On List Outgoing Friends Success"));
		
		FriendSentRequestArray.Append(Result.friendsId);
		
		FindFriends(Etb_FindFriend->GetText().ToString());
	}
	else
	{
		TutorialProjectUtilities::ShowLog(ELogVerbosity::Error, TEXT("Unable to retrieve the list of outgoing/pending friend request!"));
	}
}

void UAccelByteFindFriend::FindFriends(const FString& FriendName)
{
	FRegistry::User.SearchUsers(
		FriendName,
		THandler<FPagedPublicUsersInfo>::CreateUObject(this, &UAccelByteFindFriend::OnSuccessFindFriends),
		FErrorHandler::CreateUObject(this, &UAccelByteFindFriend::OnFailedFindFriends)
	);
}

void UAccelByteFindFriend::OnSuccessFindFriends(const FPagedPublicUsersInfo& Result)
{
	TutorialProjectUtilities::ShowLog(ELogVerbosity::Log, TEXT("Success Find Friends"));

	Sb_FriendList->ClearChildren();
	
	for (const FPublicUserInfo& UserData : Result.Data)
	{
		if (UserData.UserId != FRegistry::Credentials.GetUserId())
		{
			const TWeakObjectPtr<UAccelByteFriendEntry> SearchFriendEntry = MakeWeakObjectPtr<UAccelByteFriendEntry>(
				CreateWidget<UAccelByteFriendEntry>(this, FriendEntryClass.Get())
				);
			
			SearchFriendEntry->InitData(EFriendEntryMode::Search, UserData.UserId);
			
			for (const FString& FriendSentRequest : FriendSentRequestArray)
			{
				if (UserData.UserId == FriendSentRequest)
				{
					SearchFriendEntry->EnableAddFriendButton(false);
				}
			}

			Sb_FriendList->AddChild(SearchFriendEntry.Get());
		}
	}  
}

void UAccelByteFindFriend::OnFailedFindFriends(int32 ErrorCode, const FString& ErrorMessage)
{
	TutorialProjectUtilities::ShowLog(ELogVerbosity::Error, FString::Printf(TEXT("Failed Find Friends : %d , %s"), ErrorCode, *ErrorMessage));
}
