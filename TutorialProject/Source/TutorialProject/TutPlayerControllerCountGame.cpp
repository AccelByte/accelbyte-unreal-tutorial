// Copyright (c) 2021 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.

#include "TutPlayerControllerCountGame.h"
#include "TutGameModeCountGame.h"
#include "AccelByte/Game/AccelByteGame.h"
#include "AccelByte/Game/AccelByteScoreboardEntry.h"

void ATutPlayerControllerCountGame::BeginPlay()
{
	Super::BeginPlay();
	
	if (!IsRunningDedicatedServer())
	{
		TWeakObjectPtr<UAccelByteGame> Widget = MakeWeakObjectPtr<UAccelByteGame>(CreateWidget<UAccelByteGame>(this, AccelByteGameClass.Get()));
		Widget->AddToViewport();
		Widget->SetFocus();

		this->SetShowMouseCursor(true);
		FInputModeUIOnly InputModeUIOnly;
		InputModeUIOnly.SetWidgetToFocus(Widget->TakeWidget());
		this->SetInputMode(InputModeUIOnly);

		GameWidget = Widget.Get();
	}
}

void ATutPlayerControllerCountGame::Server_RequestScoreIncrease_Implementation()
{
	Cast<ATutGameModeCountGame>(UGameplayStatics::GetGameMode(GetWorld()))->IncreaseScore(PlayerData.UserId);
}

void ATutPlayerControllerCountGame::Client_InitCountdownTimer_Implementation(uint8 PartyId)
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

void ATutPlayerControllerCountGame::Client_ReceiveGameOverEvent_Implementation(const uint8 WinnerPartyId)
{
	GameWidget->GameOver(WinnerPartyId);
}