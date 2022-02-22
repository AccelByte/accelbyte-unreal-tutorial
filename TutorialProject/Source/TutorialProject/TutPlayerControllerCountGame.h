// Copyright (c) 2021 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.

#pragma once

#include "Engine.h"
#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TutPlayerControllerCountGame.generated.h"

/**
 * @brief Player information
 */
USTRUCT()
struct FPlayerData
{
	GENERATED_BODY()

	UPROPERTY()
	FString UserId;
	UPROPERTY()
	uint8 PartyId;
	UPROPERTY()
	FString DisplayName;
	UPROPERTY()
	int Score;
	UPROPERTY()
	int32 Click;
};

class UAccelByteGame;
class UAccelByteScoreboardEntry;
class ATutorialProjectGameModeBase;

/**
 * Custom Player Controller for Dedicated Server and Client connections
 */
UCLASS()
class TUTORIALPROJECT_API ATutPlayerControllerCountGame : public APlayerController
{
	GENERATED_BODY()
	
protected:

	/**
	* @brief Begin play when the first time player controller is initialize
	*/
	void BeginPlay() override;

	/**
	* @brief Pointer for this player's Game Widget.
	*/
	UAccelByteGame* GameWidget;

	/**
	* @brief Login Menu widget class
	*/
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UAccelByteGame> AccelByteGameClass;

public:
	
	/**
	* @brief Sends request to server asking to increase this player's score.
	*/
	UFUNCTION(Server, Reliable)
	void Server_RequestScoreIncrease();

	/**
	* @brief Notification from server to Initialize the Countdown Timer in this player's Game Widget.
	*/
	UFUNCTION(Client, Reliable)
	void Client_InitCountdownTimer(uint8 PartyId);

	/**
	* @brief Notification from server to update countdown time is left.
	*/
	UFUNCTION(Client, Reliable)
	void Client_ReceiveCountdownTimerUpdateEvent(const int& TimeLeft);
	
	/**
	* @brief Notification from server to update how much time is left.
	*/
	UFUNCTION(Client, Reliable)
	void Client_ReceiveTimeUpdateEvent(const int& TimeLeft);

	/**
	* @brief Notification from server to update a certain User's Score.
	*/
	UFUNCTION(Client, Reliable)
	void Client_ReceiveScoreIncreaseEvent(const FString& UserId, const FPlayerData& CurrentPlayerData, const int32 IncrementScore);
	
	/**
	* @brief Notification from server to Initialize the Scoreboard in this player's Game Widget.
	*/
	UFUNCTION(Client, Reliable)
	void Client_InitGameScoreboard(const TArray<FPlayerData>& CurrentPlayerData);

	/**
	* @brief Notification from server announcing the Game is Over and its winner.
	*/
	UFUNCTION(Client, Reliable)
	void Client_ReceiveGameOverEvent(const uint8 WinnerPartyId);

	/**
	* @brief Login Menu widget class
	*/
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UAccelByteScoreboardEntry> AccelByteScoreboardEntryClass;

	/**
	* @brief Current Player Information
	*/
	FPlayerData PlayerData;
};
