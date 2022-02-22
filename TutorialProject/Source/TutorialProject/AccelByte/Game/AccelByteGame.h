// Copyright (c) 2021 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AccelByteScoreboardEntry.h"
#include "TutorialProject/TutPlayerControllerCountGame.h"
#include "AccelByteGame.generated.h"

class UButton;
class UTextBlock;
class UScrollBox;
class UHorizontalBox;
class UGameInstance;
class USizeBox;
class UBorder;
class UWidgetSwitcher;

/**
 * A simple clicker game widget
 */
UCLASS()
class TUTORIALPROJECT_API UAccelByteGame : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	
	virtual void NativeConstruct() override;

	/**
	* @brief Button to request increase score for this user.
	*/
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_IncreaseScore;

	/**
	* @brief Button to back to main menu after game is over.
	*/
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_BackToMainMenu;

	/**
	* @brief Horizontal Box to hide gameplay before the user is ready.
	*/
	UPROPERTY(meta = (BindWidget))
	UHorizontalBox* Hb_WaitAllPlayersReady;

	/**
	* @brief Border to hide gameplay to countdown the play time.
	*/
	UPROPERTY(meta = (BindWidget))
	UBorder* B_Countdown;

	/**
	* @brief Scroll Box of Back To Main Menu button to handle the visibility of the current button state.
	*/
	UPROPERTY(meta = (BindWidget))
	USizeBox* Sb_BackToMainMenu;

	/**
	* @brief Text Block to display time left until the game ends.
	*/
	UPROPERTY(meta = (BindWidget))
	UTextBlock* T_Countdown;

	/**
	* @brief Text Block to display time left until the game ends.
	*/
	UPROPERTY(meta = (BindWidget))
	UTextBlock* T_Timer;

	/**
	* @brief Scroll Box to display and insert Scoreboard Entries of Team A.
	*/
	UPROPERTY(meta = (BindWidget))
	UScrollBox* ScB_Scoreboard_Team_A;

	/**
	* @brief Scroll Box to display and insert Scoreboard Entries of Team B.
	*/
	UPROPERTY(meta = (BindWidget))
	UScrollBox* ScB_Scoreboard_Team_B;

	/**
	* @brief Text Box to display total score of Team A.
	*/
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Tb_TotalScore_Team_A;

	/**
	* @brief Text Box to display total score of Team B.
	*/
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Tb_TotalScore_Team_B;

	/**
	* @brief Widget switcher to switch between win or lose of Team A.
	*/
	UPROPERTY(meta = (BindWidget))
	UWidgetSwitcher* WS_Team_A_WinOrLose;

	/**
	* @brief Widget switcher to switch between win or lose of Team B.
	*/
	UPROPERTY(meta = (BindWidget))
	UWidgetSwitcher* WS_Team_B_WinOrLose;

	/**
	* @brief Widget switcher to switch if the current player win or lose.
	*/
	UPROPERTY(meta = (BindWidget))
	UWidgetSwitcher* WS_WinOrLose;

	/**
	* @brief Subclass for Scoreboard Entry Widget Class.
	*/
	TSubclassOf<UAccelByteScoreboardEntry> ScoreboardEntryClass;
	
	/**
	* @brief TMap that binds a Scoreboard Entry Widget to a UserId.
	*/
	TMap<FString, UAccelByteScoreboardEntry*> ScoreEntryWidgetMap;

	/**
	* @brief Local variable of team A score to manage the score.
	*/
	int32 TeamAScore;

	/**
	* @brief Local variable of team B score to manage the score.
	*/
	int32 TeamBScore;

	/**
	* @brief Local variable to handle current party id.
	*/
	uint8 MyPartyId;

	/**
	* @brief Boolean function to check if a UserId exists in the ScoreEntryWidgetMap and validates its value.
	*/
	bool IsUserInScoreEntryWidgetValid(const FString& UserIdToValidate) const;

	/**
	* @brief Functionality to go back to main menu.
	*/
	UFUNCTION()
	void OnBackToMainMenuClicked();

	/**
	* @brief Functionality to request for this user's score increase.
	*/
	UFUNCTION()
	void IncreaseScore();

public:

	/**
	* @brief Functionality to init and show the Countdown Panel.
	*/
	UFUNCTION()
	void InitCountdown(uint8 PartyId);

	/**
	* @brief Functionality to init and show the Scoreboard Panel
	*/
	UFUNCTION()
	void InitScoreboard(const TArray<FPlayerData>& UserIdArray);

	/**
	* @brief Functionality to update the T_Countdown text.
	*/
	UFUNCTION()
	void UpdateCountdownTimer(const int& CountdownTimerLeft);
	
	/**
	* @brief Functionality to update the T_Timer text.
	*/
	UFUNCTION()
	void UpdateTimer(const int& TimeLeft);

	/**
	* @brief Functionality to update the user and party score in a specific Scoreboard Entry Widget.
	*/
	UFUNCTION()
	void UpdateScore(const FString& UserIdToUpdate, const FPlayerData& CurrentPlayerData, int32 IncrementScore);

	/**
	* @brief Functionality to stop the game and display the winning party.
	*/
	UFUNCTION()
	void GameOver(const uint8 WinnerPartyId);
};
