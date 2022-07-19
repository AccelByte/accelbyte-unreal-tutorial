// Copyright (c) 2021 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.

#include "TutGameModeCountGame.h"
#include "TutPlayerControllerCountGame.h"
#include "TutorialProjectGameInstance.h"
#include "TutorialProjectUtilities.h"
#include "AccelByte/Entitlement/AccelByteEntitlement.h"
#include "AccelByte/Statistic/AccelByteStatisticObject.h"
#include "Api/AccelByteEntitlementApi.h"
#include "Api/AccelByteUserApi.h"
#include "GameServerApi/AccelByteServerDSMApi.h"
#include "GameServerApi/AccelByteServerMatchmakingApi.h"
#include "Core/AccelByteRegistry.h"
#include "GameServerApi/AccelByteServerAchievementApi.h"
#include "GameServerApi/AccelByteServerCloudSaveApi.h"

#define STAT_CLICK "click"
#define STAT_WIN "win"
#define BRONZE_ABILITY_INCREMENT 20
#define SILVER_ABILITY_INCREMENT 25
#define FIRST_WIN_ACHIEVEMENT "win-first-time"
#define FRIENDLY_FACE_ACHIEVEMENT "finish-party-1"

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
	
	FRegistry::ServerDSM.GetSessionId(THandler<FAccelByteModelsServerSessionResponse>::CreateWeakLambda(this, [this, ParsedPlayerId, NewPlayerController](const FAccelByteModelsServerSessionResponse& Result)
	{
		UE_LOG(LogTemp, Log, TEXT("Success Get Session Id. Session Id: %s"), *Result.Session_id);

		FRegistry::ServerMatchmaking.QuerySessionStatus(Result.Session_id, THandler<FAccelByteModelsMatchmakingResult>::CreateWeakLambda(this, [this, ParsedPlayerId, NewPlayerController](const FAccelByteModelsMatchmakingResult& Result)
		{
			UE_LOG(LogTemp, Log, TEXT("Success Query Session Status."));

			if (Result.Status != EAccelByteMatchmakingSessionStatus::Matched)
			{
				UE_LOG(LogTemp, Log, TEXT("Matchmaking info is not matched"));
			}
			else
			{
				for (uint8 Index = 0; Index < Result.Matching_allies.Num(); Index++)
				{
					for (const auto& MatchingParty : Result.Matching_allies[Index].Matching_parties)
					{
						bool bIsPlayWithFriend = MatchingParty.Party_members.Num() > 1;
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
									
									FRegistry::User.GetUserByUserId(ParsedPlayerId, THandler<FSimpleUserData>::CreateWeakLambda(this, [this, ParsedPlayerId, Index, NewPlayerController, bIsPlayWithFriend](const FSimpleUserData& Result)
									{
										UE_LOG(LogTemp, Log, TEXT("Success Get User By User Id. Display Name: %s"), *Result.DisplayName);
                    										
										FPlayerData CurrentPlayerData;
										CurrentPlayerData.UserId = ParsedPlayerId;
										CurrentPlayerData.DisplayName = Result.DisplayName;
										CurrentPlayerData.Score = 0;
										CurrentPlayerData.Click = 0;
										CurrentPlayerData.PartyId = static_cast<EPartyId>(Index);
										CurrentPlayerData.bIsPlayWithFriend = bIsPlayWithFriend;
										ATutPlayerControllerCountGame* TutorialController = Cast<ATutPlayerControllerCountGame>(NewPlayerController);
										TutorialController->PlayerData = CurrentPlayerData;

										FRegistry::ServerCloudSave.GetPublicUserRecord(TutorialProjectUtilities::PublicCloudSaveEquipmentKey, ParsedPlayerId, THandler<FAccelByteModelsUserRecord>::CreateWeakLambda(this, [this, ParsedPlayerId, TutorialController](const FAccelByteModelsUserRecord& Result)
										{
											FPlayerEquipment PlayerEquipment = {};
											if (Result.Value.JsonObject->HasField(TutorialProjectUtilities::PublicCloudSaveBadgeJsonKey))
											{
												const FString& EquippedItem = Result.Value.JsonObject->GetStringField(TutorialProjectUtilities::PublicCloudSaveBadgeJsonKey);
												PlayerEquipment.EquippedItem = EquippedItem;
											}
											else
											{
												PlayerEquipment.EquippedItem = TutorialProjectUtilities::PublicCloudSaveNoneValue;
											}
											
											if (Result.Value.JsonObject->HasField(TutorialProjectUtilities::PublicCloudSaveAbilityJsonKey))
											{
												const FString& UsedItem = Result.Value.JsonObject->GetStringField(TutorialProjectUtilities::PublicCloudSaveAbilityJsonKey);
												PlayerEquipment.UsedItem = UsedItem;

												bool bIsItemInUsed = false;
												if (UsedItem == TutorialProjectUtilities::PublicCloudSaveAbilityBronze)
												{
													bIsItemInUsed = true;
													UserIdToScoreIncrementAbility.Emplace(ParsedPlayerId, BRONZE_ABILITY_INCREMENT);
												}
												else if (UsedItem == TutorialProjectUtilities::PublicCloudSaveAbilitySilver)
												{
													bIsItemInUsed = true;
													UserIdToScoreIncrementAbility.Emplace(ParsedPlayerId, SILVER_ABILITY_INCREMENT);
												}

												if (bIsItemInUsed)
												{
													PlayerEquipment.UsedItem = TutorialProjectUtilities::PublicCloudSaveNoneValue;
													const TSharedPtr<FJsonObject> JsonObject = FJsonObjectConverter::UStructToJsonObject(PlayerEquipment);
													constexpr bool bIsPublic = true;
													FRegistry::ServerCloudSave.ReplaceUserRecord(TutorialProjectUtilities::PublicCloudSaveEquipmentKey, ParsedPlayerId, *JsonObject, bIsPublic, FVoidHandler::CreateWeakLambda(this, [this, TutorialController]()
													{
														PlayerReady(TutorialController);
													}), FErrorHandler::CreateWeakLambda(this, [this, TutorialController](int32 ErrorCode, const FString& ErrorMessage)
													{
														UE_LOG(LogTemp, Error, TEXT("Error ReplaceUserRecord, Error Code: %d Error Message: %s"), ErrorCode, *ErrorMessage);
														PlayerReady(TutorialController);
													}));
												}
												else
												{
													UserIdToScoreIncrementAbility.Emplace(ParsedPlayerId, PlayerScoreIncrement);
													PlayerReady(TutorialController);
												}
											}
											else
											{
												UserIdToScoreIncrementAbility.Emplace(ParsedPlayerId, PlayerScoreIncrement);
												PlayerReady(TutorialController);
											}
										}), FErrorHandler::CreateWeakLambda(this, [this, TutorialController, ParsedPlayerId](int32 ErrorCode, const FString& ErrorMessage)
										{
											UE_LOG(LogTemp, Error, TEXT("Error GetPublicUserRecord, Error Code: %d Error Message: %s"), ErrorCode, *ErrorMessage);
											
											UserIdToScoreIncrementAbility.Emplace(ParsedPlayerId, PlayerScoreIncrement);

											PlayerReady(TutorialController);
										}));
									}), FErrorHandler::CreateWeakLambda(this, [](int32 ErrorCode, const FString& ErrorMessage)
									{
										UE_LOG(LogTemp, Error, TEXT("Error Get User by User Id, Error Code: %d Error Message: %s"), ErrorCode, *ErrorMessage);
									}));
								}
							}
						}
					}
				}
			}
		}), FErrorHandler::CreateWeakLambda(this, [](int32 ErrorCode, const FString& ErrorMessage)
		{
			UE_LOG(LogTemp, Log, TEXT("Error Query Session Status, Error Code: %d Error Message: %s"), ErrorCode, *ErrorMessage);
		}));
	}), FErrorHandler::CreateWeakLambda(this, [](int32 ErrorCode, const FString& ErrorMessage)
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
	const int TimeLeft = CountdownTimer--;
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

	for (const ATutPlayerControllerCountGame* TutorialController : PlayerControllers)
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
	const int TimeLeft = GameTimer--;
	NotifyClientTimeUpdate(TimeLeft);

	if (GameTimer < -1)
	{
		TArray<FPlayerData> PlayerDataArray;

		for (const ATutPlayerControllerCountGame* PlayerController : PlayerControllers)
		{
			PlayerDataArray.Add(PlayerController->PlayerData);
		}

		int32 TeamAScore = 0;
		int32 TeamBScore = 0;
		for (const FPlayerData& PlayerInfo : PlayerDataArray)
		{
			if (PlayerInfo.PartyId == EPartyId::PartyA)
			{
				TeamAScore += PlayerInfo.Score;
			}
			else if (PlayerInfo.PartyId == EPartyId::PartyB)
			{
				TeamBScore += PlayerInfo.Score;
			}
		}

		EWinnerParty WinnerParty;
		if (TeamAScore == TeamBScore)
		{
			WinnerParty = EWinnerParty::Draw;
		}
		else if (TeamAScore > TeamBScore)
		{
			WinnerParty = EWinnerParty::TeamA;
		}
		else
		{
			WinnerParty = EWinnerParty::TeamB;
		}

		// Set Statistics based on the current game data result
		SetStatisticValue(PlayerDataArray, WinnerParty);

		NotifyClientGameOver(WinnerParty);
		GetWorldTimerManager().ClearTimer(DecrementTimerHandle);
	}
}

