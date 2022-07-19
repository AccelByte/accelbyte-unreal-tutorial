// Copyright (c) 2021 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.

#include "AccelByteLobby.h"

#include "AccelByteNotificationObject.h"
#include "Api/AccelByteLobbyApi.h"
#include "Api/AccelByteQos.h"
#include "Core/AccelByteRegistry.h"
#include "Components/Button.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "Components/ComboBoxString.h"
#include "Components/SizeBox.h"
#include "TutorialProject/TutorialMenuHUD.h"
#include "TutorialProject/TutorialProjectGameInstance.h"
#include "TutorialProject/TutorialProjectUtilities.h"

#pragma region Initialization

void UAccelByteLobby::NativeConstruct()
{
	Super::NativeConstruct();

	check(GetOwningPlayer() != nullptr);
	check(GetOwningPlayer()->GetHUD() != nullptr);

	TutorialMenuHUD = Cast<ATutorialMenuHUD>(GetOwningPlayer()->GetHUD());

	ResetRegion();
	MatchmakingMenu = TutorialMenuHUD->GetMatchmakingMenu();

	Btn_LeaveLobby->OnClicked.AddUniqueDynamic(TutorialMenuHUD, &ATutorialMenuHUD::CloseLobbyMenu);
	Btn_FriendsManagement->OnClicked.AddUniqueDynamic(this, &UAccelByteLobby::OnClickedOpenFriendsManagement);
	Btn_FindMatch->OnClicked.AddUniqueDynamic(this, &UAccelByteLobby::OnClickedOpenFindMatch);
	Btn_RefreshLatencies->OnClicked.AddUniqueDynamic(this, &UAccelByteLobby::ResetRegion);
	Cbs_Gamemode->OnSelectionChanged.AddUniqueDynamic(this, &UAccelByteLobby::OnGameModeChanged);

	GetWorld()->GetTimerManager().SetTimer(MemberTimerHandle, this, &UAccelByteLobby::ResetRegion, RefreshTime, bNeedRefresh);
}

#pragma endregion

#pragma region Widget Callbacks

void UAccelByteLobby::OnClickedOpenFriendsManagement()
{
	TutorialMenuHUD->OpenFriendsMenu();
}

void UAccelByteLobby::OnClickedOpenFindMatch()
{
	MatchmakingMenu->OnClickedStartMatchmaking();
}

void UAccelByteLobby::OnGameModeChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	TutorialMenuHUD->GetPartyMenu()->CheckTotalMemberGameMode();
}

#pragma endregion

#pragma region Responses

void UAccelByteLobby::GetServerLatencies(const FGetServerLatencies& OnSuccess, const FErrorHandler& OnError)
{
	FRegistry::Qos.GetServerLatencies(
		THandler<TArray<TPair<FString, float>>>::CreateWeakLambda(this, [OnSuccess](const TArray<TPair<FString, float>>& Result)
			{
				UE_LOG(LogTemp, Log, TEXT("Get Server Latencies Success! Count: %d"), Result.Num());

				OnSuccess.ExecuteIfBound(Result);
			}),
		FErrorHandler::CreateWeakLambda(this, [OnError](int32 ErrorCode, const FString& ErrorMessage)
			{
				// Get Server Latencies Error
				UE_LOG(LogTemp, Warning, TEXT("Failed retrieving Server Latencies. %d: %s"), ErrorCode, *ErrorMessage);

				OnError.ExecuteIfBound(ErrorCode, ErrorMessage);
			}));
}

#pragma endregion

#pragma region Utilities

void UAccelByteLobby::ConnectToLobby()
{
	SetLobbyNotificationDelegate();
	FRegistry::Lobby.Connect();
}

void UAccelByteLobby::AddPartyChild(UAccelByteParty* PartyMenu) const
{
	Sb_Party->AddChild(PartyMenu);
}

void UAccelByteLobby::AddChatChild(UAccelByteChat* ChatMenu) const
{
	Sb_Chat->AddChild(ChatMenu);
}

