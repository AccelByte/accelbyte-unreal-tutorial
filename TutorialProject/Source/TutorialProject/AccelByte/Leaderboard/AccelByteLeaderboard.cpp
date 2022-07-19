// Copyright (c) 2022 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.

#include "AccelByteLeaderboard.h"

#include "AccelByteLeaderboardRankingEntry.h"
#include "Api/AccelByteLeaderboardApi.h"
#include "Api/AccelByteUserApi.h"
#include "Core/AccelByteRegistry.h"
#include "Components/Button.h"
#include "Components/ListView.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/WidgetSwitcher.h"
#include "TutorialProject/TutorialMenuHUD.h"

#define ERROR_NO_USER_RANKING_FOUND 71233

void UAccelByteLeaderboard::NativeConstruct()
{
	Super::NativeConstruct();

	TutorialMenuHUD = Cast<ATutorialMenuHUD>(GetOwningPlayer()->GetHUD());

	Btn_ClickLeaderboard->OnClicked.AddUniqueDynamic(this, &UAccelByteLeaderboard::OnClickClickLeaderboard);
	Btn_WinLeaderboard->OnClicked.AddUniqueDynamic(this, &UAccelByteLeaderboard::OnClickWinLeaderboard);
	Btn_SpenderLeaderboard->OnClicked.AddUniqueDynamic(this, &UAccelByteLeaderboard::OnClickSpenderLeaderboard);

	Btn_AllTime->OnClicked.AddUniqueDynamic(this, &UAccelByteLeaderboard::OnClickRankingsAllTime);
	Btn_Daily->OnClicked.AddUniqueDynamic(this, &UAccelByteLeaderboard::OnClickRankingsDaily);
	Btn_Weekly->OnClicked.AddUniqueDynamic(this, &UAccelByteLeaderboard::OnClickRankingsWeekly);
	Btn_Seasonal->OnClicked.AddUniqueDynamic(this, &UAccelByteLeaderboard::OnClickRankingsSeasonal);
	
	Btn_BackToMainMenu->OnClicked.AddUniqueDynamic(TutorialMenuHUD, &ATutorialMenuHUD::CloseLeaderboardMenu);
	Btn_BackToLeaderboardList->OnClicked.AddUniqueDynamic(this, &UAccelByteLeaderboard::OnClickBackToLeaderboardList);
}

void UAccelByteLeaderboard::ResetWidget() const
{
	SwitchPanel(LeaderboardList);
}

