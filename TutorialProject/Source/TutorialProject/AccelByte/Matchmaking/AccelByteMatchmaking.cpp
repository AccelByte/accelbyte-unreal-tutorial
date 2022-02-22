// Copyright (c) 2021 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.

#include "AccelByteMatchmaking.h"
#include "AccelByteMatchmakingPlayerEntry.h"
#include "Api/AccelByteLobbyApi.h"
#include "Api/AccelByteUserApi.h"
#include "Core/AccelByteRegistry.h"
#include "TutorialProject/TutorialMenuHUD.h"
#include "Components/Button.h"
#include "Components/ComboBoxString.h"
#include "Components/ListView.h"
#include "TutorialProject/TutorialProjectUtilities.h"
#include "TutorialProject/AccelByte/Lobby/AccelByteLobby.h"
#include "TutorialProject/AccelByte/Server/AccelByteServerManager.h"

void UAccelByteMatchmaking::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	Btn_ReadyMatch->OnClicked.AddUniqueDynamic(this, &UAccelByteMatchmaking::OnClickedReadyConsent);
	
	FRegistry::Lobby.SetMatchmakingNotifDelegate(Api::Lobby::FMatchmakingNotif::CreateUObject(this, &UAccelByteMatchmaking::OnMatchmakingNotification));
	FRegistry::Lobby.SetReadyConsentNotifDelegate(Api::Lobby::FReadyConsentNotif::CreateUObject(this, &UAccelByteMatchmaking::OnReadyConsentNotification));
	FRegistry::Lobby.SetDsNotifDelegate(Api::Lobby::FDsNotif::CreateUObject(this, &UAccelByteMatchmaking::OnDsNotification));

	TutorialMenuHUD = Cast<ATutorialMenuHUD>(GetOwningPlayer()->GetHUD());
}

void UAccelByteMatchmaking::NativeDestruct()
{
	Btn_ReadyMatch->OnClicked.Clear();
	FRegistry::Lobby.UnbindMatchmakingNotifEvents();

	Super::NativeDestruct();
}

void UAccelByteMatchmaking::OnClickedStartMatchmaking()
{
	FRegistry::Lobby.SetStartMatchmakingResponseDelegate(Api::Lobby::FMatchmakingResponse::CreateUObject(this, &UAccelByteMatchmaking::OnMatchmakingStartSuccess),
		FErrorHandler::CreateUObject(this, &UAccelByteMatchmaking::OnMatchmakingStartFailed));

	const FString GameMode = Cast<ATutorialMenuHUD>(GetOwningPlayer()->GetHUD())->GetLobbyMenu()->GetGameModeComboBoxSelectedOption();
	
	TMap<FString, FString> PartyAttributes;
	PartyAttributes.Add(*TutorialProjectUtilities::GameModeAttributeName, GameMode);
	
	UAccelByteServerManager* AccelByteServerManager = UAccelByteServerManager::CreateAccelByteServer(GetGameInstance());
	
	const UAccelByteLobby* LobbyMenu = Cast<ATutorialMenuHUD>(GetOwningPlayer()->GetHUD())->GetLobbyMenu();
	if(AccelByteServerManager->CheckCommandLineArgument(*TutorialProjectUtilities::LaunchArgsLocalDS))
	{
		FRegistry::Lobby.SendStartMatchmaking(
			GameMode,
			PartyAttributes,
			*TutorialProjectUtilities::LocalServerName,
			"",
			{LobbyMenu->GetSelectedLatency()});
	}
	else
	{
		FRegistry::Lobby.SendStartMatchmaking(
			GameMode,
			PartyAttributes,
			"",
			"",
			{LobbyMenu->GetSelectedLatency()});
	}
}

void UAccelByteMatchmaking::OnClickedReadyConsent()
{
	FRegistry::Lobby.SetReadyConsentResponseDelegate(Api::Lobby::FReadyConsentResponse::CreateUObject(this, &UAccelByteMatchmaking::OnReadyConsentResponse));
	FRegistry::Lobby.SendReadyConsentRequest(MatchmakingNotice.MatchId);
}

