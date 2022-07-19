// Copyright (c) 2021 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.

#include "AccelByteGame.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/ScrollBox.h"
#include "Components/HorizontalBox.h"
#include "Components/Overlay.h"
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

void UAccelByteGame::InitCountdown(const EPartyId& PartyId)
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
		
		if (PlayerInfo.PartyId == EPartyId::PartyA)
		{
			ScB_Scoreboard_Team_A->AddChild(ScoreboardEntryWidget.Get());
		}
		else if (PlayerInfo.PartyId == EPartyId::PartyB)
		{
			ScB_Scoreboard_Team_B->AddChild(ScoreboardEntryWidget.Get());
		}
	}
}

void UAccelByteGame::UpdateCountdownTimer(const int& CountdownTimerLeft) const
{
	if (CountdownTimerLeft != 0)
	{
		Tb_Countdown->SetText(FText::FromString(FString::FromInt(CountdownTimerLeft)));
	}
	else
	{
		Tb_Countdown->SetText(FText::FromString(TEXT("GO !!")));
	}
}

void UAccelByteGame::UpdateTimer(const int& TimeLeft) const
{
	const FString TimeString = "Time Left: " +  FString::FromInt(TimeLeft);
	Tb_Timer->SetText(FText::FromString(TimeString));
}

bool UAccelByteGame::IsUserInScoreEntryWidgetValid(const FString& PlayerId) const
{
	bool bIsUserInEntryMap = ScoreEntryWidgetMap.Contains(PlayerId);
	bool bIsEntryNull = false;
	if (ScoreEntryWidgetMap.Num() > 0)
	{
		bIsEntryNull = ScoreEntryWidgetMap[PlayerId] == nullptr;
	}
	
	return !bIsEntryNull && bIsUserInEntryMap;
}

void UAccelByteGame::UpdateScore(const FString& PlayerId, const FPlayerData& CurrentPlayerInfo, const int32 IncrementScore)
{
	if (IsUserInScoreEntryWidgetValid(PlayerId))
	{
		ScoreEntryWidgetMap[PlayerId]->SetScore(CurrentPlayerInfo.Score);
		
		if (CurrentPlayerInfo.PartyId == EPartyId::PartyA)
		{
			TeamAScore += IncrementScore;
		}
		else if (CurrentPlayerInfo.PartyId == EPartyId::PartyB)
		{
			TeamBScore += IncrementScore;
		}
		
		Tb_TotalScore_Team_A->SetText(FText::FromString(FString::FromInt(TeamAScore)));
		Tb_TotalScore_Team_B->SetText(FText::FromString(FString::FromInt(TeamBScore)));
	}
}

void UAccelByteGame::GameOver(const EWinnerParty& WinnerParty)
{
	WS_Team_A_WinOrLose->SetVisibility(ESlateVisibility::HitTestInvisible);
	WS_Team_B_WinOrLose->SetVisibility(ESlateVisibility::HitTestInvisible);

	if (WinnerParty == static_cast<EWinnerParty>(MyPartyId))
	{
		WS_WinOrLose->SetActiveWidget(Tb_YouWin);
	}
	else
	{
		WS_WinOrLose->SetActiveWidget(Tb_YouLose);
	}

	switch (WinnerParty)
	{
		case EWinnerParty::TeamA:
			{
				WS_Team_A_WinOrLose->SetActiveWidget(O_Team_A_Win);
				WS_Team_B_WinOrLose->SetActiveWidget(O_Team_B_Lose);
				break;
			}
		case EWinnerParty::TeamB:
			{
				WS_Team_A_WinOrLose->SetActiveWidget(O_Team_A_Lose);
				WS_Team_B_WinOrLose->SetActiveWidget(O_Team_B_Win);
				break;
			}
		case EWinnerParty::Draw:
			{
				WS_WinOrLose->SetActiveWidget(Tb_Draw);
				WS_Team_A_WinOrLose->SetActiveWidget(O_Team_A_Draw);
				WS_Team_B_WinOrLose->SetActiveWidget(O_Team_B_Draw);
				break;
			}
	}

	Btn_BackToMainMenu->OnClicked.AddUniqueDynamic(this, &UAccelByteGame::OnBackToMainMenuClicked);
	
	Btn_IncreaseScore->SetIsEnabled(false);

	Sb_BackToMainMenu->SetVisibility(ESlateVisibility::Visible);
}
