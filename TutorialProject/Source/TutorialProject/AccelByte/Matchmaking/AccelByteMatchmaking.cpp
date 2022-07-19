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
#include "Components/TextBlock.h"
#include "TutorialProject/TutorialProjectUtilities.h"
#include "TutorialProject/AccelByte/Lobby/AccelByteLobby.h"
#include "TutorialProject/AccelByte/Server/AccelByteServerManager.h"

UAccelByteMatchmaking::UAccelByteMatchmaking(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer),
	IndexTeamA(0),
	IndexTeamB(1),
	MaximumPlayersEntry(4),
	ReadyConsentTimeOutInSec(0)
{
}

void UAccelByteMatchmaking::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	TutorialMenuHUD = Cast<ATutorialMenuHUD>(GetOwningPlayer()->GetHUD());
}

void UAccelByteMatchmaking::NativeConstruct()
{
	Super::NativeConstruct();
	
	Btn_ReadyMatch->SetIsEnabled(true);

	// Because this widget is only instantiate once, and construct multiple times, then we can set the timeout every time the widget is add to viewport
	ReadyConsentTimeOutInSec = TutorialProjectUtilities::DefaultReadyConsentTimeOut;
	
	Btn_ReadyMatch->OnClicked.AddUniqueDynamic(this, &UAccelByteMatchmaking::OnClickedReadyConsent);

	SetCountDownTimer();
}

void UAccelByteMatchmaking::SetCountDownTimer()
{
	T_Countdown->SetText(FText::FromString(FString::FromInt(ReadyConsentTimeOutInSec)));
	GetWorld()->GetTimerManager().SetTimer(CountDownTimer, FTimerDelegate::CreateWeakLambda(this, [this]()
	{
		ReadyConsentTimeOutInSec--;
		T_Countdown->SetText(FText::AsNumber(ReadyConsentTimeOutInSec));
		
		if (ReadyConsentTimeOutInSec == 0)
		{
			GetWorld()->GetTimerManager().ClearTimer(CountDownTimer);
		}
	}), 1.0f, true);
}

void UAccelByteMatchmaking::SetMatchmakingNotification()
{
	FRegistry::Lobby.SetReadyConsentNotifDelegate(Api::Lobby::FReadyConsentNotif::CreateUObject(this, &UAccelByteMatchmaking::OnReadyConsentNotification));
	FRegistry::Lobby.SetDsNotifDelegate(Api::Lobby::FDsNotif::CreateUObject(this, &UAccelByteMatchmaking::OnDsNotification));
	FRegistry::Lobby.SetRematchmakingNotifDelegate(Api::Lobby::FRematchmakingNotif::CreateUObject(this, &UAccelByteMatchmaking::OnPlayerTimeOutReadyConsent));
	FRegistry::Lobby.SetMatchmakingNotifDelegate(Api::Lobby::FMatchmakingNotif::CreateUObject(this, &UAccelByteMatchmaking::OnMatchmakingNotification));
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
			"");
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
	Btn_ReadyMatch->SetIsEnabled(false);
	FRegistry::Lobby.SetReadyConsentResponseDelegate(Api::Lobby::FReadyConsentResponse::CreateUObject(this, &UAccelByteMatchmaking::OnReadyConsentResponse));
	FRegistry::Lobby.SendReadyConsentRequest(MatchmakingNotice.MatchId);
}

void UAccelByteMatchmaking::OnMatchmakingStartSuccess(const FAccelByteModelsMatchmakingResponse& Result)
{
	Cast<ATutorialMenuHUD>(GetOwningPlayer()->GetHUD())->OpenFindMatchmakingPopUp();
	UE_LOG(LogTemp, Log, TEXT("Start Matchmaking Success!"));
}

void UAccelByteMatchmaking::OnMatchmakingStartFailed(const int32 Code, const FString& Message)
{
	UE_LOG(LogTemp, Error, TEXT("Start Matchmaking Failed! %i , %s"), Code, *Message);
}

