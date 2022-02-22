// Copyright (c) 2021 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.

#include "TutGameModeCountGame.h"
#include "TutPlayerControllerCountGame.h"
#include "TutorialProjectGameInstance.h"
#include "TutorialProjectUtilities.h"
#include "AccelByte/Statistic/AccelByteStatistic.h"
#include "Api/AccelByteUserApi.h"
#include "GameServerApi/AccelByteServerDSMApi.h"
#include "GameServerApi/AccelByteServerMatchmakingApi.h"
#include "Core/AccelByteRegistry.h"

#define STAT_CLICK "click"
#define STAT_WIN "win"

ATutGameModeCountGame::ATutGameModeCountGame() :
	CountdownTimer(3),
	GameTimer(20),
	PlayerScoreIncrement(5),
	PlayerClickIncrement(1),
	StatWinIncrement(1)
{
	PlayerControllerClass = ATutPlayerControllerCountGame::StaticClass();
}

FString ATutGameModeCountGame::InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueNetId, const FString& Options, const FString& Portal)
{
	FString ParsedPlayerId = UGameplayStatics::ParseOption(Options, TEXT("PlayerId"));

	UE_LOG(LogTemp, Log, TEXT("Player Id: %s"), *ParsedPlayerId);
	
	FRegistry::ServerDSM.GetSessionId(THandler<FAccelByteModelsServerSessionResponse>::CreateLambda([this, ParsedPlayerId, NewPlayerController](const FAccelByteModelsServerSessionResponse& Result)
	{
		UE_LOG(LogTemp, Log, TEXT("Success Get Session Id. Session Id: %s"), *Result.Session_id);

		FRegistry::ServerMatchmaking.QuerySessionStatus(Result.Session_id, THandler<FAccelByteModelsMatchmakingResult>::CreateLambda([this, ParsedPlayerId, NewPlayerController](const FAccelByteModelsMatchmakingResult& Result)
		{
			UE_LOG(LogTemp, Log, TEXT("Success Query Session Status."));

			if (Result.Status != EAccelByteMatchmakingSessionStatus::Matched)
			{
				UE_LOG(LogTemp, Log, TEXT("Matchmaking info is not matched"));
			}
			else
			{
				for (int32 Index = 0; Index < Result.Matching_allies.Num(); Index++)
				{
					for (const auto& MatchingParty : Result.Matching_allies[Index].Matching_parties)
					{						
						for (const auto& PartyMember : MatchingParty.Party_members)
						{
							if (PartyMember.User_id == ParsedPlayerId)
							{
								FString GameMode;
								if (MatchingParty.Party_attributes.JsonObject->TryGetStringField(FString("GameMode"), GameMode))
								{
									if (GameMode == UEnum::GetDisplayValueAsText(EGameMode::ONEPLAYER).ToString()) { MinimumPlayersToPlay = 2; }
									else if (GameMode == UEnum::GetDisplayValueAsText(EGameMode::TWOPLAYER).ToString()) { MinimumPlayersToPlay = 4; }
									else if (GameMode == UEnum::GetDisplayValueAsText(EGameMode::THREEPLAYER).ToString()) { MinimumPlayersToPlay = 6; }
									else if (GameMode == UEnum::GetDisplayValueAsText(EGameMode::FOURPLAYER).ToString()) { MinimumPlayersToPlay = 8; }
									else if (GameMode == UEnum::GetDisplayValueAsText(EGameMode::FFA).ToString()) { MinimumPlayersToPlay = 8; }
									
									FRegistry::User.GetUserByUserId(ParsedPlayerId, THandler<FSimpleUserData>::CreateLambda([this, ParsedPlayerId, Index, NewPlayerController](const FSimpleUserData& Result)
									{
										UE_LOG(LogTemp, Log, TEXT("Success Get User By User Id. Display Name: %s"), *Result.DisplayName);
                    										
										FPlayerData CurrentPlayerData;
										CurrentPlayerData.UserId = ParsedPlayerId;
										CurrentPlayerData.DisplayName = Result.DisplayName;
										CurrentPlayerData.Score = 0;
										CurrentPlayerData.Click = 0;
										CurrentPlayerData.PartyId = Index;
										ATutPlayerControllerCountGame* TutorialController = Cast<ATutPlayerControllerCountGame>(NewPlayerController);
										TutorialController->PlayerData = CurrentPlayerData;

										PlayerReady(TutorialController);
									}), FErrorHandler::CreateLambda([](int32 ErrorCode, const FString& ErrorMessage)
									{
										UE_LOG(LogTemp, Log, TEXT("Error Get User by User Id, Error Code: %d Error Message: %s"), ErrorCode, *ErrorMessage);
									}));
								}
							}
						}
					}
				}
			}
		}), FErrorHandler::CreateLambda([](int32 ErrorCode, const FString& ErrorMessage)
		{
			UE_LOG(LogTemp, Log, TEXT("Error Query Session Status, Error Code: %d Error Message: %s"), ErrorCode, *ErrorMessage);
		}));
	}), FErrorHandler::CreateLambda([](int32 ErrorCode, const FString& ErrorMessage)
	{
		UE_LOG(LogTemp, Log, TEXT("Error Get SessionId, Error Code: %d Error Message: %s"), ErrorCode, *ErrorMessage);
	}));
	
	return Super::InitNewPlayer(NewPlayerController, UniqueNetId, Options, Portal);
}

void ATutGameModeCountGame::PlayerReady(ATutPlayerControllerCountGame* ReadyPlayerController)
{
	PlayerControllers.AddUnique(ReadyPlayerController);
	
	if (PlayerControllers.Num() >= MinimumPlayersToPlay)
	{
		Countdown();
	}
}