void UAccelByteLobby::SetFindMatchEnabled(bool bIsFindMatchEnabled) const
{
	Btn_FindMatch->SetIsEnabled(bIsFindMatchEnabled);
}

void UAccelByteLobby::ShowErrorFindMatch(int TotalMemberToKick) const
{
	Tb_FindMatchError->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	Tb_FindMatchError->SetText(FText::FromString(FString::Printf(TEXT("Kick %s Player to Start a Match"), *FString::FromInt(TotalMemberToKick))));
}

void UAccelByteLobby::RemoveErrorFindMatch() const
{
	Tb_FindMatchError->SetVisibility(ESlateVisibility::Collapsed);
}

void UAccelByteLobby::DisableMatchmakingComponents() const
{
	SetFindMatchEnabled(false);
	SetFindMatchErrorVisibility(ESlateVisibility::Collapsed);
	SetGameModeComboBoxEnabled(false);
	SetGameModeComboBoxSelectedIndex(0);
}

void UAccelByteLobby::SetGameModeComboBoxEnabled(bool bIsComboBoxEnabled) const
{
	Cbs_Gamemode->SetIsEnabled(bIsComboBoxEnabled);
}

FString UAccelByteLobby::GetGameModeComboBoxSelectedOption() const
{
	return Cbs_Gamemode->GetSelectedOption();
}

int32 UAccelByteLobby::GetMaximumPartyMemberFromGameMode() const
{
	return Cbs_Gamemode->GetSelectedIndex() + 1;
}

TPair<FString, float> UAccelByteLobby::GetSelectedLatency() const
{
	return AvailableLatencies[Cbs_Region->GetSelectedIndex()];
}

void UAccelByteLobby::SetLobbyNotificationDelegate()
{
	FRegistry::Lobby.SetConnectSuccessDelegate(FSimpleDelegate::CreateWeakLambda(this, [this]()
	{
		UE_LOG(LogTemp, Log, TEXT("Successfully Connected to Lobby"));

		FRegistry::Lobby.SendSetPresenceStatus(Availability::Availabe, "Online");

		TutorialMenuHUD->GetPartyMenu()->SetCreatePartyButtonEnabled(true);
		TutorialMenuHUD->GetPartyMenu()->RefreshPartyEntries();

		TutorialMenuHUD->GetChatMenu()->JoinDefaultChannel();
	}));

	FRegistry::Lobby.SetConnectFailedDelegate(FErrorHandler::CreateWeakLambda(this, [](int32 Code, const FString& Message)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed Connect to Lobby : Code: %d; Message: %s"), Code, *Message);
	}));

	FRegistry::Lobby.SetErrorNotifDelegate(FErrorHandler::CreateWeakLambda(this, [](int32 Code, const FString& Message)
	{
		UE_LOG(LogTemp, Error, TEXT("Error Connect to Lobby : Code: %d; Message: %s"), Code, *Message);
	}));

	FRegistry::Lobby.SetConnectionClosedDelegate(Api::Lobby::FConnectionClosed::CreateWeakLambda(this, [this](int32 StatusCode, const FString& Reason, bool bWasClean)
	{
		UE_LOG(LogTemp, Error, TEXT("Connection Closed, Code: %d Reason: %s Clean: %s"), StatusCode, *Reason, bWasClean ? TEXT("true") : TEXT("false"));
		
		TutorialMenuHUD->OnCloseLobbyMenu();
		TutorialMenuHUD->GetChatMenu()->DeleteTabButtonWidget(EChatTabType::Global);
	}));

	FRegistry::Lobby.SetDisconnectNotifDelegate(Api::Lobby::FDisconnectNotif::CreateWeakLambda(this, [this](const FAccelByteModelsDisconnectNotif& Result)
	{
		UE_LOG(LogTemp, Log, TEXT("Disconnected from Lobby"));
		
		TutorialMenuHUD->OnCloseLobbyMenu();
	}));

    //TODO deprecate code duplication in ATutPlayerControllerCountGame::BeginPlay()
	FRegistry::Lobby.SetMessageNotifDelegate(THandler<FAccelByteModelsNotificationMessage>::CreateWeakLambda(this, [this](FAccelByteModelsNotificationMessage Result)
	{
		if (Result.Topic == TutorialProjectUtilities::RewardNotificationTopic)
		{
			UAccelByteNotificationObject* AccelByteNotificationObject = NewObject<UAccelByteNotificationObject>();
			check(AccelByteNotificationObject);
			AccelByteNotificationObject->InitRewardNotification(Result, FOnInitRewardSuccess::CreateWeakLambda(this, [this](const FRewardNotifInfo& Result)
			{
				UTutorialProjectGameInstance* TutorialProjectGameInstance = Cast<UTutorialProjectGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
				check(TutorialProjectGameInstance);
					
				TutorialProjectGameInstance->GetAchievementPopUp()->InitPopUpAchievement(Result.AchievementCode, Result.RewardItemIds, Result.CurrencyAmounts);
			}), FOnInitRewardError::CreateWeakLambda(this, [](const FString& ErrorMessage)
			{
				// Do something when initiate the reward is error
			}));
		}
	}));
}

