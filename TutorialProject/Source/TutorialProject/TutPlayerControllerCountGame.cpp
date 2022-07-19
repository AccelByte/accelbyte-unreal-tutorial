// Copyright (c) 2021 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.

#include "TutPlayerControllerCountGame.h"
#include "TutGameModeCountGame.h"
#include "TutorialProjectGameInstance.h"
#include "TutorialProjectUtilities.h"
#include "AccelByte/Game/AccelByteGame.h"
#include "AccelByte/Lobby/AccelByteLobby.h"
#include "AccelByte/Lobby/AccelByteNotificationObject.h"
#include "Core/AccelByteRegistry.h"

void ATutPlayerControllerCountGame::BeginPlay()
{
	Super::BeginPlay();
	
	if (!IsRunningDedicatedServer())
	{
		const TWeakObjectPtr<UAccelByteGame> Widget = MakeWeakObjectPtr<UAccelByteGame>(CreateWidget<UAccelByteGame>(this, AccelByteGameClass.Get()));
		Widget->AddToViewport();
		Widget->bIsFocusable = true;
		Widget->SetFocus();

		SetShowMouseCursor(true);
		FInputModeUIOnly InputModeUIOnly;
		InputModeUIOnly.SetWidgetToFocus(Widget->TakeWidget());
		SetInputMode(InputModeUIOnly);

		GameWidget = Widget.Get();
		
		if (FRegistry::Lobby.IsConnected())
		{
			UE_LOG(LogTemp, Log, TEXT("Lobby is Connected"));

			FRegistry::Lobby.SetMessageNotifDelegate(THandler<FAccelByteModelsNotificationMessage>::CreateLambda([this](FAccelByteModelsNotificationMessage Result)
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
						// Do something if Initiate Reward Notification is Error
					}));
				}
			}));
		}
	}
}

void ATutPlayerControllerCountGame::Server_RequestScoreIncrease_Implementation()
{
	Cast<ATutGameModeCountGame>(UGameplayStatics::GetGameMode(GetWorld()))->IncreaseScore(PlayerData.UserId);
}

void ATutPlayerControllerCountGame::Client_InitCountdownTimer_Implementation(const EPartyId& PartyId)
{
	GameWidget->InitCountdown(PartyId);
}

void ATutPlayerControllerCountGame::Client_ReceiveCountdownTimerUpdateEvent_Implementation(const int& TimeLeft)
{
	GameWidget->UpdateCountdownTimer(TimeLeft);
}

void ATutPlayerControllerCountGame::Client_ReceiveTimeUpdateEvent_Implementation(const int& TimeLeft)
{
	GameWidget->UpdateTimer(TimeLeft);
}

void ATutPlayerControllerCountGame::Client_ReceiveScoreIncreaseEvent_Implementation(const FString& PlayerId, const FPlayerData& CurrentPlayerData, const int32 IncrementScore)
{
	GameWidget->UpdateScore(PlayerId, CurrentPlayerData, IncrementScore);
}

void ATutPlayerControllerCountGame::Client_InitGameScoreboard_Implementation(const TArray<FPlayerData>& CurrentPlayerData)
{
	GameWidget->InitScoreboard(CurrentPlayerData);
}

void ATutPlayerControllerCountGame::Client_ReceiveGameOverEvent_Implementation(const EWinnerParty& WinnerParty)
{
	GameWidget->GameOver(WinnerParty);
}