void ATutGameModeCountGame::Countdown()
{
	NotifyClientCountdownTimerUpdate(CountdownTimer);
	GetWorldTimerManager().SetTimer(CountdownTimerHandle, this, &ATutGameModeCountGame::DecrementCountdownTimer, 1.0f, true, 0.0f);

	for (ATutPlayerControllerCountGame* TutorialController : PlayerControllers)
	{
		TutorialController->Client_InitCountdownTimer(TutorialController->PlayerData.PartyId);
	}
}

void ATutGameModeCountGame::DecrementCountdownTimer()
{
	int TimeLeft = CountdownTimer--;
	NotifyClientCountdownTimerUpdate(TimeLeft);

	if (CountdownTimer < -1)
	{
		StartGame();
		GetWorldTimerManager().ClearTimer(CountdownTimerHandle);
	}
}

void ATutGameModeCountGame::StartGame()
{
	NotifyClientTimeUpdate(GameTimer);
	GetWorldTimerManager().SetTimer(DecrementTimerHandle, this, &ATutGameModeCountGame::DecrementTimer, 1.0f, true, 0.0f);

	TArray<FPlayerData> PlayerDataArray;

	for (ATutPlayerControllerCountGame* TutorialController : PlayerControllers)
	{
		PlayerDataArray.Add(TutorialController->PlayerData);
	}

	for (ATutPlayerControllerCountGame* TutorialController : PlayerControllers)
	{
		TutorialController->Client_InitGameScoreboard(PlayerDataArray);
	}
}

void ATutGameModeCountGame::DecrementTimer()
{
	int TimeLeft = GameTimer--;
	NotifyClientTimeUpdate(TimeLeft);

	if (GameTimer < -1)
	{
		TArray<FPlayerData> PlayerDataArray;

		for (ATutPlayerControllerCountGame* PlayerController : PlayerControllers)
		{
			PlayerDataArray.Add(PlayerController->PlayerData);
		}

		int32 TeamAScore = 0;
		int32 TeamBScore = 0;
		
		for (const FPlayerData& PlayerInfo : PlayerDataArray)
		{
			if (PlayerInfo.PartyId == 0)
			{
				TeamAScore += PlayerInfo.Score;
			}
			else
			{
				TeamBScore += PlayerInfo.Score;
			}
		}

		uint8 WinnerPartyId = 0;
		if (TeamAScore > TeamBScore)
		{
			WinnerPartyId = 0;
		}
		else
		{
			WinnerPartyId = 1;
		}

		// Set Statistics based on the current game data result
		SetStatisticValue(PlayerDataArray, WinnerPartyId);

		NotifyClientGameOver(WinnerPartyId);
		GetWorldTimerManager().ClearTimer(DecrementTimerHandle);
	}
}

void ATutGameModeCountGame::IncreaseScore(const FString& UserId)
{
	int32 Index = PlayerControllers.IndexOfByPredicate([UserId](ATutPlayerControllerCountGame* PlayerController)
	{
		return UserId == PlayerController->PlayerData.UserId;
	});

	// Define Player Data and add score (for gameplay) and click (for statistic).
	FPlayerData CurrentPlayerData = PlayerControllers[Index]->PlayerData;
	CurrentPlayerData.Score += PlayerScoreIncrement;
	CurrentPlayerData.Click += PlayerClickIncrement;
	PlayerControllers[Index]->PlayerData = CurrentPlayerData;
	
	NotifyClientScoreIncrease(UserId, CurrentPlayerData);
}

void ATutGameModeCountGame::SetStatisticValue(const TArray<FPlayerData>& PlayerDataArray, const uint8 WinnerPartyId)
{
	AccelByteStatistic = NewObject<UAccelByteStatistic>();
	
	// Create a Map that contains StatCode info along with its increment value
	TMap<FString, int32> StatCodes;

	// Increment users' statistic value for each players
	for (const FPlayerData& PlayerInfo : PlayerDataArray)
	{
		// Set the Player's Total Clicks as the "click" stat increment value
		StatCodes.Add(STAT_CLICK, PlayerInfo.Click);

		// Increment the "win" stat value only for the winner users
		if (PlayerInfo.PartyId == WinnerPartyId)
		{
			StatCodes.Add(STAT_WIN, StatWinIncrement);
		}
		
		AccelByteStatistic->IncrementServerStatistic(PlayerInfo.UserId, StatCodes);
	}
}

void ATutGameModeCountGame::NotifyClientGameOver(const uint8 WinnerPartyId)
{
	for (ATutPlayerControllerCountGame* TutorialController : PlayerControllers)
	{
		TutorialController->Client_ReceiveGameOverEvent(WinnerPartyId);
	}
}

void ATutGameModeCountGame::NotifyClientCountdownTimerUpdate(const int& TimeLeft)
{
	for (ATutPlayerControllerCountGame* TutorialController : PlayerControllers)
	{
		TutorialController->Client_ReceiveCountdownTimerUpdateEvent(TimeLeft);
	}
}

void ATutGameModeCountGame::NotifyClientTimeUpdate(const int& TimeLeft)
{
	for (ATutPlayerControllerCountGame* TutorialController : PlayerControllers)
	{
		TutorialController->Client_ReceiveTimeUpdateEvent(TimeLeft);
	}
}

void ATutGameModeCountGame::NotifyClientScoreIncrease(const FString& UserId, const FPlayerData& CurrentPlayerData)
{
	for (ATutPlayerControllerCountGame* TutorialController : PlayerControllers)
	{
		TutorialController->Client_ReceiveScoreIncreaseEvent(UserId, CurrentPlayerData, PlayerScoreIncrement);
	}
}

#undef STAT_CLICK
#undef STAT_WIN