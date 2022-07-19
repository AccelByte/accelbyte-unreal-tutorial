// Copyright (c) 2022 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Core/AccelByteError.h"
#include "Models/AccelByteLeaderboardModels.h"
#include "AccelByteLeaderboard.generated.h"

class UButton;
class UTextBlock;
class UVerticalBox;
class ATutorialMenuHUD;
class UWidgetSwitcher;
class UAccelByteLeaderboardRankingEntry;

/**
 * Leaderboard menu for leaderboard service implementation
 */
UCLASS()
class TUTORIALPROJECT_API UAccelByteLeaderboard : public UUserWidget
{
	GENERATED_BODY()
	
	virtual void NativeConstruct() override;

public:

	/**
	 * @brief Reset widget to show leaderboard list on open from main menu
	 */
	void ResetWidget() const;
	
private:
	enum ELeaderboardType
	{
		Click = 0,
		Win,
		Spender
	};

	enum EPanelType
	{
		LeaderboardContent,
		LeaderboardList
	};

	struct FRankingInfo
	{
		int32 Ranking;
		FText DisplayName;
		FText Point;
		bool bIsPlayer;
	};

	struct FLeaderboardInfo
	{
		FString Code;
		FString Name;
	};

#pragma region UI Binding

	/**
	 * @brief Button to show click leaderboard
	 */
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_ClickLeaderboard;

	/**
	 * @brief Button to show win leaderboard
	 */
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_WinLeaderboard;

	/**
	 * @brief Button to show vc spending leaderboard
	 */
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_SpenderLeaderboard;

	/**
	 * @brief Button to show all-time timeframe of currently opened leaderboard
	 */
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_AllTime;

	/**
	 * @brief Button to show seasonal timeframe of currently opened leaderboard
	 */
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Seasonal;

	/**
	 * @brief Button to show weekly timeframe of currently opened leaderboard
	 */
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Weekly;

	/**
	 * @brief Button to show daily timeframe of currently opened leaderboard
	 */
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Daily;

	/**
	 * @brief Button to return to main menu
	 */
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_BackToMainMenu;

	/**
	 * @brief Button to return leaderboard list
	 */
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_BackToLeaderboardList;

	/**
	 * @brief TextBox to show currently opened leaderboard
	 */
	UPROPERTY(meta = (BindWidget))
	UTextBlock* T_LeaderboardName;

	/**
	 * @brief TextBox to show currently opened timeframe
	 */
	UPROPERTY(meta = (BindWidget))
	UTextBlock* T_LeaderboardTimeFrame;

	/**
	 * @brief TextBox to show that current leaderboard and timeframe does not have any data
	 */
	UPROPERTY(meta = (BindWidget))
	UTextBlock* T_EmptyLeaderboard;

	/**
	 * @brief VerticalBox to house the ranking entries
	 */
	UPROPERTY(meta = (BindWidget))
	UVerticalBox* VB_Rankings;

	/**
	 * @brief VerticalBox to house the leaderboards button
	 */
	UPROPERTY(meta = (BindWidget))
	UVerticalBox* VB_LeaderboardList;

	/**
	 * @brief VerticalBox to house leaderboard content
	 */
	UPROPERTY(meta = (BindWidget))
	UVerticalBox* VB_LeaderboardContent;

	/**
	 * @brief WidgetSwitcher to switch between leaderboard list and content
	 */
	UPROPERTY(meta = (BindWidget))
	UWidgetSwitcher* WS_Root;

	/**
	* @brief Leaderboard Ranking Entry widget class
	*/
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UAccelByteLeaderboardRankingEntry> LeaderboardRankingEntryClass;
	
#pragma endregion

	/**
	 * @brief Leaderboard entry limit
	 */
	const uint8 LeaderboardLimit = 10;

	/**
	 * @brief Leaderboard code and name map
	 */
	const TMap<ELeaderboardType, FLeaderboardInfo> LeaderboardInfoMap = {
		{ELeaderboardType::Click, {"clicks", "Click Leaderboard"}},
		{ELeaderboardType::Win, {"wins", "Win Leaderboard"}},
		{ELeaderboardType::Spender, {"vc-spending", "Spender Leaderboard"}}
	};

	/**
	 * @brief Leaderboards timeframe text map
	 */
	const TMap<EAccelByteLeaderboardTimeFrame, FString> TimeFrameStringMap = {
		{EAccelByteLeaderboardTimeFrame::TODAY, "Daily"},
		{EAccelByteLeaderboardTimeFrame::ALL_TIME, "All Time"},
		{EAccelByteLeaderboardTimeFrame::CURRENT_WEEK, "Weekly"},
		{EAccelByteLeaderboardTimeFrame::CURRENT_MONTH, "Monthly"},
		{EAccelByteLeaderboardTimeFrame::CURRENT_SEASON, "Seasonal"}
	};

	/**
	 * @brief Current user rankings for all timeframe in currently opened leaderboard
	 */
	FAccelByteModelsUserRankingData UserRankingData;

	/**
	 * @brief Pointer to TutorialManuHUD
	 */
	ATutorialMenuHUD* TutorialMenuHUD;

	/**
	 * @brief Rankings for currently opened leaderboard and timeframe
	 */
	TArray<FRankingInfo> Rankings;

	/**
	 * @brief Currently opened leaderboard code
	 */
	FString LeaderboardCode = "";

	/**
	 * @brief Get ranking for currently opened leaderboard and timeframe
	 */
	void RequestRankings(const EAccelByteLeaderboardTimeFrame& TimeFrame = EAccelByteLeaderboardTimeFrame::ALL_TIME);

	/**
	 * @brief Get current user rankings for all timeframe in currently opened leaderboard
	 */
	void GetUserRankings(AccelByte::FVoidHandler OnSuccess);

	/**
	 * @brief Display rankings to UI
	 */
	void DisplayRankings();

	/**
	 * @brief Open and initialize a specific leaderboard
	 */
	void ShowLeaderboard(const ELeaderboardType& LeaderboardType);

	/**
	 * @brief Disable the currently opened timeframe button and enable the rest
	 */
	void SwitchEnabledButton(const EAccelByteLeaderboardTimeFrame& TimeFrame) const;

	/**
	 * @brief Switch between leaderboard list and leaderboard content panel
	 */
	void SwitchPanel(const EPanelType& PanelType) const;

	/**
	 * @brief Clear ranking UI
	 */
	void ClearRankings();

#pragma region Button Callback

	/**
	 * @brief Callback for all-time timeframe button
	 */
	UFUNCTION()
	void OnClickRankingsAllTime();

	/**
	 * @brief Callback for daily timeframe button
	 */
	UFUNCTION()
	void OnClickRankingsDaily();

	/**
	 * @brief Callback for weekly timeframe button
	 */
	UFUNCTION()
	void OnClickRankingsWeekly();

	/**
	 * @brief Callback for seasonal timeframe button
	 */
	UFUNCTION()
	void OnClickRankingsSeasonal();

	/**
	 * @brief Callback for click leaderboard button
	 */
	UFUNCTION()
	void OnClickClickLeaderboard();

	/**
	 * @brief Callback for win leaderboard button
	 */
	UFUNCTION()
	void OnClickWinLeaderboard();

	/**
	 * @brief Callback for vc spending leaderboard button
	 */
	UFUNCTION()
	void OnClickSpenderLeaderboard();

	/**
	 * @brief Callback for back to leaderboard list button
	 */
	UFUNCTION()
	void OnClickBackToLeaderboardList();
	
#pragma endregion
};