void ATutGameModeCountGame::IncreaseScore(const FString& UserId)
{
	const int32 Index = PlayerControllers.IndexOfByPredicate([UserId](const ATutPlayerControllerCountGame* PlayerController)
	{
		return UserId == PlayerController->PlayerData.UserId;
	});

	// Define Player Data and add score (for gameplay) and click (for statistic).
	FPlayerData CurrentPlayerData = PlayerControllers[Index]->PlayerData;
	if (UserIdToScoreIncrementAbility.FindRef(UserId))
	{
		CurrentPlayerData.Score += UserIdToScoreIncrementAbility.FindRef(UserId);
		CurrentPlayerData.Click += PlayerClickIncrement;
		PlayerControllers[Index]->PlayerData = CurrentPlayerData;
	
		NotifyClientScoreIncrease(UserId, CurrentPlayerData, UserIdToScoreIncrementAbility.FindRef(UserId));
	}
}

void ATutGameModeCountGame::SetStatisticValue(const TArray<FPlayerData>& PlayerDataArray, const EWinnerParty& WinnerParty)
{
	AccelByteStatisticObject = NewObject<UAccelByteStatisticObject>();
	
	// Create a Map that contains StatCode info along with its increment value
	TMap<FString, int32> StatCodes;

	// Increment users' statistic value for each players
	for (const FPlayerData& PlayerInfo : PlayerDataArray)
	{
		// Set the Player's Total Clicks as the "click" stat increment value
		StatCodes.Add(STAT_CLICK, PlayerInfo.Click);

		// Increment the "win" stat value only for the winner users, if draw then there are no user that increase their statistic
		if (static_cast<uint8>(PlayerInfo.PartyId) == static_cast<uint8>(WinnerParty))
		{
			StatCodes.Add(STAT_WIN, StatWinIncrement);

			// Unlock user Achievement for "win-first-time" achievement
			FRegistry::ServerAchievement.UnlockAchievement(PlayerInfo.UserId, FIRST_WIN_ACHIEVEMENT, FVoidHandler::CreateWeakLambda(this, [PlayerInfo]()
			{
				UE_LOG(LogTemp, Log, TEXT("Unlock Achievement Success, UserId: %s Achievement Code: %s"), *PlayerInfo.UserId, *FIRST_WIN_ACHIEVEMENT);
			}), FErrorHandler::CreateWeakLambda(this, [](int32 ErrorCode, const FString& ErrorMessage)
			{
				UE_LOG(LogTemp, Warning, TEXT("Unlock Achievement is not Success, Code: %d Message: %s"), ErrorCode, *ErrorMessage);
			}));

			if (PlayerInfo.bIsPlayWithFriend)
			{
				// Unlock user Achievement for "finish-party-1" achievement
				FRegistry::ServerAchievement.UnlockAchievement(PlayerInfo.UserId, FRIENDLY_FACE_ACHIEVEMENT, FVoidHandler::CreateWeakLambda(this, [PlayerInfo]()
				{
					UE_LOG(LogTemp, Log, TEXT("Unlock Achievement Success, UserId: %s Achievement Code: %s"), *PlayerInfo.UserId, *FRIENDLY_FACE_ACHIEVEMENT);
				}), FErrorHandler::CreateWeakLambda(this, [](int32 ErrorCode, const FString& ErrorMessage)
				{
					UE_LOG(LogTemp, Warning, TEXT("Unlock Achievement is not Success, Code: %d Message: %s"), ErrorCode, *ErrorMessage);
				}));
			}
		}
		
		AccelByteStatisticObject->IncrementServerStatistic(
			PlayerInfo.UserId,
			StatCodes,
			FVoidHandler::CreateWeakLambda(this, [this](){}),
			FErrorHandler::CreateWeakLambda(this, [this](int32 ErrorCode, const FString& ErrorMessage){}));
	}
}

void ATutGameModeCountGame::NotifyClientGameOver(const EWinnerParty& WinnerParty)
{
	for (ATutPlayerControllerCountGame* TutorialController : PlayerControllers)
	{
		TutorialController->Client_ReceiveGameOverEvent(WinnerParty);
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

void ATutGameModeCountGame::NotifyClientScoreIncrease(const FString& UserId, const FPlayerData& CurrentPlayerData, int32 ScoreIncrement)
{
	for (ATutPlayerControllerCountGame* TutorialController : PlayerControllers)
	{
		TutorialController->Client_ReceiveScoreIncreaseEvent(UserId, CurrentPlayerData, ScoreIncrement);
	}
}

#undef STAT_CLICK
#undef STAT_WIN
#undef BRONZE_ABILITY_INCREMENT
#undef SILVER_ABILITY_INCREMENT
#undef FIRST_WIN_ACHIEVEMENT
#undef FRIENDLY_FACE_ACHIEVEMENT