void UAccelByteMatchmaking::OnMatchmakingStartSuccess(const FAccelByteModelsMatchmakingResponse& Result)
{
	Cast<ATutorialMenuHUD>(GetOwningPlayer()->GetHUD())->OpenFindMatchmakingPopUp();
	TutorialProjectUtilities::ShowLog(ELogVerbosity::Log, FString::Printf(TEXT("Start Matchmaking Success!")));
}

void UAccelByteMatchmaking::OnMatchmakingStartFailed(const int32 Code, const FString& Message)
{
	TutorialProjectUtilities::ShowLog(ELogVerbosity::Error, FString::Printf(TEXT("Start Matchmaking Failed! %i , %s"), Code, *Message));
}

void UAccelByteMatchmaking::OnMatchmakingNotification(const FAccelByteModelsMatchmakingNotice& Notice)
{	
	switch(Notice.Status)
	{
	case EAccelByteMatchmakingStatus::Done:
		TutorialProjectUtilities::ShowLog(ELogVerbosity::Log, FString::Printf(TEXT("MATCH FOUND")));

		SetDummyPlayerEntry(IndexTeamA);
		SetDummyPlayerEntry(IndexTeamB);
		TutorialMenuHUD->CloseFindMatchmakingPopUp();
		TutorialMenuHUD->OpenMatchmakingMenu();

		MatchmakingNotice = Notice;
		break;
		
	case EAccelByteMatchmakingStatus::Unknown:
		TutorialProjectUtilities::ShowLog(ELogVerbosity::Error, FString::Printf(TEXT("UNKNOWN ERROR")));
		TutorialMenuHUD->CloseFindMatchmakingPopUp();
		break;
		
	case EAccelByteMatchmakingStatus::Start:
		TutorialProjectUtilities::ShowLog(ELogVerbosity::Log, FString::Printf(TEXT("START MATCHMAKING")));
		TutorialMenuHUD->OpenFindMatchmakingPopUp();
		break;
		
	case EAccelByteMatchmakingStatus::Timeout:
		TutorialProjectUtilities::ShowLog(ELogVerbosity::Warning, FString::Printf(TEXT("TIMEOUT")));
		TutorialMenuHUD->CloseFindMatchmakingPopUp();
		break;
		
	case EAccelByteMatchmakingStatus::Cancel:
		TutorialProjectUtilities::ShowLog(ELogVerbosity::Log, FString::Printf(TEXT("CANCELING MATCHMAKING")));
		TutorialMenuHUD->CloseFindMatchmakingPopUp();
		break;
		
	case EAccelByteMatchmakingStatus::Unavailable:
		TutorialProjectUtilities::ShowLog(ELogVerbosity::Error, FString::Printf(TEXT("MATCHMAKING UNAVAILABLE")));
		TutorialMenuHUD->CloseFindMatchmakingPopUp();
		break;
	}
}

void UAccelByteMatchmaking::OnReadyConsentResponse(const FAccelByteModelsReadyConsentRequest& Result)
{
	if (Result.Code == "0")
	{
		TutorialProjectUtilities::ShowLog(ELogVerbosity::Log, FString::Printf(TEXT("Ready Consent Success!")));
	}
	else
	{
		TutorialProjectUtilities::ShowLog(ELogVerbosity::Error, FString::Printf(TEXT("Ready Consent Error!")));
	}
}

void UAccelByteMatchmaking::OnReadyConsentNotification(const FAccelByteModelsReadyConsentNotice& Result)
{
	TutorialProjectUtilities::ShowLog(ELogVerbosity::Log, FString::Printf(TEXT("Get Ready Consent Notice! User %s is ready for match."), *Result.UserId));
	
	FRegistry::User.GetUserByUserId(Result.UserId, THandler<FSimpleUserData>::CreateLambda([this](const FSimpleUserData& UserData)
	{
		TutorialProjectUtilities::ShowLog(ELogVerbosity::Log, FString::Printf(TEXT("Get User by User ID Success, Display Name: %s."), *UserData.DisplayName));

		SetNewPlayerEntry(UserData);
	}), FErrorHandler::CreateLambda([](int32 Code, FString Message)
	{
		TutorialProjectUtilities::ShowLog(ELogVerbosity::Error, FString::Printf(TEXT("Error Get User by User ID! Error Log:  %s"), *Message));
	}));
}