void UAccelByteMatchmaking::OnMatchmakingNotification(const FAccelByteModelsMatchmakingNotice& Notice)
{
	switch(Notice.Status)
	{
	case EAccelByteMatchmakingStatus::Done:
		{
			UE_LOG(LogTemp, Log, TEXT("MATCH FOUND"));
		
			TutorialMenuHUD->CloseFindMatchmakingPopUp();
			TutorialMenuHUD->OpenMatchmakingMenu();

			ResetPlayerEntries();

			MatchmakingNotice = Notice;
			break;
		}
	case EAccelByteMatchmakingStatus::Unknown:
		{
			UE_LOG(LogTemp, Error, TEXT("UNKNOWN ERROR"));
			TutorialMenuHUD->CloseFindMatchmakingPopUp();
			break;
		}
	case EAccelByteMatchmakingStatus::Start:
		{
			UE_LOG(LogTemp, Log, TEXT("START MATCHMAKING"));
			TutorialMenuHUD->OpenFindMatchmakingPopUp();
			break;
		}
	case EAccelByteMatchmakingStatus::Timeout:
		{
			UE_LOG(LogTemp, Warning, TEXT("TIMEOUT"));
			TutorialMenuHUD->CloseFindMatchmakingPopUp();
			break;
		}
	case EAccelByteMatchmakingStatus::Cancel:
		{
			UE_LOG(LogTemp, Log, TEXT("CANCELING MATCHMAKING"));
			TutorialMenuHUD->CloseFindMatchmakingPopUp();
			break;
		}
	case EAccelByteMatchmakingStatus::Unavailable:
		{
			UE_LOG(LogTemp, Error, TEXT("MATCHMAKING UNAVAILABLE"));
			TutorialMenuHUD->CloseFindMatchmakingPopUp();
			break;
		}
	}
}

void UAccelByteMatchmaking::ResetPlayerEntries()
{
	// Reset all UI Entries
	TeamAPlayersEntry = {};
	TeamBPlayersEntry = {};
	SetDummyPlayerEntry(IndexTeamA);
	SetDummyPlayerEntry(IndexTeamB);
}

void UAccelByteMatchmaking::OnPlayerTimeOutReadyConsent(const FAccelByteModelsRematchmakingNotice& Notice)
{
	UE_LOG(LogTemp, Warning, TEXT("Some Player has not click ready consent after %d seconds, it has timed out. Exiting matchmaking queue. ban duration: %d seconds"), ReadyConsentTimeOutInSec, Notice.BanDuration);
	TutorialMenuHUD->OpenLobbyMenu();
	this->RemoveFromParent();
}

void UAccelByteMatchmaking::OnReadyConsentResponse(const FAccelByteModelsReadyConsentRequest& Result)
{
	if (Result.Code == "0")
	{
		UE_LOG(LogTemp, Log, TEXT("Ready Consent Success!"));
	}
	else
	{
		Btn_ReadyMatch->SetIsEnabled(true);
		UE_LOG(LogTemp, Error, TEXT("Ready Consent Error!"));
	}
}

void UAccelByteMatchmaking::OnReadyConsentNotification(const FAccelByteModelsReadyConsentNotice& Result)
{
	UE_LOG(LogTemp, Log, TEXT("Get Ready Consent Notice! User %s is ready for match."), *Result.UserId);
	
	FRegistry::User.GetUserByUserId(Result.UserId, THandler<FSimpleUserData>::CreateWeakLambda(this, [this](const FSimpleUserData& UserData)
	{
		UE_LOG(LogTemp, Log, TEXT("Get User by User ID Success, Display Name: %s."), *UserData.DisplayName);

		SetNewPlayerEntry(UserData);
	}), FErrorHandler::CreateWeakLambda(this, [](int32 Code, FString Message)
	{
		UE_LOG(LogTemp, Error, TEXT("Error Get User by User ID! Error Log:  %s"), *Message);
	}));
}

void UAccelByteMatchmaking::OnDsNotification(const FAccelByteModelsDsNotice& Result)
{
	FString IpAddress = Result.Ip;
	FString Port = FString::FromInt(Result.Port);
	
	const FString ServerAddress = IpAddress + ":" + Port + FString::Printf(TEXT("?PlayerId=%s"), *FRegistry::Credentials.GetUserId());
	
	UE_LOG(LogTemp, Log, TEXT("IP and Port : %s \n Result : %s"), *ServerAddress, *Result.Status);

	if (Result.Status == FString("busy") || Result.Status == FString("ready"))
	{
		GEngine->SetClientTravel(GetWorld(), *ServerAddress, ETravelType::TRAVEL_Absolute);
	}
}

void UAccelByteMatchmaking::SetNewPlayerEntry(const FSimpleUserData& UserData)
{
	UAccelByteMatchmakingPlayerEntry* AccelByteMatchmakingPlayerEntry = CreateWidget<UAccelByteMatchmakingPlayerEntry>(this, TutorialMenuHUD->GetMatchmakingPlayerEntryClass());
	AccelByteMatchmakingPlayerEntry->DisplayName = UserData.DisplayName;
	AccelByteMatchmakingPlayerEntry->UserId = UserData.UserId;
	
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