void UAccelByteLeaderboard::RequestRankings(const EAccelByteLeaderboardTimeFrame& TimeFrame)
{
	SwitchEnabledButton(TimeFrame);
	
	FRegistry::Leaderboard.GetRankings(
		LeaderboardCode,
		TimeFrame,
		0,
		LeaderboardLimit,
		THandler<FAccelByteModelsLeaderboardRankingResult>::CreateWeakLambda(this, [TimeFrame, this](const FAccelByteModelsLeaderboardRankingResult& Response)
		{
			UE_LOG(LogTemp, Log, TEXT("Success get rankings"));
			
			TArray<FString> UserIds;

			for (const FAccelByteModelsUserPoint& UserData : Response.data)
			{
				UserIds.Add(UserData.userId);
			}

			// bulk get username from user ids
			FRegistry::User.BulkGetUserInfo(
				UserIds,
				THandler<FListBulkUserInfo>::CreateWeakLambda(this, [Response, TimeFrame, this](const FListBulkUserInfo& UserInfosResponse)
				{
					UE_LOG(LogTemp, Log, TEXT("Success bulk get users info"));

					ClearRankings();

					for (int index = 0; index < Response.data.Num(); ++index)
					{
						const FBaseUserInfo* CurrentUserInfo = UserInfosResponse.Data.FindByPredicate([Response, index](FBaseUserInfo Entry)
						{ 
							return Entry.UserId == Response.data[index].userId; 
						});
						

						const FRankingInfo RankingInfo = {
							index,
							FText::FromString(CurrentUserInfo == nullptr? "" : CurrentUserInfo->DisplayName),
							FText::AsNumber(Response.data[index].point),
							Response.data[index].userId == FRegistry::Credentials.GetUserId()
						};

						Rankings.Add(RankingInfo);
					}

					bool bIsUserInLeaderboard = Response.data.ContainsByPredicate([](FAccelByteModelsUserPoint Entry)
					{
						return Entry.userId == FRegistry::Credentials.GetUserId();
					});

					// get current user ranking if not in current leaderboard
					if (!bIsUserInLeaderboard && !UserRankingData.userId.IsEmpty())
					{
						FRankingInfo UserRanking = {};
						
						UserRanking.DisplayName = FText::FromString(FRegistry::Credentials.GetUserDisplayName());
						UserRanking.bIsPlayer = true;
						
						switch (TimeFrame)
						{
						case EAccelByteLeaderboardTimeFrame::TODAY:
							UserRanking.Ranking = UserRankingData.daily.rank;
							UserRanking.Point = FText::AsNumber(UserRankingData.daily.point);
							break;
						case EAccelByteLeaderboardTimeFrame::ALL_TIME:
							UserRanking.Ranking = UserRankingData.allTime.rank;
							UserRanking.Point = FText::AsNumber(UserRankingData.allTime.point);
							break;
						case EAccelByteLeaderboardTimeFrame::CURRENT_WEEK:
							UserRanking.Ranking = UserRankingData.weekly.rank;
							UserRanking.Point = FText::AsNumber(UserRankingData.weekly.point);
							break;
						case EAccelByteLeaderboardTimeFrame::CURRENT_MONTH:
							UserRanking.Ranking = UserRankingData.monthly.rank;
							UserRanking.Point = FText::AsNumber(UserRankingData.monthly.point);
							break;
						case EAccelByteLeaderboardTimeFrame::CURRENT_SEASON:
							UserRanking.Ranking = UserRankingData.current.rank;
							UserRanking.Point = FText::AsNumber(UserRankingData.current.point);
							break;
						}

						// Check if the current point is 0, then we are no putting the current user into the board
						if (!UserRanking.Point.EqualTo(FText::FromString("0")))
						{
							if (Rankings.Num() < LeaderboardLimit)
							{
								Rankings.Add(UserRanking);
							}
							else
							{
								Rankings[LeaderboardLimit - 1] = UserRanking;
							}
						}
					}

					DisplayRankings();
				}),
				FErrorHandler::CreateWeakLambda(this, [](int32 ErrorCode, const FString& ErrorMessage)
				{
					UE_LOG(LogTemp, Error, TEXT("Fail to get bulk users info"));
				}));
		}),
		FErrorHandler::CreateWeakLambda(this, [this](int32 ErrorCode, const FString& ErrorMessage)
		{
			if (ErrorCode == 71235)
			{
				UE_LOG(LogTemp, Error, TEXT("Leaderboard ranking is empty"));

				T_EmptyLeaderboard->SetVisibility(ESlateVisibility::Visible);
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Fail to get rankings"));
			}
		}));
}

void UAccelByteLeaderboard::GetUserRankings(FVoidHandler OnSuccess)
{
	FRegistry::Leaderboard.GetUserRanking(
		FRegistry::Credentials.GetUserId(),
		LeaderboardCode,
		THandler<FAccelByteModelsUserRankingData>::CreateWeakLambda(this, [this, OnSuccess](const FAccelByteModelsUserRankingData& Response)
		{
			UE_LOG(LogTemp, Log, TEXT("Success get current user rankings"));
			
			UserRankingData = Response;
			OnSuccess.Execute();
		}),
		FErrorHandler::CreateWeakLambda(this, [this, OnSuccess](int32 ErrorCode, const FString& ErrorMessage)
		{
			UserRankingData = {};

			if (ErrorCode == ERROR_NO_USER_RANKING_FOUND)
			{
				UE_LOG(LogTemp, Error, TEXT("No user ranking found"));

				OnSuccess.Execute();
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Fail to get current user rankings"));
			}
		}));
}