void UAccelByteMatchmaking::OnDsNotification(const FAccelByteModelsDsNotice& Result)
{
	FString IpAddress = Result.Ip;
	FString Port = FString::FromInt(Result.Port);
	
	const FString ServerAddress = IpAddress + ":" + Port + FString::Printf(TEXT("?PlayerId=%s"), *FRegistry::Credentials.GetUserId());

	TutorialProjectUtilities::ShowLog(ELogVerbosity::Log, FString::Printf(TEXT("IP and Port : %s \n Result : %s"), *ServerAddress, *Result.Status));

	if (Result.Status == FString("busy") || Result.Status == FString("ready"))
	{
		GEngine->SetClientTravel(GetWorld(), *ServerAddress, ETravelType::TRAVEL_Absolute);
	}
}

void UAccelByteMatchmaking::SetNewPlayerEntry(const FSimpleUserData& UserData)
{
	UAccelByteMatchmakingPlayerEntry* AccelByteMatchmakingPlayerEntry = CreateWidget<UAccelByteMatchmakingPlayerEntry>(this, TutorialMenuHUD->GetMatchmakingPlayerEntryClass());
	AccelByteMatchmakingPlayerEntry->DisplayName = UserData.DisplayName;
	
	for (int32 Index = 0; Index < MatchmakingNotice.MatchingAllies.Data.Num(); Index++)
	{
		for (const FAccelByteModelsMatchingParty& MatchingParty : MatchmakingNotice.MatchingAllies.Data[Index].Matching_parties)
		{						
			for (const auto& PartyMember : MatchingParty.Party_members)
			{
				if (PartyMember.User_id == UserData.UserId)
				{
					if (Index == IndexTeamA)
					{
						TeamAPlayersEntry.Add(AccelByteMatchmakingPlayerEntry);
						SetDummyPlayerEntry(IndexTeamA);
					}
					else if (Index == IndexTeamB)
					{
						TeamBPlayersEntry.Add(AccelByteMatchmakingPlayerEntry);
						SetDummyPlayerEntry(IndexTeamB);
					}
					break;
				}
			}
		}
	}
}

void UAccelByteMatchmaking::SetDummyPlayerEntry(uint8 IndexTeam)
{
	int32 StartIndex;
	if (IndexTeam == IndexTeamA)
	{
		TArray<UAccelByteMatchmakingPlayerEntry*> TeamAEntries;
		StartIndex = TeamAPlayersEntry.Num();
		TeamAEntries.Append(TeamAPlayersEntry);
		for(int32 i = StartIndex; i < MaximumPlayersEntry; i ++)
		{
			TeamAEntries.Add(CreateWidget<UAccelByteMatchmakingPlayerEntry>(this, TutorialMenuHUD->GetMatchmakingPlayerEntryClass()));
		}
		Lv_Team_A->ClearListItems();
		Lv_Team_A->SetListItems(TeamAEntries);
	}
	else if (IndexTeam == IndexTeamB)
	{
		TArray<UAccelByteMatchmakingPlayerEntry*> TeamBEntires;
		StartIndex = TeamBPlayersEntry.Num();
		TeamBEntires.Append(TeamBPlayersEntry);
		for(int32 i = StartIndex; i < MaximumPlayersEntry; i ++)
		{
			TeamBEntires.Add(CreateWidget<UAccelByteMatchmakingPlayerEntry>(this, TutorialMenuHUD->GetMatchmakingPlayerEntryClass()));
		}
		Lv_Team_B->ClearListItems();
		Lv_Team_B->SetListItems(TeamBEntires);
	}
}