void UAccelByteLobby::ResetRegion()
{
	GetServerLatencies(
		FGetServerLatencies::CreateWeakLambda(this, [this](const TArray<TPair<FString, float>>& Latencies)
		{
			UE_LOG(LogTemp, Log, TEXT("Qos| Get Server Latencies Success"));

			AvailableLatencies = Latencies;

			// Get Previous Latency Key from Combo Box
			if (Cbs_Region->GetOptionCount() > 0)
			{
				TArray<FString> LatencyKey;
				Cbs_Region->GetSelectedOption().ParseIntoArray(LatencyKey, TEXT(" ("), false);
				PreviousLatency.Key = LatencyKey[0];
			}

			// Clear Combo Box
			Cbs_Region->ClearOptions();

			// Assign Latencies to Combo Box
			for (const TPair<FString, float>& ServerLatency : AvailableLatencies)
			{
				const FString& CurrentOption = ServerLatency.Key + " (" + FString::FromInt(FMath::RoundToInt(ServerLatency.Value)) + "ms)";
				Cbs_Region->AddOption(CurrentOption);

				if (ServerLatency.Key == PreviousLatency.Key)
				{
					Cbs_Region->SetSelectedOption(CurrentOption);
					PreviousLatency = ServerLatency;
				}
			}

			// Append fastest Latency index on first time run
			if (bIsFirstTimeRun)
			{
				float FastestLatency = 99999.0f;
				int32 FastestIndex = 0;

				for (int32 Index = 0; Index < AvailableLatencies.Num(); ++Index)
				{
					if (FastestLatency > AvailableLatencies[Index].Value)
					{
						FastestIndex = Index;
						FastestLatency = AvailableLatencies[Index].Value;
					}
				}

				PreviousLatency = AvailableLatencies[FastestIndex];
				Cbs_Region->SetSelectedIndex(FastestIndex);
				bIsFirstTimeRun = false;
			}

			Cbs_Region->SetSelectedIndex(0);
		}),
		FErrorHandler::CreateWeakLambda(this, [](int32 ErrorCode, const FString& ErrorMessage)
		{
			UE_LOG(LogTemp, Error, TEXT("Qos| Error Get Server Latencies, Error Code: %d Error Message: %s"), ErrorCode, *ErrorMessage);
		}));
}

void UAccelByteLobby::SetFindMatchErrorVisibility(const ESlateVisibility& IsVisibility) const
{
	Tb_FindMatchError->SetVisibility(IsVisibility);
}

void UAccelByteLobby::SetGameModeComboBoxSelectedIndex(int32 SelectedIndex) const
{
	Cbs_Gamemode->SetSelectedIndex(SelectedIndex);
}

#pragma endregion