void UAccelByteLeaderboard::DisplayRankings()
{
	for (FRankingInfo Ranking : Rankings)
	{
		UAccelByteLeaderboardRankingEntry* RankingEntry = CreateWidget<UAccelByteLeaderboardRankingEntry>(this, LeaderboardRankingEntryClass);
		RankingEntry->InitData(
			FText::AsNumber(Ranking.Ranking + 1),
			Ranking.DisplayName,
			Ranking.Point,
			Ranking.bIsPlayer,
			Ranking.Ranking % 2 == 0);
		VB_Rankings->AddChildToVerticalBox(RankingEntry);
	}
}

void UAccelByteLeaderboard::ShowLeaderboard(const ELeaderboardType& LeaderboardType)
{
	T_LeaderboardName->SetText(FText::FromString(LeaderboardInfoMap[LeaderboardType].Name));
	LeaderboardCode = LeaderboardInfoMap[LeaderboardType].Code;
	SwitchEnabledButton(EAccelByteLeaderboardTimeFrame::ALL_TIME);
	
	GetUserRankings(FVoidHandler::CreateWeakLambda(this, [this]()
	{
		RequestRankings();
	}));
}

void UAccelByteLeaderboard::SwitchEnabledButton(const EAccelByteLeaderboardTimeFrame& TimeFrame) const
{
	T_LeaderboardTimeFrame->SetText(FText::FromString(TimeFrameStringMap[TimeFrame]));
	
	Btn_AllTime->SetIsEnabled(TimeFrame != EAccelByteLeaderboardTimeFrame::ALL_TIME);
	Btn_Daily->SetIsEnabled(TimeFrame != EAccelByteLeaderboardTimeFrame::TODAY);
	Btn_Weekly->SetIsEnabled(TimeFrame != EAccelByteLeaderboardTimeFrame::CURRENT_WEEK);
	Btn_Seasonal->SetIsEnabled(TimeFrame != EAccelByteLeaderboardTimeFrame::CURRENT_SEASON);
}

void UAccelByteLeaderboard::SwitchPanel(const EPanelType& PanelType) const
{
	bool bIsContent = PanelType == LeaderboardContent;
	WS_Root->SetActiveWidget(bIsContent ? VB_LeaderboardContent : VB_LeaderboardList);
	Btn_BackToLeaderboardList->SetVisibility(bIsContent ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
}

void UAccelByteLeaderboard::ClearRankings()
{
	VB_Rankings->ClearChildren();
	Rankings.Empty();
	T_EmptyLeaderboard->SetVisibility(ESlateVisibility::Collapsed);
}

#pragma region Button Callback

void UAccelByteLeaderboard::OnClickRankingsAllTime()
{
	ClearRankings();
	RequestRankings(EAccelByteLeaderboardTimeFrame::ALL_TIME);
}

void UAccelByteLeaderboard::OnClickRankingsDaily()
{
	ClearRankings();
	RequestRankings(EAccelByteLeaderboardTimeFrame::TODAY);
}

void UAccelByteLeaderboard::OnClickRankingsWeekly()
{
	ClearRankings();
	RequestRankings(EAccelByteLeaderboardTimeFrame::CURRENT_WEEK);
}

void UAccelByteLeaderboard::OnClickRankingsSeasonal()
{
	ClearRankings();
	RequestRankings(EAccelByteLeaderboardTimeFrame::CURRENT_SEASON);
}

void UAccelByteLeaderboard::OnClickClickLeaderboard()
{
	ClearRankings();
	ShowLeaderboard(Click);
	SwitchPanel(EPanelType::LeaderboardContent);
}

void UAccelByteLeaderboard::OnClickWinLeaderboard()
{
	ClearRankings();
	ShowLeaderboard(Win);
	SwitchPanel(EPanelType::LeaderboardContent);
}

void UAccelByteLeaderboard::OnClickSpenderLeaderboard()
{
	ClearRankings();
	ShowLeaderboard(Spender);
	SwitchPanel(EPanelType::LeaderboardContent);
}

void UAccelByteLeaderboard::OnClickBackToLeaderboardList()
{
	SwitchPanel(LeaderboardList);
}

#pragma endregion

#undef ERROR_NO_USER_RANKING_FOUND