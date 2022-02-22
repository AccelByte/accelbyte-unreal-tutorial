// Copyright (c) 2021 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.

#pragma once

#include "CoreMinimal.h"
#include "TutPlayerControllerCountGame.h"
#include "GameFramework/GameModeBase.h"
#include "TutGameModeCountGame.generated.h"

class UAccelByteStatistic;

/**
 * Custom Game Mode that serves as the Dedicated Server, controlling Game Logic and Functionality
 */
UCLASS()
class TUTORIALPROJECT_API ATutGameModeCountGame : public AGameModeBase
{
	GENERATED_BODY()
	
protected:

	ATutGameModeCountGame();

	/**
	* @brief Initialize when the player is first time login to the server
	*/
	virtual FString InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal) override;

	/**
	* @brief Functionality of the countdown before game started
	*/
	void Countdown();

	/**
	* @brief Functionality to decrement the countdown timer.
	*/
	void DecrementCountdownTimer();
	
	/**
	* @brief Functionality to Start the Game.
	*/
	void StartGame();

	/**
	* @brief Functionality to Decrement the time elapsed.
	*/
	void DecrementTimer();

	/**
	* @brief Functionality to Notify the Clients to Update their Countdown Timer.
	*/
	UFUNCTION()
	void NotifyClientCountdownTimerUpdate(const int& TimeLeft);

	/**
	* @brief Functionality to Notify the Clients to Update their Time Left.
	*/
	UFUNCTION()
	void NotifyClientTimeUpdate(const int& TimeLeft);

	/**
	* @brief Functionality to Notify the Clients to Update the specified user's score.
	*/
	UFUNCTION()
	void NotifyClientScoreIncrease(const FString& UserId, const FPlayerData& CurrentPlayerData);

	/**
	* @brief Functionality to Notify the Clients that the Game is Over and a Winner is determined.
	*/
	UFUNCTION()
	void NotifyClientGameOver(const uint8 WinnerPartyId);
	
	/**
	* @brief Functionality to set a Player's Status as Ready.
	*/
	void PlayerReady(ATutPlayerControllerCountGame* ReadyPlayerController);

	/**
	* @brief TArray that lists the Player Controllers who are ready.
	*/
	TArray<ATutPlayerControllerCountGame*> PlayerControllers;

	/**
	* @brief Timer Handle which will call CountdownTimer() every second.
	*/
	FTimerHandle CountdownTimerHandle;
	
	/**
	* @brief Timer Handle which will call DecrementTime() every second.
	*/
	FTimerHandle DecrementTimerHandle;

	/**
	* @brief Integer to set the minimum amount of players needed to be ready before the game can start automatically.
	*/
	int MinimumPlayersToPlay;

	/**
	* @brief Integer to limit the amount of time (seconds) that the game will endure.
	*/
	int CountdownTimer;
	
	/**
	* @brief Integer to limit the amount of time (seconds) that the game will endure.
	*/
	int GameTimer;

	/**
	* @brief Integer that controls how much is added to each user's score.
	*/
	int PlayerScoreIncrement;

	/**
	* @brief Integer that controls how much is added to each user's click.
	*/
	int PlayerClickIncrement;

public:
		
	/**
	* @brief Functionality to Increase a Player's Score.
	*/
	void IncreaseScore(const FString& UserId);

#pragma region Statistic 

private:

	/**
	* @brief Integer that controls stat "win" increment value.
	*/
	int StatWinIncrement;
	
	/**
	 * @brief Set Statistic Value from Player's game data.
	 */
	void SetStatisticValue(const TArray<FPlayerData>& PlayerDataArray, const uint8 WinnerPartyId);
	
	/**
	 * @brief Initialize Statistic Class to be able to call its functions
	 */
	UPROPERTY()
	UAccelByteStatistic* AccelByteStatistic;
	
#pragma endregion 
};