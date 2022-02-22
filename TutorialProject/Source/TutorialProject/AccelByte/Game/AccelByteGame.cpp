// Copyright (c) 2021 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.

#include "AccelByteGame.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/ScrollBox.h"
#include "Components/HorizontalBox.h"
#include "Components/SizeBox.h"
#include "Components/WidgetSwitcher.h"

void UAccelByteGame::NativeConstruct()
{
	Super::NativeConstruct();

	Btn_IncreaseScore->OnClicked.AddUniqueDynamic(this, &UAccelByteGame::IncreaseScore);
}

void UAccelByteGame::OnBackToMainMenuClicked()
{
	GEngine->SetClientTravel(GetWorld(), *FString("MainMenu"), ETravelType::TRAVEL_Absolute);
}

void UAccelByteGame::IncreaseScore()
{
	Cast<ATutPlayerControllerCountGame>(GetOwningPlayer())->Server_RequestScoreIncrease();
}

void UAccelByteGame::InitCountdown(uint8 PartyId)
{
	MyPartyId = PartyId;
	Hb_WaitAllPlayersReady->SetVisibility(ESlateVisibility::Collapsed);
}

void UAccelByteGame::InitScoreboard(const TArray<FPlayerData>& CurrentPlayerDatas)
{
	B_Countdown->SetVisibility(ESlateVisibility::Collapsed);
	
	ScB_Scoreboard_Team_A->ClearChildren();
	ScB_Scoreboard_Team_B->ClearChildren();
	Tb_TotalScore_Team_A->SetText(FText::FromString(TEXT("0")));
	Tb_TotalScore_Team_B->SetText(FText::FromString(TEXT("0")));
	
	for (const FPlayerData& PlayerInfo : CurrentPlayerDatas)
	{
		const TWeakObjectPtr<UAccelByteScoreboardEntry> ScoreboardEntryWidget = MakeWeakObjectPtr<UAccelByteScoreboardEntry>(
			CreateWidget<UAccelByteScoreboardEntry>(GetGameInstance(), Cast<ATutPlayerControllerCountGame>(GetOwningPlayer())->AccelByteScoreboardEntryClass.Get())
			);

		ScoreboardEntryWidget->InitData(PlayerInfo);
		ScoreEntryWidgetMap.Add(PlayerInfo.UserId, ScoreboardEntryWidget.Get());
		
		if (PlayerInfo.PartyId == 0)
		{
			ScB_Scoreboard_Team_A->AddChild(ScoreboardEntryWidget.Get());
		}
		else
		{
			ScB_Scoreboard_Team_B->AddChild(ScoreboardEntryWidget.Get());
		}
	}
}

void UAccelByteGame::UpdateCountdownTimer(const int& CountdownTimerLeft)
{
	if (CountdownTimerLeft != 0)
	{
		T_Countdown->SetText(FText::FromString(FString::FromInt(CountdownTimerLeft)));
	}
	else
	{
		T_Countdown->SetText(FText::FromString(TEXT("GO !!")));
	}
}

void UAccelByteGame::UpdateTimer(const int& TimeLeft)
{
	FString TimeString = "Timeleft: " +  FString::FromInt(TimeLeft);
	T_Timer->SetText(FText::FromString(TimeString));
}

bool UAccelByteGame::IsUserInScoreEntryWidgetValid(const FString& PlayerId) const
{
	bool bIsUserInEntryMap = ScoreEntryWidgetMap.Contains(PlayerId);
	bool bIsEntryNull = ScoreEntryWidgetMap[PlayerId] == nullptr;
	
	return !bIsEntryNull && bIsUserInEntryMap;
}

void UAccelByteGame::UpdateScore(const FString& PlayerId, const FPlayerData& CurrentPlayerInfo, const int32 IncrementScore)
{
	if (IsUserInScoreEntryWidgetValid(PlayerId))
	{
		ScoreEntryWidgetMap[PlayerId]->SetScore(CurrentPlayerInfo.Score);
		
		if (CurrentPlayerInfo.PartyId == 0)
		{
			TeamAScore += IncrementScore;
		}
		else
		{
			TeamBScore += IncrementScore;
		}
		
		Tb_TotalScore_Team_A->SetText(FText::FromString(FString::FromInt(TeamAScore)));
		Tb_TotalScore_Team_B->SetText(FText::FromString(FString::FromInt(TeamBScore)));
	}
}

void UAccelByteGame::GameOver(const uint8 WinnerPartyId)
{
	WS_Team_A_WinOrLose->SetVisibility(ESlateVisibility::HitTestInvisible);
	WS_Team_B_WinOrLose->SetVisibility(ESlateVisibility::HitTestInvisible);

	if (WinnerPartyId == 0)
	{
		WS_Team_A_WinOrLose->SetActiveWidgetIndex(0);
		WS_Team_B_WinOrLose->SetActiveWidgetIndex(1);
	}
	else
	{
		WS_Team_A_WinOrLose->SetActiveWidgetIndex(1);
		WS_Team_B_WinOrLose->SetActiveWidgetIndex(0);
	}
	
	if (WinnerPartyId == MyPartyId)
	{
		WS_WinOrLose->SetActiveWidgetIndex(1);
	}
	else
	{
		WS_WinOrLose->SetActiveWidgetIndex(2);
	}

	Btn_BackToMainMenu->OnClicked.AddUniqueDynamic(this, &UAccelByteGame::OnBackToMainMenuClicked);
	
	Btn_IncreaseScore->SetIsEnabled(false);

	Sb_BackToMainMenu->SetVisibility(ESlateVisibility::Visible);
}
