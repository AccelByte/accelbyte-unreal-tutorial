// Copyright (c) 2021 AccelByte Inc. All Rights Reserved.
// This is licensed software from AccelByte Inc, for limitations
// and restrictions contact your company contract manager.

#pragma once

#include "CoreMinimal.h"
#include "Core/AccelByteError.h"
#include "Blueprint/UserWidget.h"
#include "Models/AccelByteLobbyModels.h"
#include "AccelByteMatchmaking.generated.h"

class UTextBlock;
class UListView;
class UButton;
class ATutorialMenuHUD;
class UAccelByteMatchmakingPlayerEntry;

/** Matchmaking base, where most AccelByte Matchmaking system
 * Implemented here. In this example we do have 2 Team fight
 * that we need to set up inside Admin Portal.
* This code covers AccelByte services including :
*
* - Start Matchmaking
* - Ready Consent
* - Matchmaking Notification
*/

DECLARE_DELEGATE_TwoParams(FGetServerLatencies, const TArray<TPair<FString, float>>& Latencies);

UCLASS()
class TUTORIALPROJECT_API UAccelByteMatchmaking : public UUserWidget
{
	GENERATED_BODY()
	
protected:

	UAccelByteMatchmaking(const FObjectInitializer& ObjectInitializer);

	virtual void NativeOnInitialized() override;

	virtual void NativeConstruct() override;

	void SetCountDownTimer();

	UPROPERTY()
	ATutorialMenuHUD* TutorialMenuHUD;

	uint8 IndexTeamA;

	uint8 IndexTeamB;

	uint8 MaximumPlayersEntry;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* T_Countdown;

	FTimerHandle CountDownTimer;

	/**
	 * @brief the ready consent timeout is set in Admin Portal. the link is here: https://demo.accelbyte.io/admin/lobby-configuration/unrealtutorialproject
	 */
	int32 ReadyConsentTimeOutInSec;
	
private:
	
	/**
	* @brief List View for Team A member.
	*/
	UPROPERTY(meta = (BindWidget))
	UListView* Lv_Team_A;
	
	/**
	* @brief List View for Team B member.
	*/
	UPROPERTY(meta = (BindWidget))
	UListView* Lv_Team_B;
	
	/**
	* @brief Button for Ready Match.
	*/
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_ReadyMatch;

	/**
	* @brief Callback matchmaking start success.
	*/
	UFUNCTION()
	void OnMatchmakingStartSuccess(const FAccelByteModelsMatchmakingResponse& Result);
	
	/**
	* @brief Callback matchmaking start failed.
	*/
	void OnMatchmakingStartFailed(const int32 Code, const FString& Message);
	
	/**
	* @brief Callback when event triggered on matchmaking i.e (find match, cancel match, etc).
	*/
	UFUNCTION()
	void OnMatchmakingNotification(const FAccelByteModelsMatchmakingNotice& Notice);

	void ResetPlayerEntries();

	/**
	* @brief Callback when event triggered on rematchmaking or ready consent timeout.
	*/
	UFUNCTION()
	void OnPlayerTimeOutReadyConsent(const FAccelByteModelsRematchmakingNotice& Notice);
	
	/**
	* @brief Response for ready consent.
	*/
	void OnReadyConsentResponse(const FAccelByteModelsReadyConsentRequest& Result);
	
	/**
	* @brief Notified all member when game ready consent.
	*/
	void OnReadyConsentNotification(const FAccelByteModelsReadyConsentNotice& Result);
	
	/**
	* @brief Notified Dedicated Server when matchmaking sent to DS.
	*/
	void OnDsNotification(const FAccelByteModelsDsNotice& Result);

	/**
	 * @brief Add new Matchmaking Team on the List View after the player successfully ready a match.
	 * @param UserData The player user data to specify the full information of the player and team, also split up the team
	 */
	void SetNewPlayerEntry(const FSimpleUserData& UserData);

	/**
	 * @brief Set Dummy Player Entry and maximum of the player entry is 4 player
	 * @param IndexTeam the Index of the team, whether Team A or B
	 */
	void SetDummyPlayerEntry(uint8 IndexTeam);

	/**
	 * @brief This will help to initiate new widget for the Matchmaking Player Entry widget.
	 */
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UAccelByteMatchmakingPlayerEntry> MatchmakingPlayerEntryClass;

	/**
	 * @brief All Players in Team A that are ready for a match.
	 */
	TArray<UAccelByteMatchmakingPlayerEntry*> TeamAPlayersEntry;

	/**
	* @brief All Players in Team B that are ready for a match.
	*/
	TArray<UAccelByteMatchmakingPlayerEntry*> TeamBPlayersEntry;

	/**
	 * @brief The notice after player found a match
	 */
	FAccelByteModelsMatchmakingNotice MatchmakingNotice;

public:

	void SetMatchmakingNotification();
	
	/**
	* @brief Match ID container when created.
	*/
	FString MatchId;
	
	/**
	* @brief Callback when button Start matchmaking pressed.
	*/
	UFUNCTION()
	void OnClickedStartMatchmaking();

	/**
	* @brief Callback when button Start Game Ready Consent pressed.
	*/
	UFUNCTION()
	void OnClickedReadyConsent();
};
